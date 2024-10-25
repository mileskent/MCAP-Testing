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

int main() {
    Socket socket(DEFAULT_IP, DEFAULT_PORT);
    socket.bind();

    const char *inputFilename = "testdata.mcap"; // TODO: shouldn't be hardcoded
    const std::string channelName = "mcu_error_states_data"; // TODO shouldn't be hardcoded
    const std::string schemaName = "mcu_error_states"; // TODO shouldn't be hardcoded

    mcap::McapReader reader;
    const auto res = reader.open(inputFilename);
    if (!res.ok()) {
        return 1;
    }

    auto messageView = reader.readMessages();
    gp::SimpleDescriptorDatabase protoDb;
    gp::DescriptorPool protoPool(&protoDb);
    gp::DynamicMessageFactory protoFactory(&protoPool);
    reader.close();

    // Server Loop
    while (true) {
        std::string received_message;
        if (socket.receive(received_message)) {
            const gp::Descriptor* descriptor = protoPool.FindMessageTypeByName(schemaName);
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