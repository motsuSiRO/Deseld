#pragma once

#include "Scene.h"

#include "model.h"
#include "model_renderer.h"
#include "Sprite.h"
#include "GeoPrimitive.h"
#include "Reticule.h"
#include "SkyBox.h"
#include "Shader.h"

class SceneGame :public Scene
{
private:
	std::unique_ptr<Mo2Lib::Model>		model;
	std::unique_ptr<Mo2Lib::Model>		model2;
	std::unique_ptr<SkyBox>				sky_box;

	std::unique_ptr<Reticule>			reticule;

	std::unique_ptr<ShaderEx>			skinned_bp;
	std::unique_ptr<ShaderEx>			static_bp;
	std::unique_ptr<ShaderEx>			skybox_NoL;
	std::unique_ptr<ShaderEx>			primi_shader;

	bool visiblity = false;
	float rad;
public:
	SceneGame() {}
	virtual ~SceneGame()
	{

	};
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render() override;
	void ImGui();
};
