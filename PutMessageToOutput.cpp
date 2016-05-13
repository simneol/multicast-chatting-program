/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "PutMessageToOutput.h"

// Constructor : read end pipe (from Receive Process)
PutMessageToOutput::PutMessageToOutput(int _pipe) : recv_pipe(_pipe){}

void PutMessageToOutput::run()
{
	char buffer[BUF_SIZE + 1];
	ssize_t rlen;
	while(true)
	{
		if((rlen = read(recv_pipe, buffer, BUF_SIZE)) < 0)
		{
			std::cerr << "** Error : cannot read from Recive Process\n";
			exit(-1);
		}
		buffer[rlen] = '\0';
		std::cout << buffer;
	}
}
