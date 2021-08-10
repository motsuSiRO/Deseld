#include "GatesQuad.h"
#include "StateMachine.h"
#include "model_renderer.h"



void GateQuad::Start()
{
	phong = std::make_shared<ShaderEx>();
	phong->Create(L"model_vs", L"model_ps");

	std::shared_ptr<ModelResource> model_resource;
	Mo2Lib::ModelLoader::GetInstance().Setup(true);

	Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/enemy/gates_quad.fbx");

	model_resource = Mo2Lib::ModelLoader::GetInstance().Apply();
	model = std::make_unique<Mo2Lib::Model>(model_resource);

	Mo2Lib::ModelLoader::GetInstance().End();


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