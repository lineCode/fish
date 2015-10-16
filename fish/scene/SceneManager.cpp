#include "SceneManager.h"


SceneManager::SceneManager(void)
{
}


SceneManager::~SceneManager(void)
{
}

int SceneManager::Init()
{
	Scene* scene = new Scene(1);
	_sceneMgr[1] = scene;
	scene->Run();
	return 0;
}
