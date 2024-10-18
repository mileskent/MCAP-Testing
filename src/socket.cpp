#include <iostream>
#include <cstring>
#include "socket.hpp"

Socket::Socket(std::string server_ip, uint16_t port){
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		exit(1);
	}

	memset((char *)&sAddr, 0, sizeof(sAddr));
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, server_ip.c_str(), &sAddr.sin_addr);
	sAddr.sin_port = htons(port);
}

bool Socket::send(const std::string& serialized, bool returning){
	if(returning){
		if (sendto(fd, serialized.data(), serialized.size(), 0, (struct sockaddr *)&remaddr, sizeof(sAddr)) < 0){
			return false;
		}
	}
	else{
		if (sendto(fd, serialized.data(), serialized.size(), 0, (struct sockaddr *)&sAddr, sizeof(sAddr)) < 0){
			return false;
		}
	}
	
	return true;
}

bool Socket::receive(std::string& serialized){
	const int bufferSize = 1024;
    socklen_t addrlen = sizeof(remaddr);

    unsigned char buf[bufferSize];  

    int recvlen = recvfrom(fd, buf, bufferSize, 0, (struct sockaddr *)&remaddr, &addrlen);
    if(recvlen < 0){
        return false;
    } else {
        std::cout << "recvlen: "<< recvlen << std::endl;
    }

    serialized.assign(reinterpret_cast<char*>(buf), recvlen);
    
    return true;
}


void Socket::close(){
	::close(fd);
}

bool Socket::bind(){
	if (::bind(fd, (struct sockaddr *)&sAddr, sizeof(sAddr)) < 0) {
		return false;
	}
	return true;
}
