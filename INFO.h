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
	}

};