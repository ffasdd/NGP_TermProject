// 서버 실행 메인부분, 

#include"stdafx.h"

XMFLOAT3 calcMove(XMFLOAT3 vec1, XMFLOAT3 vec2, float Accelerator)
{
	float acc = Accelerator;

	vec1.x = vec1.x + vec2.x * acc;
	vec1.y = vec1.y + vec2.y * acc;
	vec1.z = vec1.z + vec2.z * acc;

	return vec1;
}
XMFLOAT3 RotateLook(XMFLOAT3& lookVector, float rotationAngleInDegrees)
{
	// 각도를 라디안으로 변환
	float angleInRadians = DirectX::XMConvertToRadians(rotationAngleInDegrees);

	// 현재 룩 벡터를 XMVECTOR로 변환
	XMVECTOR lookVectorXM = XMLoadFloat3(&lookVector);

	// Y축을 중심으로 주어진 각도만큼 회전하는 행렬을 생성
	XMMATRIX rotationMatrix = XMMatrixRotationY(angleInRadians);

	// 회전 적용
	XMVECTOR rotatedLookVectorXM = XMVector3TransformCoord(lookVectorXM, rotationMatrix);

	// 결과를 XMFLOAT3로 변환하여 저장
	XMStoreFloat3(&lookVector, rotatedLookVectorXM);

	return lookVector;
}

XMFLOAT3 Rotateright(XMFLOAT3& lookVector, XMFLOAT3& rightVector)
{
	// 현재의 up 벡터 (보통 (0, 1, 0)으로 가정)를 사용하여 right 벡터 계산
	XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotatedLookVectorXM = XMLoadFloat3(&lookVector);
	XMVECTOR rightVectorXM = XMVector3Cross(upVector, rotatedLookVectorXM);

	// 결과를 XMFLOAT3로 변환하여 저장
	XMStoreFloat3(&rightVector, rightVectorXM);

	return rightVector;
}

enum { ST_EMPTY, ST_RUNNING };

int fired_bnum =0;
class CLIENT
{
private:
	int m_id = 0;
	SOCKET m_sock;
	char	m_state;
	char	recv_buf[BUF_SIZE];

	XMFLOAT3 Lookvec;
	XMFLOAT3 Rightvec;
	XMFLOAT3 m_pos;
	float m_yaw, m_pitch, m_roll;
	char name[NAME_SIZE];
	int m_hp;
	float m_speed;

	// 총알 관련 추가 
	int bullet_power;
	int bnum;

public:
	CRITICAL_SECTION m_cs;

	CLIENT()
	{
		m_id = -1;
		m_sock = 0;
		m_state = ST_EMPTY;
		Lookvec = { 0.0f, 0.0f, 1.0f };
		m_pos = { 100.0f,0.0f,100.0f };
		m_yaw = m_pitch = m_roll = 0.0f;
		Rightvec = { 1.0f,0.0f,0.0f };
		m_speed = 10;
		m_hp = 0;
		name[0] = 0;
		InitializeCriticalSection(&m_cs);
		bnum = -1;
		bullet_power =10;
	}
	~CLIENT() {}

public:

	SOCKET getSock() { return m_sock; }
	int getID() { return m_id; }
	XMFLOAT3 getPos() { return m_pos; }
	XMFLOAT3 getRight() { return Rightvec; }
	float getYaw() { return m_yaw; }
	float getPitch() { return m_pitch; }
	float getRoll() { return m_roll; }
	float getSpeed() { return m_speed; }
	char getState() { return m_state; }
	int getHp() { return m_hp; }
	char* getName() { return name; }
	int getBulletPower() { return bullet_power; }
	int getBulletNum() { return bnum; }
	XMFLOAT3 getLookVec() { return Lookvec; }

	void setSocket(SOCKET socket) { m_sock = socket; }
	void setID(int c_id) { m_id = c_id; }
	void setPos(XMFLOAT3 pos) { m_pos = pos; }
	void setYaw(float yaw) { m_yaw = yaw; }
	void setPitch(float pitch) { m_pitch = pitch; }
	void setRoll(float roll) { m_roll = roll; }
	void setSpeed(float Speed) { m_speed += Speed; }
	void setState(char state) { m_state = state; }
	void setHp(int hp) { m_hp = hp; }
	void setName(char* _name) { strcpy_s(name, _name); }
	void setLook(XMFLOAT3 look) { Lookvec = look; }
	void setRight(XMFLOAT3 Right) { Rightvec = Right; }
	void setBulletPower(int bulletpower) { bullet_power += bulletpower; }

public:
	// Network
	void sendLoginPacket(SC_LOGIN_PACKET packet)
	{
		send(m_sock, (char*)&packet, sizeof(SC_LOGIN_PACKET), 0);
	}

	void sendAddPakcet(SC_ADD_PLAYER_PACKET packet)
	{
		send(m_sock, (char*)&packet, sizeof(SC_ADD_PLAYER_PACKET), 0);

	}
	void sendUpdatePacket(SC_UPDATE_PACKET packet)
	{
		send(m_sock, (char*)&packet, sizeof(SC_UPDATE_PACKET), 0);
	}
	void sendItemPacket(SC_ITEM_PACKET packet)
	{
		send(m_sock, (char*)&packet, sizeof(SC_ITEM_PACKET), 0);
	}
	void sendShootPacket(SC_FIREBULLET_PACKET packet)
	{
		send(m_sock, (char*)&packet, sizeof(SC_FIREBULLET_PACKET), 0);
	}
	void sendRemovePacket(SC_REMOVE_PACKET packet)
	{
		send(m_sock, (char*)&packet, sizeof(SC_REMOVE_PACKET), 0);
	}


};

array<CLIENT, MAX_USER> clients;
int client_id = 0;
int bullet_num = 0;
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
		if (clients[i].getState() == ST_EMPTY) //체크를 어떻게?  
		{
			//0 
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
	clients[client_id].setID(client_id); // 0 
	XMFLOAT3 clientPos{ 300.f * (client_id + 1) ,0.0f,100.0f * (client_id + 1) };
	XMFLOAT3 clientLook{ 0.0f,0.0f,1.0f };
	XMFLOAT3 clientRight{ 1.0f,0.0f,0.0f };
	clients[client_id].setPos(clientPos);
	clients[client_id].setHp(100);
	clients[client_id].setSpeed(5.0f);
	clients[client_id].setLook(clientLook);
	clients[client_id].setRight(clientRight);

	LeaveCriticalSection(&clients[client_id].m_cs);

	SC_LOGIN_PACKET packet;
	packet.type = SC_LOGIN_PLAYER;
	packet.size = sizeof(SC_LOGIN_PACKET);
	packet.id = clients[client_id].getID();
	packet.pos = clients[client_id].getPos();
	packet.speed = clients[client_id].getSpeed();
	packet.hp = clients[client_id].getHp();
	packet.Look = clients[client_id].getLookVec();
	packet.Right = clients[client_id].getRight();
	if (client_id == 0)
		strcpy_s(packet.name, "SDY");
	if (client_id == 1)
		strcpy_s(packet.name, "Test");
	if (client_id == 2)
		strcpy_s(packet.name, "Server");


	clients[client_id].sendLoginPacket(packet);
	// 일단 로그인 되는지 확인부터 ok 

	while (true)
	{
		if (clients[2].getID() != -1)
		{
			for (auto& pl : clients)
			{
				// 내 정보를 다른 클라이언트들한테 보내기 
				if (client_id == pl.getID())continue;
				SC_ADD_PLAYER_PACKET p;
				p.size = sizeof(SC_ADD_PLAYER_PACKET);
				p.type = SC_ADD_PLAYER;
				p.id = client_id;
				p.hp = clients[client_id].getHp();
				strcpy_s(p.name, clients[client_id].getName());
				p.pos = clients[client_id].getPos();
				p.speed = clients[client_id].getSpeed();
				p.Look = clients[client_id].getLookVec();
				p.Right = clients[client_id].getRight();
				pl.sendAddPakcet(p);
			}
		}
		if (clients[0].getState() == ST_RUNNING && clients[1].getState() == ST_RUNNING && clients[2].getState() == ST_RUNNING)
			break;
		// recv - 키 입력 받아보아요
	}


	// 패킷 수신 
	while (1)
	{
		char recvbuf[BUF_SIZE];
		recv(client_sock, recvbuf, sizeof(CS_EVENT_PACKET), 0);
		switch (recvbuf[1])
		{
		case CS_MOVE_PLAYER:
		{
			CS_EVENT_PACKET* p = reinterpret_cast<CS_EVENT_PACKET*>(&recvbuf);
			switch (p->direction)
			{
			case 0:
				//cout << " Up " << endl;
				{

					XMFLOAT3 Move_Vertical_Result{ 0, 0, 0 };

					XMFLOAT3 xm3look = clients[client_id].getLookVec();
					
					XMVECTOR vector = XMLoadFloat3(&xm3look);
					vector = XMVector3Normalize(vector);

					XMFLOAT3 normalizedVec;
					XMStoreFloat3(&normalizedVec, vector);

					Move_Vertical_Result = calcMove(clients[client_id].getPos(), normalizedVec, clients[client_id].getSpeed());

					EnterCriticalSection(&clients[client_id].m_cs);
					clients[client_id].setPos(Move_Vertical_Result);
					LeaveCriticalSection(&clients[client_id].m_cs);


					for (auto& pl : clients)
					{
						SC_UPDATE_PACKET movepacket;
						movepacket.type = SC_UPDATE_PLAYER;
						movepacket.size = sizeof(SC_UPDATE_PLAYER);
						movepacket._id = clients[client_id].getID();
						movepacket.pos = clients[client_id].getPos();
						movepacket.look = clients[client_id].getLookVec();
						movepacket.speed = clients[client_id].getSpeed();
						movepacket.right = clients[client_id].getRight();
						pl.sendUpdatePacket(movepacket);
					}


				}
				break;
			case 1:
				//cout << " Down " << endl;
				{

					XMFLOAT3 Move_Vertical_Result{ 0, 0, 0 };
					XMFLOAT3 lookVector = clients[client_id].getLookVec();
					XMVECTOR vector = XMLoadFloat3(&lookVector);
					vector = XMVector3Normalize(vector);

					XMFLOAT3 normalizedVec;
					XMStoreFloat3(&normalizedVec, vector);
					XMFLOAT3 backVector;
					XMMATRIX rotationMatrix = XMMatrixRotationY(XM_PI); // 180도 회전
					XMVECTOR backVectorXM = XMVector3Transform(XMLoadFloat3(&normalizedVec), rotationMatrix);
					XMStoreFloat3(&backVector, backVectorXM);
					Move_Vertical_Result = calcMove(clients[client_id].getPos(), backVector, clients[client_id].getSpeed());

					EnterCriticalSection(&clients[client_id].m_cs);
					clients[client_id].setPos(Move_Vertical_Result);
					LeaveCriticalSection(&clients[client_id].m_cs);


					for (auto& pl : clients)
					{
						SC_UPDATE_PACKET movepacket;
						movepacket.type = SC_UPDATE_PLAYER;
						movepacket.size = sizeof(SC_UPDATE_PLAYER);
						movepacket._id = clients[client_id].getID();
						movepacket.pos = clients[client_id].getPos();
						movepacket.look = clients[client_id].getLookVec();
						movepacket.speed = clients[client_id].getSpeed();
						movepacket.right = clients[client_id].getRight();
						pl.sendUpdatePacket(movepacket);
					}


				}
				break;
			case 2:
				//cout << " Left " << endl;
				{
					// 여기서 받고 clients 위치 정보 처리 이후 다시 send 
					// send...
					// move(dir); 함수 만들어서 후처리 이후에
					// send(client_sock,...);
					// client에 넣어서 값 수정할때 m_cs 임계영역 지정해야함 
					XMFLOAT3 Move_Vertical_Result{ 0, 0, 0 };
					XMFLOAT3 rightVector = clients[client_id].getRight();

					XMVECTOR vector = XMLoadFloat3(&rightVector);
					vector = XMVector3Normalize(vector);

					XMFLOAT3 normalizedVec;
					XMStoreFloat3(&normalizedVec, vector);

					XMFLOAT3 leftVector = XMFLOAT3(-normalizedVec.x, normalizedVec.y, -normalizedVec.z);

					Move_Vertical_Result = calcMove(clients[client_id].getPos(), leftVector, clients[client_id].getSpeed());

					EnterCriticalSection(&clients[client_id].m_cs);
					clients[client_id].setPos(Move_Vertical_Result);
					LeaveCriticalSection(&clients[client_id].m_cs);


					for (auto& pl : clients)
					{
						SC_UPDATE_PACKET movepacket;
						movepacket.type = SC_UPDATE_PLAYER;
						movepacket.size = sizeof(SC_UPDATE_PLAYER);
						movepacket._id = clients[client_id].getID();
						movepacket.pos = clients[client_id].getPos();
						movepacket.look = clients[client_id].getLookVec();
						movepacket.speed = clients[client_id].getSpeed();
						movepacket.right = clients[client_id].getRight();
						pl.sendUpdatePacket(movepacket);
					}

				}
				break;
			case 3:
				//cout << " Right " << endl;
				{

					XMFLOAT3 Move_Vertical_Result{ 0, 0, 0 };
					XMFLOAT3 rightVector = clients[client_id].getRight();

					XMVECTOR vector = XMLoadFloat3(&rightVector);
					vector = XMVector3Normalize(vector);

					XMFLOAT3 normalizedVec;
					XMStoreFloat3(&normalizedVec, vector);

					Move_Vertical_Result = calcMove(clients[client_id].getPos(), normalizedVec, clients[client_id].getSpeed());

					EnterCriticalSection(&clients[client_id].m_cs);
					clients[client_id].setPos(Move_Vertical_Result);
					LeaveCriticalSection(&clients[client_id].m_cs);

					for (auto& pl : clients) // 움직인 내 정보를 모두에게 
					{
						SC_UPDATE_PACKET movepacket;
						movepacket.type = SC_UPDATE_PLAYER;
						movepacket.size = sizeof(SC_UPDATE_PLAYER);
						movepacket._id = clients[client_id].getID();
						movepacket.pos = clients[client_id].getPos();
						movepacket.look = clients[client_id].getLookVec();
						movepacket.speed = clients[client_id].getSpeed();
						movepacket.right = clients[client_id].getRight();
						pl.sendUpdatePacket(movepacket);
					}


				}
				break;
			case 4:
			{

				cout << "Left turn" << endl;

				XMFLOAT3 rightVector = clients[client_id].getRight();

				XMFLOAT3 xm3look = clients[client_id].getLookVec();

				XMVECTOR vector = XMLoadFloat3(&xm3look);
				vector = XMVector3Normalize(vector);

				XMFLOAT3 lookVector;
				XMStoreFloat3(&lookVector, vector);

				vector = XMLoadFloat3(&rightVector);
				vector = XMVector3Normalize(vector);

				XMFLOAT3 NormalizedrightVector;
				XMStoreFloat3(&NormalizedrightVector, vector);

				lookVector = RotateLook(lookVector, -10.0f);
				NormalizedrightVector = Rotateright(lookVector, NormalizedrightVector);

				EnterCriticalSection(&clients[client_id].m_cs);
				clients[client_id].setLook(lookVector);
				clients[client_id].setRight(NormalizedrightVector);
				LeaveCriticalSection(&clients[client_id].m_cs);

				for (auto& pl : clients) // 움직인 내 정보를 모두에게 
				{
					SC_UPDATE_PACKET rotpacket;
					rotpacket.type = SC_UPDATE_PLAYER;
					rotpacket.size = sizeof(SC_UPDATE_PLAYER);
					rotpacket._id = clients[client_id].getID();
					rotpacket.pos = clients[client_id].getPos();
					rotpacket.look = clients[client_id].getLookVec();
					rotpacket.speed = clients[client_id].getSpeed();
					rotpacket.right = clients[client_id].getRight();
					pl.sendUpdatePacket(rotpacket);
				}
				break;
			}
			case 5:
			{
				cout << " Right turn" << endl;
				XMFLOAT3 rightVector = clients[client_id].getRight();
				XMFLOAT3 xm3look = clients[client_id].getLookVec();
				XMVECTOR vector = XMLoadFloat3(&xm3look);
				vector = XMVector3Normalize(vector);

				XMFLOAT3 lookVector;
				XMStoreFloat3(&lookVector, vector);
				//
				vector = XMLoadFloat3(&rightVector);
				vector = XMVector3Normalize(vector);

				XMFLOAT3 NormalizedrightVector;
				XMStoreFloat3(&NormalizedrightVector, vector);

				lookVector = RotateLook(lookVector, 10.0f);
				NormalizedrightVector = Rotateright(lookVector, NormalizedrightVector);

				EnterCriticalSection(&clients[client_id].m_cs);
				clients[client_id].setLook(lookVector);
				clients[client_id].setRight(NormalizedrightVector);
				LeaveCriticalSection(&clients[client_id].m_cs);

				for (auto& pl : clients) // 움직인 내 정보를 모두에게 
				{
					SC_UPDATE_PACKET rotpacket;
					rotpacket.type = SC_UPDATE_PLAYER;
					rotpacket.size = sizeof(SC_UPDATE_PLAYER);
					rotpacket._id = clients[client_id].getID();
					rotpacket.pos = clients[client_id].getPos();
					rotpacket.look = clients[client_id].getLookVec();
					rotpacket.speed = clients[client_id].getSpeed();
					rotpacket.right = clients[client_id].getRight();
					pl.sendUpdatePacket(rotpacket);
				}
				break;

			}
			case 6:
				//cout << " Fire Bullet " << endl;
				{

					for (auto& pl : clients)
					{
					SC_FIREBULLET_PACKET shootpacket;
					shootpacket.type = SC_FIREBULLET_PLAYER;
					shootpacket.size = sizeof(SC_FIREBULLET_PACKET);
					shootpacket.m_state = true;
					shootpacket.bpos = clients[client_id].getPos();
					shootpacket.bulletpower = clients[client_id].getBulletPower();
					shootpacket.look = clients[client_id].getLookVec();
					shootpacket.num = fired_bnum;
					shootpacket.who = client_id;

					pl.sendShootPacket(shootpacket);
					}
					EnterCriticalSection(&clients[client_id].m_cs);
					fired_bnum++;
					LeaveCriticalSection(&clients[client_id].m_cs);

					cout << fired_bnum << endl;
					
					break;
				}
			default:
				break;
			}
		}
		break;
		case CS_ITEM:
		{
			CS_ITEM_PACKET* p = reinterpret_cast<CS_ITEM_PACKET*>(&recvbuf);
			//%// i의 값이 짝수면 type을 false로, 홀수일땐 true로 설정했습니다.
		    //%// m_ppGameObjects[i]->type이 false -> 속도 증가 / true -> 총알 커지는 아이템 입니다.
			int item_num = (p->num) % 2;
			if (item_num) {
				clients[client_id].setSpeed(2.0f);
			}
			else {
				clients[client_id].setBulletPower(5.0);
			}

			for (auto& pl : clients)
			{
				SC_ITEM_PACKET itempacket;
				itempacket.type = SC_ITEM;
				itempacket.size = sizeof(SC_ITEM_PACKET);
				itempacket.num = p->num;

				pl.sendItemPacket(itempacket);

			}
		}
		break;
		case CS_REMOVE:
		{
			CS_REMOVE_PACKET* p = reinterpret_cast<CS_REMOVE_PACKET*>(&recvbuf);

			for (auto& pl : clients)
			{
				SC_REMOVE_PACKET removepacket;
				removepacket.type = SC_REMOVE;
				removepacket.size = sizeof(SC_REMOVE_PACKET);
				removepacket.id = client_id;

				pl.sendRemovePacket(removepacket);

			}
		}
		break;
		}

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