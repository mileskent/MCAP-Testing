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
#include "helper.hpp"

namespace gp = google::protobuf;

int main() {
    Socket socket(DEFAULT_IP, DEFAULT_PORT);
    socket.bind();

    // TEMP Const
    const std::string channelName = "mcu_error_states_data";

    // Assuming you already have the Protobuf Descriptor setup
    gp::SimpleDescriptorDatabase protoDb;
    gp::DescriptorPool protoPool(&protoDb);
    gp::DynamicMessageFactory protoFactory(&protoPool);

    // Server Loop
    while (true) {
        std::string received_message;
        if (socket.receive(received_message)) {
            const gp::Descriptor* descriptor = protoPool.FindMessageTypeByName(channelName);
            if (descriptor == nullptr) {
                std::cerr << "Error: Message descriptor is null" << std::endl;
                continue;
            }

            std::unique_ptr<gp::Message> message(protoFactory.GetPrototype(descriptor)->New());
            if (!message->ParseFromString(received_message)) {
                std::cerr << "Failed to parse received message" << std::endl;
                continue;
            }

            std::cout << "Received message: " << message->DebugString() << std::endl;
        }
    }

    socket.close();
    return 0;
}