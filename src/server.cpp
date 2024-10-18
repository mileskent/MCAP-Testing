#include <iostream>
#include "socket.hpp"
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection.h>


namespace gp = google::protobuf;

int main() {
    Socket socket(DEFAULT_IP, DEFAULT_PORT);
    socket.bind();

    // Assuming you already have the Protobuf Descriptor setup
    gp::SimpleDescriptorDatabase protoDb;
    gp::DescriptorPool protoPool(&protoDb);
    gp::DynamicMessageFactory protoFactory(&protoPool);

    while (true) {
        std::string received_message;
        if (socket.receive(received_message)) {
            const gp::Descriptor* descriptor = protoPool.FindMessageTypeByName("mcu_pedal_readings");
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


// int main(){
//     Socket socket(DEFAULT_IP, DEFAULT_PORT);
//     socket.bind();
    
//     while (true) {
//         data::info message;
//         if(socket.receive(message)){
//             std::cout << message.sender_name() << std::endl;
//             message.set_sender_name("server");
//             if(!socket.send(message, true)){
//                 return 1;
//             }
//         }
//     }
//     socket.close();
//     return 0;
// }