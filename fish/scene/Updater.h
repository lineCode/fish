#ifndef UPDATER_H
#define UPDATER_H
#include "updatermanager.h"

class Updater
{
public:
	Updater(void)
	{

	}

	~Updater(void)
	{

	}

	virtual void SetId(int id)
	{
		_id = id;
	}

	virtual int GetId()
	{
		return _id;
	}

	virtual void Run()
	{
		UpdaterManager::GetSingleton().Add(this);
	}

	virtual void Destroy()
	{
		UpdaterManager::GetSingleton().Remove(_id);
	}

	virtual void Init() = 0;
	virtual void Fina() = 0;
	virtual int Update(int now) = 0;

private:
	int _id;
};

#endif