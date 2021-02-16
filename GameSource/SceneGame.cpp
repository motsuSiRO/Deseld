
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
#include "GhostShip03.h"

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
	obj->AddComponent<Firearm>();
	obj->AddComponent<GhostShip03>();

	//obj_list.push_back(obj);
	Mo2Lib::game.obj_list.emplace_back(obj);
	//obj_list.emplace_back(new Object);
	//obj_list.emplace_back(new Object);
	//obj_list.emplace_back(new Object);

	skinned_bp = std::make_unique<ShaderEx>();
	skinned_bp->Create(L"SkinnedModel_VS", L"SkinnedModel_PS");

	static_bp = std::make_unique<ShaderEx>();
	static_bp->Create(L"StaticModel_VS", L"StaticModel_PS");

	skybox_NoL = std::make_unique<ShaderEx>();
	skybox_NoL->Create(L"NoLightObj_VS", L"NoLightObj_PS");

	//sphere = std::make_unique<GeoPrimitive>(device);
	//sphere->CreateSphere(device, 16, 16);
	//sphere->transform.scale = { 50.f, 50.f, 50.f };

	sky_box = std::make_unique<SkyBox>();
	sky_box->Initialize();

	CAM_LIST()->Init();
	ModelRenderer::GetInstance().Initialize(device, context);
	//model_renderer = std::make_unique<ModelRenderer>(device, context);


	reticule = std::make_unique<Reticule>();
	reticule->Initialize();

	std::shared_ptr<ModelData> m_data = std::make_shared<ModelData>();
	std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();

	const char* fbx_filename = "./Data/Assets/Field/dezzart_square.fbx";

	if (!m_data->Load(fbx_filename, ModelData::BINARY))
	{
		m_data->SaveToFile();
	}
	model_resource->Load(device, m_data.get(), LOAD_SKINNED_MODEL);
	model = std::make_unique<Mo2Lib::Model>(model_resource);


	//DirectX::XMStoreFloat4(&model->rotate, DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(-90.f), 0.f, 0.f));
	//model->scale = { 100.f, 100.f, 100.f };

	fbx_filename = "./Data/Assets/enemy/gates_quad_mesh.fbx";
	m_data->Clear();
	model_resource = std::make_shared<ModelResource>();

	if (!m_data->Load(fbx_filename, ModelData::BINARY))
	{
		m_data->SaveToFile();
	}
	model_resource->Load(device, m_data.get(), LOAD_SKINNED_MODEL);
	model2 = std::make_unique<Mo2Lib::Model>(model_resource);


	Mo2CD()->Initialize();
	Mo2CD()->LoadFaceData(0, "./Data/Assets/Field/dezzart_square.fbx");

	
	Mo2CD()->face_map[0].transforms.resize(9);
	static float margin = 100800.f;
	for (size_t i = 0; i < Mo2CD()->face_map[0].transforms.size(); ++i)
	{
		Mo2CD()->face_map[0].transforms[i].translate = { -margin + margin * (i % 3), 0.f, -margin + margin * (i / 3) };
		Mo2CD()->face_map[0].transforms[i].rotate = DirectX::XMQuaternionRotationRollPitchYaw(0.f, DirectX::XMConvertToRadians(90.f) * (rand() % 4), 0.f);
		Mo2CD()->face_map[0].transforms[i].scale = { 1.f, 1.f, 1.f };
	}

}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------
void SceneGame::Update(float elapsed_time)
{
	elapsed_time = min(0.1f, elapsed_time);

	auto buff = Mo2Lib::game.obj_list;
	for (auto obj : buff)
	{
		obj->Update(elapsed_time);

		if (obj->destroy)
		{
			Mo2Lib::game.obj_list.remove(obj);
		}
	}
	CAM_LIST()->Interpolation(elapsed_time);
}


//--------------------------------------------------------
//		描画
//--------------------------------------------------------
void SceneGame::Render()
{
	ID3D11DeviceContext* context = Mo2System->DX11context.Get();

	// モデルの描画
	{
		// ビュー行列を作成
		DirectX::XMMATRIX V;
		{
			V = CAM_LIST()->GetView();
		}

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

		}

		// ビュー行列、プロジェクション行列を合成し行列データを取り出す。
		DirectX::XMFLOAT4X4 view_projection;
		{
			DirectX::XMMATRIX C = DirectX::XMMatrixSet(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, -1, 0,
				0, 0, 0, 1
			);


			DirectX::XMMATRIX VP;
			VP = C * V * P;

			DirectX::XMStoreFloat4x4(&view_projection, VP);
		}
		Mo2Render().Begin(CAM_LIST()->main_cam->GetEye(),
			view_projection,
			DirectX::XMFLOAT4(0, -1, -1, 0)	// ライトの向き
		);
		context->OMSetBlendState(Blender->states[Blender->ALPHA].Get(), nullptr, 0xFFFFFFFF);

		Mo2Render().RSSet(D3D11_CULL_FRONT);
		Mo2Render().Draw(skybox_NoL.get(), *sky_box->sky);
		Mo2Render().RSSet(D3D11_CULL_BACK);

		for (auto t : Mo2CD()->face_map[0].transforms)
		{
			model->SetTransform(t);
			Mo2Render().Draw(static_bp.get(), *model);
		}
		Mo2Render().Draw(skinned_bp.get(), *model2);


		for (auto obj : Mo2Lib::game.obj_list)
		{
			obj->Draw();
		}

		Mo2Render().End();


#ifdef _DEBUG 
		if (visiblity)
		{
			Mo2CD()->Render(context, view_projection,
				DirectX::XMFLOAT4(0, -1, -1, 0));
		}
#endif

		reticule->Render(context, view_projection);


	}


	ImGui();
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
		for (auto& obj : Mo2Lib::game.obj_list)
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

	if (Mo2Gui()->win_reticule)reticule->ImGui();

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



	for (auto obj : Mo2Lib::game.obj_list)
	{
		obj->ImGui();
	}


}


//#undef Mo2_GUI
//#undef CAM_LIST