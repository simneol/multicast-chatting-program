#include "Common.h"

class ReceiveFile
{
public:
	ReceiveFile(int);
	~ReceiveFile();
	void run();
private:
	int recv_pipe;
	int client_socket;
	int server_socket;
	char *filename, *username;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	void waitClient();
	void getFile();
};
