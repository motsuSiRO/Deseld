#include "Stage01.h"
#include "model_renderer.h"
#include "framework.h"

void Stage01::Start()
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	trans = parent->GetComponent<Transform>();

	trans->scale = { 5.f, 50.f, 5.f };

	std::shared_ptr<ModelResource> model_resource;
	Mo2Lib::ModelLoader::GetInstance().Setup(/*true*/);

	Mo2Lib::ModelLoader::GetInstance().Load("./Data/Assets/Field/dezzart_square.fbx");

	model_resource = Mo2Lib::ModelLoader::GetInstance().Apply();
	model = std::make_unique<Mo2Lib::Model>(model_resource);
	Mo2Lib::ModelLoader::GetInstance().End();

	phong = std::make_shared<ShaderEx>();
	phong->Create(L"model_vs", L"model_ps");

}

void Stage01::Update()
{

}

void Stage01::Draw()
{
	model->SetTransform(trans->translate, trans->GetFixedQuaternion(), trans->scale);

	Mo2Render().Draw(phong.get(), *model);
}

void Stage01::ImGui()
{

}