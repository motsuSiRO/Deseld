#include "GatesQuad.h"
#include "StateMachine.h"
#include "model_renderer.h"



void GateQuad::Start()
{
	phong = std::make_shared<ShaderEx>();
	phong->Create(L"model_vs", L"model_ps");

	const char* fbx_filename;
	std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();

	fbx_filename = "./Data/Assets/enemy/gates_quad_mesh.fbx";
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_SKINNED_MODEL);
	model = std::make_unique<Mo2Lib::Model>(model_resource);
	//model->InitializeAnimation();


	trans = parent->GetComponent<Transform>();

	trans->translate = { 0.f, 0.f, 300.f };
}

void GateQuad::Update()
{

}

void GateQuad::Draw()
{
	model->SetTransform(trans->translate, trans->GetFixedQuaternion(), trans->scale);

	Mo2Render().Draw(phong.get(), *model);
}

void GateQuad::ImGui()
{

}