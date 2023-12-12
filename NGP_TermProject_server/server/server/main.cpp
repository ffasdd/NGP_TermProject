// ���� ���� ���κκ�, 

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
	// ������ �������� ��ȯ
	float angleInRadians = DirectX::XMConvertToRadians(rotationAngleInDegrees);

	// ���� �� ���͸� XMVECTOR�� ��ȯ
	XMVECTOR lookVectorXM = XMLoadFloat3(&lookVector);

	// Y���� �߽����� �־��� ������ŭ ȸ���ϴ� ����� ����
	XMMATRIX rotationMatrix = XMMatrixRotationY(angleInRadians);

	// ȸ�� ����
	XMVECTOR rotatedLookVectorXM = XMVector3TransformCoord(lookVectorXM, rotationMatrix);

	// ����� XMFLOAT3�� ��ȯ�Ͽ� ����
	XMStoreFloat3(&lookVector, rotatedLookVectorXM);

	return lookVector;
}
XMFLOAT3 Rotateright(XMFLOAT3& lookVector, XMFLOAT3& rightVector)
{
	// ������ up ���� (���� (0, 1, 0)���� ����)�� ����Ͽ� right ���� ���
	XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rotatedLookVectorXM = XMLoadFloat3(&lookVector);
	XMVECTOR rightVectorXM = XMVector3Cross(upVector, rotatedLookVectorXM);

	// ����� XMFLOAT3�� ��ȯ�Ͽ� ����
	XMStoreFloat3(&rightVector, rightVectorXM);

	return rightVector;
}



//bool ForwardCollide() {};
//bool BackCollide() {};
//bool LeftCollide() {};
//bool RightCollide() {};


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

	// �Ѿ� ���� �߰� 
	int bullet_size;
	int bnum;

public:
	// �浹 üũ 


	bool forwardcol;
	bool backcol;
	bool leftcol;
	bool rightcol;


	XMFLOAT3 prevpos;


public:
	CRITICAL_SECTION m_cs;
	BoundingOrientedBox m_oobb;
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
		m_oobb = { m_pos,XMFLOAT3(20.0f,20.0f,20.0f),XMFLOAT4{0,0,0,1.0f} };
		InitializeCriticalSection(&m_cs);
		bnum = -1;

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
	int getBulletSize() { return bullet_size; }
	int getBulletNum() { return bnum; }
	XMFLOAT3 getLookVec() { return Lookvec; }

	void setSocket(SOCKET socket) { m_sock = socket; }
	void setID(int c_id) { m_id = c_id; }
	void setPos(XMFLOAT3 pos) { m_pos = pos; m_oobb = { m_pos, XMFLOAT3(30.0f, 30.0f, 30.0f), XMFLOAT4{ 0,0,0,1.0f } }; }
	void setYaw(float yaw) { m_yaw = yaw; }
	void setPitch(float pitch) { m_pitch = pitch; }
	void setRoll(float roll) { m_roll = roll; }
	void setSpeed(float Speed) { m_speed = Speed; }
	void setState(char state) { m_state = state; }
	void setHp(int hp) { m_hp = hp; }
	void setName(char* _name) { strcpy_s(name, _name); }
	void setLook(XMFLOAT3 look) { Lookvec = look; }
	void setRight(XMFLOAT3 Right) { Rightvec = Right; }
	void setBulletSize(int bulletsize) { bullet_size = bulletsize; }

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

// ���� �Լ� ���� ���
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

// ���� �Լ� ���� ���
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[����] %s\n", (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


DWORD WINAPI ProcessCollider(LPVOID arg)
{
	int m_client_id = (int)arg;
	//�� ������� Ŭ���̾�Ʈ�� �浹�ϴ��� Ȯ���Ѵ�. 
	// ��� ���ư��鼭 �浹�� ���� �Ѵ�. 
	// �� ���̵�� �ٸ� Ŭ���� ���̵� �� Ŭ�� ���̵�� ��� Ȯ�� ? 
	// �����带 ���� ������ ����? �̷��� ���� ������ Ű�Է��� �ް� ������ �浹�ߴ��� ������. 

	while (true)
	{
		clients[client_id].forwardcol = false;
		clients[client_id].backcol = false;
		clients[client_id].leftcol = false;
		clients[client_id].rightcol = false;
		Sleep(10);

		for (auto& pl : clients)
		{
			if (pl.getID() == m_client_id) continue; // �� �ڽŰ��� �浹�� �˻����� ���� 
			if (clients[m_client_id].m_oobb.Intersects(pl.m_oobb))
			{
				cout << " Collide " << m_client_id << " and  " << pl.getID() << " !! " << endl;
				if (pl.getPos().z > clients[m_client_id].getPos().z)
				{
					XMFLOAT3 prevpos = clients[m_client_id].getPos();
					// ���� ��ü�� ������ �տ� �ִٴ� ��,
					EnterCriticalSection(&clients[m_client_id].m_cs);
					clients[m_client_id].forwardcol = true;
					LeaveCriticalSection(&clients[m_client_id].m_cs);
					Sleep(10);
				}
				if (pl.getPos().z < clients[m_client_id].getPos().z)
				{
					// ���� ��ü�� ������ �ڿ� �ִٴ� ��,

					EnterCriticalSection(&clients[m_client_id].m_cs);
					clients[m_client_id].backcol = true;
					LeaveCriticalSection(&clients[m_client_id].m_cs);
					Sleep(10);
				}
				if (pl.getPos().x > clients[m_client_id].getPos().x)
				{
					// ���� ��ü�� ������ ������ �ִٴ� ��,

					EnterCriticalSection(&clients[m_client_id].m_cs);
					clients[m_client_id].rightcol = true;
					LeaveCriticalSection(&clients[m_client_id].m_cs);
					Sleep(10);
				}

				if (pl.getPos().x < clients[m_client_id].getPos().x)
				{
					// ���� ��ü�� ������ ������ �ִٴ� ��,

					EnterCriticalSection(&clients[m_client_id].m_cs);
					clients[m_client_id].leftcol = true;
					LeaveCriticalSection(&clients[m_client_id].m_cs);
					Sleep(10);
				}
	
			}


		}
	}
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
	XMFLOAT3 clientPos{ 300.f * (client_id + 1) ,8.0f,100.0f * (client_id + 1) };
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
	// �ϴ� �α��� �Ǵ��� Ȯ�κ��� ok 

	while (true)
	{
		if (clients[2].getID() != -1)
		{
			for (auto& pl : clients)
			{
				// �� ������ �ٸ� Ŭ���̾�Ʈ������ ������ 
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
		// recv - Ű �Է� �޾ƺ��ƿ�
	}


	HANDLE cThread;
	cThread = CreateThread(NULL, 0, ProcessCollider, (LPVOID)client_id, 0, NULL);

	// ��Ŷ ���� 
	while (1)
	{
		char recvbuf[BUF_SIZE];
		recv(client_sock, recvbuf, sizeof(CS_EVENT_PACKET), 0);

		switch (recvbuf[1])
		{
		case CS_MOVE_PLAYER:
		{
			CS_EVENT_PACKET* p = reinterpret_cast<CS_EVENT_PACKET*>(&recvbuf);
			clients[client_id].prevpos = clients[client_id].getPos();

			// �浹 ó���� ���� Ŭ���̾�Ʈ�� ���ο� ��ġ ���
			switch (p->direction)
			{
			case 0:
				cout << " Up " << endl;
				{

					XMFLOAT3 Move_Vertical_Result{ 0, 0, 0 };

					XMFLOAT3 xm3look = clients[client_id].getLookVec();
					
					XMVECTOR vector = XMLoadFloat3(&xm3look);
					vector = XMVector3Normalize(vector);

					XMFLOAT3 normalizedVec;
					XMStoreFloat3(&normalizedVec, vector);

					Move_Vertical_Result = calcMove(clients[client_id].getPos(), normalizedVec, clients[client_id].getSpeed());

					if (clients[client_id].forwardcol == false)
					{

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
				
				}
				break;
			case 1:
				cout << " Down " << endl;
				{

					XMFLOAT3 Move_Vertical_Result{ 0, 0, 0 };
					XMFLOAT3 lookVector = clients[client_id].getLookVec();
					XMVECTOR vector = XMLoadFloat3(&lookVector);
					vector = XMVector3Normalize(vector);

					XMFLOAT3 normalizedVec;
					XMStoreFloat3(&normalizedVec, vector);
					XMFLOAT3 backVector;
					XMMATRIX rotationMatrix = XMMatrixRotationY(XM_PI); // 180�� ȸ��
					XMVECTOR backVectorXM = XMVector3Transform(XMLoadFloat3(&normalizedVec), rotationMatrix);
					XMStoreFloat3(&backVector, backVectorXM);
					Move_Vertical_Result = calcMove(clients[client_id].getPos(), backVector, clients[client_id].getSpeed());

					if (clients[client_id].backcol == false)
					{
						clients[client_id].forwardcol = true;
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
					//else
					//	clients[client_id].backcol = false;
				}
				break;
			case 2:
				cout << " Left " << endl;
				{
		
					XMFLOAT3 Move_Vertical_Result{ 0, 0, 0 };
					XMFLOAT3 rightVector = clients[client_id].getRight();

					XMVECTOR vector = XMLoadFloat3(&rightVector);
					vector = XMVector3Normalize(vector);

					XMFLOAT3 normalizedVec;
					XMStoreFloat3(&normalizedVec, vector);

					XMFLOAT3 leftVector = XMFLOAT3(-normalizedVec.x, normalizedVec.y, -normalizedVec.z);

					Move_Vertical_Result = calcMove(clients[client_id].getPos(), leftVector, clients[client_id].getSpeed());

					if (clients[client_id].leftcol == false)
					{

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
					//else
					//	clients[client_id].leftcol = false;
				}
				break;
			case 3:
				cout << " Right " << endl;
				{

					XMFLOAT3 Move_Vertical_Result{ 0, 0, 0 };
					XMFLOAT3 rightVector = clients[client_id].getRight();

					XMVECTOR vector = XMLoadFloat3(&rightVector);
					vector = XMVector3Normalize(vector);

					XMFLOAT3 normalizedVec;
					XMStoreFloat3(&normalizedVec, vector);

					Move_Vertical_Result = calcMove(clients[client_id].getPos(), normalizedVec, clients[client_id].getSpeed());

					if (clients[client_id].rightcol == false) {

						EnterCriticalSection(&clients[client_id].m_cs);
						clients[client_id].setPos(Move_Vertical_Result);
						LeaveCriticalSection(&clients[client_id].m_cs);

						for (auto& pl : clients) // ������ �� ������ ��ο��� 
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
					//else
					//	clients[client_id].rightcol = false;
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

				for (auto& pl : clients) // ������ �� ������ ��ο��� 
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

				for (auto& pl : clients) // ������ �� ������ ��ο��� 
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
				cout << " Fire Bullet " << endl;
				{
					for (auto& pl : clients)
					{
					SC_FIREBULLET_PACKET shootpacket;
					shootpacket.type = SC_FIREBULLET_PLAYER;
					shootpacket.size = sizeof(SC_FIREBULLET_PACKET);
					shootpacket.m_state = true;
					shootpacket.bpos = clients[client_id].getPos();
					shootpacket.bulletsize = clients[client_id].getBulletSize();
					shootpacket.look = clients[client_id].getLookVec();
					shootpacket.num = fired_bnum;

						SC_FIREBULLET_PACKET shootpacket;
						shootpacket.type = SC_FIREBULLET_PLAYER;
						shootpacket.size = sizeof(SC_FIREBULLET_PACKET);
						shootpacket.bpos = clients[client_id].getPos();
						shootpacket.bulletsize = clients[client_id].getBulletSize();
						shootpacket.look = clients[client_id].getLookVec();

						pl.sendShootPacket(shootpacket);
					}
					EnterCriticalSection(&clients[client_id].m_cs);
					fired_bnum++;
					LeaveCriticalSection(&clients[client_id].m_cs);

					cout << fired_bnum << endl;
					
					break;
				}

			}
		}

		}

	}
}

int main()
{
	// ���� �ʱ�ȭ
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