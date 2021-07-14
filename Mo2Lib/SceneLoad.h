#pragma once
#include "Scene.h"
#include "Sprite.h"



class SceneLoad : public Scene
{
private:
	std::unique_ptr<Scene> nextScene;
	std::unique_ptr<Sprite> spr;
	// Now Loading...�A�j���[�V�����p
	float angle = 0.0f;
	DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	float timer = 0.0f;
	int counter = 0;
public:
	SceneLoad(Scene* nextScene)
	{	// ���[�f�B���O��ɑJ�ڂ���V�[����ێ�
		this->nextScene.reset(nextScene);
	}
	virtual ~SceneLoad()
	{

	}

	void Initialize() override;
	void Update(float elapsed_time) override;
	void Render() override;
private:
	static void LoadingThread(SceneLoad* scene);

};
