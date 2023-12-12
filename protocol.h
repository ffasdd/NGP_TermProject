// 패킷 정리


constexpr int PORT_NUM = 9000;
constexpr int BUF_SIZE = 512;
constexpr int NAME_SIZE = 20;
constexpr int MAX_ITEM = 30;
// Client to Server Packet type
constexpr char CS_LOGIN_PLAYER = 0;
constexpr char CS_EVENT = 1;
constexpr char CS_ROTATE_PLAYER = 2;
constexpr char CS_FIREBULLET_PLAYER = 3;
constexpr char CS_REMOVE = 4;
constexpr char CS_ITEM = 5;

// Server to Client Packet type

constexpr char SC_LOGIN_PLAYER = 1;
constexpr char SC_UPDATE_PLAYER = 2;
constexpr char SC_ROTATE_PLAYER = 3;
constexpr char SC_FIREBULLET_PLAYER = 4;
constexpr char SC_COLLIDER = 5;
constexpr char SC_REMOVE = 6;
constexpr char SC_END = 7;
constexpr char SC_UPDATE = 8;
constexpr char SC_ADD_PLAYER = 9;
constexpr char SC_ITEM = 10;




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

struct CS_EVENT_PACKET {
	unsigned char size;
	char	type;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT, 4 : LEFT, 5 : RIGHT , 6 : FIREBULLET
	int bnum;
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

struct CS_ITEM_PACKET
{
	unsigned char size;
	char type;
	int num;
	float p_speed;
	float p_bulletsize;
};

struct SC_LOGIN_PACKET
{
	unsigned char size;
	char type;
	XMFLOAT3 pos;
	int id;
	int hp;
	int speed;
	int bulletsize;
	XMFLOAT3 Look;
	XMFLOAT3 Right;
	char name[NAME_SIZE];


};
struct SC_ADD_PLAYER_PACKET
{
	unsigned char size;
	char type;
	XMFLOAT3 pos;
	XMFLOAT3 Look;
	XMFLOAT3 Right;
	int id;
	int hp;
	int speed;
	int bulletsize;
	char name[NAME_SIZE];
};
struct SC_UPDATE_PACKET
{
	unsigned char size;
	char type;
	int _id;
	XMFLOAT3 pos;
	XMFLOAT3 look;
	XMFLOAT3 right;
	int speed;
	int bulletsize;
	int bnum;
};
struct SC_ITEM_PACKET {
	unsigned char size;
	char type;
	int num;
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
	int bulletsize;
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

