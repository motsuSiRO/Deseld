#include "Scene.h"

void SceneManager::Update(float elapsedTime)
{
	current_scene->Update(elapsedTime);
}

void SceneManager::Render()
{
	current_scene->Render();
}


void SceneManager::ChangeScene(Scene* new_scene)
{
	//currentScene.reset(new_scene);

	//currentScene->Initialize();

	previous_scene.reset(current_scene.release());

	current_scene.reset(new_scene);
	// 2重初期化されないようにする。
	if (!current_scene->initialized)
	{
		current_scene->Initialize();
		current_scene->initialized = true;
	}

}


//void SceneManager::ChangeScene(Scene* new_scene, int load_list)
//{
//	previous_scene.reset(current_scene.release());
//
//	current_scene.reset(new_scene, load_list);
//	// 2重初期化されないようにする。
//	if (!current_scene->initialized)
//	{
//		current_scene->Initialize();
//		current_scene->initialized = true;
//	}
//
//}
