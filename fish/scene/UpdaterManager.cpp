#include "UpdaterManager.h"
#include "Updater.h"

template <> 
UpdaterManager * Singleton<UpdaterManager>::singleton_ = 0;

UpdaterManager::UpdaterManager(void)
{
}


UpdaterManager::~UpdaterManager(void)
{
}

void UpdaterManager::Add(Updater* updater)
{
	_newers.push_back(updater);
	updater->SetId(++_inc);
}

void UpdaterManager::Remove(int id)
{
	_diers.push_back(id);
}

int UpdaterManager::Update(int now)
{
	Updaters::iterator iter = _newers.begin();
	for (;iter != _newers.end();iter++)
	{
		Updater* updater = *iter;
		updater->Init();
		_updaters[updater->GetId()] = updater;
	}

	_newers.clear();

	std::vector<int>::iterator iterDie = _diers.begin();
	for (;iterDie != _diers.end();iterDie++)
	{
		int id = *iterDie;
		Updater* updater = _updaters[id];
		updater->Fina();
		_updaters.erase(id);
	}

	_diers.clear();

	UpdaterMap::iterator iterMap = _updaters.begin();
	for (;iterMap != _updaters.end();iterMap++)
	{
		iterMap->second->Update(now);
	}

	return 0;
}