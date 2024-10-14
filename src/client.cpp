#include <iostream>
#include "socket.hpp"
#include "data.pb.h"

int main(){
    Socket socket(DEFAULT_IP, DEFAULT_PORT);
    data::info message;
    message.set_sender_name("client");

    socket.send(message, false);

    while(!socket.receive(message));

    std::cout << message.sender_name() << std::endl;

    socket.close();

    return 0;
}
