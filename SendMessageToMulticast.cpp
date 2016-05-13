/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "SendMessageToMulticast.h"

SendMessageToMulticast::SendMessageToMulticast(int _pipe, char* _ip, char* _port) : recv_pipe(_pipe)
{
	memset(&multicast_group, 0, sizeof(multicast_group));
	multicast_group.sin_family = AF_INET;
	multicast_group.sin_port = htons(atoi(_port));
	inet_pton(AF_INET, _ip, &multicast_group.sin_addr);
	if((send_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		std::cerr << "** Error : cannot create send socket !\n";
		exit(-1);
	}
	std::cout << "Send Socket Successfully Set\n";
}

SendMessageToMulticast::~SendMessageToMulticast()
{
	close(send_socket);
}

// main function
void SendMessageToMulticast::run()
{
	char buffer[BUF_SIZE];
	ssize_t rlen;
	while(true)
	{
		// Read from Input Process
		rlen = read(recv_pipe, buffer, BUF_SIZE);
		buffer[rlen]='\0';
		if(sendto(send_socket, buffer, strlen(buffer), 0, (struct sockaddr*)&multicast_group, sizeof(multicast_group)) < rlen)
		{
			std::cerr << "** Error : cannot send\n";
			exit(-1);
		}
	}
}
