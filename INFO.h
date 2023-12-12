#include"stdafx.h"
#pragma once
enum { ST_EMPTY, ST_RUNNING };
class CLIENT
{
public:
	int c_id;
	int m_state;
	XMFLOAT3 c_pos;
	XMFLOAT3 c_right;
	XMFLOAT3 c_up;
	XMFLOAT3 c_look;
	int _hp;
	int _speed;
	
	char name[20];

	// √—æÀ 
	int bullet_size;
	int bnum;
public:
	CLIENT()
	{
		c_id = -1;
		c_pos = { 0.0f,0.0f,0.0f };
		c_right = { 0.0f,0.0f,0.0f };
		c_up = { 0.0f,0.0f,0.0f };
		c_look = { 0.0f,0.0f,0.0f };
		_hp = 0;
		_speed = 0;
		name[0] = 0;
		bullet_size = 0;
		bnum = 0;
	}

};
// **** √—æÀ ≈¨∑°Ω∫
class bULLET
{
public:
	int c_id;      // √—æÀ ¿Œµ¶Ω∫
	bool m_state;
	XMFLOAT3 c_pos;
	//XMFLOAT3 c_right;
	//XMFLOAT3 c_up;
	XMFLOAT3 c_look;
	int _speed;

	//char name[20];

	// √—æÀ ªÁ¿Ã¡Ó
	float bullet_size;
	int bnum; 
public:
	bULLET()
	{
		c_id = -1;
		c_pos = { 0.0f,0.0f,0.0f };
		m_state = false;
		//c_right = { 0.0f,0.0f,0.0f };
		//c_up = { 0.0f,0.0f,0.0f };
		c_look = { 0.0f,0.0f,0.0f };
		//_hp = 0;
		_speed = 0;
		//name[0] = 0;
		bullet_size = 1.0;
		bnum = 0;
	}
};