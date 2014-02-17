#include "Message.h"

Message& Message::add_byte(char data)
{
	message.push_back(data);
	return *this;
}

Message& Message::add_2byte(short data)
{
	message.insert(message.end(), reinterpret_cast<char*>(&data), reinterpret_cast<char*>(&data)+2);
	return *this;
}

Message& Message::add_4byte(int data)
{
	message.insert(message.end(), reinterpret_cast<char*>(&data), reinterpret_cast<char*>(&data)+4);
	return *this;
}

Message& Message::add_string(string data)
{
	message.insert(message.end(), data.c_str(), data.c_str() + data.size()+1);
	return *this;
}

char* Message::construct()
{
	return message.data();
}

unsigned int Message::size()
{
	return message.size();
}
