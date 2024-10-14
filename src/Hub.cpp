#include <iostream>
#include <cstring>
#include "Hub.hpp"

Hub::Hub(std::string server_ip, uint16_t port){
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		exit(1);
	}

	memset((char *)&sAddr, 0, sizeof(sAddr));
	sAddr.sin_family = AF_INET;
	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, server_ip.c_str(), &sAddr.sin_addr);
	sAddr.sin_port = htons(port);
}

bool Hub::send(data::info message, bool returning){
	std::string serialized;
	if(!message.SerializeToString(&serialized)){
		return false;
	}

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

bool Hub::receive(data::info& message){
	const int bufferSize = 1024;
	socklen_t addrlen = sizeof(remaddr);      

	unsigned char buf[bufferSize];  

	int recvlen = recvfrom(fd, buf, bufferSize, 0, (struct sockaddr *)&remaddr, &addrlen);
	if(recvlen < 0){
		return false;
	} else {
		std::cout << "recvlen: "<< recvlen << std::endl;
	}

	if (!message.ParseFromArray(buf, recvlen)) {
		return false;
	}

	return true;
}

void Hub::close(){
	::close(fd);
}

bool Hub::bind(){
	if (::bind(fd, (struct sockaddr *)&sAddr, sizeof(sAddr)) < 0) {
		return false;
	}
	return true;
}
