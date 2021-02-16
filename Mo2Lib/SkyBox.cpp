#include "SkyBox.h"
#include "Texture.h"
#include <Windows.h>
#include <string>
#include "framework.h"
#include "Mo2GUI.h"


void SkyBox::Initialize()
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	const char* fbx_filename = "./Data/Assets/SkyBox/skybox.fbx";
	std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_SKINNED_MODEL);
	//if (!model_resource->LoadFromBinary(device, fbx_filename))
	//{
	//	model_resource->SaveToData(fbx_filename);
	//}
	sky = std::make_unique<Mo2Lib::Model>(model_resource);
	sky->transform.scale = { 50.f, 50.f, 50.f };
		
	
	HANDLE hFind;
	//defined at Windwos.h
	WIN32_FIND_DATA win32fd;
	//�t�@�C�����̔z��
	std::vector<std::wstring> file_names;

	//�T���t�@�C�����w��@���C���h�J�[�h���g�p
	std::string search_name = "./Data/Assets/SkyBox\\*.png";
	//�t�@�C������
	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	//������Ȃ�������
	if (hFind == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("file not found");
	}

	//���̃t�@�C�����������ǂݍ��ݑ�����
	int index = 0;
	while (FindNextFile(hFind, &win32fd)) 
	{
		//�f�B���N�g���Ȃ疳��
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {}
		else
		{
			//�t�@�C�������p�X�t�Ŏ擾
			std::string filename = win32fd.cFileName;
			filename.insert(0, "./Data/Assets/SkyBox/");
			size_t length;
			wchar_t wfilename[256];
			::mbstowcs_s(&length, wfilename, 256, filename.c_str(), _TRUNCATE);


			Texture::LoadFromFile(device, wfilename, tex_list[index].GetAddressOf(), nullptr);
		}
		++index;
	}
	//����
	FindClose(hFind);

	//if (tex_list.size() > 0)
	//{
	//	Set(0);
	//}
}

bool SkyBox::Set(int index)
{
	if (tex_list.size() <= (size_t)index)return false;

	sky->GetModelResource()->Set(tex_list[index].Get());
}

void SkyBox::ImGui()
{
	if (ImGui::Begin("SkyBox"))
	{
		static int tex_num = 0;
		ImGui::Text("MAX_TEX_NUM : %d", tex_list.size());
		ImGui::InputInt("SetTexNum", &tex_num);

		if (tex_list.size() > 0)
		{
			Mo2Lib::Clamp(tex_num, 0, (int)tex_list.size() - 1);

			if (ImGui::Button("SetTexture"))
			{
				Set(tex_num);
			}
		}
		else
		{
			tex_num = 0;
		}


		ImGui::End();
	}
}



