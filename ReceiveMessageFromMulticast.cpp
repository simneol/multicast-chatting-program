#include "ReceiveMessageFromMulticast.h"

ReceiveMessageFromMulticast::ReceiveMessageFromMulticast(int _pipe, char* _ip, char* _port)
{
	this->send_pipe = _pipe;
	struct sockaddr_in multicast_group;
	struct ip_mreq mreq;
	unsigned int yes = 1;

	memset(&multicast_group, 0, sizeof(multicast_group));
	multicast_group.sin_family = AF_INET;
	multicast_group.sin_port = htons(atoi(_port));
	inet_pton(AF_INET, _ip, &multicast_group.sin_addr);

	mreq.imr_multiaddr = multicast_group.sin_addr;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if((recv_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		std::cerr << "Cannot create receive socket";
		exit(-1);
	}
	if(setsockopt(recv_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		std::cerr << "Cannot add membership";
		exit(-1);
	}
	if(setsockopt(recv_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) // yes???
	{
		std::cerr << "Cannot set reuse socket option";
		exit(-1);
	}
	if(bind(recv_socket, (struct sockaddr*)&multicast_group, sizeof(multicast_group)) < 0)
	{
		std::cerr << "Cannot bind receive socket";
		exit(-1);
	}
	std::cout << "Receive Socket Successfully Set\n";
}

void ReceiveMessageFromMulticast::run()
{
	size_t n;
	char buffer[BUF_SIZE + 1];
	socklen_t len;
	struct sockaddr_in from;
	while(true)
	{
		char* buffer_with_ip = new char[BUF_SIZE + 20];
		len = sizeof(from);
		if((n = recvfrom(recv_socket, buffer, BUF_SIZE, 0, (struct sockaddr*)&from, &len)) < 0)
		{
			std::cerr << "Failed to receive data";
			exit(-1);
		}
		buffer[n] = '\0';
		sprintf(buffer_with_ip, "[%s]%s", inet_ntoa(from.sin_addr), buffer);
		write(send_pipe, buffer_with_ip, strlen(buffer_with_ip));
	}
}
