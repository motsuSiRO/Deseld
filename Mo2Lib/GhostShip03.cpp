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



class GhostShip03::GShipImpl
{
public:
	std::unique_ptr <Mo2Lib::Model> model;
	std::shared_ptr<ShaderEx> phong;
	Mo2Lib::Float3 arm_pos;
	Physics2* physics;
	Transform* trans;
	PlayerControl* pctrl;
	BoxComponent* box;
	SphereComponent* s0;

	void LoadModel()
	{
		//const char* fbx_filename;
		std::shared_ptr<ModelResource> model_resource;
		Mo2Lib::ModelLoader::GetInstance().Setup(/*true*/);

		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/GhostShip03.fbx");
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Idle.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Appeal_bunbun.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Run.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Slash1.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Slash2.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Slash3.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword DashSlash.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Counter.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword JumpSlash.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Jump.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Rolling.fbx", true);
		Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/GhostShip/Sword Death.fbx", true);
		
		model_resource = Mo2Lib::ModelLoader::GetInstance().Apply();
		model = std::make_unique<Mo2Lib::Model>(model_resource);

		Mo2Lib::ModelLoader::GetInstance().End();

		model->root_motion = (Mo2Lib::ROOT_MOTION)(Mo2Lib::NO_ROOT_MOTION);
		model->InitializeAnimation(GhostShip03::IDLE);

	}

	void LoadShader()
	{
		phong = std::make_shared<ShaderEx>();
		phong->Create(L"model_vs", L"model_ps");
	}

	Mo2Lib::Vec3 CulcDirection()
	{
		Mo2Lib::Vec3 dir = { sinf(trans->rotate.y), 0.f, cosf(trans->rotate.y) };

		return dir;
	}

	void AddMoveSpeed(Mo2Lib::Vec3& dir)
	{
		physics->AddMoveSpeed(dir);
	}

	void AddMoveSpeedToTheDirection()
	{
		physics->AddMoveSpeed(CulcDirection());
	}

	bool IsMoving()
	{
		if (pctrl->dir_vec != 0)return true;
		else return false;
	}

	void SetZeroVelocity()
	{
		physics->velocity = { 0.f,0.f,0.f };
	}

	void AdaptBoxTransform()
	{
		box->trans.translate = model->GetNodes(0)->GetWorldPos();
		box->trans.rotate = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&model->GetNodes(0)->world_transform));
		box->trans.scale = { 30.f, 100.f, 30.f };

	}

	void AdaptSphereTransform()
	{
		s0->trans.translate = trans->translate;
		s0->trans.scale = { 30.f, 30.f, 30.f };

	}

	void SetMainCamOrientation()
	{
		CAM_LIST()->main_cam->SetOrientation(trans->translate);
	}

	void AnimSpdByPhysics()
	{
		model->data.anim_spd = physics->MAX_VELOCITY / INIT_MAX_VELOCITY;
	}

	void DeffaultAnimSpd()
	{
		model->data.anim_spd = 1.f;
	}

	Mo2Lib::Vec3 GetVelocityXZ()
	{
		return Mo2Lib::Vec3(physics->velocity.x, 0.f, physics->velocity.z);
	}

	void PlayAnim(int index, bool loop = false, bool force = true)
	{
		model->PlayAnim(index, loop, force);
	}

	void PlayBlendAnim(int index, bool loop = false, bool force = true)
	{
		model->PlayBlendAnim(index, loop, force);
	}


	void ModelDraw()
	{
		model->SetTransform(trans->translate, trans->GetFixedQuaternion(), trans->scale);

		Mo2Render().Draw(phong.get(), *model);
	}
};


//-----------------------------------------------------
//
//		Action State
//
//-----------------------------------------------------
class PL_Idle : public State<GhostShip03>
{
	void Begin(GhostShip03* p)
	{
		//p->anim.data.anim_spd = p->GetPrivate()->physics->MAX_VELOCITY / INIT_MAX_VELOCITY;
		p->GetPrivate()->AnimSpdByPhysics();
	}

	void Execute(GhostShip03* p)
	{

		if (p->GetPrivate()->pctrl->dir_vec != 0)
		{
			fsm->ChangeState(GhostShip03::PL_MOVE);
		}
		else if (p->GetPrivate()->pctrl->Pressed("Firstary"))
		{
			fsm->ChangeState(GhostShip03::PL_ATK);

		}
		else if (p->GetPrivate()->pctrl->Pressed("Dodge"))
		{
			fsm->ChangeState(GhostShip03::PL_DODGE);
		}
		else
		{
			p->GetPrivate()->PlayBlendAnim(GhostShip03::IDLE);
		}
	}

	void End(GhostShip03* p)
	{

	}

};

class PL_Move : public State<GhostShip03>
{
	bool ismoving;

	void Begin(GhostShip03* p)
	{
		//p->anim.data.anim_spd = p->GetPrivate()->physics->MAX_VELOCITY / INIT_MAX_VELOCITY;

		p->GetPrivate()->physics->mass = INIT_MASS;
		p->GetPrivate()->physics->limit_velocity = true;
		p->GetPrivate()->physics->dynamic_friction = 10.f;
		p->GetPrivate()->physics->MAX_VELOCITY = INIT_MAX_DASH_VELOCITY;
		p->GetPrivate()->physics->MAX_MOVE_SPEED = MAX_WALK_SPEED;

		p->GetPrivate()->AnimSpdByPhysics();

		p->MoveXZ();
	}

	void Execute(GhostShip03* p)
	{
		p->LookForward();
		p->MoveXZ();


		//p->anim.data.anim_spd = p->GetPrivate()->physics->MAX_VELOCITY / INIT_MAX_VELOCITY;
		p->GetPrivate()->PlayBlendAnim(GhostShip03::RUN, true, true);


		if (p->GetPrivate()->pctrl->dir_vec.LengthSq() <= 0)
		{
			fsm->ChangeState(GhostShip03::PL_IDLE);
		}
		else if (p->GetPrivate()->pctrl->Pressed("Firstary"))
		{
			fsm->ChangeState(GhostShip03::PL_ATK);
		}
		else if (p->GetPrivate()->pctrl->Pressed("Dodge"))
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
	int index;
	void Begin(GhostShip03* p)
	{
		static int i = 0;
		index = i + GhostShip03::SLASH1;

		p->GetPrivate()->model->root_motion = (Mo2Lib::ROOT_MOTION)(Mo2Lib::ROOT_MOTION_XZ | Mo2Lib::ROOT_MOTION_Y);
		p->GetPrivate()->DeffaultAnimSpd();
		p->GetPrivate()->PlayBlendAnim(index, false);

		i++;
		if (i >= GhostShip03::MAX_ANIM - GhostShip03::SLASH1)
		{
			i = 0;
		}
	}

	void Execute(GhostShip03* p)
	{
		//p->anim.data.anim_spd += 1.f + Mo2System->delta_time;

		if (p->GetPrivate()->pctrl->Pressed("Dodge"))
		{
			fsm->ChangeState(GhostShip03::PL_DODGE);
		}

		if (p->GetPrivate()->model->data.end_anim)
		{
			if (p->GetPrivate()->pctrl->dir_vec != 0)
			{
				fsm->ChangeState(GhostShip03::PL_MOVE);
			}

			else if (p->GetPrivate()->pctrl->dir_vec.LengthSq() == 0.f)
			{
				fsm->ChangeState(GhostShip03::PL_IDLE);
			}

		}
	}
	void End(GhostShip03* p)
	{
		p->GetPrivate()->model->root_motion = Mo2Lib::NO_ROOT_MOTION;
	}
};


class PL_Dodge : public State<GhostShip03>
{
	float timer;
	void Begin(GhostShip03* p)
	{
		p->GetPrivate()->physics->MAX_VELOCITY = INIT_MAX_VELOCITY * 1.5f;
		p->GetPrivate()->physics->MAX_MOVE_SPEED = MAX_WALK_SPEED * 5.f;
		timer = 0.5f;

		p->GetPrivate()->physics->velocity = p->InputDirection() * 100.f;

		//p->anim.data.anim_spd = p->GetPrivate()->physics->MAX_VELOCITY / INIT_MAX_VELOCITY;
		p->GetPrivate()->AnimSpdByPhysics();

		p->GetPrivate()->PlayAnim(GhostShip03::DODGE, false);
	}

	void Execute(GhostShip03* p)
	{
		p->LookForward();
		//Mo2Lib::Float3 dir = { sinf(p->GetPrivate()->trans->rotate.y), 0.f, cosf(p->GetPrivate()->trans->rotate.y) };
		//p->GetPrivate()->physics->AddForce(dir * p->GetPrivate()->physics->MAX_MOVE_SPEED * p->GetPrivate()->physics->mass);
		p->GetPrivate()->AddMoveSpeedToTheDirection();
		
		if (p->GetPrivate()->model->data.end_anim)
		{
			if (p->GetPrivate()->IsMoving())
			{
				p->GetPrivate()->PlayAnim(GhostShip03::RUN, true);
				fsm->ChangeState(GhostShip03::PL_MOVE);
			}

			else
			{
				p->GetPrivate()->SetZeroVelocity();
				p->GetPrivate()->PlayAnim(GhostShip03::IDLE, true);
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
	pimpl = std::make_shared<GShipImpl>();
	pimpl->trans = parent->GetComponent<Transform>();
	pimpl->physics = parent->GetComponent<Physics2>();
	pimpl->pctrl = parent->GetComponent<PlayerControl>();
	pimpl->box = parent->AddComponent<BoxComponent>();
	pimpl->s0 = parent->AddComponent<SphereComponent>();
	//gun = Parent->GetComponent<Firearm>();

	pimpl->LoadShader();

	pimpl->LoadModel();

	fsm = std::make_unique<StateMachine<GhostShip03>>(this);
	fsm->AddState(PL_IDLE, std::make_shared<PL_Idle>())
		.AddState(PL_MOVE, std::make_shared<PL_Move>())
		.AddState(PL_ATK, std::make_shared<PL_Attack>())
		.AddState(PL_DODGE, std::make_shared<PL_Dodge>());

	fsm->SetCurrentState(PL_IDLE);



}

void GhostShip03::Update()
{
	ismoving = false;

	//anim.data.anim_spd = physics->velocity.Length() / physics->MAX_VELOCITY;
	pimpl->physics->Deceleration();

	fsm->Update(parent->delta_time);


	pimpl->model->UpdateAnimation(parent->delta_time);
	//pimpl->trans->translate += pimpl->model->RootAnimTransform(pimpl->trans->Convert()) * 0.01f;

	pimpl->AdaptBoxTransform();

	pimpl->AdaptSphereTransform();

	pimpl->SetMainCamOrientation();
}

void GhostShip03::Draw()
{
	//model->SetTransform(pimpl->trans->translate, pimpl->trans->GetFixedQuaternion(), pimpl->trans->scale);

	//Mo2Render().Draw(phong.get(), *model);
	pimpl->ModelDraw();
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
		for (size_t i = 0; i < pimpl->model->GetNodes().size(); i++)
		{
			ImGui::Text("%d %s", i, pimpl->model->GetNodes().at(i).name);
		}
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

		key_vec = pimpl->pctrl->dir_vec;

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
	Mo2Lib::Vec3 v = pimpl->GetVelocityXZ();
	if (v == 0.f)return;

	t_for = v.GetNormalize();//Mo2Lib::Float3(INPUT.StickVector().x, 0.f, INPUT.StickVector().y);

	now_angle = forward.Dot(t_for);
	if (now_angle <= -1.0f)now_angle = -1.0f;
	now_angle = acosf(now_angle);

	if (forward.Cross(v).y < 0.f)
	{
		now_angle = DirectX::XM_2PI - now_angle;
	}
	pimpl->trans->rotate.y = now_angle;

}


void GhostShip03::MoveXZ()
{
	Mo2Lib::Vec3 i_vec = InputDirection();
	pimpl->physics->AddMoveSpeed(i_vec);

}