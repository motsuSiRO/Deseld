#include "GhostShip03.h"
#include "GameSystem.h"

#include "model_renderer.h"
#include "Camera.h"
#include "Bullet.h"
#include "Input.h"
//------------Component-----------------------
#include "Physics.h"
#include "PlayerControl.h"
#include "Firearm.h"
#include "ColliderComponents.h"
//--------------------------------------------

namespace GShip
{
	Mo2Lib::Float3 arm_pos;
	Physics2* physics;
	Transform* trans;
	PlayerControl* pctrl;
	BoxComponent* box;
}
using namespace GShip;

//-----------------------------------------------------
//
//		Constant term
//
//-----------------------------------------------------
const float MAX_WALK_SPEED = 5000.f;
const float INIT_MAX_DASH_VELOCITY = 600.f;
const float INIT_MAX_AIM_VELOCITY = 300.f;
const float INIT_MAX_VELOCITY = 600.f;
const float INIT_MASS = 40.f;
const float INIT_G_SCALE = 200.f;
const float JUMP_POW = 150.f;
const float JUMP_RESPTION_TIME = 0.1f;


//-----------------------------------------------------
//
//		Action State
//
//-----------------------------------------------------
class PL_Idle : public State<GhostShip03>
{
	void Begin(GhostShip03* p)
	{
		p->anim.data.anim_spd = physics->MAX_VELOCITY / INIT_MAX_VELOCITY;
	}

	void Execute(GhostShip03* p)
	{

		if (pctrl->dir_vec != 0)
		{
			fsm->ChangeState(GhostShip03::PL_MOVE);
		}
		else if (pctrl->Pressed("Firstary"))
		{
			fsm->ChangeState(GhostShip03::PL_ATK);

		}
		else if (pctrl->Pressed("Dodge"))
		{
			fsm->ChangeState(GhostShip03::PL_DODGE);
		}
		else
		{
			p->anim.PlayBlendAnim(GhostShip03::IDLE);
		}
	}

	void End(GhostShip03* p)
	{

	}

};

class PL_Move : public State<GhostShip03>
{
	float move_speed = 0.f;
	bool ismoving;

	void Begin(GhostShip03* p)
	{
		p->anim.data.anim_spd = physics->MAX_VELOCITY / INIT_MAX_VELOCITY;

		physics->mass = INIT_MASS;
		physics->limit_velocity = true;
		physics->dynamic_friction = 10.f;
		physics->MAX_VELOCITY = INIT_MAX_DASH_VELOCITY;
		physics->MAX_MOVE_SPEED = MAX_WALK_SPEED;

		move_speed = physics->MAX_MOVE_SPEED * physics->mass;

		Mo2Lib::Vec3 i_vec = p->InputDirection();
		physics->AddForce(i_vec * move_speed);
	}

	void Execute(GhostShip03* p)
	{
		p->LookForward();
		p->MoveXZ(move_speed);


		p->anim.data.anim_spd = physics->MAX_VELOCITY / INIT_MAX_VELOCITY;
		p->anim.PlayBlendAnim(GhostShip03::RUN, true, true);


		if (pctrl->dir_vec.LengthSq() <= 0)
		{
			fsm->ChangeState(GhostShip03::PL_IDLE);
		}
		else if (pctrl->Pressed("Firstary"))
		{
			fsm->ChangeState(GhostShip03::PL_ATK);
		}
		else if (pctrl->Pressed("Dodge"))
		{
			fsm->ChangeState(GhostShip03::PL_DODGE);
		}

	}

	void End(GhostShip03* p)
	{

	}

};


class PL_Attack : public State<GhostShip03>
{
	float move_speed = 0.f;
	float timer;
	void Begin(GhostShip03* p)
	{
		p->anim.data.anim_spd = 1.f;
		p->anim.PlayBlendAnim(GhostShip03::ATTACK, false);
	}

	void Execute(GhostShip03* p)
	{
		//p->anim.data.anim_spd += 1.f + Mo2System->delta_time;

		if (pctrl->Pressed("Dodge"))
		{
			fsm->ChangeState(GhostShip03::PL_DODGE);
		}

		if (p->anim.data.end_anim)
		{
			if (pctrl->dir_vec != 0)
			{
				fsm->ChangeState(GhostShip03::PL_MOVE);
			}

			else if (pctrl->dir_vec.LengthSq() == 0.f)
			{
				fsm->ChangeState(GhostShip03::PL_IDLE);
			}

		}
	}
	void End(GhostShip03* p)
	{

	}
};


class PL_Dodge : public State<GhostShip03>
{
	float timer;
	void Begin(GhostShip03* p)
	{
		physics->MAX_VELOCITY = INIT_MAX_VELOCITY * 1.5f;
		physics->MAX_MOVE_SPEED = MAX_WALK_SPEED * 5.f;
		timer = 0.5f;

		p->anim.data.anim_spd = physics->MAX_VELOCITY / INIT_MAX_VELOCITY;
		p->anim.PlayAnim(GhostShip03::DODGE, false);
	}

	void Execute(GhostShip03* p)
	{
		p->LookForward();
		Mo2Lib::Float3 dir = { sinf(trans->rotate.y), 0.f, cosf(trans->rotate.y) };
		physics->AddForce(dir * physics->MAX_MOVE_SPEED * physics->mass);
		
		
		if (p->anim.data.end_anim)
		{
			if (pctrl->dir_vec != 0)
			{
				p->anim.PlayAnim(GhostShip03::RUN, true);
				fsm->ChangeState(GhostShip03::PL_MOVE);
			}

			else if (pctrl->dir_vec.LengthSq() == 0.f)
			{
				physics->velocity = { 0.f,0.f,0.f };
				p->anim.PlayAnim(GhostShip03::IDLE, true);
				fsm->ChangeState(GhostShip03::PL_IDLE);
			}

		}
	}

	void End(GhostShip03* p)
	{

	}
};


//---------------------------------------------------------------------
//
//			Common Function
//
//---------------------------------------------------------------------
void GhostShip03::Start()
{
	trans = parent->GetComponent<Transform>();
	physics = parent->GetComponent<Physics2>();
	pctrl = parent->GetComponent<PlayerControl>();
	box = parent->AddComponent<BoxComponent>();
	//gun = Parent->GetComponent<Firearm>();

	fsm = std::make_unique<StateMachine<GhostShip03>>(this);
	fsm->AddState(PL_IDLE, std::make_shared<PL_Idle>())
		.AddState(PL_MOVE, std::make_shared<PL_Move>())
		.AddState(PL_ATK, std::make_shared<PL_Attack>())
		.AddState(PL_DODGE, std::make_shared<PL_Dodge>());

	fsm->SetCurrentState(PL_IDLE);


	phong = std::make_shared<ShaderEx>();
	phong->Create(L"model_vs", L"model_ps");


	const char* fbx_filename;
	std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();

	//choose binary or fbx load when start up function
	//choose save or not to binary when end function
	fbx_filename = "./Data/Assets/GhostShip/GhostShip03.fbx";
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_SKINNED_MODEL);
	fbx_filename = "./Data/Assets/GhostShip/Sword Idle.fbx";
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	fbx_filename = "./Data/Assets/GhostShip/Sword Run.fbx";
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	fbx_filename = "./Data/Assets/GhostShip/Sword Slash.fbx";
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	fbx_filename = "./Data/Assets/GhostShip/Rolling.fbx";
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol WalkBackward.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol sideStepL.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol sideStepR.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol AimUp.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol AimDown.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	model = std::make_unique<Mo2Lib::Model>(model_resource);
	model->InitializeAnimation(GhostShip03::IDLE);

	anim.Initialize();


}

void GhostShip03::Update()
{
	ismoving = false;

	//anim.data.anim_spd = physics->velocity.Length() / physics->MAX_VELOCITY;
	physics->Deceleration();

	fsm->Update(parent->delta_time);


	model->UpdateAnimation(&anim, parent->delta_time);

	box->trans.translate = model->GetNodes(0)->GetWorldPos();
	box->trans.rotate = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&model->GetNodes(0)->world_transform));
	box->trans.scale = { 30.f, 100.f, 30.f };
	arm_pos = model->GetNodes(26)->GetWorldPos();

	CAM_LIST()->main_cam->SetOrientation(trans->translate);
}

void GhostShip03::Draw()
{
	model->SetTransform(trans->translate, trans->GetFixedQuaternion(), trans->scale);

	Mo2Render().Draw(phong.get(), *model);
}

void GhostShip03::ImGui()
{
	std::string str, s;
	s = "##" + parent->GetID();
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
	str = "GhostShip03" + s;


	if (ImGui::CollapsingHeader(str.c_str()))
	{
		Super::ImGui();
		ImGui::SameLine();
		ImGui::Text("Visible");
		ImGui::NewLine();

		ImGui::Checkbox("Is_Moving", &ismoving);
		ImGui::NewLine();

		ImGui::Text("CurrentState");
		ImGui::Text(fsm->GetCurrentName());

		ImGui::NewLine();

		ImGui::NewLine();
		for (size_t i = 0; i < model->GetNodes().size(); i++)
		{
			ImGui::Text("%d %s", i, model->GetNodes().at(i).name);
		}

		anim.AnimImGui(std::string(typeid(GhostShip03).name()));
	}


}

//-----------------------------------------------------
//
//		Function
//
//-----------------------------------------------------
Mo2Lib::Vec3 GhostShip03::InputDirection()
{
	Mo2Lib::Vec3 input_vec = { 0.f, 0.f, 0.f };
	{
		float angle;
		float move_x;
		float move_z;

		Mo2Lib::Float2 key_vec = {};
		Mo2Lib::Float2 zero = Mo2Lib::Float2(0, 1.f);

		key_vec = pctrl->dir_vec;

		key_vec.Normalize();

		if (key_vec != 0)
		{
			ismoving = true;
			angle = acosf(key_vec.Dot(zero));
			if (key_vec.Cross(zero) < 0.f)
			{
				angle = DirectX::XM_2PI - angle;
			}

			angle -= CAM_LIST()->main_cam->GetAnglerY();

			if (DirectX::XM_2PI < angle)
			{
				angle -= DirectX::XM_2PI;
			}

			move_z = cosf(angle);
			move_x = -sinf(angle);


			input_vec.x += move_x;
			input_vec.z += move_z;
		}

		return input_vec;
	}


}

void GhostShip03::LookForward()
{

	Mo2Lib::Vec3 t_for, axis;
	float now_angle = 0.f;

	Mo2Lib::Vec3 forward = { 0.f, 0.f, 1.f };
	Mo2Lib::Vec3 v = { physics->velocity.x, 0.f, physics->velocity.z };
	if (v == 0.f)return;

	t_for = v.GetNormalize();//Mo2Lib::Float3(INPUT.StickVector().x, 0.f, INPUT.StickVector().y);

	now_angle = forward.Dot(t_for);
	if (now_angle <= -1.0f)now_angle = -1.0f;
	now_angle = acosf(now_angle);

	if (forward.Cross(v).y < 0.f)
	{
		now_angle = DirectX::XM_2PI - now_angle;
	}
	trans->rotate.y = now_angle;

}


void GhostShip03::MoveXZ(float speed)
{
	Mo2Lib::Vec3 i_vec = InputDirection();
	physics->AddForce(i_vec * speed);

}