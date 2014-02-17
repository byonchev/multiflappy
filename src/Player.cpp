#include "Player.h"

Player::Player()
: nickname("")
{}

Player::Player(string nickname)
: nickname(nickname)
{}

string Player::get_nickname()
{
	return nickname;
}

void Player::set_nickname(string new_nickname)
{
	nickname = new_nickname;
}
