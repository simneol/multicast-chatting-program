#include "ReceiveFile.h"

ReceiveFile::ReceiveFile(int _pipe):recv_pipe(_pipe){}
ReceiveFile::~ReceiveFile(){}

void ReceiveFile::run()
{
	int option;
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Cannot create file server socket";
		exit(-1);
	}
	option = 1;
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
	{
		std::cerr << "Set reuse sockopt failed(file server)";
		exit(-1);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(FILE_TRANSFER_PORT);
	if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "** Error : Cannot bind file server socket\n";
		std::cerr << "** (Maybe other process already use port number 20000)\n";
		exit(-1);
	}
	if(listen(server_socket, 5) < 0)
	{
		std::cerr << "Cannot listen";
		exit(-1);
	}
	std::cout << "File Transfer Receive Socket Successfully binded" << std::endl;
	std::cout << "Now you can receive file from another user" << std::endl;
	waitClient();
}

void ReceiveFile::waitClient()
{
	char buffer[BUF_SIZE];
	char *fname, *uname;
	ssize_t rlen;
	socklen_t client_addr_len;
	client_addr_len = sizeof(client_addr);
	if((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) < 0)
	{
		std::cerr << "Accept Error";
		exit(-1);
	}
	if((rlen = read(client_socket, buffer, BUF_SIZE)) < 0)
	{
		std::cerr << "read(filename from client) error";
		exit(-1);
	}
	filename = new char[rlen + 1]; // must delete !(deny)
	strncpy(filename, buffer, rlen);
	filename[rlen] = '\0';
	fname = strtok(filename, " ");
	uname = strtok(NULL, " ");
	std::cout << "> "<< uname << " want to send file \"" << filename << "\" to you." << std::endl;
	std::cout << "> If you want receive this file, type \"/yes\"" << std::endl;
	do
	{
		sleep(1);
		if((rlen = read(recv_pipe, buffer, BUF_SIZE)) < 0)
		{
			std::cerr << "read(ok sign from user) error";
			exit(-1);
		}
	}while(rlen == 0);
	username = new char[rlen];
	strncpy(username, buffer, rlen);
	getFile();
}

void ReceiveFile::getFile()
{
	int recv_fd;
	char buffer[BUF_SIZE];
	ssize_t rlen, wlen;
	if((recv_fd = open(filename, O_WRONLY | O_CREAT, 0644)) < 0)
	{
		std::cerr << "file open error(receive side)";
		exit(-1);
	}
	if((wlen = write(client_socket, username, strlen(username))) < 0)
	{
		std::cerr << "write(ok sign to client) error";
		exit(-1);
	}
	while((rlen = read(client_socket, buffer, strlen(buffer))) > 0)
	{
		if((wlen = write(recv_fd, buffer, rlen)) != rlen)
		{
			std::cerr << "recv(write to file) error";
			exit(-1);
		}
		if(rlen < BUF_SIZE) break;
	}
	if(rlen < 0)
	{
		std::cerr << "read error(from client)";
		exit(-1);
	}
	std::cout << "> File \"" << filename << "\" has been completely downloaded." << std::endl;
	delete[] username;
	delete[] filename;
	close(recv_fd);
	close(client_socket);
//	waitClient();
}
