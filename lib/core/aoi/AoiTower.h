#pragma once
class AoiObject;
class AoiTower
{
public:
	AoiTower(void);
	~AoiTower(void);

	int AddObject(AoiObject* obj);
	int RemoveObject(AoiObject* obj);

	int AddWatcher(AoiObject* obj);
	int RemoveWatcher(AoiObject* obj);
};

