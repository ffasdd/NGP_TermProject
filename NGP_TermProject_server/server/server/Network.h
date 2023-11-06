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
public:
	Network();
	~Network();

	void Init();
	void Start();
	DWORD WINAPI ProcessThread(LPVOID arg);

};

