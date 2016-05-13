#include "Common.h"

class ReceiveMessageFromMulticast
{
public:
	ReceiveMessageFromMulticast(int, char*, char*);
	void run();
private:
	int send_pipe;
	int recv_socket;
};
