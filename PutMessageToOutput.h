/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "Common.h"

class PutMessageToOutput
{
public:
	PutMessageToOutput(int);
	void run();
private:
	int recv_pipe;
};
