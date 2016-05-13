/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "Common.h"

class ReceiveMessageFromMulticast
{
public:
	ReceiveMessageFromMulticast(int, char*, char*);
	~ReceiveMessageFromMulticast();
	void run();
private:
	int send_pipe;
	int recv_socket;
};
