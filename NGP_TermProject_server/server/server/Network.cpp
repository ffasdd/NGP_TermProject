#include"stdafx.h"
#include "Network.h"

extern array<Player, 3>clients;

//Debug
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 소켓 함수 오류 출력
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[오류] %s\n", (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


Network::Network()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		cerr << "wsaStartUp Error\n";
		WSACleanup();
		exit(-1);
	}
}

Network::~Network()
{

}

void Network::Init()
{
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT_NUM);

	listensocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);

	if (bind(listensocket, (sockaddr*)&serveraddr, sizeof(serveraddr) == SOCKET_ERROR))
		err_quit("bind()");
	if (listen(listensocket, SOMAXCONN) == SOCKET_ERROR)
		err_quit("listen()");
	std::cout << "Server Listening" << std::endl;
}

DWORD WINAPI Network::ProcessThread(LPVOID arg)
{
	SOCKET clientsocket = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUF_SIZE];

	Player PlayerInfo;
	addrlen = sizeof(clientaddr);
	getpeername(clientsocket, (SOCKADDR*)&clientaddr, &addrlen);
	

	closesocket(clientsocket);
	return 0;
}


void Network::Accept()
{

	int addrlen;
	SOCKADDR_IN clientaddr;
	
	while (true)
	{
		addrlen = sizeof(clientaddr);
		clientsocket = accept(listensocket, (sockaddr*)&clientaddr, &addrlen);
		if (clientsocket == INVALID_SOCKET)
			err_quit("accept()");
		_handle = CreateThread(NULL, 0, ProcessThread, (LPVOID)clientsocket, 0, NULL);

	}
	std::cout << " all client accept " << std::endl;
}


void Network::Start()
{


}


