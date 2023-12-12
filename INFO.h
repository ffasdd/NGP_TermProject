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

	// 총알 
	int bullet_power;
	int bnum;

	// 회전 -> 왼쪽 회전 : 0, 오른쪽 회전 : 1, 회전x : 2
	int rotNum;
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
		bullet_power = 10;
		bnum = 0;
		rotNum = 2;
	}

};
// **** 총알 클래스
class bULLET
{
public:
	int c_id;      // 총알 인덱스
	bool m_state;
	XMFLOAT3 c_pos;
	//XMFLOAT3 c_right;
	//XMFLOAT3 c_up;
	XMFLOAT3 c_look;
	int _speed;
	int who;
	//char name[20];

	// 총알 사이즈
	float bullet_power;
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
		bullet_power = 10.0;
		bnum = 0;
	}
};