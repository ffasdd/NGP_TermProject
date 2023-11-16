// 서버 실행 메인부분, 

#include"stdafx.h"
#include<random>
vector<Player> clients(3);
array<Item, MAX_ITEM>items;	
array<SOCKET, 3> connectclients;

random_device rd;
default_random_engine dre{ rd() };
uniform_int_distribution<float> uid{ 1,100 };

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
	// 클라한테 전송받은 로그인 패킷 저장 
	
	int ret;
	SOCKET clientsocket = reinterpret_cast<SOCKET>(arg);
	sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	int size;
	char buf[BUF_SIZE];

	getpeername(clientsocket, (sockaddr*)(&clientaddr), &addrlen);
	WaitForSingleObject(hEvent, INFINITE);
	// Player 컨테이너 안에 클라이언트 정보들을 저장 동시에 접근할 수 있기 때문에 임계영역이나 Event로 관리해줘야함 
	for (int i = 0; i < clients.size(); ++i)
	{
		if (clients[i].socket == 0)
		{
			// 여기서 패킷에 담아야함, 

			clients[i].socket = clientsocket;
			clients[i].cl_addr = clientaddr;
			clients[i].c_id = client_id;
			clients[i].pos.x = uid(dre);
			clients[i].pos.y = 0.0f;
			clients[i].pos.z = uid(dre);
			
			client_id++;
			cout << " Client " << i << " connect" << endl;
			break;
		}
	}
	if (recv(clientsocket, buf, sizeof(buf), 0) == SOCKET_ERROR)
		err_display("recv name ");
	cout << buf << endl;

	if(send(clientsocket,))

	SetEvent(hEvent);
	
	// 클라에서 받은 name을 리시브해줘야함 

	// 서버에서 클라에게 좌표정보를 전송해줘야 함 



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