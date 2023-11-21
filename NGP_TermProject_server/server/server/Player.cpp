#include"stdafx.h"
#include "Player.h"

extern vector<Player> clients(3);

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

void Player::send_add_packet(int c_id)
{
	SC_ADD_PLAYER_PACKET p;
	p.id = c_id;
	strcpy_s(p.name, clients[c_id].name);
	p.size = sizeof(SC_ADD_PLAYER_PACKET);
	p.type = SC_ADD_PLAYER;
	p.pos.x = clients[c_id].pos.x;
	p.pos.y = clients[c_id].pos.y;
	p.pos.z = clients[c_id].pos.z;
	p.rot.x = clients[c_id].rot.x;
	p.rot.y = clients[c_id].rot.y;
	p.rot.z = clients[c_id].rot.z;
	p.speed = clients[c_id].speed;

	do_send(&p);
	
}



void Player::hitmissile()
{
	// hp 감소부분 

}
