#include"stdafx.h"
#include "Player.h"


void Player::do_send(void* packet)
{
	char* sendbuf = reinterpret_cast<char*>(&packet);
	sendlen = sendbuf[0];
	send(socket, (char*)sendlen, sizeof(int), 0);

	send(socket, sendbuf, sendlen, 0);
	delete[] sendbuf;

}

void Player::do_recv()
{
	recv(socket, (char*)&recvlen, sizeof(int), 0);

	recv(socket, recvbuf, recvlen, 0);
}


void Player::hitmissile()
{
	// hp 감소부분 

}
