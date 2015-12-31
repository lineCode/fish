#pragma once
class AoiObject;
class AoiManager;
class AoiTower
{
	friend AoiManager;
public:
	AoiTower(void);
	~AoiTower(void);

	int AddObject(AoiObject* obj);
	int RemoveObject(AoiObject* obj);

	int AddWatcher(AoiObject* obj);
	int RemoveWatcher(AoiObject* obj);

private:
	AoiObject* _objHead;
	AoiObject* _objTail;
	AoiObject* _watcherHead;
	AoiObject* _watcherTail;
};

