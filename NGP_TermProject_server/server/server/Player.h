#pragma once
class Player
{
public:

	void do_send(void* packet);
	void do_recv();
	void send_login_packet();
	//void send_move_packet(int c_id);
	void send_add_packet(int c_id);
	//void send_remove_packet(int c_id);


	void hitmissile();

public:
	int c_id;
	int hp;
	int speed;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	char name[200];

	int sendlen;
	int recvlen;
	SOCKET socket;
	sockaddr_in cl_addr;
	char recvbuf[1000];

	int prev_remain;

public:
	Player()
	{
		c_id = -1;
		socket = 0;
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;

		rot.x = 0;
		rot.y = 0;
		rot.z = 0;

		name[0] = 0;
		prev_remain = 0;

	}
	~Player() {}

	
	


};

