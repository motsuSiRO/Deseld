#include "TheEmblion.h"
#include "StateMachine.h"
#include "model_renderer.h"
#include "ColliderComponents.h"

namespace TheE
{
	Transform* trans;
	BoxComponent* hitbox[3];
};
using namespace TheE;

void TheEmblion::Start()
{
	phong = std::make_shared<ShaderEx>();
	phong->Create(L"model_vs", L"model_ps");

	const char* fbx_filename;
	std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();

	fbx_filename = "./Data/Assets/enemy/the embulion.fbx";
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_SKINNED_MODEL);
	model = std::make_unique<Mo2Lib::Model>(model_resource);
	model->InitializeAnimation();


	trans = parent->GetComponent<Transform>();
	trans->scale = { 3.f, 3.f, 3.f };

	hitbox[0] = parent->AddComponent<BoxComponent>();

}

void TheEmblion::Update()
{
	hitbox[0]->trans.translate = model->GetNodes(1)->GetWorldPos();
}

void TheEmblion::Draw()
{
	model->SetTransform(trans->translate, trans->GetFixedQuaternion(), trans->scale);

	Mo2Render().Draw(phong.get(), *model);
}

void TheEmblion::ImGui()
{
	std::string str, s;
	s = "##" + parent->GetID();
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
	str = "The Emblion" + s;


	if (ImGui::CollapsingHeader(str.c_str()))
	{

	}
}