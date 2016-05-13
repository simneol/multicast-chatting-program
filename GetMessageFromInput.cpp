#include "GetMessageFromInput.h"

GetMessageFromInput::GetMessageFromInput(int _pipe, char* _username)
{
	this->send_pipe = _pipe;
	this->username = new char[strlen(_username)];
	strcpy(this->username, _username);
}

GetMessageFromInput::~GetMessageFromInput()
{
	close(send_pipe);
	delete[] username;
}

void GetMessageFromInput::run()
{
	int pipes[2];
	char message[BUF_SIZE];
	pid_t pid;
	if(pipe(pipes) < 0)
	{
		std::cerr << "pipe error!";
		exit(-1);
	}
	if((pid = fork()) < 0)
	{
		std::cerr << "fork error!";
		exit(-1);
	}
	if(pid ==0)
	{
		ReceiveFile recvfile(pipes[0]);
		recvfile.run();
		return;
	}
	while(fgets(message, BUF_SIZE, stdin) != NULL)
	{
		char *named_buffer = new char[BUF_SIZE + strlen(username) + 5];
		if(message[0] == '/')
		{
			char *filename, *ip;
			if(strncmp(message, "/sendfile", 9) == 0)
			{
				strtok(message, " ");
				filename = strtok(NULL, " ");
				ip = strtok(NULL, " ");
				if(filename == NULL || ip == NULL)
				{
					std::cout << " Usage : /sendfile <FileName> <IP>\n";
					continue;
				}
				if((pid = fork()) < 0)
				{
					std::cerr << "fork error!";
					exit(-1);
				}
				if(pid == 0)
				{
					SendFile sendfile(filename, username, ip);
					sendfile.run();
					return;
				}
				continue;
			}
			if(strncmp(message, "/yes", 4) == 0)
			{
				write(pipes[1], username, strlen(username));
				continue;
			}
			else
			{
				std::cout << "Invalid Command !\n";
				continue;
			}
		}
		sprintf(named_buffer, " %s : %s", username, message);
		//std::cout << named_buffer << " // size: "<< strlen(named_buffer)<<std::endl;
		write(send_pipe, named_buffer, strlen(named_buffer));
		delete[] named_buffer;
		memset(message, '\0', sizeof(message));
		// need error handler
	}
}
