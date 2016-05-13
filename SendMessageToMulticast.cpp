#include "SendMessageToMulticast.h"

SendMessageToMulticast::SendMessageToMulticast(int _pipe, char* _ip, char* _port)
{
	this->recv_pipe = _pipe;
	memset(&multicast_group, 0, sizeof(multicast_group));
	multicast_group.sin_family = AF_INET;
	multicast_group.sin_port = htons(atoi(_port));
	inet_pton(AF_INET, _ip, &multicast_group.sin_addr);
	if((send_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		std::cerr << "Cannot create send socket !";
		exit(-1);
	}
	std::cout << "Send Socket Successfully Set\n";
}
// Destructor / close(send_socket);
void SendMessageToMulticast::run()
{
	char buffer[BUF_SIZE];
	//ssize_t len;
	socklen_t len;
	while(true)//fgets(buffer, BUF_SIZE, stdin) != NULL)
	{
		//len = strlen(buffer);
		len = read(recv_pipe, buffer, BUF_SIZE);
		buffer[len]='\0';
		if(sendto(send_socket, buffer, strlen(buffer), 0, (struct sockaddr*)&multicast_group, sizeof(multicast_group)) < len)
		{
			std::cerr << "Cannot send";
			exit(-1);
		}
	}
}
