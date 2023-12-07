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

DWORD WINAPI ConnecttoServer(LPVOID arg);

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);


array<CLIENT, 3>Clients;
int id = 0;
HANDLE conevent = CreateEvent(NULL, FALSE, FALSE, NULL);

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
	network_th = CreateThread(NULL, 0, ConnecttoServer, NULL, 0, NULL);

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
			if (gGameFramework.m_pPlayer != NULL) {
				for (int i = 0; i < 3; i++) {
					gGameFramework.myFunc_SetPosition(i, Clients[i].c_id, Clients[i].c_pos);
				}
			}

			gGameFramework.FrameAdvance();
		}
	}

	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}

DWORD WINAPI ConnecttoServer(LPVOID arg)
{
	int retval;
	int my_id;
	SOCKET clientsocket = socket(AF_INET, SOCK_STREAM, 0);
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
			Clients[my_id].c_right = p->right;
			Clients[my_id].bullet_size = p->bulletsize;
			SetEvent(conevent);
			break;
		}
		case SC_ADD_PLAYER:
		{
			SC_ADD_PLAYER_PACKET* p = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(&recvbuf);
			my_id = p->id;
			Clients[my_id].c_id = my_id;
			Clients[my_id].m_state = ST_RUNNING;
			Clients[my_id]._hp = p->hp;
			Clients[my_id].c_pos.x = p->pos.x;
			Clients[my_id].c_pos.y = p->pos.y;
			Clients[my_id].c_pos.z = p->pos.z;
			Clients[my_id].c_look = p->Look;
			Clients[my_id].c_right = p->right;
			Clients[my_id].bullet_size = p->bulletsize;
			strcpy_s(Clients[my_id].name, p->name);
			break;
		}
		case SC_MOVE_PLAYER:
		{
			SC_MOVE_PACKET* p = reinterpret_cast<SC_MOVE_PACKET*>(&recvbuf);
			Clients[p->_id].c_pos = p->pos;
			Clients[p->_id].c_look = p->look;
			Clients[p->_id].c_right = p->right;
			break;
		}
		case SC_ROTATE_PLAYER:
		{
			SC_ROTATE_PACKET* p = reinterpret_cast<SC_ROTATE_PACKET*>(&recvbuf);
			Clients[p->id].c_look = p->look;
			Clients[p->id].c_right = p->right;
			break; 
		}
		case SC_FIREBULLET_PLAYER:
		{
			SC_ROTATE_PACKET* p = reinterpret_cast<SC_ROTATE_PACKET*>(&recvbuf);
			Clients[p->id].c_look = p->look;
			Clients[p->id].c_right = p->right;
			break;
		}
		case SC_REMOVE:
		{
			SC_REMOVE_PACKET* p = reinterpret_cast<SC_REMOVE_PACKET*>(&recvbuf);
			Clients[p->id].m_state = ST_EMPTY;
			break;
		}
		case SC_END:
		{}
		case SC_UPDATE:
		{}
		}
		if (Clients[0].c_id != -1 && Clients[1].c_id != -1 && Clients[2].c_id != -1)
		{
			SetEvent(conevent);
			break;
		}

	}
	// 게임실행 
	while (true)
	{
		while (true)
		{
			// CS_MOVE_PACKET
			if (!gGameFramework.is_KeyInput_Empty()) {

				char send_keyValue = gGameFramework.pop_keyvalue();									// 키입력 큐에 있는 키값 중 가장 먼저 입력된 키값을
				CS_MOVE_PACKET keyvalue_pack;
				keyvalue_pack.type = CS_MOVE_PLAYER;
				keyvalue_pack.direction = send_keyValue;
				retval = send(clientsocket, (char*)&keyvalue_pack, sizeof(CS_MOVE_PACKET), 0);		// 서버로 전송합니다.

				cout << "Key: " << keyvalue_pack.direction << endl; //test

				break;
			}

			// CS_ROTATE_PACKET

			// CS_FIREBULLET_PACKET

			// CS_ITEM_PACKET
			if (gGameFramework.is_Item_Collision()) {
				CS_ITEM_PACKET item_pack;
				item_pack.type = CS_ITEM;
				item_pack.num = gGameFramework.GetItemNum();
				item_pack.p_speed = gGameFramework.GetPlayerSpeed();
				item_pack.p_bulletsize = gGameFramework.GetPlayerBulletSize();
				retval = send(clientsocket, (char*)&item_pack, sizeof(CS_ITEM_PACKET), 0);		// 서버로 전송합니다.
				// cout << item_pack.p_speed << endl; //test

				break;
			}

			// CS_STATE_PACKET
		}
	}
	return 0;
};

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
