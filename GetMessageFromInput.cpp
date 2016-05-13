/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "GetMessageFromInput.h"

// Constructor : write end pipe (to Send Process)
GetMessageFromInput::GetMessageFromInput(int _pipe, char* _username) : send_pipe(_pipe)
{
	this->username = new char[strlen(_username)];
	strcpy(this->username, _username);
}

GetMessageFromInput::~GetMessageFromInput()
{
	// close pipe & return memory
	close(send_pipe);
	delete[] username;
}

// main function
void GetMessageFromInput::run()
{
	int pipes[2];
	char message[BUF_SIZE];
	pid_t pid;
	ssize_t wlen;
	// Create pipe for IPC
	// Input Process <-> Receive File Process
	if(pipe(pipes) < 0)
	{
		std::cerr << "** Error : pipe error!\n";
		exit(-1);
	}

	// Create Process
	// Input Process & Receive File Process
	if((pid = fork()) < 0)
	{
		std::cerr << "** Error : fork error!\n";
		exit(-1);
	}

	// Child Process (Receive File)
	if(pid == 0)
	{
		// Give read end pipe
		ReceiveFile recvfile(pipes[0]);
		recvfile.run();
		// Prevent role duplication (two of input processes)
		return;
	}

	// Input from standard input(user)
	while(fgets(message, BUF_SIZE, stdin) != NULL)
	{
		char *named_buffer = new char[BUF_SIZE + strlen(username) + 5];
		// If input is command (start with '/')
		if(message[0] == '/')
		{
			char *filename, *ip;
			if(strncmp(message, "/sendfile", 9) == 0)
			{
				// tokenize message
				strtok(message, " ");		// "/sendfile"
				filename = strtok(NULL, " ");		// "<filename>"
				ip = strtok(NULL, " ");		// "<IP>"
				if(filename == NULL || ip == NULL)
				{
					std::cout << " Usage : /sendfile <FileName> <IP>\n";
					continue;
				}

				// Create Process
				// Send File Process & Input Process
				if((pid = fork()) < 0)
				{
					std::cerr << "** Error : fork error!\n";
					exit(-1);
				}
				// Child Process (Send File)
				if(pid == 0)
				{
					SendFile sendfile(filename, username, ip);
					sendfile.run();
					// after send, it will return to end.
					return;
				}
			}
			else if(strncmp(message, "/yes", 4) == 0)
				// It send ok sign to Receive File Process to receive file
				write(pipes[1], username, strlen(username));
			else
				std::cout << "Invalid Command !\n";
			continue;
		}
		// named_buffer forat : " username : message"
		sprintf(named_buffer, " %s : %s", username, message);
		if((wlen = write(send_pipe, named_buffer, strlen(named_buffer))) < 0)
		{
				std::cerr << "** Error : cannot write to send process\n";
				exit(-1);
		}
		delete[] named_buffer;
		// initialize message buffer
		// if do not this action, buffer may be like below
		// 1. User input 'abcdefghij'
		//  -> buffer : 'abcdefghij'
		// 2. User input '1'
		//  -> buffer : '1 cdefghij'
		// It could be problem.
		memset(message, '\0', sizeof(message));
	}
}
