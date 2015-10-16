#ifndef UPDATERMANAGER_H
#define UPDATERMANAGER_H

#include <vector>
#include <map>
#include "../core/util/Singleton.h"

class Updater;

class UpdaterManager : public Singleton<UpdaterManager>
{
public:
	typedef std::vector<Updater*> Updaters;
	typedef std::map<int,Updater*> UpdaterMap;
public:
	UpdaterManager(void);
	~UpdaterManager(void);

	void Add(Updater* updater);
	void Remove(int id);

	int Update(int now);

private:
	int _inc;
	UpdaterMap _updaters;
	Updaters _newers;
	std::vector<int> _diers;;
};

#endif