#include <iostream>
#include "Socket.hpp"
#include "data.pb.h"

int main(){
    Socket socket(DEFAULT_IP, DEFAULT_PORT);
    Socket.bind();
    
    while(true){
        data::info message;
        if(socket.receive(message)){
            std::cout << message.sender_name() << std::endl;
            message.set_sender_name("server");
            if(!socket.send(message, true)){
                std::cout << "server did not send!?" << std::endl;
                return 1;
            }
        }
    }

    socket.close();

    return 0;
}