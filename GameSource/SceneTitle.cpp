
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
//		�^�C�g���V�[��
//
//**********************************************
//--------------------------------------------------------
//		������
//--------------------------------------------------------

void SceneTitle::Initialize()
{


}

//--------------------------------------------------------
//		�X�V
//--------------------------------------------------------

void SceneTitle::Update(float elapsedTime)
{
	if (INPUT.keys[DIK_SPACE] == 1)
	{
		Mo2Scene.ChangeScene(new SceneLoad(new SceneGame));
	}

}

//--------------------------------------------------------
//		�`��
//--------------------------------------------------------

void SceneTitle::Render()
{
	//ID3D11DeviceContext* context = Mo2System->DX11context.Get();

	//// ���f���̕`��
	//{
	//	// �r���[�s����쐬
	//	DirectX::XMMATRIX V;
	//	{
	//		V = CAM_LIST()->GetView();
	//	}

	//	// �v���W�F�N�V�����s����쐬
	//	DirectX::XMMATRIX P;
	//	{
	//		// ��ʃT�C�Y�擾�̂��߃r���[�|�[�g���擾
	//		D3D11_VIEWPORT viewport;
	//		UINT num_viewports = 1;

	//		context->RSGetViewports(&num_viewports, &viewport);

	//		float aspect = viewport.Width / viewport.Height;	// ��ʔ䗦
	//		float fov_y = CAM_LIST()->main_cam->GetFOV();
	//		CAM_LIST()->main_cam->SetAspect(aspect);


	//		P = CAM_LIST()->GetPerspective();

	//	}

	//	// �r���[�s��A�v���W�F�N�V�����s����������s��f�[�^�����o���B
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
	//		DirectX::XMFLOAT4(0, -1, -1, 0)	// ���C�g�̌���
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