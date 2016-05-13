#include "SendFile.h"

SendFile::SendFile(char *fname, char *uname, char *_ip)
{
//	fileInfo = new char[strlen(fname) + strlen(uname) + 2];
//	sprintf(fileInfo, "%s %s", fname, uname);
	filename = new char[strlen(fname)];
	strcpy(filename, fname);
	username = new char[strlen(uname)];
	strcpy(username, uname);
	ip = new char[strlen(_ip)];
	strcpy(ip, _ip);
}

SendFile::~SendFile()
{
//	delete[] fileInfo;
	delete[] filename;
	delete[] username;
	delete[] ip;
}

void SendFile::run()
{
	char buffer[BUF_SIZE];
	ssize_t rlen, wlen;
	struct stat f_stat;
	mode_t f_mode;
	char *fileInfo = new char[strlen(filename) + strlen(username) + 2];
	sprintf(fileInfo, "%s %s", filename, username);
	if(stat(filename, &f_stat) < 0)
	{
		std::cerr << "Cannot access file !\n";
		exit(-1);
	}
	f_mode = f_stat.st_mode;
	if(!S_ISREG(f_mode))
	{
		std::cerr << filename << " is not regular file !\n";
		exit(-1);
	}
	if((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Cannot create file client socket";
		exit(-1);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(FILE_TRANSFER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(ip);
	if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "Cannot connect to server";
		exit(-1);
	}
	if((wlen = write(client_socket, fileInfo, strlen(fileInfo))) < 0)
	{
		std::cerr << "write(fileInfo to server) error";
		exit(-1);
	}
	std::cout << "> File transfer request has been sent" << std::endl;
	do
	{
		sleep(1);
		if((rlen = read(client_socket, buffer, BUF_SIZE)) < 0)
		{
			std::cerr << "read(username from server) error";
			exit(-1);
		}
	}while(rlen == 0);
	recv_name = new char[rlen];
	strncpy(recv_name, buffer, rlen);
	std::cout << "> " << recv_name << " accept your file transfer !" << std::endl;
	delete[] fileInfo;
//	if(strncmp(buffer, "ok", 2) == 0)
		transferFile();
//	else
//		std::cout << "Transfer denied by receiver\n";
}

void SendFile::transferFile()
{
	int send_fd;
	char buffer[BUF_SIZE];
	ssize_t rlen, wlen;
	if((send_fd = open(filename, O_RDONLY)) < 0)
	{
		std::cerr << "file open error";
		exit(-1);
	}
	std::cout << "> Start file transfering..." << std::endl;
	while((rlen = read(send_fd, buffer, BUF_SIZE)) > 0)
	{
		if((wlen = write(client_socket, buffer, rlen)) != rlen)
		{
			std::cerr << "send(write to socket) error";
			exit(-1);
		}
	}
	if(rlen < 0)
	{
		std::cerr << "read error";
		exit(-1);
	}
	std::cout << "> Your file \"" << filename << "\" has been sent to " << recv_name << std::endl;
	close(send_fd);
	close(client_socket);
}
