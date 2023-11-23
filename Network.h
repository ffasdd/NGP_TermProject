#pragma once
struct Client
{
	char name[20];
	XMFLOAT3 pos;
	XMFLOAT3 rot;
};

class Network
{
public:
	SOCKET clientsocket;
	HANDLE _handle;
	
	Client* clientinfo;

	int sendlen;
	int recvlen;
	
	char* sendbuf;
	char* recvbuf;

public:

	Network();
	~Network();

	void Init();
	void Connect();

	void do_send(void* packet);
	void do_recv();
	
	void process();


};

