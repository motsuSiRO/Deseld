#include "Reticule.h"
#include "Camera.h"
#include "Mo2GUI.h"
#include "Player.h"
#include "framework.h"

void Reticule::Initialize()
{

	ID3D11Device* device = Mo2System->DX11device.Get();

	rota[HORIZONTAL] = DirectX::XMQuaternionIdentity();
	rota[VERTICAL] = DirectX::XMQuaternionRotationAxis(CAM_LIST()->main_cam->GetFront_VEC(), DirectX::XMConvertToRadians(90.f));;

	line = std::make_unique<GeoLine>(device);
	line->CreateLine(device);
	line_tex = std::make_unique<Sprite>(device, L"./Data/Assets/reticule/reticule.png");
	center_pt = std::make_unique<Sprite>(device, L"./Data/Assets/reticule/reticule_center.png");
}

void Reticule::Update(float dt)
{
	Camera* m_cam = CAM_LIST()->main_cam;
	const Mo2Lib::Float3* axis[MAX_] = { &m_cam->GetRight(), &m_cam->GetUp() };

	center = m_cam->GetEye() - m_cam->GetFront() * 10.f;
	center.z = -center.z;



	direction[HORIZONTAL] = m_cam->GetRight();
	direction[VERTICAL] = m_cam->GetUp();
	direction[HORIZONTAL].z = -direction[HORIZONTAL].z;
	direction[VERTICAL].z = -direction[VERTICAL].z;

	//center_pt->transform.translate = center;
}


void Reticule::Render(ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& view_projection)
{
	Camera* m_cam = CAM_LIST()->main_cam;
	// 画面サイズ取得のためビューポートを取得
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;

	context->RSGetViewports(&num_viewports, &viewport);

	Mo2Lib::Float2 c_pos = { viewport.Width / 2, viewport.Height / 2 };

	//center_pt->RenderExtendSprite(context, c_pos.x, c_pos.y-1, 16, 16, 0.2f, 0.2f);

	line_tex->RenderSprite(context, c_pos.x + offset[HORIZONTAL]-4, c_pos.y-2, 8, 4);
	line_tex->RenderSprite(context, c_pos.x - offset[HORIZONTAL], c_pos.y-2, 8, 4);
	line_tex->RenderRotaSprite2(context, c_pos.x-2, c_pos.y + offset[VERTICAL]-4, 8, 4, 4.f, 2.f, 90.f);
	line_tex->RenderRotaSprite2(context, c_pos.x-2, c_pos.y - offset[VERTICAL], 8, 4, 4.f, 2.f, 90.f);

	//Mo2Lib::Transform& transform = line->transform;
	//for (int i = 0; i < MAX_; i++)
	//{
	//	transform.rotate = rota[i];

	//	transform.translate = center + direction[i] * offset[i];
	//	line->render(context, view_projection);

	//	transform.translate = center - direction[i] * offset[i];

	//	line->render(context, view_projection);

	//}
}

void Reticule::ImGui()
{
	ImGui::Begin("Reticule");

	ImGui::DragFloat3("center##Reticule", &center.x);

	ImGui::DragFloat3("Horizon_Dir##Reticule", &direction[HORIZONTAL].x);
	ImGui::DragFloat3("Vertical_Dir##Reticule", &direction[VERTICAL].x);
	ImGui::DragFloat("Horizon_Offset##Reticule", &offset[HORIZONTAL]);
	ImGui::DragFloat("Vertical_Offset##Reticule", &offset[VERTICAL]);



	ImGui::End();
}