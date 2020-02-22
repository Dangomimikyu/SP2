#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <vector>
#include "Scene.h"

class SceneManager
{
private:
	SceneManager();
	static SceneManager* instance;

	std::vector<Scene*> sceneStore;
	int currSceneID = 0;
	//int nextSceneID = 0;

public:
	~SceneManager();
	static SceneManager* getInstance();

	int getcurrSceneID();
	void setcurrSceneID(int number);

	int getvSize();

	Scene* getScene();
	void AddScene(Scene* scene);
	void Update(double ElapsedTime);
};

#endif
