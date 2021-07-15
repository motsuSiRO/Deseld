
#if 0
#include "Texture.h"
#include "misc.h"

#include <WICTextureLoader.h>
#include <wrl.h>
#include <map>
#include <string>
#include <iostream>

namespace Texture
{
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;

	HRESULT LoadFromFile(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		//hr = DirectX::CreateWICTextureFromFile(device, file_name, resource.GetAddressOf(), shader_resource_view);
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	
		//std::wstring full_path(file_name);
		//int path_index = full_path.find_last_of(L"\\")+1;
		//std::wstring name(full_path.substr(path_index, full_path.size() - path_index));

		//file_path += file_name;
		// UNIT.05
		auto it = cache.find(file_name);
		if (it != cache.end())
		{
			//it->second.Attach(*shader_resource_view);
			*shader_resource_view = it->second.Get();
			(*shader_resource_view)->AddRef();
			(*shader_resource_view)->GetResource(resource.GetAddressOf());
		}
		else
		{
			// UNIT.04
			hr = DirectX::CreateWICTextureFromFile(device, file_name, resource.GetAddressOf(), shader_resource_view);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			// UNIT.05
			cache.insert(std::make_pair(file_name, *shader_resource_view));
		}

		if (texture2d_desc)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			texture2d->GetDesc(texture2d_desc);
		}

		return hr;
	}
}
#else

#include "framework.h"
#include "Texture.h"
//#include "Sampler.h"
#if _DEBUG
#pragma comment ( lib, "DirectXTex/DirectXTexD.lib")
#else
#pragma comment ( lib, "DirectXTex/DirectXTex.lib")
#endif


Texture::Texture() :ShaderResourceView(nullptr)
{
}



Texture::~Texture()
{
}


bool Texture::Load(const wchar_t* filename)
{
	ID3D11Device* device = Mo2System->DX11device.Get();


	// �摜�t�@�C���ǂݍ��� DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	HRESULT hr = LoadFromWICFile(filename, 0, &metadata, image);

	assert(SUCCEEDED(hr));

	// �摜����V�F�[�_���\�[�XView
	hr = DirectX::CreateShaderResourceView(
		device,
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	//�e�N�X�`���f�[�^�擾
	texture2d_desc.Width = metadata.width;
	texture2d_desc.Height = metadata.height;


	return true;
}


bool Texture::LoadMipMap(const wchar_t* filename)
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	// �摜�t�@�C���ǂݍ��� DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	HRESULT hr = LoadFromWICFile(filename, 0, &metadata, image);
	assert(SUCCEEDED(hr));


	//�~�b�v�}�b�v����
	DirectX::ScratchImage mipChain;
	DirectX::GenerateMipMaps(
		image.GetImages(),		//���摜
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_DEFAULT,
		0,						//�~�b�v�}�b�v�����ׂč��
		mipChain);

	// �摜����V�F�[�_���\�[�XView
	hr = CreateShaderResourceView(
		device,
		mipChain.GetImages(),
		mipChain.GetImageCount(),
		mipChain.GetMetadata(),
		ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));
	//�e�N�X�`���f�[�^�擾
	texture2d_desc.Width = metadata.width;
	texture2d_desc.Height = metadata.height;

	return true;
}

//bool Texture::LoadMipMap(const wchar_t* filename)
//{
//	ID3D11Device* device = pSystem->GetDevice();
//	HRESULT hr = S_OK;
//
//	//	�V�F�[�_�[���\�[�X�r���[�쐬
//	ComPtr<ID3D11Resource>resource;
//	//hr = DirectX::CreateWICTextureFromFile(device,
//	//	filename, resource.GetAddressOf(), ShaderResourceView.GetAddressOf());
//	hr = DirectX::CreateWICTextureFromFileEx(device,
//		filename,
//		0,
//		D3D11_USAGE_DEFAULT, 
//		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
//		0,
//		D3D11_RESOURCE_MISC_GENERATE_MIPS,
//		DirectX::WIC_LOADER_FORCE_SRGB,
//		resource.GetAddressOf(), ShaderResourceView.GetAddressOf());
//
//
//
//	if (FAILED(hr))
//	{
//		assert(SUCCEEDED(hr));
//		return false;
//	}
//
//	//�e�N�X�`�����擾
//	ComPtr<ID3D11Texture2D> m_pTexture2D;
//	resource.Get()->QueryInterface(m_pTexture2D.GetAddressOf());
//	m_pTexture2D.Get()->GetDesc(&texture2d_desc);
//
//	//	�T���v���X�e�[�g�쐬
//	D3D11_SAMPLER_DESC sd;
//	ZeroMemory(&sd, sizeof(sd));
//	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
//	sd.MinLOD = 0;
//	sd.MaxLOD = D3D11_FLOAT32_MAX;
//
//	hr = device->CreateSamplerState(
//		&sd, sampler.GetAddressOf());
//	assert(SUCCEEDED(hr));
//
//	return true;
//
//}
//

void Texture::Set(UINT Slot, BOOL flg)
{
	ID3D11DeviceContext* device_context = Mo2System->DX11context.Get();

	if (flg == FALSE) {

		ID3D11ShaderResourceView* rtv[1] = { NULL };
		ID3D11SamplerState* ss[1] = { NULL };
		device_context->PSSetShaderResources(Slot, 1, rtv);
		device_context->PSSetSamplers(Slot, 1, ss);
		device_context->DSSetShaderResources(Slot, 1, rtv);
		device_context->DSSetSamplers(Slot, 1, ss);
		return;
	}
	if (ShaderResourceView) {
		device_context->PSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
		device_context->DSSetShaderResources(Slot, 1, ShaderResourceView.GetAddressOf());
	}
}

bool Texture::Create(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;

	HRESULT hr = S_OK;
	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�����_�[�^�[�Q�b�g�r���[�쐬
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}

bool Texture::CreateDepth(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	// �[�x�X�e���V���r���[�ݒ�
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(Texture2D.Get(), &dsvd, DepthStencilView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;
}

bool Texture::CreateCube(u_int width, u_int height, DXGI_FORMAT format, int miplevel)
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = miplevel;//��
	texture2d_desc.ArraySize = 6;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	//	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�����_�[�^�[�Q�b�g�r���[�쐬
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;//2D�e�N�X�`���z��
	rtvd.Texture2DArray.FirstArraySlice = 0;
	rtvd.Texture2DArray.ArraySize = 6;
	rtvd.Texture2DArray.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.TextureCube.MostDetailedMip = 0;
	srvd.TextureCube.MipLevels = miplevel;
	//srvd.TextureCube.MipLevels = 0;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	return true;

}
bool Texture::CreateCubeDepthStencil(u_int width, u_int height)
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 6;
	texture2d_desc.Format = DXGI_FORMAT_D32_FLOAT;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	//	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�f�v�X�X�e���V���r���[�쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;//2D�e�N�X�`���z��
	dsvd.Texture2DArray.FirstArraySlice = 0;
	dsvd.Texture2DArray.ArraySize = 6;
	dsvd.Texture2DArray.MipSlice = 0;
	dsvd.Flags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	hr = device->CreateDepthStencilView(Texture2D.Get(), &dsvd, DepthStencilView.GetAddressOf());
	assert(SUCCEEDED(hr));

	return true;


}




bool Texture::CreateMipMap(u_int width, u_int height, DXGI_FORMAT format)
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	//	�e�N�X�`���쐬
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 9;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�����_�[�^�[�Q�b�g�r���[�쐬
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 9;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	///////////////////////////////////////
	//	//DirectX::TexMetadata metadata;
	//	DirectX::ScratchImage image;
	//	//HRESULT hr = LoadFromWICFile(filename, 0, &metadata, image);
	////	DirectX::SaveToWICMemory(&image, 0,format,
	//
	//	//�~�b�v�}�b�v����
	//	DirectX::ScratchImage mipChain;
	//	DirectX::GenerateMipMaps(
	//		image.GetImages(),		//���摜
	//		image.GetImageCount(),
	//		image.GetMetadata(),
	//		DirectX::TEX_FILTER_DEFAULT,
	//		0,						//�~�b�v�}�b�v�����ׂč��
	//		mipChain);
	//
	//	// �摜����V�F�[�_���\�[�XView
	//	hr = CreateShaderResourceView(
	//		device,
	//		mipChain.GetImages(),
	//		mipChain.GetImageCount(),
	//		mipChain.GetMetadata(),
	//		ShaderResourceView.GetAddressOf());
	//
	//	assert(SUCCEEDED(hr));
	//
	////////////////////////////////////////////////////////////


	return true;
}

#endif
