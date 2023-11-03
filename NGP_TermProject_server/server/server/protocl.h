#include"stdafx.h"
#include"Player.h"
#include"Bullet.h"
#include"Item.h"
// 패킷 정리
constexpr int PORT_NUM = 9000;
constexpr int BUF_SIZE = 512;
constexpr int NAME_SIZE = 20;

// Client to Server Packet type
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 0;
constexpr char CS_ROTATE = 0;
constexpr char CS_FIREBULLET = 0;
constexpr char CS_REMOVE = 0;

// Server to Client Packet type

constexpr char SC_LOGIN = 1;
constexpr char SC_MOVE = 2;
constexpr char SC_ROTATE = 3;
constexpr char SC_FIREBULLET = 4;
constexpr char SC_COLLIDER = 5;
constexpr char SC_REMOVE = 6;
constexpr char SC_END = 7;
constexpr char SC_UPDATE_PACKET = 8;

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
	char rotdir
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
struct SC_UPDATE_PACKET_PACKET
{
	unsigned char size;
	char type;
	int id;
	int hp;
	int speed;
};