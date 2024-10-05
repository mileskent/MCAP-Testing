#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>
#include <google/protobuf/message.h>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <string>

void fillMessageFromFields(const google::protobuf::Message* sourceMessage, google::protobuf::Message* targetMessage) {
    const google::protobuf::Descriptor* descriptor = sourceMessage->GetDescriptor();
    const google::protobuf::Reflection* reflection = sourceMessage->GetReflection();
    const google::protobuf::Reflection* targetReflection = targetMessage->GetReflection();
    
    std::vector<const google::protobuf::FieldDescriptor*> fields;
    reflection->ListFields(*sourceMessage, &fields);

    for (const auto* field : fields) {
        if (field->is_repeated()) {
            // Handle repeated fields here
            int fieldSize = reflection->FieldSize(*sourceMessage, field);
            for (int i = 0; i < fieldSize; ++i) {
                if (field->type() == google::protobuf::FieldDescriptor::TYPE_STRING) {
                    const std::string& value = reflection->GetRepeatedString(*sourceMessage, field, i);
                    targetReflection->AddString(targetMessage, field, value);
                }
                // Handle other repeated types...
            }
        } else {
            // Handle non-repeated fields
            if (field->type() == google::protobuf::FieldDescriptor::TYPE_STRING) {
                const std::string& value = reflection->GetString(*sourceMessage, field);
                targetReflection->SetString(targetMessage, field, value);
            } else if (field->type() == google::protobuf::FieldDescriptor::TYPE_INT32) {
                int32_t value = reflection->GetInt32(*sourceMessage, field);
                targetReflection->SetInt32(targetMessage, field, value);
            }
            // Handle other field types...
        }
    }
}

std::string serializeMessage(const google::protobuf::Message& message) {
    std::string serialized_message;
    if (message.SerializeToString(&serialized_message)) {
        return serialized_message;
    } else {
        std::cerr << "Failed to serialize message" << std::endl;
        return "";
    }
}

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

int main() {
    // Simulate reading MCAP data and constructing Protobuf messages...
    google::protobuf::Message* sourceMessage = /* Load or parse source Protobuf message */;
    google::protobuf::Message* targetMessage = /* New Protobuf message instance to fill */;
    
    // Fill the target message with data from source fields
    fillMessageFromFields(sourceMessage, targetMessage);

    // Serialize the filled message
    std::string serialized_message = serializeMessage(*targetMessage);

    // Send the serialized message to localhost
    if (!serialized_message.empty()) {
        sendMessageToLocalhost(serialized_message);
    }

    return 0;
}



