#include "Stage01.h"
#include "model_renderer.h"
#include "framework.h"

void Stage01::Start()
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	trans = parent->GetComponent<Transform>();

	trans->scale = { 5.f, 50.f, 5.f };

	std::shared_ptr<ModelData> m_data = std::make_shared<ModelData>();
	std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();

	const char* fbx_filename = "./Data/Assets/Field/dezzart_square.fbx";

	if (!m_data->Load(fbx_filename, ModelData::BINARY))
	{
		m_data->SaveToFile();
	}
	model_resource->Load(device, m_data.get(), LOAD_SKINNED_MODEL);
	model = std::make_unique<Mo2Lib::Model>(model_resource);

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