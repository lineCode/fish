#pragma once
#include "Common.h"
class AoiObject
{
	friend class AoiTower;
	friend class AoiManager;
public:
	AoiObject(int id);
	~AoiObject(void);

	float GetX()
	{
		return _pos.x;
	}

	void SetX(float x)
	{
		_pos.x = x;
	}

	float GetY()
	{
		return _pos.y;
	}

	void SetY(float y)
	{
		_pos.y = y;
	}

	void SetRange(int range)
	{
		_range = range;
	}

	virtual void OnEnter(AoiObject* obj) = 0;
	virtual void OnLeave(AoiObject* obj) = 0;

private:
	AoiObject* _objPrev;
	AoiObject* _objNext;
	AoiObject* _watcherPrev;
	AoiObject* _watcherNext;
	int _id;
	Pos _pos;
	int _range;
};

