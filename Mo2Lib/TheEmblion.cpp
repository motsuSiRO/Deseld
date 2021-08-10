#include "TheEmblion.h"
#include "StateMachine.h"
#include "model_renderer.h"
#include "ColliderComponents.h"

namespace TheE
{
	Transform* trans;
	BoxComponent* hitbox[3];
	SphereComponent* s0;
};
using namespace TheE;

void TheEmblion::Start()
{
	phong = std::make_shared<ShaderEx>();
	phong->Create(L"model_vs", L"model_ps");

	std::shared_ptr<ModelResource> model_resource;
	Mo2Lib::ModelLoader::GetInstance().Setup(/*true*/);

	Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/enemy/the embulion.fbx");
	Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/enemy/the embulion Idle.fbx", true);

	model_resource = Mo2Lib::ModelLoader::GetInstance().Apply();
	model = std::make_unique<Mo2Lib::Model>(model_resource);

	Mo2Lib::ModelLoader::GetInstance().End();

	model->InitializeAnimation();

	model->PlayAnim(1, true);

	trans = parent->GetComponent<Transform>();
	trans->scale = { 3.f, 3.f, 3.f };

	//hitbox[0] = parent->AddComponent<BoxComponent>();
	s0 = parent->AddComponent<SphereComponent>();

	s0->trans.scale = { 60.f, 60.f, 60.f };

}

void TheEmblion::Update()
{
	//hitbox[0]->trans.translate = model->GetNodes(1)->GetWorldPos();

	s0->trans.translate = trans->translate;

	model->UpdateAnimation(parent->delta_time);
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
		Super::ImGui();
		ImGui::SameLine();
		ImGui::Text("Visible");
		ImGui::NewLine();

		//ImGui::Checkbox("Is_Moving", &ismoving);
		//ImGui::NewLine();

		//ImGui::Text("CurrentState");
		//ImGui::Text(fsm->GetCurrentName());

		ImGui::NewLine();

		ImGui::NewLine();
		for (size_t i = 0; i < model->GetNodes().size(); i++)
		{
			ImGui::Text("%d %s", i, model->GetNodes().at(i).name);
		}


	}
}