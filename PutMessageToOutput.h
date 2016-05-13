#include "Common.h"

class PutMessageToOutput
{
public:
	PutMessageToOutput(int);
	void run();
private:
	int recv_pipe;
};
