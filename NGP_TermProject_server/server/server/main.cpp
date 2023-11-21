// 서버 실행 메인부분, 

#include"stdafx.h"

vector<Player> clients(3);
array<Item, MAX_ITEM>items;
array<SOCKET, 3> connectclients;


int client_id = 0;
HANDLE hEvent;
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

void process(int client_id)
{
	clients[client_id].do_recv();
	char* buf = clients[client_id].recvbuf;

	switch (buf[1])
	{
	CS_LOGIN_PLAYER:
		{
			CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(&buf);
			strcpy_s(clients[client_id].name, sizeof(clients[client_id].name), p->name);
			clients[client_id].send_login_packet();
			for (auto& pl : clients)
			{
				if (clients.size() == 3)
				{
					if (pl.c_id == client_id)continue;
					pl.send_add_packet(client_id);
					clients[client_id].send_add_packet(pl.c_id);
				}
			}

			SetEvent(hEvent);
			break;
		}
	CS_MOVE_PLAYER:
		{
			break;
		}
	}

}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	// 클라한테 전송받은 로그인 패킷 저장 

	int ret;
	int c_id = (int)arg;
	SOCKET clientsocket = reinterpret_cast<SOCKET>(arg);
	sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	int size;
	char buf[BUF_SIZE];

	

	WaitForSingleObject(hEvent, INFINITE);

	while (true)
	{
		process(c_id);

	}
	// Player 컨테이너 안에 클라이언트 정보들을 저장 동시에 접근할 수 있기 때문에 임계영역이나 Event로 관리해줘야함 
	

	return 0;
}
void init(int client_id, SOCKET socket)
{
	sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(socket, (sockaddr*)(&clientaddr), &addrlen);
	clients[client_id].c_id = client_id;
	clients[client_id].socket = socket;
	clients[client_id].cl_addr = clientaddr;
	clients[client_id].pos.x = 0;
	clients[client_id].pos.y = 0;
	clients[client_id].pos.z = 0;
	clients[client_id].name[0] = 0;

}
int main()
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	SOCKET listensocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listensocket == INVALID_SOCKET)
		err_quit("serversocket");
	sockaddr_in serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT_NUM);

	if (bind(listensocket, (sockaddr*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
		err_quit("bind()");
	if (listen(listensocket, SOMAXCONN) == SOCKET_ERROR)
		err_quit("listen()");

	SOCKET clientsocket;
	sockaddr_in clientaddr;
	int addrlen;
	HANDLE hThread;
	int client_id = 0;

	hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	while (true)
	{
		addrlen = sizeof(clientaddr);
		clientsocket = accept(listensocket, (sockaddr*)&clientaddr, &addrlen);
		if (clientsocket == INVALID_SOCKET)
			err_quit("accept()");
		cout << "Client ACCEPT" << endl;

		init(client_id, clientsocket);

		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)(uintptr_t)client_id, 0, NULL);

		client_id++;

		if (hThread == NULL) { closesocket(clientsocket); }
		else { CloseHandle(hThread); }

	}
	closesocket(listensocket);

	CloseHandle(hEvent);

	WSACleanup();
}