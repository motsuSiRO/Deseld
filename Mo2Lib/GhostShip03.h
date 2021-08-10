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
		AFK_APPEAL,
		RUN,
		SLASH1, 
		SLASH2,
		SLASH3, 
		DASH_SLASH,
		COUNTER,
		JUMP_SLASH,
		JUMP,
		DODGE,
		DEATH,
		MAX_ANIM,
	};


	void Start();
	void Update();
	void Draw();
	void ImGui();

	void MoveXZ();
	Mo2Lib::Vec3 InputDirection();
	void LookForward();

	//Mo2Lib::Model* GetModel() { return model.get(); }
private:
	std::unique_ptr<StateMachine<GhostShip03>> fsm;

	class GShipImpl;
	std::shared_ptr<GShipImpl> pimpl;
public:
	GShipImpl* GetPrivate() { return pimpl.get(); }
};
