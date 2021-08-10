
#include "scene.h"
#include "Input.h"
#include "Mo2GUI.h"
#include "SceneGame.h"
#include "SceneLoad.h"
#include "model_renderer.h"
#include "GameSystem.h"
#include "Camera.h"

//**********************************************
//
//		タイトルシーン
//
//**********************************************
//--------------------------------------------------------
//		初期化
//--------------------------------------------------------

void SceneTitle::Initialize()
{


}

//--------------------------------------------------------
//		更新
//--------------------------------------------------------

void SceneTitle::Update(float elapsedTime)
{
	if (INPUT.keys[DIK_SPACE] == 1)
	{
		Mo2Scene.ChangeScene(new SceneLoad(new SceneGame));
	}

}

//--------------------------------------------------------
//		描画
//--------------------------------------------------------

void SceneTitle::Render()
{
	//ID3D11DeviceContext* context = Mo2System->DX11context.Get();

	//// モデルの描画
	//{
	//	// ビュー行列を作成
	//	DirectX::XMMATRIX V;
	//	{
	//		V = CAM_LIST()->GetView();
	//	}

	//	// プロジェクション行列を作成
	//	DirectX::XMMATRIX P;
	//	{
	//		// 画面サイズ取得のためビューポートを取得
	//		D3D11_VIEWPORT viewport;
	//		UINT num_viewports = 1;

	//		context->RSGetViewports(&num_viewports, &viewport);

	//		float aspect = viewport.Width / viewport.Height;	// 画面比率
	//		float fov_y = CAM_LIST()->main_cam->GetFOV();
	//		CAM_LIST()->main_cam->SetAspect(aspect);


	//		P = CAM_LIST()->GetPerspective();

	//	}

	//	// ビュー行列、プロジェクション行列を合成し行列データを取り出す。
	//	DirectX::XMFLOAT4X4 view_projection;
	//	{
	//		DirectX::XMMATRIX C = DirectX::XMMatrixSet(
	//			1, 0, 0, 0,
	//			0, 1, 0, 0,
	//			0, 0, -1, 0,
	//			0, 0, 0, 1
	//		);


	//		DirectX::XMMATRIX VP;
	//		VP = C * V * P;

	//		DirectX::XMStoreFloat4x4(&view_projection, VP);
	//	}
	//	Mo2Render().Begin(CAM_LIST()->main_cam->GetEye(),
	//		view_projection,
	//		DirectX::XMFLOAT4(0, -1, -1, 0)	// ライトの向き
	//	);


	//	for (auto obj : Mo2Lib::game.obj_list)
	//	{
	//		obj->Draw();
	//	}

	//	Mo2Render().End();


	//}

	ImGui();
}


void SceneTitle::ImGui()
{
	//ImGui::ShowDemoWindow();

	INPUT.ImGui();

	Mo2Gui()->ShowFPS();
}