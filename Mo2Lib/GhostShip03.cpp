#include "GhostShip03.h"
#include "GameSystem.h"

#include "model_renderer.h"
#include "Camera.h"
#include "Bullet.h"

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

	}

	void Execute(GhostShip03* p)
	{
		if (p->pctrl->Pressed("Dodge"))
		{
			fsm->ChangeState(GhostShip03::PL_DODGE);
		}
		else if (p->pctrl->dir_vec != 0)
		{
			fsm->ChangeState(GhostShip03::PL_MOVE);
		}
		else if (p->pctrl->Pressed("Firstary"))
		{
			if (p->gun->Shoot())
			{
				Object* obj = new Object("Bullet");
				Transform* t = obj->AddComponent<Transform>();
				t->translate = { CAM_LIST()->main_cam->GetEye() };
				Bullet* b = obj->AddComponent<Bullet>();
				b->forward = CAM_LIST()->main_cam->GetFront();

				Mo2Lib::game.obj_list.emplace_back(obj);
			}
			fsm->ChangeState(GhostShip03::PL_AIM);
		}

		//for (int i = 0; i < GhostShip03::FULLBODY; i++)
		{
			p->anim.CallBlendAnim(GhostShip03::SPINE                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                , GhostShip03::IDLE, true, true);
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

		p->physics->mass = INIT_MASS;
		p->physics->limit_velocity = true;
		p->physics->dynamic_friction = 10.f;
		p->physics->MAX_VELOCITY = INIT_MAX_DASH_VELOCITY;
		p->physics->MAX_MOVE_SPEED = MAX_WALK_SPEED;

		move_speed = p->physics->MAX_MOVE_SPEED * p->physics->mass;

		Mo2Lib::Vec3 i_vec = p->InputDirection();
		p->physics->AddForce(i_vec * move_speed);
	}

	void Execute(GhostShip03* p)
	{
		p->MoveXZ(move_speed);

		p->LookForward();

		p->anim.anim_spd = p->physics->MAX_VELOCITY / INIT_MAX_VELOCITY;
		p->anim.CallBlendAnim(GhostShip03::SPINE, GhostShip03::RUN, true, true);
		//p->anim.CallBlendAnim(GhostShip03::HEAD, GhostShip03::RUN, true, true);
		//p->anim.CallBlendAnim(GhostShip03::L_ARM, GhostShip03::IDLE, true, true);
		//p->anim.CallBlendAnim(GhostShip03::R_ARM, GhostShip03::IDLE, true, true);
		//p->anim.CallBlendAnim(GhostShip03::L_FOOT, GhostShip03::RUN, true, true);
		//p->anim.CallBlendAnim(GhostShip03::R_FOOT, GhostShip03::RUN, true, true);


		if (p->physics->velocity == 0)
		{
			fsm->ChangeState(GhostShip03::PL_IDLE);
		}
		else if (p->pctrl->Pressed("Firstary"))
		{
			if (p->gun->Shoot())
			{
				Object* obj = new Object("Bullet");
				Transform * t = obj->AddComponent<Transform>();
				t->translate = { CAM_LIST()->main_cam->GetEye() };
				Bullet* b = obj->AddComponent<Bullet>();
				b->forward = CAM_LIST()->main_cam->GetFront();
				
				Mo2Lib::game.obj_list.emplace_back(obj);
			}
			//fsm->ChangeState(GhostShip03::PL_AIM);
		}


	}

	void End(GhostShip03* p)
	{

	}

};


class PL_Aim : public State<GhostShip03>
{
	float move_speed = 0.f;
	float timer;
	void Begin(GhostShip03* p)
	{
		p->physics->MAX_VELOCITY = INIT_MAX_AIM_VELOCITY;
		move_speed = p->physics->MAX_MOVE_SPEED * p->physics->mass;
		timer = 5.f;
	}

	void Execute(GhostShip03* p)
	{
		timer -= Mo2System->delta_time;

		p->MoveXZ(move_speed);

		float range = 0.5f;

		if (p->pctrl->Pressed("Firstary"))
		{
			if (p->gun->Shoot())
			{
				Object* obj = new Object("Bullet");
				Transform* t = obj->AddComponent<Transform>();
				t->translate = { CAM_LIST()->main_cam->GetEye() };
				Bullet* b = obj->AddComponent<Bullet>();
				b->forward = CAM_LIST()->main_cam->GetFront();

				Mo2Lib::game.obj_list.emplace_back(obj);
			}
			fsm->ChangeState(GhostShip03::PL_AIM);
		}

		//if (DirectX::XM_2PI - range <= CAM_LIST()->main_cam->rotate.y || range > CAM_LIST()->main_cam->rotate.y)
		//{//前
		//	p->anim.CallRatioDefAnim(GhostShip03::SPINE, GhostShip03::MOVE_F, GhostShip03::TPose, 0.f);
		//	p->anim.CallRatioDefAnim(GhostShip03::L_FOOT, GhostShip03::MOVE_F, GhostShip03::TPose, 0.f);
		//	p->anim.CallRatioDefAnim(GhostShip03::R_FOOT, GhostShip03::MOVE_F, GhostShip03::TPose, 0.f);
		//}
		//else if (DirectX::XM_PIDIV2 - range <= CAM_LIST()->main_cam->rotate.y && DirectX::XM_PIDIV2 + range > CAM_LIST()->main_cam->rotate.y)
		//{//右
		//	p->anim.CallRatioDefAnim(GhostShip03::SPINE, GhostShip03::MOVE_R, GhostShip03::TPose, 0.f);
		//	p->anim.CallRatioDefAnim(GhostShip03::L_FOOT, GhostShip03::MOVE_R, GhostShip03::TPose, 0.f);
		//	p->anim.CallRatioDefAnim(GhostShip03::R_FOOT, GhostShip03::MOVE_R, GhostShip03::TPose, 0.f);
		//}
		//else if (DirectX::XM_PI - range <= CAM_LIST()->main_cam->rotate.y && DirectX::XM_PI + range > CAM_LIST()->main_cam->rotate.y)
		//{//後
		//	p->anim.CallRatioDefAnim(GhostShip03::SPINE, GhostShip03::MOVE_B, GhostShip03::TPose, 0.f);
		//	p->anim.CallRatioDefAnim(GhostShip03::L_FOOT, GhostShip03::MOVE_B, GhostShip03::TPose, 0.f);
		//	p->anim.CallRatioDefAnim(GhostShip03::R_FOOT, GhostShip03::MOVE_B, GhostShip03::TPose, 0.f);
		//}
		//else if (DirectX::XM_PI + DirectX::XM_PIDIV2 - range <= CAM_LIST()->main_cam->rotate.y && DirectX::XM_PI + DirectX::XM_PIDIV2 + range > CAM_LIST()->main_cam->rotate.y)
		//{//左
		//	p->anim.CallRatioDefAnim(GhostShip03::SPINE, GhostShip03::MOVE_L, GhostShip03::TPose, 0.f);
		//	p->anim.CallRatioDefAnim(GhostShip03::L_FOOT, GhostShip03::MOVE_L, GhostShip03::TPose, 0.f);
		//	p->anim.CallRatioDefAnim(GhostShip03::R_FOOT, GhostShip03::MOVE_L, GhostShip03::TPose, 0.f);
		//}
		//else
		//{
		//	float rate = 0.f;
		//	if (0.f <= CAM_LIST()->main_cam->rotate.y && DirectX::XM_PIDIV2 > CAM_LIST()->main_cam->rotate.y)
		//	{//前右
		//		p->anim.CallRatioAnim(GhostShip03::SPINE, GhostShip03::MOVE_F, GhostShip03::MOVE_R, CAM_LIST()->main_cam->rotate.y / DirectX::XM_PIDIV2);
		//		p->anim.CallRatioAnim(GhostShip03::L_FOOT, GhostShip03::MOVE_F, GhostShip03::MOVE_R, CAM_LIST()->main_cam->rotate.y / DirectX::XM_PIDIV2);
		//		p->anim.CallRatioAnim(GhostShip03::R_FOOT, GhostShip03::MOVE_F, GhostShip03::MOVE_R, CAM_LIST()->main_cam->rotate.y / DirectX::XM_PIDIV2);
		//	}
		//	else if (DirectX::XM_PIDIV2 <= CAM_LIST()->main_cam->rotate.y && DirectX::XM_PI > CAM_LIST()->main_cam->rotate.y)
		//	{//後右
		//		rate = CAM_LIST()->main_cam->rotate.y - DirectX::XM_PIDIV2;
		//		p->anim.CallRatioAnim(GhostShip03::SPINE, GhostShip03::MOVE_R, GhostShip03::MOVE_B, rate / DirectX::XM_PIDIV2);
		//		p->anim.CallRatioAnim(GhostShip03::L_FOOT, GhostShip03::MOVE_R, GhostShip03::MOVE_B, rate / DirectX::XM_PIDIV2);
		//		p->anim.CallRatioAnim(GhostShip03::R_FOOT, GhostShip03::MOVE_R, GhostShip03::MOVE_B, rate / DirectX::XM_PIDIV2);

		//	}
		//	else if (DirectX::XM_PI <= CAM_LIST()->main_cam->rotate.y && DirectX::XM_PI + DirectX::XM_PIDIV2 > CAM_LIST()->main_cam->rotate.y)
		//	{//後左
		//		rate = CAM_LIST()->main_cam->rotate.y - DirectX::XM_PI;
		//		p->anim.CallRatioAnim(GhostShip03::SPINE, GhostShip03::MOVE_B, GhostShip03::MOVE_L, rate / DirectX::XM_PIDIV2);
		//		p->anim.CallRatioAnim(GhostShip03::L_FOOT, GhostShip03::MOVE_B, GhostShip03::MOVE_L, rate / DirectX::XM_PIDIV2);
		//		p->anim.CallRatioAnim(GhostShip03::R_FOOT, GhostShip03::MOVE_B, GhostShip03::MOVE_L, rate / DirectX::XM_PIDIV2);
		//	}
		//	else
		//	{//前左
		//		rate = CAM_LIST()->main_cam->rotate.y - (DirectX::XM_PI + DirectX::XM_PIDIV2);
		//		p->anim.CallRatioAnim(GhostShip03::SPINE, GhostShip03::MOVE_L, GhostShip03::MOVE_F, rate / DirectX::XM_PIDIV2);
		//		p->anim.CallRatioAnim(GhostShip03::L_FOOT, GhostShip03::MOVE_L, GhostShip03::MOVE_F, rate / DirectX::XM_PIDIV2);
		//		p->anim.CallRatioAnim(GhostShip03::R_FOOT, GhostShip03::MOVE_L, GhostShip03::MOVE_F, rate / DirectX::XM_PIDIV2);
		//	}

		if (p->physics->velocity == 0)
		{
			fsm->ChangeState(GhostShip03::PL_IDLE);
		}
		if (timer <= 0.f)
		{
			fsm->ChangeState(GhostShip03::PL_AIM);
		}
		//}
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
		p->physics->MAX_VELOCITY = INIT_MAX_VELOCITY * 1.5f;
		p->physics->MAX_MOVE_SPEED = MAX_WALK_SPEED * 5.f;
		timer = 0.5f;
	}

	void Execute(GhostShip03* p)
	{
		DirectX::XMMatrixRotationY(p->trans->rotate.y);

		timer -= Mo2System->delta_time;
		if (timer <= 0.f)
		{
			fsm->ChangeState(GhostShip03::PL_DODGE);
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
	trans = Parent->GetComponent<Transform>();
	physics = Parent->GetComponent<Physics2>();
	pctrl = Parent->GetComponent<PlayerControl>();
	gun = Parent->GetComponent<Firearm>();

	fsm = std::make_unique<StateMachine<GhostShip03>>(this);
	fsm->AddState(PL_IDLE, std::make_shared<PL_Idle>())
		.AddState(PL_MOVE, std::make_shared<PL_Move>())
		.AddState(PL_AIM, std::make_shared<PL_Aim>())
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
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION, true);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol WalkBackward.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol sideStepL.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol sideStepR.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Rolling.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol AimUp.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	//fbx_filename = "./Data/Assets/GhostShip/Pistol AimDown.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_ANIMATION);
	model = std::make_unique<Mo2Lib::Model>(model_resource);
	model->InitializeAnimation();

	anim.prev_nodes.resize(model->m_nodes.size());

	anim.data.resize(MAX_ANIM);
	anim.data[TPose].priority = 0;
	anim.data[IDLE].priority = 1;
	anim.data[RUN].priority = 2;
	anim.data[MOVE_F].priority = 2;
	anim.data[MOVE_B].priority = 2;
	anim.data[MOVE_L].priority = 2;
	anim.data[MOVE_R].priority = 2;
	anim.data[DODGEROLL].priority = 4;



	//anim.AddLayer(0, 3);
	//anim.AddLayer(4, 6);
	//anim.AddLayer(7, 22);
	//anim.AddLayer(23, 38);
	//anim.AddLayer(39, 43);
	anim.AddLayer(0, anim.prev_nodes.size());

}

void GhostShip03::Update()
{
	ismoving = false;

	//anim.anim_spd = physics->velocity.Length() / physics->MAX_VELOCITY;
	fsm->Update(Parent->delta_time);

	model->UpdateAnimation(&anim, Parent->delta_time);
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
	s = "##" + Parent->GetID();
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
	str = "GhostShip03" + s;
	if (ImGui::CollapsingHeader(str.c_str()))
	{
		ImGui::Checkbox("Is_Moving", &ismoving);
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
			angle = acosf(key_vec.Dot(zero) /*/ zero.Length()*/);
			if (key_vec.Cross(zero) < 0.f)
			{
				angle = DirectX::XM_2PI - angle;
			}

			//angle += cam_angle.y;

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
	if (!ismoving)physics->Deceleration();

	Mo2Lib::Vec3 i_vec = InputDirection();
	physics->AddForce(i_vec * speed);

}