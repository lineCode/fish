#pragma once
#include "Common.h"
class AoiObject
{
public:
	AoiObject(int id);
	~AoiObject(void);

	float GetX()
	{
		return _pos.x;
	}

	float GetY()
	{
		return _pos.y;
	}


protected:
	int _id;
	Pos _pos;
};

