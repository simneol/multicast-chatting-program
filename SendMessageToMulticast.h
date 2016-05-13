/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "Common.h"

class SendMessageToMulticast
{
public:
	SendMessageToMulticast(int, char*, char*);
	~SendMessageToMulticast();
	void run();
private:
	struct sockaddr_in multicast_group;
	int send_socket;
	int recv_pipe;
};
