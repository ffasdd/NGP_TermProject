// 서버 실행 메인부분, 

#include"stdafx.h"


array<Item, MAX_ITEM>items;
array<SOCKET, 3> connectclients;


enum { ST_EMPTY, ST_RUNNING};


class CLIENT
{
private: 
	int m_id = 0;
	SOCKET m_sock;
	char	m_state;
	char	recv_buf[BUF_SIZE];
	
	
	m_Pos m_pos;
	float m_yaw, m_pitch, m_roll;
	char name[NAME_SIZE];
	int m_hp;
	float m_speed;



public:
	CRITICAL_SECTION m_cs;

	CLIENT()
	{
		m_id = -1;
		m_sock = 0;
		m_state = ST_EMPTY;
		m_pos = { 0.0f,0.0f,0.0f };
		m_yaw = m_pitch = m_roll = 0.0f;
		m_speed = 0;
		m_hp = 0;
		name[0] = 0;
		InitializeCriticalSection(&m_cs);

	}
	~CLIENT() {}

public:
	SOCKET getSock() { return m_sock; }
	int getID() { return m_id; }
	m_Pos getPos() { return m_pos; }
	float getYaw() { return m_yaw;}
	float getPitch() { return m_pitch;}
	float getRoll() { return m_roll;}
	float getSpeed() { return m_speed; }
	char getState() { return m_state; }
	int getHp() { return m_hp; }

	void setSocket(SOCKET socket) { m_sock = socket; }
	void setID(int c_id) { m_id = c_id; }
	void setPos(m_Pos pos) { m_pos = pos; }
	void setYaw(float yaw) { m_yaw = yaw; }
	void setPitch(float pitch) { m_pitch = pitch; }
	void setRoll(float roll) { m_roll = roll; }
	void setSpeed(float Speed) { m_speed = Speed; }
	void setState(char state) { m_state = state; }
	void setHp(int hp) { m_hp = hp; }
	void setName(char* _name) { strcpy_s(name, _name); }
public:
	// Network
	void sendLoginPacket(SC_LOGIN_PACKET packet)
	{
		send(m_sock, (char*)&packet, sizeof(SC_LOGIN_PACKET), 0);
	}
	


};

array<CLIENT, MAX_USER> clients;
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


DWORD WINAPI ProcessClient(LPVOID arg)
{
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	struct sockaddr_in clientaddr;
	char addr[INET_ADDRSTRLEN];
	int addrlen;

	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (sockaddr*)&clientaddr, &addrlen);
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));

	int client_id = 0;
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (clients[i].getState() == ST_EMPTY)
		{
			client_id = i;
			EnterCriticalSection(&clients[client_id].m_cs);
			clients[client_id].setID(client_id);
			clients[client_id].setSocket(client_sock);
			clients[client_id].setState(ST_RUNNING);/*
			recv(clients[client_id].socket, clients[client_id].recv_buf, NAME_SIZE, 0);
			clients[client_id].setName(clients[client_id].recv_buf);*/
			LeaveCriticalSection(&clients[client_id].m_cs);

			break;
		}
	}
	EnterCriticalSection(&clients[client_id].m_cs);
	clients[client_id].setID(client_id);
	m_Pos clientPos{ 100.f * client_id,0.0f,10.0f * client_id };
	clients[client_id].setPos(clientPos);
	clients[client_id].setHp(100);
	clients[client_id].setSpeed(10.0f);

	LeaveCriticalSection(&clients[client_id].m_cs);

	SC_LOGIN_PACKET packet;
	packet.type = SC_LOGIN_PLAYER;
	packet.size = sizeof(SC_LOGIN_PACKET);
	packet.id = clients[client_id].getID();
	packet.pos = clients[client_id].getPos();
	packet.speed = clients[client_id].getSpeed();
	packet.hp = clients[client_id].getHp();

	clients[client_id].sendLoginPacket(packet);

	return 0;
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

		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)clientsocket, 0, NULL);

		if (hThread == NULL) { closesocket(clientsocket); }
		else { CloseHandle(hThread); }
	}
	closesocket(listensocket);

	CloseHandle(hEvent);

	WSACleanup();
}