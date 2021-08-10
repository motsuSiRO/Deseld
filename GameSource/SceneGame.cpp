
#include "SceneGame.h"
#include "GameSystem.h"

#include "Camera.h"
#include "GameOption.h"
#include "Mo2GUI.h"
#include "Collision.h"
#include "framework.h"
#include "Blender.h"
#include "Input.h"
#include "Firearm.h"
#include "Physics.h"
#include "PlayerControl.h"
#include "Firearm.h"
#include "GhostShip03.h"
#include "TheEmblion.h"
#include "GatesQuad.h"
#include "Stage01.h"
#include "ColliderComponents.h"
//**********************************************
//
//		メインシーン
//
//**********************************************

//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void SceneGame::Initialize()
{

	ID3D11Device* device = Mo2System->DX11device.Get();;
	ID3D11DeviceContext* context = Mo2System->DX11context.Get();

	Object* obj = new Object("Player");
	obj->AddComponent<Transform>();
	obj->AddComponent<Physics2>();
	obj->AddComponent<PlayerControl>();
	//obj->AddComponent<Firearm>();
	obj->AddComponent<GhostShip03>();
	obj_list.emplace_back(obj);

	obj = new Object("Stage01");
	obj->AddComponent<Transform>();
	obj->AddComponent<Stage01>();
	obj_list.emplace_back(obj);

	obj = new Object("TheEmblion");
	obj->AddComponent<Transform>();
	obj->AddComponent<Physics2>();
	//obj->AddComponent<Firearm>();
	obj->AddComponent<TheEmblion>();
	obj_list.emplace_back(obj);


	skinned_bp = std::make_unique<ShaderEx>();
	skinned_bp->Create(L"SkinnedModel_VS", L"SkinnedModel_PS");

	static_bp = std::make_unique<ShaderEx>();
	static_bp->Create(L"StaticModel_VS", L"StaticModel_PS");

	skybox_NoL = std::make_unique<ShaderEx>();
	skybox_NoL->Create(L"NoLightObj_VS", L"NoLightObj_PS");

	std::vector<D3D11_INPUT_ELEMENT_DESC> bIL(std::begin(billbord_input_element_desc), std::end(billbord_input_element_desc));
	point_sprite = std::make_unique<ShaderEx>(bIL);
	point_sprite->Create(L"pointSprite_vs", L"pointSprite_gs", L"pointSprite_ps");

	sky_box = std::make_unique<SkyBox>(device);
	sky_box->Set(2);
	particleSys = std::make_unique<cParticleSystem>(10000);

	CAM_LIST()->Init();
	Mo2Render().Initialize(device, context);





	//Mo2CD()->Initialize();
	//Mo2CD()->LoadFaceData(0, "./Data/Assets/Field/dezzart_square.fbx");

	//
	//Mo2CD()->face_map[0].transforms.resize(1);
	//static float margin = 100800.f;
	////for (size_t i = 0; i < Mo2CD()->face_map[0].transforms.size(); ++i)
	//{
	//	//Mo2CD()->face_map[0].transforms[0].translate = { -margin + margin * (i % 3), 0.f, -margin + margin * (i / 3) };
	//	//Mo2CD()->face_map[0].transforms[0].rotate = DirectX::XMQuaternionRotationRollPitchYaw(0.f, DirectX::XMConvertToRadians(90.f) * (rand() % 4), 0.f);
	//	Mo2CD()->face_map[0].transforms[0].scale = { 5.f, 50.f, 5.f };
	//}

}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void SceneGame::Update(float elapsed_time)
{
	elapsed_time = min(0.1f, elapsed_time);

	auto buff = obj_list;
	for (auto obj : buff)
	{
		obj->Update(elapsed_time);

		if (obj->destroy)
		{
			obj_list.remove(obj);
		}
	}
	CAM_LIST()->Interpolation(elapsed_time);

	particleSys->Update();

	//static int time = 0;
	//time++;
	//if (time > 100)
	//{
	//	particleSys->Snow(GShip::arm_pos/*DirectX::XMFLOAT3(0.f, 0.f, 100.f)*/, 1);
	//	time = 0;
	//}
}


//--------------------------------------------------------
//		描画
//--------------------------------------------------------
void SceneGame::Render()
{
	ID3D11DeviceContext* context = Mo2System->DX11context.Get();

	// モデルの描画
	{
		// プロジェクション行列を作成
		DirectX::XMMATRIX P, Sky_P;
		{
			// 画面サイズ取得のためビューポートを取得
			D3D11_VIEWPORT viewport;
			UINT num_viewports = 1;

			context->RSGetViewports(&num_viewports, &viewport);

			float aspect = viewport.Width / viewport.Height;	// 画面比率
			float fov_y = CAM_LIST()->main_cam->GetFOV();
			CAM_LIST()->main_cam->SetAspect(aspect);


			P = CAM_LIST()->GetPerspective();
			Sky_P = DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, 0.1f, 100.f);
		}

		// ビュー行列、プロジェクション行列を合成し行列データを取り出す。
		DirectX::XMFLOAT4X4 view_projection;
		
			DirectX::XMMATRIX C = DirectX::XMMatrixSet(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, -1, 0,
				0, 0, 0, 1
			);


			DirectX::XMMATRIX V;
			{
				V = CAM_LIST()->GetOriginView();
			}
			DirectX::XMMATRIX VP;
			VP = C * V * Sky_P;

			DirectX::XMStoreFloat4x4(&view_projection, VP);
		
		Mo2Render().Begin(CAM_LIST()->main_cam->GetEye(),
			view_projection,
			DirectX::XMFLOAT4(0, -1, -1, 0)	// ライトの向き
		);
		Mo2System->SetBlendState(BLEND_STATE::ALPHA);


		Mo2System->SetRSState(RS_STATE::RS_CULL_FRONT);
		sky_box->Render(Mo2System->DX11context.Get(), view_projection, true);


		Mo2System->SetRSState(RS_STATE::RS_CULL_BACK);

		{
			V = CAM_LIST()->GetView();
		}

		VP = C * V * P;
		DirectX::XMStoreFloat4x4(&view_projection, VP);
		Mo2Render().Begin(CAM_LIST()->main_cam->GetEye(),
			view_projection,
			DirectX::XMFLOAT4(0, -1, -1, 0)	// ライトの向き
		);

		//Mo2Render().Draw(skybox_NoL.get(), *model);


		for (auto obj : obj_list)
		{
			obj->Draw();
		}

		Mo2Render().End();


		particleSys->Render(point_sprite.get(), &(C*V), &P);



	}

#ifdef _DEBUG
	ImGui();

#endif
}



void SceneGame::ImGui()
{
	ID3D11DeviceContext* context = Mo2System->DX11context.Get();


	// 画面サイズ取得のためビューポートを取得
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;

	context->RSGetViewports(&num_viewports, &viewport);
	//ImGui::SetNextWindowPos(ImVec2(0, f.screen_height / 2));
	//ImGui::SetNextWindowSize(ImVec2(200, f.screen_height / 2));


	ImGui::Begin("SCENE_GAME");
	{

		Mo2Gui()->ImGui();
		visiblity = Mo2Gui()->visible_collider;

		static int anim_index = 0;
		static float anim_speed = 1.0f;
		static float blend_speed = 1.0f;
		ImGui::Text("%.3f  %.3f", acosf(rad), DirectX::XMConvertToDegrees(acosf(rad)));
		ImGui::Text("%.3f  %.3f", rad, DirectX::XMConvertToDegrees(rad));

		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250.f, 100.f), ImGuiWindowFlags_NoTitleBar);
		static bool* only_obj;
		for (auto& obj : obj_list)
		{
			ImGui::Text(obj->tag);
			ImGui::SameLine();
			std::string button = "T/F##" + std::to_string(obj->GetID());
			if (ImGui::Button(button.c_str()))
			{
				if (only_obj != &obj->selected)
				{
					if (only_obj)*only_obj = false;
					only_obj = &obj->selected;
					*only_obj = true;
				}
				else
				{
					if (!only_obj)only_obj = &obj->selected;
					*only_obj ^= (bool)1;
				}
			}
		}
		ImGui::EndChild();


		ImGui::End();

	}

	sky_box->ImGui();

	if (Mo2Gui()->win_showFPS)Mo2Gui()->ShowFPS();

	if (Mo2Gui()->win_option)GAME_OPTION.ImGui();

	if (Mo2Gui()->win_input)INPUT.ImGui();

	if (Mo2Gui()->win_camera)CAM_LIST()->ImGui();

	//if (Mo2Gui()->win_reticule)reticule->ImGui();

	if (Mo2Gui()->win_cursor)
	{
		while (1)
		{
			int show_count = ShowCursor(TRUE);
			if (show_count > 0)
			{
				break;
			}
		}
	}
	else
	{
		while (1)
		{
			int show_count = ShowCursor(FALSE);
			if (show_count < 0)
			{
				break;
			}
		}
		SetCursorPos((int)(viewport.Width / 2.f), (int)(viewport.Height / 2.f));
	}


	for (auto obj : obj_list)
	{
		obj->ImGui();

		for (auto com : obj->component_list)
		{
			if (dynamic_cast<ColliderComponent*>(com))
			{
				com->is_visible = Mo2Gui()->visible_collider;
			}
		}

	}


}


//#undef Mo2_GUI
//#undef CAM_LIST