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
	// 패킷 재조립 부분 추가해야됌
	recv(socket, (char*)recvlen, sizeof(int), 0);
	
	recv(socket, recvbuf, recvlen, 0);
}

void Player::send_login_packet()
{
	SC_LOGIN_PACKET p;
	p.type = SC_LOGIN_PLAYER;
	p.id = c_id;
	p.hp = hp;
	p.speed = speed;
	p.pos.x = pos.x;
	p.pos.y = pos.y;
	p.pos.z = pos.z;
	p.rot.x = rot.x;
	p.rot.y = rot.y;
	p.rot.z = rot.z;
	
	do_send(&p);
}

void Player::hitmissile()
{
	// hp 감소부분 

}
