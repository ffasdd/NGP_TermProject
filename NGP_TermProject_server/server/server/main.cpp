// 서버 실행 메인부분, 

#include"stdafx.h"
//#include"Bullet.h"
#include"Player.h"
//#include"Item.h"
#include"protocl.h"


array<Player,3>clients;
array<Item, MAX_ITEM>items;

int client_id = 0;
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
    int ret;
    SOCKET clientsocket = (SOCKET)arg;
    sockaddr_in clientaddr;
    int addrlen = sizeof(clientaddr);
    int client_id = 0;
    int size;
    char buf[BUF_SIZE];
    if (recv(clientsocket, (char*)&size, sizeof(int), 0) == SOCKET_ERROR)
        err_quit("recv size");
    if (recv(clientsocket, buf, size, 0) != SOCKET_ERROR)
    {
        strcpy_s(clients[client_id].buf, buf);
        client_id++;
    }
    if (client_id == 2)
    {
        // 게임시작 
        // send 
    }
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
    while (true)
    {
        addrlen = sizeof(clientaddr);
        clientsocket = accept(listensocket, (sockaddr*)&clientaddr, &addrlen);
        if (clientsocket == INVALID_SOCKET)
            err_quit("accept()");
        hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)clientsocket, 0, NULL);
        if (hThread == NULL) { closesocket(clientsocket); }
        else { CloseHandle(hThread); }
        
    }
    closesocket(listensocket);

    WSACleanup();
}