#include "SceneManager.h"

SceneManager* SceneManager::instance = 0;

SceneManager::SceneManager()
{
	currSceneID = 0;
}

SceneManager::~SceneManager()
{
	for (int i = 0; i < sceneStore.size(); ++i)
	{
		delete sceneStore[i];
	}
}

SceneManager* SceneManager::getInstance()
{
	if (!instance) //check for null
	{
		instance = new SceneManager();
		return instance;
	}
	else
	{
		return instance;
	}
}

int SceneManager::getcurrSceneID()
{
	return currSceneID;
}

void SceneManager::setcurrSceneID(int number)
{
	currSceneID = number;	
}

int SceneManager::getvSize()
{
	return sceneStore.size();
}

Scene* SceneManager::getScene()
{
	return sceneStore[currSceneID];
}

void SceneManager::AddScene(Scene* scene)
{
	sceneStore.push_back(scene);
}

void SceneManager::Update(double ElapsedTime)
{
	sceneStore[currSceneID]->Update(ElapsedTime);
	sceneStore[currSceneID]->Render();
}


