#include "stdafx.h"
#include "Network.h"

void Network::Init()
{
    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) == 0)
        printf("���� �ʱ�ȭ \n");
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

}
