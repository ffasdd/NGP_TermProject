#pragma once
class Network
{
public:
	SOCKET clientsocket;
	HANDLE _handle;

public:

	void Init();
	void Connect();

};

