#include "Scene.h"
#include "../core/Logger.h"
#include "../core/util/format.h"
Scene::Scene(int id):_id(id)
{
}


Scene::~Scene(void)
{
}

void Scene::Init()
{

}

void Scene::Fina()
{

}

int Scene::Update(int now)
{
	LOG_ERROR(fmt::format("scene:{} running",_id));
	return 0;
}