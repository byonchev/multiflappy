#include <iostream>
#include "FlappyServer.h"

using namespace std;

int main()
{
	FlappyServer *server = FlappyServer::get_instance();

	server->listen("0.0.0.0", "8080");
	server->start();

	delete server;
	cout << endl << "Server has gone offline" << endl;

	return 0;	
}

