#ifndef PLAYER_H
#define PLAYER_H

#include <string>
using namespace std;

class Player
{
	private:
		string nickname;

	public:
		Player();
		Player(string nickname);

		string get_nickname();
		void set_nickname(string new_nickname);
};

#endif
