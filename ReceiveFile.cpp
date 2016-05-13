/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "ReceiveFile.h"

// Constructor : read end pipe (from Input Process)
ReceiveFile::ReceiveFile(int _pipe):recv_pipe(_pipe){}
ReceiveFile::~ReceiveFile(){}

// main function
void ReceiveFile::run()
{
	// ReceiveFile is server side
	// If sender process connect, it receive file.
	int option;
	// create socket
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "** Error : cannot create file server socket\n";
		exit(-1);
	}
//	option = 1;
//	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
//	{
//		std::cerr << "** Error : cannot set reuse sockopt failed(file server)\n";
//		exit(-1);
//	}
	// Set IP & Port
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(FILE_TRANSFER_PORT);
	// Bind socket
	if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "** Error : Cannot bind file server socket\n";
		std::cerr << "** (Maybe other process already use port number 20000)\n";
		exit(-1);
	}
	if(listen(server_socket, 5) < 0)
	{
		std::cerr << "** Error : cannot listen\n";
		exit(-1);
	}
	std::cout << "File Transfer Receive Socket Successfully binded" << std::endl;
	std::cout << "Now you can receive file from another user" << std::endl;
	waitClient();
}

void ReceiveFile::waitClient()
{
	char buffer[BUF_SIZE];
	char *uname;	// filename, username
	ssize_t rlen;
	socklen_t client_addr_len;
	client_addr_len = sizeof(client_addr);
	// if client(sender) try connect, accept.
	if((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) < 0)
	{
		std::cerr << "** Error : accept error\n";
		exit(-1);
	}
	// read fileinfo(filename and username) from client
	if((rlen = read(client_socket, buffer, BUF_SIZE)) < 0)
	{
		std::cerr << "** Error : read(fileinfo from client) error\n";
		exit(-1);
	}
	filename = new char[rlen + 1];
	strncpy(filename, buffer, rlen);
	filename[rlen] = '\0';
	strtok(filename, " ");
	uname = strtok(NULL, " ");
	std::cout << "> " << uname << " want to send file \"" << filename << "\" to you." << std::endl;
	std::cout << "> If you want receive this file, type \"/yes\"" << std::endl;
	do
	{
		sleep(1);
		// check ok sign from input(user) every 1 sec
		if((rlen = read(recv_pipe, buffer, BUF_SIZE)) < 0)
		{
			std::cerr << "** Error : read(ok sign from user) error\n";
			exit(-1);
		}
	}while(rlen == 0);	// if read something, break
	// ok sign is equal to username
	username = new char[rlen];
	strncpy(username, buffer, rlen);
	getFile();
}

void ReceiveFile::getFile()
{
	int recv_fd;
	char buffer[BUF_SIZE];
	ssize_t rlen, wlen;

	// Create & Open file
	if((recv_fd = open(filename, O_WRONLY | O_CREAT, 0644)) < 0)
	{
		std::cerr << "** Error : file open error(receive side)\n";
		exit(-1);
	}

	// Send ok sign to client(sender)
	if((wlen = write(client_socket, username, strlen(username))) < 0)
	{
		std::cerr << "** Error : write(ok sign to client) error\n";
		exit(-1);
	}

	// Receive Data from server & Write Data to file
	while((rlen = read(client_socket, buffer, strlen(buffer))) > 0)
	{
		if((wlen = write(recv_fd, buffer, rlen)) != rlen)
		{
			std::cerr << "** Error : recv(write to file) error\n";
			exit(-1);
		}
		if(rlen < BUF_SIZE) break;	// if rlen < BUF_SIZE, this data is last
	}
	if(rlen < 0)
	{
		std::cerr << "** Error : read error(from client)\n";
		exit(-1);
	}
	std::cout << "> File \"" << filename << "\" has been completely downloaded." << std::endl;
	delete[] username;
	delete[] filename;
	close(recv_fd);
	close(client_socket);
	// Wait for another file
	waitClient();
}
