/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "SendFile.h"

SendFile::SendFile(char *fname, char *uname, char *_ip)
{
	filename = new char[strlen(fname)];
	strcpy(filename, fname);
	username = new char[strlen(uname)];
	strcpy(username, uname);
	ip = new char[strlen(_ip)];
	strcpy(ip, _ip);
}

SendFile::~SendFile()
{
	delete[] filename;
	delete[] username;
	delete[] ip;
}

// main function
void SendFile::run()
{
	char buffer[BUF_SIZE];
	ssize_t rlen, wlen;
	struct stat f_stat;
	mode_t f_mode;
	// Fileinfo inclue filename, sender's name
	char *fileInfo = new char[strlen(filename) + strlen(username) + 2];
	sprintf(fileInfo, "%s %s", filename, username);

	// file check
	if(stat(filename, &f_stat) < 0)
	{
		std::cerr << "** Error : cannot access file !\n";
		exit(-1);
	}
	f_mode = f_stat.st_mode;
	if(!S_ISREG(f_mode))
	{
		std::cerr << "** Error : " << filename << " is not regular file !\n";
		exit(-1);
	}

	// create socket
	if((client_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "** Error : cannot create file client socket\n";
		exit(-1);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(FILE_TRANSFER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(ip);
	if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "** Error : cannot connect to server\n";
		exit(-1);
	}
	if((wlen = write(client_socket, fileInfo, strlen(fileInfo))) < 0)
	{
		std::cerr << "** Error : write(fileInfo to server) error\n";
		exit(-1);
	}
	std::cout << "> File transfer request has been sent" << std::endl;
	do
	{
		sleep(1);
		if((rlen = read(client_socket, buffer, BUF_SIZE)) < 0)
		{
			std::cerr << "** Error : read(username from server) error\n";
			exit(-1);
		}
	}while(rlen == 0);
	recv_name = new char[rlen];
	strncpy(recv_name, buffer, rlen);
	std::cout << "> " << recv_name << " accept your file transfer !" << std::endl;
	delete[] fileInfo;
	transferFile();
}

void SendFile::transferFile()
{
	int send_fd;
	char buffer[BUF_SIZE];
	ssize_t rlen, wlen;
	if((send_fd = open(filename, O_RDONLY)) < 0)
	{
		std::cerr << "** Error : file open error\n";
		exit(-1);
	}
	std::cout << "> Start file transfering..." << std::endl;

	// Read Data from file & Send Data to server
	while((rlen = read(send_fd, buffer, BUF_SIZE)) > 0)
	{
		if((wlen = write(client_socket, buffer, rlen)) != rlen)
		{
			std::cerr << "** Error : send(write to socket) error\n";
			exit(-1);
		}
	}
	if(rlen < 0)
	{
		std::cerr << "** Error : read error\n";
		exit(-1);
	}
	std::cout << "> Your file \"" << filename << "\" has been sent to " << recv_name << std::endl;
	close(send_fd);
	close(client_socket);
}
