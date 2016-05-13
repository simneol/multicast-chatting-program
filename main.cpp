#include "Common.h"
#include "GetMessageFromInput.h"
#include "SendMessageToMulticast.h"
#include "ReceiveMessageFromMulticast.h"
#include "PutMessageToOutput.h"

int main(int argc, char** argv)
{
	if(argc != 4)
	{
		std::cout << "Argument usage : <IP> <Port> <Name>" << std::endl;
		exit(-1);
	}
	int pipes[2];
	pid_t pid;
	std::cout << "=====================================================" << std::endl;
	std::cout << "Enjoy multicast chat program !" << std::endl;
	std::cout << "You can send message by press enter !" << std::endl;
	std::cout << "And You can send file by using below command " << std::endl;
	std::cout << " * Send file : type \"/sendfile <FilePath> <IP>\"" << std::endl;
	std::cout << "=====================================================" << std::endl;

	// Create process for divide into
	// Input, Send Process & Ouput, Receive Process
	if((pid = fork()) < 0)
	{
		std::cerr << "** Error : fork error!\n";
		exit(-1);
	}
	if(pipe(pipes) == -1)
	{
		std::cerr << "** Error : pipe error!\n";
		exit(-1);
	}

	if(pid == 0)
	{
		if((pid = fork()) < 0)
		{
			std::cerr << "fork error!";
			exit(-1);
		}
		if(pid == 0)
		{
			PutMessageToOutput output(pipes[0]);
			output.run();
		}
		else
		{
			ReceiveMessageFromMulticast recv_from(pipes[1], argv[1], argv[2]);
			recv_from.run();
		}
	}
	else
	{
		if((pid = fork()) < 0)
		{
			std::cerr << "fork error!";
			exit(-1);
		}
		if(pid == 0)
		{
			GetMessageFromInput input(pipes[1], argv[3]);
			input.run();
		}
		else
		{
			SendMessageToMulticast send_to(pipes[0], argv[1], argv[2]);
			send_to.run();
		}
	}
	return 0;
}
