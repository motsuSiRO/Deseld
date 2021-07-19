#pragma once
#include "Mo2Model.h"
#include "Shader.h"
#include "StateMachine.h"
#include "Animation.h"
#include "ComponentObject.h"

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
		PL_AIM,
		PL_DODGE,
	};

	enum ANIM_STATE
	{
		TPose = 0,
		IDLE,
		RUN,
		MOVE_F,
		MOVE_B,
		MOVE_L,
		MOVE_R,
		DODGEROLL,
		AIMUP,
		AIMDOWN,
		MAX_ANIM,
	};

	enum ANIM_LAYER
	{
		//FULLBODY,
		SPINE,
		HEAD,
		L_ARM,
		R_ARM,
		L_FOOT,
		R_FOOT,
		FULLBODY,
	};

	void Start();
	void Update();
	void Draw();
	void ImGui();

	void MoveXZ(float speed);
	Mo2Lib::Vec3 InputDirection();
	void LookForward();
private:
	std::unique_ptr <Mo2Lib::Model> model;
	std::shared_ptr<ShaderEx> phong;
};
