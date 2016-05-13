#include "Common.h"

class SendMessageToMulticast
{
public:
	SendMessageToMulticast(int, char*, char*);
	void run();
private:
	struct sockaddr_in multicast_group;
	int send_socket;
	int recv_pipe;
};
