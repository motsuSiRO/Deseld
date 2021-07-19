#pragma once
#include "Physics.h"
#include "Mo2Model.h"
#include "Animation.h"
#include "Vector3D.h"
#include "StateMachine.h"
#include <map>

class Player : public Physics
{
public:
	Player() :Physics() {};
	~Player() = default;

	Mo2Lib::Float3 right;
	Mo2Lib::Float3 up;
	Mo2Lib::Float3 forward;
	Mo2Lib::Float3 scale;
	Mo2Lib::Float3 input_vec;
	//Mo2Lib::Float2 key_vec;
	Mo2Lib::Float2 cam_angle;
	Mo2Lib::Float2 prev_angle;
	Mo2Lib::Float3 fire_pos;
	Mo2Lib::Float3 target_pos;
	float HALF_CAM_ANGLE_X = DirectX::XMConvertToRadians(70.f);
	bool is_moving = false;
	bool is_ground = true;
	float jump;
	float jump_time;
	float MAX_JUMP_TIME;
	float vec_angle;
	bool isAiming;
	float aim_timer;
	std::unique_ptr<Mo2Lib::Model> model;
	Mo2Lib::Animator anim;


#if 0
	enum ANIM_STATE
	{
		TPose = 0,
		IDLE,
		RUN,
		MOVE_L,
		MOVE_R,
		JUMP_S,
		JUMP_L,
		JUMP_E,
		DODGEROLL,
		FIRE,
		MAX_ANIM,
	};
#else
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
	};
#endif

	//struct Current_Animation
	//{
	//	int called;
	//	float anim_spd;
	//	float blend_spd;
	//	bool loop;
	//};
	//Current_Animation c_anim;

	void Init();
	void Update(float dt);
	//void Render();
	Mo2Lib::Model& RenderRdy();
	void ImGui();
	void UpdOrinetation(const Mo2Lib::Float3& vec);

	void InputDirection();
	void MoveXZ();
	void Jump();
	void CtrlFreecam();
	void CtrlAimcam();

	//void Call_Animation(int index, float anim_spd, float blend_spd, bool loop);

	//State_Data
	enum STATE_TYPE
	{
		MOVING_STATE = 0,
		AIM_STATE,
		DODGE_STATE,
	};


	int current_state_index;
	//StateMachine<Player>* GetFSM() { return fsm.get(); }
	std::map<int, std::shared_ptr<State<Player>>> state_list;
	std::unique_ptr<StateMachine<Player>> fsm;
};

class Moving_State : public State<Player>
{
	void Begin(Player* p);
	void Execute(Player* p);
	void End(Player* p);
};

class Aiming_State : public State<Player>
{
	void Begin(Player* p);
	void Execute(Player* p);
	void End(Player* p);
};

class Dodge_State : public State<Player>
{
	void Begin(Player * p);
	void Execute(Player* p);
	void End(Player* p);
};


