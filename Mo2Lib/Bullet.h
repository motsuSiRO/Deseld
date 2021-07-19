#pragma once
#include "ComponentObject.h"
#include "Mo2Model.h"
#include "Shader.h"

class Bullet : public Component
{
public:
	Transform* trans;
	Mo2Lib::Vec3 forward;

	void Start();
	void Update();
	void Draw();
	void ImGui();

private:
	std::unique_ptr <Mo2Lib::Model> model;
	std::shared_ptr<ShaderEx> phong;
	float forward_speed;
};