#pragma once

#include <memory>

class Scene
{
private:
	// SceneLoad��SceneManager�݂̂Őݒ�ł���t���O���`
	friend class SceneLoad;
	friend class SceneManager;
	bool initialized = false;//���[�h�I��FG

public:
	Scene() {}
	virtual ~Scene() {}
	virtual void Initialize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render() = 0;
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
//		SceneManager(�Ǘ�)�N���X
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

};

#define SceneMgr SceneManager::GetInstance()