#include <ctime>
#include <chrono>
#include "FlappyServer.h"

FlappyServer* FlappyServer::instance;

FlappyServer::FlappyServer()
: running(false)
{}

FlappyServer::~FlappyServer()
{}

FlappyServer* FlappyServer::get_instance()
{
	if (instance == NULL)
	{
		instance = new FlappyServer();		
	}

	return instance;
}

void FlappyServer::listen(string ip, string port)
{
	context = libwebsock_init();

	if (context != NULL)
	{
		libwebsock_bind(context, (char*)ip.c_str(), (char*)port.c_str());
		cout << "Listening on " << ip << ":" << port << endl;

		context->onmessage = receive_callback;
		context->onopen = connect_callback;
		context->onclose = disconnect_callback;
	}
	else
	{
		cerr << "Error during socket initialization" << endl;
	}
}

int FlappyServer::connect_callback(libwebsock_client_state *state)
{
	FlappyServer *instance = get_instance();

	cout << "New player connected (socket descriptor " << state->sockfd << ")" << endl;

	instance->send_message(state, instance->pipes_message);

	instance->send_message(state, Message().add_byte(MESSAGE_PLAYER).add_4byte(state->sockfd));

	instance->players_mutex.lock();
	instance->players[state->sockfd] = Player();
	instance->players_mutex.unlock();

	return 0;
}

int FlappyServer::disconnect_callback(libwebsock_client_state *state)
{
	FlappyServer *instance = get_instance();

	cout << "Player \"" << instance->players[state->sockfd].get_nickname() << "\" disconnected" << endl;

    if(state->close_info)
	{
		cout << "- Code: " << state->close_info->code << endl << "- Reason: " << state->close_info->reason << endl;
    }

	instance->players_mutex.lock();
	instance->disconnected_players.push_back(state->sockfd);
	instance->players_mutex.unlock();

	return 0;
}

int FlappyServer::receive_callback(libwebsock_client_state *state, libwebsock_message *message)
{
	FlappyServer *instance = get_instance();

	Player &player = instance->players[state->sockfd];

	switch(message->payload[0])
	{
		case MESSAGE_NICKNAME_UPDATE:
		{
			player.set_nickname(string(message->payload+1));

			break;
		}

		case MESSAGE_UPDATES:
		{
			instance->players_mutex.lock();
			int jumps_count = *(reinterpret_cast<unsigned short*>(message->payload+1));
			vector<unsigned short> jumps;
		
			for (int i = 0; i < jumps_count; i++)
			{
				jumps.push_back(*(reinterpret_cast<unsigned short*>(message->payload+3+i*2)));
			}

			instance->jump_updates[state->sockfd] = jumps;
			instance->players_mutex.unlock();
			break;
		}
	}

	return 0;
}

void FlappyServer::start()
{	
	if (context != NULL)
	{
		generate_pipes();

		cout << "Server started" << endl;

		running = true;
		update_thread = thread(&FlappyServer::send_updates, this);		

		libwebsock_wait(context);
		
		running = false;
		update_thread.join();
	}
}

void FlappyServer::generate_pipes()
{
	srand(time(NULL));

	cout << "Generating pipes...";	

	pipes_message.add_byte(MESSAGE_PIPES);
	pipes_message.add_4byte(PIPES_COUNT);

	for (int i = 0; i < PIPES_COUNT; i++)
	{
		pipes_message.add_4byte((i+2)*PIPES_DISTANCE);
		pipes_message.add_4byte((rand() % (MAX_PIPE_HOLE_POSITION-MIN_PIPE_HOLE_POSITION)) + MIN_PIPE_HOLE_POSITION);
	}

	cout << " - Finished" << endl;
}

void FlappyServer::send_message(libwebsock_client_state *state, Message message)
{
	char *message_data = message.construct();

	if (state == NULL)
	{
		libwebsock_client_state *iterator;
		for (iterator = context->clients_HEAD; iterator != NULL; iterator = iterator->next)
		{
			libwebsock_send_binary(iterator, message_data, message.size());
		}
	}
	else
	{
		libwebsock_send_binary(state, message_data, message.size());
	}
}

void FlappyServer::remove_disconnected_players()
{
	for (vector<unsigned int>::iterator it = disconnected_players.begin(); it != disconnected_players.end(); it++)
	{
		players.erase(*it);
	}

	disconnected_players.clear();
}

void FlappyServer::send_updates()
{
	while(running)
	{
		if (jump_updates.size() > 0)
		{
			players_mutex.lock();

			Message update_message;
			update_message.add_byte(MESSAGE_UPDATES);
			update_message.add_4byte(players.size() - disconnected_players.size());
			update_message.add_4byte(jump_updates.size());				


			for (map<unsigned int, vector<unsigned short>>::iterator it1 = jump_updates.begin(); it1 != jump_updates.end(); it1++)
			{
				update_message.add_4byte(it1->first);

				update_message.add_string(players[it1->first].get_nickname());

				update_message.add_2byte((it1->second).size());

				for (vector<unsigned short>::iterator it2 = (it1->second).begin(); it2 != (it1->second).end(); it2++)
				{
					update_message.add_2byte(*it2);	
				}
			
				(it1->second).clear();
			}

			jump_updates.clear();

			send_message(NULL, update_message);

			jump_updates.clear();
			remove_disconnected_players();

			players_mutex.unlock();
		}

		this_thread::sleep_for(chrono::milliseconds(UPDATE_INTERVAL));
	}
}
