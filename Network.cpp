#include "stdafx.h"
#include "Network.h"
#include"protocol.h"
#include<iostream>

Network::Network() : clientsocket(0),_handle(0),sendlen(0),recvlen(0)
{
    sendbuf = 0;
    recvbuf = 0;

}

Network::~Network()
{
    closesocket(clientsocket);
    CloseHandle(_handle);
}

void Network::Init()
{
    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) == 0)
        printf("윈속 초기화 \n");

    Connect();

}

void Network::Connect()
{

    clientsocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in clientaddr;
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(9000);
    inet_pton(AF_INET,"127.0.0.1", &clientaddr.sin_addr);
    connect(clientsocket, (sockaddr*)&clientaddr, sizeof(clientaddr));

    std::cout << "사용하실 이름을 입력하시오 : " << endl;
    std::cin >> clientinfo->name;
        
  
    
}

void Network::do_send(void* packet)
{
    sendbuf = reinterpret_cast<char*>(&packet);
    sendlen = sendbuf[0];
    send(clientsocket, (char*)&sendlen, sizeof(int), 0);

    send(clientsocket, sendbuf, sendlen, 0);
}

void Network::do_recv()
{
    recv(clientsocket, (char*)&recvlen, sizeof(int), 0);

    recvbuf = new char[recvlen];

    recv(clientsocket, recvbuf, recvlen, 0);
}
