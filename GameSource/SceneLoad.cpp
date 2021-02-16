#include "SceneLoad.h"

#include "framework.h"
#include <thread>
#include "Mo2GUI.h"

void SceneLoad::Initialize()
{
	ID3D11Device* device = Mo2System->DX11device.Get();
	ID3D11DeviceContext* context = Mo2System->DX11context.Get();

	spr = std::make_unique<Sprite>(device, L"./Data/Assets/textures/appeear_efx.png");

	// �X���b�h�J�n�ithis�͈����Astatic�֐��ȊO���ĂԂ̂ŕK�v�j
	std::thread thread(LoadingThread, this);
	// �����̊֐��������Ă�LoadingThead()�����s�����邽�ߎ����
	thread.detach();



	// ��ʃT�C�Y�擾�̂��߃r���[�|�[�g���擾
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;

	context->RSGetViewports(&num_viewports, &viewport);

	// �p�x�����W�A��(��)�ɕϊ�
	//float fov_y = DirectX::XMConvertToRadians(80.0f);	// ��p
	float aspect = viewport.Width / viewport.Height;	// ��ʔ䗦


}


void SceneLoad::LoadingThread(SceneLoad* scene)
{

	scene->nextScene->Initialize();
	scene->nextScene->initialized = true;
}


void SceneLoad::Update(float elapsedTime)
{


	// ���̃V�[����
	if (nextScene->initialized)
	{
		SceneMgr.ChangeScene(nextScene.release());
		return;
	}



}


void SceneLoad::Render()
{
	//static float angle = 0.f;
	//framework& frame = framework::GetInstance();
	//ID3D11Device* device = frame.device.Get();
	//ID3D11DeviceContext* context = frame.immediate_context.Get();

	//angle++;
	//if (angle >= 360.f)
	//{
	//	angle -= 360.f;
	//}

	//context->OMSetBlendState(Blenderer->states[Blenderer->MODE_ADD].Get(), nullptr, 0xFFFFFFFF);

	//for (int i = 0; i < 5; i++)
	//{
	//	spr->render(context, 100 + (i%10)*64, 100 + (i/10)*16,
	//		164 + (i % 10) * 64, 164 + (i / 10) * 16,
	//		0, 0, 64, 64, 0, 0, angle);
	//}
	//spr->render(context, 100, 100,
	//	164, 164,
	//	0, 0, 64, 64, 0, 0, angle);

	Mo2Gui()->ShowFPS();
}
