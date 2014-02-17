#ifndef FLAPPY_SERVER_H
#define FLAPPY_SERVER_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include "Player.h"
#include "Message.h"

using namespace std;

extern "C"
{
	#include <websock/websock.h>
}

#define PIPES_COUNT 200
#define PIPES_DISTANCE 200
#define MIN_PIPE_HOLE_POSITION 50
#define MAX_PIPE_HOLE_POSITION 225

#define MESSAGE_PLAYER 0
#define MESSAGE_UPDATES 2
#define MESSAGE_PIPES 3
#define MESSAGE_NICKNAME_UPDATE 5

#define UPDATE_INTERVAL 10 //In milliseconds

class FlappyServer
{
	private:
		bool running;

		libwebsock_context *context;

		/* Singleton because of C type callbacks */
		static FlappyServer *instance;

		FlappyServer();
		FlappyServer(FlappyServer const&);
		void operator=(FlappyServer const&);

		/* Ugly C style callbacks */
		static int connect_callback(libwebsock_client_state *state);
		static int disconnect_callback(libwebsock_client_state *state);
		static int receive_callback(libwebsock_client_state *state, libwebsock_message *message);		

		void send_message(libwebsock_client_state *state, Message message);

		/* Game related variables */
		map<unsigned int, Player> players;
		vector<unsigned int> disconnected_players; // Buffer for disconnected, that will be removed after their updates are sent

		void remove_disconnected_players();

		Message pipes_message;
		void generate_pipes();

		map<unsigned int, vector<unsigned short>> jump_updates;

		thread update_thread;
		mutex players_mutex;
		void send_updates();		

	public:
		~FlappyServer();

		static FlappyServer* get_instance();

		void listen(string ip, string port);

		void start();
};

#endif
