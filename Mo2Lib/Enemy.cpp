#include "Enemy.h"
#include "framework.h"
#include "Camera.h"
#include "Mo2Func.h"

#include "GatesQuad.h"
#include "TheEmblion.h"
#include "Mo2GUI.h"
//-----------------------------------------------------
//
//		constant term
//
//-----------------------------------------------------
const float INIT_MAX_MOVE_SPD = 1000.f;
const float INIT_MAX_VELOCITY = 600.f;
const float INIT_MASS = 1.f;
const float FLOWER_HIT_SCALE = 200.f;


EnemyManager* EnmManager()
{
	return EnemyManager::Getinstance();
}


void Enemy::Init()
{
	PhysicsInit(INIT_MASS);
	rotation = DirectX::XMQuaternionIdentity();
	//rotation = DirectX::XMQuaternionRotationRollPitchYaw(-DirectX::XM_PIDIV2, 0.f, 0.f);
	//rotation *= DirectX::XMQuaternionRotationRollPitchYaw(0.f, 0.f, DirectX::XMConvertToRadians(45.f));
	scale = { 1.f,1.f,1.f };
	ragidbody = false;
	SetMAX_MOVE_SPEED(INIT_MAX_MOVE_SPD);
	SetMAX_VELOCITY(INIT_MAX_VELOCITY);

	exist = false;
}

void Enemy::Update(float dt)
{
	Integrate(dt);

	SetAllHitPos(position);
}

void Enemy::SetAllHitPos(Mo2Lib::Float3 p)
{
	for (size_t i = 0; i < collider.size(); ++i)
	{
		collider[i].p = p;
	}
}

void EnemyManager::Init()
{

	//framework& frame = framework::GetInstance();
	//ID3D11Device* device = frame.device.Get();

	////enms.resize(200);
	//ModelData m_data;

	//char* fbx_filename = "./Data/Assets/enemy/flower.fbx";
	//std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();
	//if (!model_resource->LoadFromBinary(device, fbx_filename, &m_data))
	//{
	//	model_resource->SaveToData(fbx_filename, &m_data);
	//}
	//model[FLOWER] = std::make_shared<Mo2Lib::Model>(model_resource);	
	//
	//fbx_filename = "./Data/Assets/enemy/gates_quad_mesh.fbx";
	//model_resource = std::make_shared<ModelResource>();
	//if (!model_resource->LoadFromBinary(device, fbx_filename, &m_data))
	//{
	//	model_resource->SaveToData(fbx_filename, &m_data);
	//}
	//model[GATESQUAD] = std::make_shared<Mo2Lib::Model>(model_resource);
	//
	//fbx_filename = "./Data/Assets/enemy/the embulion02.fbx";
	//model_resource = std::make_shared<ModelResource>();
	//if (!model_resource->LoadFromBinary(device, fbx_filename, &m_data))
	//{
	//	model_resource->SaveToData(fbx_filename, &m_data);
	//}
	//model[THE_EMBLION] = std::make_shared<Mo2Lib::Model>(model_resource);

	////flower->m_nodes[0].transform.rotate = DirectX::XMQuaternionRotationRollPitchYaw(-DirectX::XM_PIDIV2, 0.f, 0.f);

	////flower->InitializeAnimation();

	//colider_data[FLOWER].resize((size_t)2);

	//colider_data[GATESQUAD].resize((size_t)2);
	//colider_data[GATESQUAD][0].r = 50.f;
	//colider_data[GATESQUAD][1].r = 50.f;

	//colider_data[THE_EMBLION].resize((size_t)2);

	//for (size_t i = 0; i < flower->GetNodes().size(); i++)
	//{
	//	colider_data[FLOWER_STATE].emplace_back();

	//	colider_data[FLOWER_STATE].back().p = flower->GetNodes()[i].transform.translate;
	//	colider_data[FLOWER_STATE].back().r = 5.f;//FLOWER_HIT_SCALE;
	//}
}

void EnemyManager::Release()
{
	for (auto enm : enm_list)
	{
		delete enm;
	}
	enm_list.clear();
}

void EnemyManager::Update(float dt)
{
	for (auto& it : enm_list)
	{
		if (!it->exist)continue;
		it->target_pos = target_pos;
		it->Update(dt);
	}
}

//void EnemyManager::Render(ShaderEx* shader, ModelRenderer* render)
//{
//	Mo2Lib::Transform trans;
//	for (auto& it : enm_list)
//	{
//		if (!it->visiblity ||!it->exist)continue;
//		trans.translate = it->GetPos();
//		trans.rotate = it->rotation;
//		trans.scale = it->scale;
//
//		
//		model[it->type]->transform = trans;
//		render->Draw(shader, *model[it->type]);
//	}
//}

bool EnemyManager::Set(int type, Mo2Lib::Float3 pos)
{
	//for (auto& it : enms)
	//{
	//	if (it.exist)continue;
	//	it.Init();
	//	it.type = type;
	//	it.SetPos(pos);

	//	it.exist = true;
	//	return true;
	//}
	Enemy* enm;
	switch (type)
	{
	case GATESQUAD:
		enm = new GatesQuad();
		break;
	case THE_EMBLION:
		enm = new GatesQuad();
		break;
default:
	enm = new GatesQuad();
	break;
	}

	enm->Init();
	enm->type = type;
	enm->SetPos(pos);
	//enm->collider.resize(colider_data[type].size());
	
	//for (size_t i = 0; i < enm->collider.size(); ++i)
	//{
	//	enm->collider[i].r = colider_data[type][i].r;
	//}


	enm->exist = true;
	enm_list.push_back(enm);
	return false;
}


void Enemy::UpdOrinetation(const Mo2Lib::Float3& vec)
{
	//モデルの姿勢更新

	Mo2Lib::Float4* orient = &rotation;
	Mo2Lib::Float4x4 m;
	DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationQuaternion(orient->ConvertToXMVECTOR()));

	Mo2Lib::Float3 right = DirectX::XMVectorSet(m._11, m._12, m._13, 0.f);
	Mo2Lib::Float3 up = DirectX::XMVectorSet(m._21, m._22, m._23, 0.f);
	Mo2Lib::Float3 forward = DirectX::XMVectorSet(m._31, m._32, m._33, 0.f);

	right.Normalize();
	up.Normalize();
	forward.Normalize();

	Mo2Lib::Vec3 t_for, axis;
	float angle = 0.f;


	Mo2Lib::Vec3 v = { vec.x, 0.f, vec.z };
	if (v == 0.f)return;
	t_for = v.GetNormalize();//Mo2Lib::Float3(INPUT.StickVector().x, 0.f, INPUT.StickVector().y);

	angle = forward.Dot(t_for);
	if (angle <= -1.0f)angle = -1.0f;
	angle = acosf(angle);

	if (fabs(angle) > 1e-8f)
	{
		axis = forward.Cross(t_for);
		axis.Normalize();

		if (axis == 0.f)
		{
			//バグ直すため
			Mo2Lib::Float3 tmp_axis;
			tmp_axis = axis;
			if (axis == 0.0f)
			{
				axis.x = 0.f;
				axis.y = 1.f;
				axis.z = 0.f;
			}
		}

		Mo2Lib::Float4 q = DirectX::XMQuaternionRotationAxis(axis.ConvertToXMVECTOR(), angle);

		*orient *= q;
		//6orientation.QuaternionSlerp(q, 0.6f);

		//DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixRotationQuaternion(orient->ConvertToXMVECTOR()));
		//right = DirectX::XMVectorSet(m._11, m._12, m._13, 0.f);
		//up = DirectX::XMVectorSet(m._21, m._22, m._23, 0.f);
		//forward = DirectX::XMVectorSet(m._31, m._32, m._33, 0.f);
	}


}

void EnemyManager::ImGui()
{
	//enms.begin()->PhysicsImGui("EnemysFirst");

	//ImGui::DragFloat3("scale##enemy", &enms.begin()->scale.x, 0.1f);
	static int summon_index = 0;

	ImGui::InputInt("MODEL_INDEX", &summon_index);
	Mo2Lib::Clamp(summon_index, 0, MODEL_MAX - 1);
	if (ImGui::Button("Summon"))
	{
		Mo2Lib::Float3 SetPoint = { target_pos.x + 2000.f * Mo2Lib::RandFloat(),
		target_pos.y + 100.f,
		target_pos.z + 2000.f * Mo2Lib::RandFloat() };

		Set(summon_index, SetPoint);
	}



}



//---------------------------------------------------------------------------
//
//					State_List
//
//---------------------------------------------------------------------------
