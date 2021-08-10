#pragma once
#include "Enemy.h"
#include "Mo2Model.h"
#include "ComponentObject.h"
#include "Shader.h"
#include "Animation.h"
#include "StateMachine.h"

enum THE_EMBLION_STATE
{
	TheE_IDLE = 0,
};


class TheEmblion : public Component
{
public:
	std::unique_ptr<StateMachine<TheEmblion>> fsm;

	void Start();
	void Update();
	void Draw();
	void ImGui();

private:
	std::unique_ptr <Mo2Lib::Model> model;
	std::shared_ptr<ShaderEx> phong;
};