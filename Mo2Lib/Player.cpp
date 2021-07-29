//#include "Player.h"
//#include "framework.h"
//#include "Mo2GUI.h"
//#include "PlayerConstant.h"
//#include "Bullet.h"
//
//
//ID3D11Device* GetDevice()
//{
//	return FrameWork::GetInstance()->DX11device.Get();
//}
//
//
////-------------------------------------------------------------------
////
////		Initialize
////
////-------------------------------------------------------------------
//void Player::Init()
//{
//	const char* fbx_filename;
//	ModelData m_data;
//	std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();
//	
//#if 0
//	fbx_filename = "./Data/Assets/GhostShip/GhostShip.fbx";
//	if (!model_resource->LoadFromBinary(GetDevice(), fbx_filename, &m_data))
//	{
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol Idle.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol Runforward.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol leftside.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol rightside.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol Jump_Start.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol Jump_Loop.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol Jump_Finish.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol Rolling.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Fire.fbx");
//		model_resource->SaveToData(fbx_filename, &m_data);
//	}
//	model = std::make_unique<Mo2Lib::Model>(model_resource);
//
//	anim.prev_nodes.resize(model->m_nodes.size());
//	//anim.nodes = model->m_nodes;
//	//anim.animation_speed = 1.f;
//
//	anim.data.resize(MAX_ANIM);
//	anim.data[TPose].priority =	0;
//	anim.data[IDLE].priority =	1;
//	anim.data[RUN].priority =	2;
//	anim.data[MOVE_L].priority = 2;
//	anim.data[MOVE_R].priority = 2;
//	anim.data[JUMP_S].priority = anim.data[JUMP_L].priority = anim.data[JUMP_E].priority = 3;
//	anim.data[DODGEROLL].priority = 4;
//	anim.data[FIRE].priority = 3;
//
//#else
//	
//	fbx_filename = "./Data/Assets/GhostShip/GhostShip03.fbx";
//	if (!m_data.Load(fbx_filename, ModelData::BINARY))
//	{
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol Idle.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/standard Run.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol WalkForward.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol WalkBackward.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol sideStepL.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol sideStepR.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Rolling.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol AimUp.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol AimDown.fbx");
//		m_data.AddAnimation("./Data/Assets/GhostShip/Pistol Fire.fbx");
//		m_data.SaveToFile();
//	}
//	model_resource->Load(GetDevice(), &m_data, LOAD_SKINNED_MODEL);
//	model = std::make_unique<Mo2Lib::Model>(model_resource);
//
//	anim.prev_nodes.resize(model->m_nodes.size());
//
//	anim.data.resize(MAX_ANIM);
//	anim.data[TPose].priority = 0;
//	anim.data[IDLE].priority = 1;
//	anim.data[RUN].priority = 2;
//	anim.data[MOVE_F].priority = 2;
//	anim.data[MOVE_B].priority = 2;
//	anim.data[MOVE_L].priority = 2;
//	anim.data[MOVE_R].priority = 2;
//	anim.data[DODGEROLL].priority = 4;
//	//anim.data[FIRE].priority = 3;
//
//
//#endif
//
//	anim.AddLayer(0, 3);
//	anim.AddLayer(4, 6);
//	anim.AddLayer(7, 22);
//	anim.AddLayer(23, 38);
//	anim.AddLayer(39, 43);
//	anim.AddLayer(43, anim.prev_nodes.size());
//	model->InitializeAnimation();
//	anim.CallAnim(0, IDLE, true);
//
//	//State_Initialize
//	fsm = std::make_unique<StateMachine<Player>>(this);
//
//	state_list[MOVING_STATE] = std::make_shared<Moving_State>();
//	state_list[AIM_STATE] = std::make_shared<Aiming_State>();
//	state_list[DODGE_STATE] = std::make_shared<Dodge_State>();
//	fsm->SetCurrentState(MOVING_STATE);
//
//	PhysicsInit(INIT_MASS);
//	ragidbody = true;
//	SetMAX_MOVE_SPEED(MAX_WALK_SPEED);
//	SetMAX_VELOCITY(INIT_MAX_VELOCITY);
//	SetG_Scale(INIT_G_SCALE);
//
//	cam_angle = { 0.f, 0.f };
//	prev_angle = { 0.f, 0.f };
//	jump = JUMP_POW;
//	jump_time = 0.f;
//	MAX_JUMP_TIME = JUMP_RESPTION_TIME;
//
//	Mo2Lib::Float4* orient = &model->transform.rotate;
//	Mo2Lib::Float4x4 m;
//	DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationQuaternion(orient->ConvertToXMVECTOR()));
//	
//	isAiming = false;
//	aim_timer = 0.f;
//
//	right = DirectX::XMVectorSet(m._11, m._12, m._13, 0.f);
//	up = DirectX::XMVectorSet(m._21, m._22, m._23, 0.f);
//	forward = DirectX::XMVectorSet(m._31, m._32, m._33, 0.f);
//
//	position = { 10.f, 100.f, 10.f };
//	scale = { 1.0f, 1.0f, 1.0f };
//}
//
//
////-------------------------------------------------------------------
////
////		Update
////
////-------------------------------------------------------------------
//void Player::Update(float dt)
//{
//	//anim.layers[0].call[0].first = ANIM_STATE::TPose;
//	//for (auto& layer : anim.layers)
//	//{
//	//	layer.called = -1;
//	//}
//	anim.call_count = 0;
//
//	if (is_ground)velocity.y = 0.f;
//
//	InputDirection();
//
//	fsm->SetElapsedtime(dt);
//	fsm->Update(fsm->dt);
//
//	aim_timer -= dt;
//	if (aim_timer < FLT_EPSILON)
//	{
//		aim_timer = 0.f;
//	}
//
//	if (!is_moving)Deceleration();
//
//	AddGravity();
//
//
//	is_ground = false;
//
//	Integrate(dt);          
//
//
//
//	//Animation Idle
//	if (anim.call_count == 0)
//	{
//		for (size_t i = 0; i < anim.layers.size(); ++i)
//		{
//			anim.CallBlendAnim(i, ANIM_STATE::IDLE, true, true);
//			//anim.CallAnim(IDLE, true, Mo2Lib::Animator::ANIM_BLEND);
//		}
//	}
//	else
//	{
//		for (size_t i = 0; i < anim.layers.size(); ++i)
//		{
//			anim.CallBlendAnim(0, ANIM_STATE::IDLE, true, false);
//		}
//	}
//
//
//	//anim.anim_speed = c_anim.anim_spd;
//	//anim.PlayBlendAnim(anim.called);
//	model->UpdateAnimation(&anim, dt);
//}
//
//
////void Player::Render()
////{
////	if (!model)return;
////
////	model->SetTransform(position, orientation, scale);
////
////	
////}
//
//
////-------------------------------------------------------------------
////
////		Render Ready
////
////-------------------------------------------------------------------
//Mo2Lib::Model& Player::RenderRdy()
//{
//	//if (!model)return;
//
//	model->SetTranslate(position);
//	model->SetScale(scale);
//
//	return *model.get();
//}
//
//
////-------------------------------------------------------------------
////
////		Update Orientation
////
////-------------------------------------------------------------------
//void Player::UpdOrinetation(const Mo2Lib::Float3& vec)
//{
//	//モデルの姿勢更新
//
//	Mo2Lib::Float4* orient = &model->transform.rotate;
//	Mo2Lib::Float4x4 m;
//	DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationQuaternion(orient->ConvertToXMVECTOR()));
//
//	right = DirectX::XMVectorSet(m._11, m._12, m._13, 0.f);
//	up = DirectX::XMVectorSet(m._21, m._22, m._23, 0.f);
//	forward = DirectX::XMVectorSet(m._31, m._32, m._33, 0.f);
//
//	right.Normalize();
//	up.Normalize();
//	forward.Normalize();
//
//	Mo2Lib::Vec3 t_for, axis;
//	float angle = 0.f;
//
//
//	Mo2Lib::Vec3 v = { vec.x, 0.f, vec.z };
//	if (v == 0.f)return;
//	t_for = v.GetNormalize();//Mo2Lib::Float3(INPUT.StickVector().x, 0.f, INPUT.StickVector().y);
//
//	angle = forward.Dot(t_for);
//	if (angle <= -1.0f)angle = -1.0f;
//	angle = acosf(angle);
//
//	if (fabs(angle) > 1e-8f)
//	{
//		axis = forward.Cross(t_for);
//		axis.Normalize();
//
//		if(axis == 0.f)
//		{
//			//バグ直すため
//			Mo2Lib::Float3 tmp_axis;			
//			tmp_axis = axis;
//			if (axis == 0.0f)
//			{
//				axis.x = 0.f;
//				axis.y = 1.f;
//				axis.z = 0.f;
//			}
//		}
//
//		Mo2Lib::Float4 q = DirectX::XMQuaternionRotationAxis(axis.ConvertToXMVECTOR(), angle);
//
//		*orient *= q;
//		//6orientation.QuaternionSlerp(q, 0.6f);
//
//		DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationQuaternion(orient->ConvertToXMVECTOR()));
//		right = DirectX::XMVectorSet(m._11, m._12, m._13, 0.f);
//		up = DirectX::XMVectorSet(m._21, m._22, m._23, 0.f);
//		forward = DirectX::XMVectorSet(m._31, m._32, m._33, 0.f);
//	}
//
//
//}
//
////---------------------------------------------------------------------------
////
////					ImGui
////
////---------------------------------------------------------------------------
//void Player::ImGui()
//{
//	ImGui::Begin("Player");
//
//	PhysicsImGui(std::string(typeid(Player).name()));
//
//	ImGui::NewLine();
//
//	ImGui::DragFloat("jump##PLAYER", &jump, 0.1f);
//	ImGui::DragFloat("jumptime##PLAYER", &jump_time, 0.1f);
//	ImGui::DragFloat("MAX_JUMP_TIME##PLAYER", &MAX_JUMP_TIME, 0.1f);
//	if (is_ground)ImGui::Text("OnGround");
//	else ImGui::NewLine();
//
//	ImGui::NewLine();
//
//	ImGui::DragFloat4("orient", &model->transform.rotate.x);
//
//	Mo2Gui()->str = "Current State";
//	ImGui::Text(Mo2Gui()->str.c_str());
//	//Mo2Gui()->str = fsm->GetName_CurrentState();
//	ImGui::Text("%s", Mo2Gui()->str.c_str());
//
//	ImGui::DragFloat2("Cam_angle", &cam_angle.x, 0.01f);
//
//	ImGui::NewLine();
//
//	ImGui::DragFloat3("Input_vec", &input_vec.x, 0.01f);
//
//	ImGui::Text("%.3f" , vec_angle);
//
//	float vertical_facter = cosf(vec_angle);
//	float horizon_facter = -sinf(vec_angle);
//
//	ImGui::Text("%.3f %.3f", vertical_facter, horizon_facter);
//
//	float rate = 0.f;
//	if (0.f <= vec_angle && DirectX::XM_PIDIV2 > vec_angle)
//	{//前右
//		ImGui::Text("F_R");
//	}
//	else if (DirectX::XM_PIDIV2 <= vec_angle && DirectX::XM_PI > vec_angle)
//	{//後右
//		ImGui::Text("R_B");
//	}
//	else if (DirectX::XM_PI <= vec_angle && DirectX::XM_PI + DirectX::XM_PIDIV2 > vec_angle)
//	{//前左
//		ImGui::Text("B_L");
//	}
//	else if (DirectX::XM_PI + DirectX::XM_PIDIV2 <= vec_angle && DirectX::XM_2PI > vec_angle)
//	{//後左
//		ImGui::Text("L_F");
//	}
//
//
//	anim.AnimImGui(std::string(typeid(Player).name()));
//	//if (ImGui::Button("change_blendrate_anim##Player"))
//	//{
//	//	anim.PlayBlendRateAnim(anim_index, next_anim_index, anim.blend_rate);
//	//}
//	//if (anim.ratio_flg)
//	//{
//	//	ImGui::SliderFloat("ANIM_RATE##Player", &anim.blend_rate, 0.f, 1.f);
//	//}
//
//	for (size_t i = 0; i < model->GetNodes().size(); i++)
//	{
//		ImGui::Text("%d %s", i, model->GetNodes().at(i).name);
//	}
//	int index = 0;
//	for(auto& it : model->GetModelResource()->GetAnimations().at(0).keyframes.at(0).node_keys)
//	{
//
//		ImGui::Text("%d %s", index, it.name.c_str());
//		++index;
//	}
//
//
//	//ImGui::DragFloat("angleeeee", &testangle);
//
//	ImGui::End();
//}
//
//
