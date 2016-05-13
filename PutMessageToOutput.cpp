#include "PutMessageToOutput.h"

PutMessageToOutput::PutMessageToOutput(int _pipe)
{
	this->recv_pipe = _pipe;
}

void PutMessageToOutput::run()
{
	char buffer[BUF_SIZE + 1];
	ssize_t len;
	while(true)
	{
		len = read(recv_pipe, buffer, BUF_SIZE);
		buffer[len] = '\0';
		std::cout << buffer;
	}
}
