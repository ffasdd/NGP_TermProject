#include<iostream>
#include <string.h> // strncpy(), ...

#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더
#pragma comment(lib, "ws2_32") // ws2_32.lib 링크
using namespace std;

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <Mmsystem.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

using namespace DirectX;
using namespace DirectX::PackedVector;

#include<thread>
#include<array>
#include<map>
#include<vector>
#include"Player.h"
#include"Bullet.h"
#include"Item.h"
// 패킷 정리
constexpr int PORT_NUM = 9000;
constexpr int BUF_SIZE = 512;
constexpr int NAME_SIZE = 20;
constexpr int MAX_ITEM = 30;
// Client to Server Packet type
constexpr char CS_LOGIN_PLAYER = 0;
constexpr char CS_MOVE_PLAYER = 1;
constexpr char CS_ROTATE_PLAYER = 2;
constexpr char CS_FIREBULLET_PLAYER = 3;
constexpr char CS_REMOVE = 4;

// Server to Client Packet type

constexpr char SC_LOGIN_PLAYER = 1;
constexpr char SC_MOVE_PLAYER = 2;
constexpr char SC_ROTATE_PLAYER = 3;
constexpr char SC_FIREBULLET_PLAYER = 4;
constexpr char SC_COLLIDER = 5;
constexpr char SC_REMOVE = 6;
constexpr char SC_END = 7;
constexpr char SC_UPDATE = 8;

enum COLLIDERTYPE
{
	ITEM,
	BULLET,
	PLAYER
};

#pragma pack(push,1)
struct CS_LOGIN_PACKET
{
	unsigned char size;
	char type;
	char name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned char size;
	char	type;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT

};

struct CS_ROTATE_PACKET
{
	unsigned char size;
	char type;
	char rotdir;
	// 각도 좌표 
};
struct CS_FIREBULLET_PACKET
{
	unsigned char size;
	char type;
	char bulletdir;

};
struct CS_REMOVE_PACKET
{
	unsigned char size;
	char type;
};

struct SC_LOGIN_PACKET
{
	unsigned char size;
	char type;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	int id;
	int hp;
	int speed;
	char name[NAME_SIZE];


};
struct SC_MOVE_PACKET
{
	unsigned char size;
	char type;
	XMFLOAT3 pos;
	XMFLOAT3 look;
	XMFLOAT3 up;
	int id;
	int hp;
	int speed;


};
struct SC_ROTATE_PACKET
{
	unsigned char size;
	char type;
	int id;
	XMFLOAT3 rot;

};
struct SC_FIREBULLET_PACKET
{
	unsigned char size;
	char type;
	XMFLOAT3 bpos;
	char dir;

};
struct SC_COLLIDER_PACKET
{
	unsigned char size;
	char type;
	int id;
	COLLIDERTYPE ctype;

};
struct SC_REMOVE_PACKET
{
	unsigned char size;
	char type;
	int id;

};



