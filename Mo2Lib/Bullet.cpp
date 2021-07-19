#include "Bullet.h"
#include "model_renderer.h"
#include "GameSystem.h"

void Bullet::Start()
{
	phong = std::make_shared<ShaderEx>();
	phong->Create(L"StaticModel_VS", L"StaticModel_PS");

	const char* fbx_filename;
	std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();

	fbx_filename = "./Data/Assets/weapon/Bullet.fbx";
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_SKINNED_MODEL);
	model = std::make_unique<Mo2Lib::Model>(model_resource);


	trans = parent->GetComponent<Transform>();
	model->color = { Mo2Lib::RandFloat(), Mo2Lib::RandFloat(), Mo2Lib::RandFloat(), 1.f };
	trans->scale = { 1.f, 1.f, 1.f };
	
	forward_speed = 10.f;
}

void Bullet::Update()
{
	trans->translate += forward * forward_speed;

	if (trans->translate.x < 0)
	{
		//parent->destroy = true;
		parent->Destroy();
		return;
	}
}

void Bullet::Draw()
{
	model->transform.translate = trans->translate;
	model->transform.rotate = trans->GetFixedQuaternion();
	model->transform.scale = trans->scale;
	Mo2Render().Draw(phong.get(), *model);
}

void Bullet::ImGui()
{

}
