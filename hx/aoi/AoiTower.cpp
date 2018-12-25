#include "AoiTower.h"
#include "AoiObject.h"
#include <stddef.h>

AoiTower::AoiTower(void)
{
}


AoiTower::~AoiTower(void)
{
}

int AoiTower::AddObject(AoiObject* obj)
{
	if (_objTail == NULL)
		_objHead = _objTail = obj;
	else
	{
		this->_objTail->_objNext = obj;
		obj->_objPrev = this->_objTail;
		this->_objTail = obj;
	}
	return 0;
}

int AoiTower::RemoveObject(AoiObject* obj)
{
	if (obj->_objPrev != NULL)
		obj->_objPrev->_objNext = obj->_objNext;
	if (obj->_objNext != NULL)
		obj->_objNext->_objPrev = obj->_objPrev;

	if (obj == _objHead)
		_objHead = obj->_objNext;
	return 0;
}

int AoiTower::AddWatcher(AoiObject* obj)
{
	if (_watcherTail == NULL)
		_watcherHead = _watcherTail = obj;
	else
	{
		this->_watcherTail->_watcherNext = obj;
		obj->_watcherPrev = this->_watcherTail;
		this->_watcherTail = obj;
	}
	return 0;
}

int AoiTower::RemoveWatcher(AoiObject* obj)
{
	if (obj->_watcherPrev != NULL)
		obj->_watcherPrev->_watcherNext = obj->_watcherNext;
	if (obj->_watcherNext != NULL)
		obj->_watcherNext->_watcherPrev = obj->_watcherPrev;

	if (obj == _watcherHead)
		_watcherHead = obj->_watcherNext;
	return 0;
}
