#include "stdafx.h"
#include "LabProject07-9-1.h"
#include "GameFramework.h"
#include"protocol.h"
#include"INFO.h"

#include<iostream>

#define MAX_LOADSTRING 100

HINSTANCE						ghAppInstance;
TCHAR							szTitle[MAX_LOADSTRING];
TCHAR							szWindowClass[MAX_LOADSTRING];

CGameFramework					gGameFramework;

int _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

DWORD WINAPI ConnecttoServer(LPVOID arg);
DWORD WINAPI recvtoserver(LPVOID arg);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

SOCKET clientsocket;

array<CLIENT, 3>Clients;
array<bULLET, 150>Bullets;
int id = 0;
HANDLE conevent = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE recvevent = CreateEvent(NULL, FALSE, FALSE, NULL);


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	::LoadString(hInstance, IDC_LABPROJECT0791, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) return(FALSE);

	hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDI_LABPROJECT0791));

	int ret;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return -1;
	}

	HANDLE network_th;
	network_th = CreateThread(NULL, 0, ConnecttoServer, NULL, 0, NULL); // 로그인 쓰레드 

	WaitForSingleObject(conevent, INFINITE);
	cout << " CONNECT " << endl;
	int retval;
	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
		{
			// 값 전달해주기
			if (!gGameFramework.is_KeyInput_Empty())
			{
				char send_keyvalue = gGameFramework.pop_keyvalue();
				int bulletnum = gGameFramework.fired_bnum;
				CS_EVENT_PACKET p;
				p.type = CS_EVENT;
				p.direction = send_keyvalue;
				p.size = sizeof(CS_EVENT_PACKET);
				p.bnum = bulletnum;
				send(clientsocket, (char*)&p, sizeof(CS_EVENT_PACKET), 0);
				//SetEvent(recvevent);
			}
			if (!gGameFramework.is_Item_Empty())
			{
				char send_itemvalue = gGameFramework.pop_itemvalue();
				CS_ITEM_PACKET p;
				p.type = CS_ITEM;
				p.size = sizeof(CS_ITEM_PACKET);
				p.num = send_itemvalue;
				send(clientsocket, (char*)&p, sizeof(CS_ITEM_PACKET), 0);
				//SetEvent(recvevent);
			}
			if (gGameFramework.Is_dead()) {
				CS_REMOVE_PACKET p;
				p.type = CS_REMOVE;
				p.size = sizeof(CS_REMOVE_PACKET);
				send(clientsocket, (char*)&p, sizeof(CS_REMOVE_PACKET), 0);
			}
			if (gGameFramework.m_pPlayer != NULL) {
				for (int i = 0; i < 3; i++) {
					gGameFramework.obj_SetPosition(i, Clients[i].c_id, Clients[i].c_pos);
					gGameFramework.obj_SetLookRight(i, Clients[i].c_id, Clients[i].c_look, Clients[i].c_right, Clients[i].rotNum);
				}
			}
			// **** 총알
			for (int j = 0; j < 150; j++)
			{
				if (Bullets[j].m_state == true) {
					Bullets[j].c_pos = gGameFramework.calcmove(Bullets[j].c_pos, Bullets[j].c_look);
					gGameFramework.bullet_setposition(Bullets[j].bnum, Bullets[j].c_pos, Bullets[j].c_look, Bullets[j].bullet_power, Bullets[j].who);
				}
			}
			gGameFramework.FrameAdvance();
		}
	}

	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}
//로그인 쓰레드 

DWORD WINAPI ConnecttoServer(LPVOID arg)
{
	int retval;
	int my_id;
	clientsocket = socket(AF_INET, SOCK_STREAM, 0);
	char recvbuf[BUF_SIZE];
	if (clientsocket == INVALID_SOCKET)
	{
		return 0;
	}
	sockaddr_in clientaddr;
	clientaddr.sin_family = AF_INET;
	clientaddr.sin_port = htons(9000);
	inet_pton(AF_INET, "127.0.0.1", &clientaddr.sin_addr);
	connect(clientsocket, (sockaddr*)&clientaddr, sizeof(clientaddr));

	// 로그인 
	while (true)
	{
		recv(clientsocket, recvbuf, BUF_SIZE, 0);
		switch (recvbuf[1])
		{
		case SC_LOGIN_PLAYER:
		{
			SC_LOGIN_PACKET* p = reinterpret_cast<SC_LOGIN_PACKET*>(&recvbuf);
			my_id = p->id;
			gGameFramework.Login_ID = p->id;
			Clients[my_id].c_id = my_id;
			Clients[my_id].c_pos.x = p->pos.x;
			Clients[my_id].c_pos.y = p->pos.y;
			Clients[my_id].c_pos.z = p->pos.z;
			Clients[my_id].m_state = ST_RUNNING;
			strcpy_s(Clients[my_id].name, p->name);
			Clients[my_id]._hp = p->hp;
			Clients[my_id]._speed = p->speed;
			Clients[my_id].c_look = p->Look;
			Clients[my_id].c_right = p->Right;
			Clients[my_id].bullet_power = p->bulletpower;
			Clients[my_id].rotNum = p->rotNum;
			//SetEvent(conevent);
			break;
		}
		case SC_ADD_PLAYER:
			SC_ADD_PLAYER_PACKET* p = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(&recvbuf);
			my_id = p->id;
			Clients[my_id].c_id = my_id;
			Clients[my_id].m_state = ST_RUNNING;
			Clients[my_id]._hp = p->hp;
			Clients[my_id]._speed = p->speed;
			Clients[my_id].c_pos.x = p->pos.x;
			Clients[my_id].c_pos.y = p->pos.y;
			Clients[my_id].c_pos.z = p->pos.z;
			Clients[my_id].c_look = p->Look;
			Clients[my_id].c_right = p->Right;
			Clients[my_id].bullet_power = p->bulletpower;
			Clients[my_id].rotNum = p->rotNum;
			strcpy_s(Clients[my_id].name, p->name);
			break;
		}
		if (Clients[0].c_id != -1 && Clients[1].c_id != -1 && Clients[2].c_id != -1)
		{
			for (int i = 0; i < 150; ++i)
				Bullets[i].m_state = false;
			Sleep(10);
			SetEvent(conevent);

			HANDLE recv_th;
			recv_th = CreateThread(NULL, 0, recvtoserver, (LPVOID)(clientsocket), 0, NULL);  // 로그인 완료시 서버와 통신 쓰레드 생성 
			break;
		}

	}


};

DWORD WINAPI recvtoserver(LPVOID arg)
{
	SOCKET _socket = (SOCKET)arg;
	int retval;
	int my_id;
	char recvbuf[BUF_SIZE];
	if (clientsocket == INVALID_SOCKET)
	{
		return 0;
	}

	while (true)
	{
		recv(_socket, recvbuf, sizeof(SC_UPDATE_PACKET), 0);

		switch (recvbuf[1])
		{
		case SC_UPDATE_PLAYER:
		{
			SC_UPDATE_PACKET* p = reinterpret_cast<SC_UPDATE_PACKET*>(&recvbuf);
			Clients[p->_id].c_pos = p->pos;
			Clients[p->_id].c_look = p->look;
			Clients[p->_id]._speed = p->speed;
			Clients[p->_id].c_right = p->right;
			Clients[p->_id].rotNum = p->rotNum;
			Clients[p->_id].c_id = p->_id;
			if (p->rotNum != 2)
				gGameFramework.a = 0;
			break;
		}
		case SC_ITEM:
		{
			SC_ITEM_PACKET* p = reinterpret_cast<SC_ITEM_PACKET*>(&recvbuf);
			gGameFramework.delete_item(p->num);
			break;
		}
		case SC_FIREBULLET_PLAYER:
		{
			SC_FIREBULLET_PACKET* p = reinterpret_cast<SC_FIREBULLET_PACKET*>(&recvbuf);
			Bullets[p->num].m_state = p->m_state;
			Bullets[p->num].c_pos = p->bpos;
			Bullets[p->num].bullet_power = p->bulletpower;
			Bullets[p->num].c_look = p->look;
			Bullets[p->num].bnum = p->num;
			Bullets[p->num].who = p->who;
			break;
		}
		case SC_REMOVE:
		{
			SC_REMOVE_PACKET* p = reinterpret_cast<SC_REMOVE_PACKET*>(&recvbuf);
			XMFLOAT3 deadpos = XMFLOAT3(-9999.0, -9999.0, -9999.0);

			Clients[p->id].c_pos = deadpos;
			gGameFramework.delete_player(p->id);
			// Clients[p->id].m_state = false;
			break;
		}
		}
	}
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABPROJECT0791));
	wcex.hCursor = ::LoadCursor(NULL, IDC_CROSS);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;//MAKEINTRESOURCE(IDC_PROTOTYPEUI);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return ::RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	ghAppInstance = hInstance;

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!hMainWnd) return(FALSE);

	gGameFramework.OnCreate(hInstance, hMainWnd);

	::ShowWindow(hMainWnd, nCmdShow);
	::UpdateWindow(hMainWnd);

	return(TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_TIMER:
	case WM_COMMAND:
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return(::DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return((INT_PTR)TRUE);
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			::EndDialog(hDlg, LOWORD(wParam));
			return((INT_PTR)TRUE);
		}
		break;
	}
	return((INT_PTR)FALSE);
}
