#include <iostream>
#include "Hub.hpp"
#include "data.pb.h"

int main(){
    Hub hub(DEFAULT_IP, DEFAULT_PORT);
    hub.bind();
    
    while (true) {
        data::info message;
        if(hub.receive(message)){
            std::cout << message.sender_name() << std::endl;
            message.set_sender_name("server");
            if(!hub.send(message, true)){
                return 1;
            }
        }
    }
    hub.close();
    return 0;
}