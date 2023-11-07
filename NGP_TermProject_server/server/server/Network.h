#pragma once
#include"stdafx.h"
#include"protocl.h"



class Network
{
public:
	HANDLE _handle;
	SOCKET listensocket;
	SOCKET clientsocket;

	char buf[BUF_SIZE];
	int c_id = 0;

public:
	Network();
	~Network();

	void Init();
	void Start();
	void Accept();
	static DWORD WINAPI ProcessThread(LPVOID arg);
	static void Process(int c_id, SOCKET socket);


};

