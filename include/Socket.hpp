#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include "data.pb.h"

class Socket {
private:
    int fd;
    struct sockaddr_in sAddr;
    struct sockaddr_in remaddr;

public:
    Socket(std::string server_ip, uint16_t port);

    void close();
    bool send(data::info message, bool returning);
    bool receive(data::info& message);
    bool bind();
};

#endif