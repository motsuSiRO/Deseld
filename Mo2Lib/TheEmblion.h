#pragma once
#include "Enemy.h"
#include "Mo2Model.h"
#include "ComponentObject.h"
#include "Shader.h"

enum THE_EMBLION_STATE
{
	TheE_IDLE = 0,
	TheE_APPROACH,
	TheE_SIDEMOVE,

	TheE_SPINATK,
	TheE_RAPID_FIRE,
	TheE_ENERGY_FIRE,
	TheE_FALL_FIRE,
};


class TheEmblion : public Component
{
public:

	void Start();
	void Update();
	void Draw();
	void ImGui();

private:
	std::unique_ptr <Mo2Lib::Model> model;
	std::shared_ptr<ShaderEx> phong;

};