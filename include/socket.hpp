#ifndef HUB_HPP 
#define HUB_HPP 

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include "data.pb.h"

static const std::string DEFAULT_IP = "127.0.0.1";
static const uint16_t DEFAULT_PORT = 1153;

class Socket {
private:
    int fd;
    struct sockaddr_in sAddr;
    struct sockaddr_in remaddr;

public:
    Socket (std::string server_ip, uint16_t port);

    void close();
    bool send (const std::string&, bool returning);
    bool receive (std::string&);
    bool bind();
};

#endif