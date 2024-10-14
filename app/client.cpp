#include <iostream>
#include "Hub.hpp"
#include "data.pb.h"

int main(){
    Hub hub(DEFAULT_IP, DEFAULT_PORT);
    data::info message;
    message.set_sender_name("client");

    hub.send(message, false);

    while(!hub.receive(message));

    std::cout << message.sender_name() << std::endl;

    hub.close();

    return 0;
}
