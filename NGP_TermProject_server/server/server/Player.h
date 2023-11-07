#pragma once

class Player
{
public:

	void do_send(void* packet);
	void do_recv();
	void hitmissile();

public:
	int c_id;
	int hp;
	int speed;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	char name[NAME_SIZE];
	SOCKET socket;
	char buf[BUF_SIZE];


};

