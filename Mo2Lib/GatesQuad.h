#pragma once
#include "Enemy.h"
#include "Mo2Model.h"
#include "ComponentObject.h"
#include "Shader.h"

enum GATESQUAD_STATE
{
	QUAD_IDLE = 0,
	QUAD_APPROACH,
	QUAD_SIDEMOVE,

	QUAD_BODYATK,
	QUAD_EXPLOSION,
};

class GateQuad : public Component
{
public:
	Transform* trans;

	void Start();
	void Update();
	void Draw();
	void ImGui();

private:
	std::unique_ptr <Mo2Lib::Model> model;
	std::shared_ptr<ShaderEx> phong;

};

