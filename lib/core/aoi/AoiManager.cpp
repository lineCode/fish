#include "AoiManager.h"
#include "AoiTower.h"
#include "AoiObject.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRANSPOS(val,max) ((int)val/max)

AoiManager::AoiManager(int id,int width,int height,int tileSize)
{
	_id = id;
	_width = width;
	_height = height;
	_maxX = width / tileSize - 1;
	_maxY = height / tileSize - 1;

	_towers = (AoiTower***)malloc(sizeof(AoiTower**) * _maxX * _maxY);
	for (int i = 0;i <= _maxX;i++)
	{
		_towers[i] = (AoiTower**)malloc(sizeof(AoiTower*) * _maxY);
		for (int j = 0;j <= _maxY;j++)
		{
			_towers[i][j] = new AoiTower();
		}

	}
}


AoiManager::~AoiManager(void)
{
}

int AoiManager::AddObject(AoiObject* obj)
{
	float posX = obj->GetX();
	float posY = obj->GetY();

	int x = TRANSPOS(posX,_width);
	int y = TRANSPOS(posY,_height);

	return _towers[x][y]->AddObject(obj);
}

int AoiManager::RemoveObject(AoiObject* obj)
{
	float posX = obj->GetX();
	float posY = obj->GetY();

	int x = TRANSPOS(posX,_width);
	int y = TRANSPOS(posY,_height);

	return _towers[x][y]->RemoveObject(obj);
}

int AoiManager::AddWatcher(AoiObject* obj,int range)
{
	if (range < 0)
		return -1;

	range = range > 5? 5:range;

	float posX = obj->GetX();
	float posY = obj->GetY();

	int x = TRANSPOS(posX,_width);
	int y = TRANSPOS(posY,_height);

	Pos start;
	Pos end;

	GetRange(x,y,range,start,end);

	for (int i = start.x;i <= end.x;i++)
	{
		for (int j = start.y;j <= end.y;j++)
		{
			_towers[i][j]->AddWatcher(obj);
		}
	}
	return 0;
}

int AoiManager::RemoveWatcher(AoiObject* obj,int range)
{
	if (range < 0)
		return -1;

	range = range > 5? 5:range;

	float posX = obj->GetX();
	float posY = obj->GetY();

	int x = TRANSPOS(posX,_width);
	int y = TRANSPOS(posY,_height);

	Pos start;
	Pos end;

	GetRange(x,y,range,start,end);

	for (int i = start.x;i <= end.x;i++)
	{
		for (int j = start.y;j <= end.y;j++)
		{
			_towers[i][j]->RemoveWatcher(obj);
		}
	}
	return 0;
}

int AoiManager::Update(AoiObject* obj)
{
	return 0;
}

void AoiManager::GetRange(int x,int y,int range,Pos& start,Pos& end)
{

}