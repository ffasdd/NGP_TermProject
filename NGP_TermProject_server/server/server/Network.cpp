#include "Network.h"

extern array<Player, 3>clients;

Network::Network()
{
	Network::Init();
}

Network::~Network()
{

}

void Network::Init()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		cerr << "wsaStartUp Error\n";
		WSACleanup();
		exit(-1);
	}
}

void Network::Start()
{
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT_NUM);
	
	listensocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);

	if(bind(listensocket,(sockaddr*)&serveraddr,sizeof(serveraddr) == SOCKET_ERROR))
		err_quit("bind()");
	if (listen(listensocket, SOMAXCONN) == SOCKET_ERROR)
		err_quit("listen()");
	
	int addrlen;
	SOCKADDR_IN clientaddr;
	while (true)
	{
		addrlen = sizeof(clientaddr);
		clientsocket = accept(listensocket, (sockaddr*)&clientaddr, &addrlen);
		if (clientsocket == INVALID_SOCKET)
			err_quit("accept()");

	}
}

