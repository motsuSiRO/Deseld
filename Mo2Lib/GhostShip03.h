#pragma once
#include "Mo2Model.h"
#include "Shader.h"
#include "StateMachine.h"
#include "Animation.h"
#include "ComponentObject.h"

namespace GShip
{
	extern Mo2Lib::Float3 arm_pos;
}

class GhostShip03 : public Component
{
public:
	bool ismoving;
	//Firearm* gun;
	std::unique_ptr<StateMachine<GhostShip03>> fsm;
	Mo2Lib::Animator anim;

	enum STATE_LIST
	{
		PL_IDLE = 0,
		PL_MOVE,
		PL_ATK,
		PL_DODGE,
	};

	enum ANIM_STATE
	{
		TPose = 0,
		IDLE,
		RUN,
		ATTACK,
		DODGE,
		MAX_ANIM,
	};


	void Start();
	void Update();
	void Draw();
	void ImGui();

	void MoveXZ(float speed);
	Mo2Lib::Vec3 InputDirection();
	void LookForward();

	Mo2Lib::Model* GetModel() { return model.get(); }
private:
	std::unique_ptr <Mo2Lib::Model> model;
	std::shared_ptr<ShaderEx> phong;
};
