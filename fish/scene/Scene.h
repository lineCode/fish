#ifndef SCENE_H
#define SCENE_H

#include "updater.h"

class Scene : public Updater
{
public:
	Scene(int id);
	~Scene(void);
	
	void Init();
	
	void Fina();

	int Update(int now);

private:
	int _id;
};

#endif