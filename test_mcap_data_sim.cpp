#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor_database.h>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <memory>
#include "mcap/reader.hpp"  // Include your MCAP reader header

namespace gp = google::protobuf;

// Function to fill target message with fields from the source message
void fillMessageFromFields(const gp::Message* sourceMessage, gp::Message* targetMessage) {
    const gp::Descriptor* descriptor = sourceMessage->GetDescriptor();
    const gp::Reflection* reflection = sourceMessage->GetReflection();
    const gp::Reflection* targetReflection = targetMessage->GetReflection();
    
    std::vector<const gp::FieldDescriptor*> fields;
    reflection->ListFields(*sourceMessage, &fields);

    for (const auto* field : fields) {
        if (field->is_repeated()) {
            // Handle repeated fields here
            int fieldSize = reflection->FieldSize(*sourceMessage, field);
            for (int i = 0; i < fieldSize; ++i) {
                if (field->type() == gp::FieldDescriptor::TYPE_STRING) {
                    const std::string& value = reflection->GetRepeatedString(*sourceMessage, field, i);
                    targetReflection->AddString(targetMessage, field, value);
                }
                // Handle other repeated types...
            }
        } else {
            // Handle non-repeated fields
            if (field->type() == gp::FieldDescriptor::TYPE_STRING) {
                const std::string& value = reflection->GetString(*sourceMessage, field);
                targetReflection->SetString(targetMessage, field, value);
            } else if (field->type() == gp::FieldDescriptor::TYPE_INT32) {
                int32_t value = reflection->GetInt32(*sourceMessage, field);
                targetReflection->SetInt32(targetMessage, field, value);
            }
            // Handle other field types...
        }
    }
}

// Serialize message to string
std::string serializeMessage(const gp::Message& message) {
    std::string serialized_message;
    if (message.SerializeToString(&serialized_message)) {
        return serialized_message;
    } else {
        std::cerr << "Failed to serialize message" << std::endl;
        return "";
    }
}

// Send serialized message to localhost
void sendMessageToLocalhost(const std::string& serialized_message) {
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed!" << std::endl;
        return;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12345); // Port number
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(sockfd, serialized_message.data(), serialized_message.size(), 0,
           (const struct sockaddr*)&servaddr, sizeof(servaddr));

    std::cout << "Message sent to localhost" << std::endl;
    close(sockfd);
}

// Load schema into the descriptor database
bool LoadSchema(const mcap::SchemaPtr schema, gp::SimpleDescriptorDatabase *protoDb) {
    gp::FileDescriptorSet fdSet;
    if (!fdSet.ParseFromArray(schema->data.data(), static_cast<int>(schema->data.size()))) {
        std::cerr << "failed to parse schema data" << std::endl;
        return false;
    }
    
    gp::FileDescriptorProto unused; // Ensure this type is included
    for (int i = 0; i < fdSet.file_size(); ++i) {
        const auto &file = fdSet.file(i);
        if (!protoDb->FindFileByName(file.name(), &unused)) {
            if (!protoDb->Add(file)) {
                std::cerr << "failed to add definition " << file.name() << " to protoDB" << std::endl;
                return false;
            }
        }
    }
    return true;
}

// Main function for reading MCAP data
int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.mcap>" << std::endl;
        return 1;
    }
    const char *inputFilename = argv[1];

    mcap::McapReader reader;
    const auto res = reader.open(inputFilename);
    if (!res.ok()) {
        std::cerr << "Failed to open " << inputFilename << " for reading: " << res.message << std::endl;
        return 1;
    }

    auto messageView = reader.readMessages();
    gp::SimpleDescriptorDatabase protoDb;
    gp::DescriptorPool protoPool(&protoDb);
    gp::DynamicMessageFactory protoFactory(&protoPool);

    for (auto it = messageView.begin(); it != messageView.end(); it++) {
        if (it->schema->encoding != "protobuf") {
            continue;
        }
        if (it->channel->messageEncoding != "protobuf") {
            std::cerr << "expected channel with ID " << it->channel->id << " and schema ID "
                      << it->schema->id << " to use message encoding 'protobuf', but message encoding is "
                      << it->channel->messageEncoding << std::endl;
            reader.close();
            return 1;
        }
        
        const gp::Descriptor *descriptor = protoPool.FindMessageTypeByName(it->schema->name);
        if (descriptor == nullptr) {
            if (!LoadSchema(it->schema, &protoDb)) {
                reader.close();
                return 1;
            }
            descriptor = protoPool.FindMessageTypeByName(it->schema->name);
            if (descriptor == nullptr) {
                std::cerr << "failed to find descriptor after loading pool" << std::endl;
                reader.close();
                return 1;
            }
        }

        auto message = std::unique_ptr<gp::Message>(protoFactory.GetPrototype(descriptor)->New());
        if (!message->ParseFromArray(it->message.data, static_cast<int>(it->message.dataSize))) {
            std::cerr << "failed to parse message using included schema" << std::endl;
            reader.close();
            return 1;
        }

        std::string serialized_message = serializeMessage(*message);
        if (!serialized_message.empty()) {
            sendMessageToLocalhost(serialized_message);
        }
    }
    reader.close();
    return 0;
}
