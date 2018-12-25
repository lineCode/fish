#include "AoiManager.h"
#include "AoiTower.h"
#include "AoiObject.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRANSPOS(val,max) (((int)val)/max)

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

	obj->SetRange(range);

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
	_towers[x][y]->AddObject(obj);
	return 0;
}

int AoiManager::RemoveWatcher(AoiObject* obj)
{
	float posX = obj->GetX();
	float posY = obj->GetY();

	int x = TRANSPOS(posX,_width);
	int y = TRANSPOS(posY,_height);

	Pos start;
	Pos end;

	GetRange(x,y,obj->_range,start,end);

	for (int i = start.x;i <= end.x;i++)
	{
		for (int j = start.y;j <= end.y;j++)
		{
			_towers[i][j]->RemoveWatcher(obj);
		}
	}
	_towers[x][y]->RemoveObject(obj);
	return 0;
}

int AoiManager::UpdateObject(AoiObject* obj,float nx,float ny)
{
	float ox = obj->GetX();
	float oy = obj->GetY();
	
	obj->SetX(nx);
	obj->SetY(ny);

	int oxIndex = TRANSPOS(ox,_width);
	int oyIndex = TRANSPOS(oy,_height);
	
	_towers[oxIndex][oyIndex]->RemoveObject(obj);

	AoiObject* tmp = _towers[oxIndex][oyIndex]->_watcherHead;
	while (tmp != NULL)
	{
		tmp->OnLeave(obj);
		tmp = tmp->_watcherNext;
	}

	int nxIndex = TRANSPOS(nx,_width);
	int nyIndex = TRANSPOS(ny,_height);

	_towers[nxIndex][nyIndex]->AddObject(obj);

	tmp = _towers[nxIndex][nyIndex]->_watcherHead;
	while (tmp != NULL)
	{
		tmp->OnEnter(obj);
		tmp = tmp->_watcherNext;
	}
	return 0;
}

int AoiManager::UpdateWatcher(AoiObject* obj,float nx,float ny)
{
	float ox = obj->GetX();
	float oy = obj->GetY();

	obj->SetX(nx);
	obj->SetY(ny);

	int oxIndex = TRANSPOS(ox,_width);
	int oyIndex = TRANSPOS(oy,_height);
	int nxIndex = TRANSPOS(nx,_width);
	int nyIndex = TRANSPOS(ny,_height);

	Pos oStart,nStart;
	Pos oEnd,nEnd;

	GetRange(oxIndex,oyIndex,obj->_range,oStart,oEnd);
	GetRange(nxIndex,nyIndex,obj->_range,nStart,nEnd);

	for (int i = oStart.x;i <= oEnd.x;i++)
	{
		for (int j = oStart.y;j <= oEnd.y;j++)
		{
			if (i >= nStart.x && i <= nEnd.x && j >= nStart.y && j <= nEnd.y)
				continue;

			_towers[i][j]->RemoveWatcher(obj);

			AoiObject* tmp = _towers[i][j]->_objHead;
			while (tmp != NULL)
			{
				obj->OnLeave(tmp);
				tmp = tmp->_objNext;
			}
		}
	}

	for (int i = nStart.x;i <= nEnd.x;i++)
	{
		for (int j = nStart.y;j <= nEnd.y;j++)
		{
			if (i >= oStart.x && i <= oEnd.x && j >= oStart.y && j <= oEnd.y)
				continue;

			_towers[i][j]->AddWatcher(obj);

			AoiObject* tmp = _towers[i][j]->_objHead;
			while (tmp != NULL)
			{
				obj->OnEnter(tmp);
				tmp = tmp->_objNext;
			}
		}
	}

	return 0;
}

void AoiManager::GetRange(int x,int y,int range,Pos& start,Pos& end)
{

}