#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H
#include "scene.h"
#include <map>
class SceneManager
{
public:
	SceneManager(void);
	~SceneManager(void);

	int Init();

private:
	std::map<int,Scene*> _sceneMgr;
};

#endif
