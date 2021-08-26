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
