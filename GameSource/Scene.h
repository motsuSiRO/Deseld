#pragma once

#include <memory>
#include "ComponentObject.h"

class Scene
{
private:
	// SceneLoadとSceneManagerのみで設定できるフラグを定義
	friend class SceneLoad;
	friend class SceneManager;
	bool initialized = false;//ロード終了FG

protected:
	std::list<std::shared_ptr<Object>> obj_list;

public:
	Scene() {}
	virtual ~Scene() {}
	virtual void Initialize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render() = 0;
	std::list<std::shared_ptr<Object>>& GetObjects() { return obj_list; }
};







class SceneTitle :public Scene
{
private:


public:
	SceneTitle() {}
	virtual ~SceneTitle() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render() override;
	void ImGui();
};


//----------------------------------------------------
//		SceneManager(管理)クラス
//----------------------------------------------------
class SceneManager
{
private:
	std::unique_ptr<Scene> current_scene;
	std::unique_ptr<Scene> previous_scene;

private:
	SceneManager() {}
	~SceneManager() {}

public:
	static SceneManager& GetInstance()
	{
		static SceneManager inst;
		return inst;
	}

	void Update(float elapsedTime);
	void Render();
	void ChangeScene(Scene* new_scene);

	std::list<std::shared_ptr<Object>>& GetObjects() { return current_scene->GetObjects(); }
};

#define Mo2Scene SceneManager::GetInstance()