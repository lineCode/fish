#pragma once
#include "Common.h"

class AoiTower;
class AoiObject;
class AoiManager
{
public:
	AoiManager(int id,int width,int height,int tileSize);
	~AoiManager(void);

	int AddObject(AoiObject* obj);
	int RemoveObject(AoiObject* obj);

	int AddWatcher(AoiObject* obj,int range);
	int RemoveWatcher(AoiObject* obj,int range);

	int Update(AoiObject* obj);

	void GetRange(int x,int y,int range,Pos& start,Pos& end);

protected:
	int _id;
	int _width;
	int _height;
	int _maxX;
	int _maxY;

	AoiTower*** _towers;
};

