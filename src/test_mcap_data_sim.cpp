#define MCAP_IMPLEMENTATION
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/message.h>

#include "../mcap/reader.hpp"
#include <memory>
#include <iostream>
#include <vector>

#include "socket.hpp"

namespace gp = google::protobuf;

// Loads the FileDescriptorSet from a protobuf schema definition into a SimpleDescriptorDatabase.
bool LoadSchema(const mcap::SchemaPtr schema, gp::SimpleDescriptorDatabase *protoDb) {
    gp::FileDescriptorSet fdSet;
    if (!fdSet.ParseFromArray(schema->data.data(), static_cast<int>(schema->data.size()))) {
        return false;
    }
    gp::FileDescriptorProto unused;
    for (int i = 0; i < fdSet.file_size(); ++i) {
        const auto &file = fdSet.file(i);
        if (!protoDb->FindFileByName(file.name(), &unused)) {
            if (!protoDb->Add(file)) {
                return false;
            }
        }
    }
    return true;
}


int main(int argc, char **argv) {

    Socket socket(DEFAULT_IP, DEFAULT_PORT);
    
    // stuff that will be passed in later on, but is hardcoded now
    const char *inputFilename;
    const std::string channelName = "mcu_error_states_data";

    if (argc != 2) {

        inputFilename = "testdata.mcap";
    } else {
        inputFilename = argv[1];
    }

    mcap::McapReader reader;
    const auto res = reader.open(inputFilename);
    if (!res.ok()) {
        return 1;
    }

    auto messageView = reader.readMessages();
    gp::SimpleDescriptorDatabase protoDb;
    gp::DescriptorPool protoPool(&protoDb);
    gp::DynamicMessageFactory protoFactory(&protoPool);

    int foo = 0;

    for (auto it = messageView.begin(); it != messageView.end(); it++) {
        if (it->schema->encoding != "protobuf") {
            continue;
        }
        if (it->channel->messageEncoding != "protobuf") {
            std::cerr << "expected channel with ID " << it->channel->id << " and schema ID "
                      << it->schema->id << "to use message encoding 'protobuf', but message encoding is "
                      << it->channel->messageEncoding << std::endl;
            reader.close();
            return 1;
        }

        // TEMP FOR TESTING SPECIFIC CHANNEL
        if (it->channel->topic != channelName) {
            continue;
        }

        // DEBUG OUT
        std::cout << "Channel Topic Name: " << it->channel->topic << std::endl; 

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

        std::cout << "Message before serialization:" << std::endl;
        std::cout << message->DebugString() << std::endl;

        std::string serializedMessage;
        if (!message->SerializeToString(&serializedMessage)) {
            std::cerr << "Failed to serialize protobuf message" << std::endl;
            reader.close();
            return 1;
        }

        // Send the serialized message to the server using UDP
        socket.send(serializedMessage, false);

        foo++;
        if (foo > 100) break;
    }

    reader.close();
    socket.close();
    return 0;
}