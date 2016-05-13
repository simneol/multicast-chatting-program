/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

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

	// Create pipe for IPC
	// Input Process <-> Send Process
	// Output Process <-> Receive Process
	if(pipe(pipes) == -1)
	{
		std::cerr << "** Error : pipe error!\n";
		exit(-1);
	}

	// Child Process (Output, Receive)
	if(pid == 0)
	{
		// Create Process
		// Output Process & Receive Process
		if((pid = fork()) < 0)
		{
			std::cerr << "fork error!";
			exit(-1);
		}
		// Child of Child Process (Output)
		if(pid == 0)
		{
			// Give read end pipe (from Receive Process)
			PutMessageToOutput output(pipes[0]);
			output.run();
		}
		// Parent of Child Process (Receive)
		else
		{
			// Give write end pipe (to Output Proces)
			ReceiveMessageFromMulticast receive_from(pipes[1], argv[1], argv[2]);
			receive_from.run();
		}
	}
	// Parent Process (Input, Send)
	else
	{
		// Create Process
		// Input Process & Send Process
		if((pid = fork()) < 0)
		{
			std::cerr << "fork error!";
			exit(-1);
		}
		// Child of Parent Process (Input)
		if(pid == 0)
		{
			// Give write end pipe (to Send Process) and username
			GetMessageFromInput input(pipes[1], argv[3]);
			input.run();
		}
		// Parent of Parent Process (Send)
		else
		{
			// Give read end pipe (from Input Process), IP and Port
			SendMessageToMulticast send_to(pipes[0], argv[1], argv[2]);
			send_to.run();
		}
	}
	return 0;
}
