#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>
using namespace std;

class Message
{
	private:
		vector<char> message;

	public:
		Message& add_byte(char data);
		Message& add_2byte(short data);
		Message& add_4byte(int data);
		Message& add_string(string data);

		char* construct();

		unsigned int size();
};

#endif
