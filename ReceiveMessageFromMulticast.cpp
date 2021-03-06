/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "ReceiveMessageFromMulticast.h"

// Constructor : write end pipe (to Output Process)
ReceiveMessageFromMulticast::ReceiveMessageFromMulticast(int _pipe, char* _ip, char* _port) : send_pipe(_pipe)
{
	struct sockaddr_in multicast_group;
	struct ip_mreq mreq;
	unsigned int yes = 1;

	// Set multicast group info
	memset(&multicast_group, 0, sizeof(multicast_group));
	multicast_group.sin_family = AF_INET;
	multicast_group.sin_port = htons(atoi(_port));
	inet_pton(AF_INET, _ip, &multicast_group.sin_addr);

	mreq.imr_multiaddr = multicast_group.sin_addr;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	// Create UDP socket
	if((recv_socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		std::cerr << "** Error : cannot create receive socket\n";
		exit(-1);
	}
	// Add membership (join multicast group)
	if(setsockopt(recv_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		std::cerr << "** Error : cannot add membership\n";
		exit(-1);
	}
	// Set socket reuse option
	if(setsockopt(recv_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) // yes???
	{
		std::cerr << "** Error : cannot set reuse socket option\n";
		exit(-1);
	}
	// bind
	if(bind(recv_socket, (struct sockaddr*)&multicast_group, sizeof(multicast_group)) < 0)
	{
		std::cerr << "** Error : cannot bind receive socket\n";
		exit(-1);
	}
}

ReceiveMessageFromMulticast::~ReceiveMessageFromMulticast()
{
	close(recv_socket);
}

// main function
void ReceiveMessageFromMulticast::run()
{
	size_t n;
	char buffer[BUF_SIZE + 1];
	socklen_t len;
	struct sockaddr_in sender_addr;
	while(true)
	{
		char* buffer_with_ip = new char[BUF_SIZE + 20];
		len = sizeof(sender_addr);
		// Receive from multicast
		if((n = recvfrom(recv_socket, buffer, BUF_SIZE, 0, (struct sockaddr*)&sender_addr, &len)) < 0)
		{
			std::cerr << "** Error : failed to receive data\n";
			exit(-1);
		}
		buffer[n] = '\0';
		// Add ip to message
		sprintf(buffer_with_ip, "[%s]%s", inet_ntoa(sender_addr.sin_addr), buffer);
		// Send to Ouput Process
		write(send_pipe, buffer_with_ip, strlen(buffer_with_ip));
	}
}
