#include "Firearm.h"
#include "model_renderer.h"


void Firearm::Start()
{
	//cooling = false;
	//recast_time = 0.f;
	//MAX_RECAST_TIME = 0.f;
	//vertical_recoil = 0.f;
	//MAX_V_RECOIL = 0.f;
	//ONCE_V_RECOIL = 0.f;
	//v_recoil_control = 1.f;
	//horizontal_recoil = 0.f;
	//MAX_H_RECOIL = 0.f;
	//ONCE_H_RECOIL = 0.f;
	//h_recoil_control = 1.f;
	//mag.MAX_SIZE = 16;
	//MAX_MAGAGINE = 1;

	//trans = parent->GetComponent<Transform>();


	//phong = std::make_shared<ShaderEx>();
	//phong->Create(L"model_vs", L"model_ps");


	//const char* fbx_filename;
	//std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();

	//fbx_filename = "./Data/Assets/weapon/CZ75P07Duty.fbx";
	//Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_SKINNED_MODEL, false);
	//model = std::make_unique<Mo2Lib::Model>(model_resource);
	//model->InitializeAnimation();

}

void Firearm::Update()
{
	Cooling();

	vertical_recoil -= ONCE_V_RECOIL * parent->delta_time * (v_recoil_control * v_recoil_control);
	Mo2Lib::Clamp(vertical_recoil, 0.f, MAX_V_RECOIL);

	horizontal_recoil -= ONCE_H_RECOIL * parent->delta_time * (h_recoil_control * h_recoil_control);
	Mo2Lib::Clamp(horizontal_recoil, 0.f, MAX_H_RECOIL);

	if (!cooling)
	{
		v_recoil_control += parent->delta_time * 15.f;
		h_recoil_control += parent->delta_time * 15.f;
	}
}

void Firearm::Draw()
{

	model->SetTransform(trans->translate, trans->GetFixedQuaternion(), trans->scale);

	Mo2Render().Draw(phong.get(), *model);

}

void Firearm::ImGui()
{
	std::string str, s;
	s = "##" + parent->GetID();
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
	str = "GunParameter" + s;
	if (ImGui::CollapsingHeader(str.c_str()))
	{
		str = "Cooling" + s;
		ImGui::Checkbox(str.c_str(), &cooling);

		str = "MAX_RECAST_TIME" + s;
		ImGui::DragFloat(str.c_str(), &MAX_RECAST_TIME);
		str = "recast" + s;
		ImGui::SliderFloat(str.c_str(), &recast_time, 0.f, MAX_RECAST_TIME);

		str = "MAX_VERTICAL_RECOIL" + s;
		ImGui::DragFloat(str.c_str(), &MAX_V_RECOIL);
		str = "Once_VERTICAL_RECOIL" + s;
		ImGui::DragFloat(str.c_str(), &ONCE_V_RECOIL);
		str = "vertical_recoil" + s;
		ImGui::SliderFloat(str.c_str(), &vertical_recoil, 0.f, MAX_V_RECOIL);

		str = "MAX_HORIZONTAL_RECOIL" + s;
		ImGui::DragFloat(str.c_str(), &MAX_H_RECOIL);
		str = "Once_HORIZONTAL_RECOIL" + s;
		ImGui::DragFloat(str.c_str(), &ONCE_H_RECOIL);
		str = "horizontal_recoil" + s;
		ImGui::SliderFloat(str.c_str(), &horizontal_recoil, 0.f, MAX_H_RECOIL);

	}
}


bool Firearm::Shoot()
{
	if (recast_time > 0.000001f)return false;

	cooling = true;
	recast_time = MAX_RECAST_TIME;

	vertical_recoil += ONCE_V_RECOIL;
	horizontal_recoil += ONCE_H_RECOIL;

	v_recoil_control = 1.f;
	h_recoil_control = 1.f;

	return true;
}

void Firearm::Cooling()
{
	recast_time -= parent->delta_time;

	if (recast_time <= 0.f)
	{
		cooling = false;
		recast_time = 0.f;
	}
}
