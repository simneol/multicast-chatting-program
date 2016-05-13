#include "Common.h"

class SendFile
{
public:
	SendFile(char*, char*, char*);
	~SendFile();
	void run();
private:
	char *filename, *username, *ip, *recv_name;
	int client_socket;
	struct sockaddr_in server_addr;
	void transferFile();
};
