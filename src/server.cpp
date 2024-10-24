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