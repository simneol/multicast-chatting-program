/**********************************
 * Author : Jeong minhyeok (정민혁)
 * Date : 2016-05-14
 **********************************/

#include "Common.h"
#include "SendFile.h"
#include "ReceiveFile.h"

class GetMessageFromInput
{
public:
	GetMessageFromInput(int, char*);
	~GetMessageFromInput();
	void run();
private:
	int send_pipe;
	char* username;
};
