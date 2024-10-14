#include <iostream>
#include "socket.hpp"
#include "data.pb.h"

int main(){
    Socket socket(DEFAULT_IP, DEFAULT_PORT);
    socket.bind();
    
    while (true) {
        data::info message;
        if(socket.receive(message)){
            std::cout << message.sender_name() << std::endl;
            message.set_sender_name("server");
            if(!socket.send(message, true)){
                return 1;
            }
        }
    }
    socket.close();
    return 0;
}