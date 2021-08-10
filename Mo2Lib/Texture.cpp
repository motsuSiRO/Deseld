
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
//#if _DEBUG
//#pragma comment ( lib, "ExternalLib/DirectXTex/DirectXTexD.lib")
//#else
//#pragma comment ( lib, "ExternalLib/DirectXTex/DirectXTex.lib")
//#endif


Texture::Texture() :ShaderResourceView(nullptr)
{
}



Texture::~Texture()
{
}


bool Texture::Load(const wchar_t* filename)
{
	ID3D11Device* device = Mo2System->DX11device.Get();


	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	HRESULT hr = LoadFromWICFile(filename, (DirectX::WIC_FLAGS)0, &metadata, image);

	assert(SUCCEEDED(hr));

	// 画像からシェーダリソースView
	hr = DirectX::CreateShaderResourceView(
		device,
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));

	//テクスチャデータ取得
	texture2d_desc.Width = metadata.width;
	texture2d_desc.Height = metadata.height;


	return true;
}

bool Texture::LoadArray(std::initializer_list<const wchar_t*> list)
{
	ID3D11Device* device = Mo2System->DX11device.Get();
	ID3D11DeviceContext* context = Mo2System->DX11context.Get();

#if 1
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage texture;
	std::vector<DirectX::Image> mem;
	
	HRESULT hr;

	std::vector<D3D11_SUBRESOURCE_DATA> srs_d;
	for (const wchar_t* filename : list)
	{
		DirectX::ScratchImage img;

		// 画像ファイル読み込み DirectXTex
		//hr = LoadFromWICFile(filename, 0, &metadata, img);
		hr = LoadFromDDSFile(filename, (DirectX::DDS_FLAGS)0, &metadata, img);

		assert(SUCCEEDED(hr));

		mem.push_back(*img.GetImages());

		//srs_d.emplace_back();
		//
		//srs_d.back().pSysMem = mem->pixels;
		//srs_d.back().SysMemPitch = mem->rowPitch;
		//srs_d.back().SysMemSlicePitch = mem->slicePitch;

		wchar_t fname[_MAX_PATH];
		::_wsplitpath_s(filename, nullptr, 0, nullptr, 0, fname, _MAX_PATH, nullptr, 0);
		std::wstring str =  L"./Data/Assets/Effect/";
		str += fname;
		str += L".dds";
		DirectX::SaveToDDSFile(mem.back(), (DirectX::DDS_FLAGS)0, str.c_str());
	}
	

	hr = texture.InitializeArrayFromImages(mem.data(), mem.size());
	assert(SUCCEEDED(hr));
	
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	////	テクスチャ作成
	//ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	//texture2d_desc.Width = metadata.width;
	//texture2d_desc.Height = metadata.height;
	//texture2d_desc.MipLevels = 1;
	//texture2d_desc.ArraySize = srs_d.size();
	//texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//texture2d_desc.SampleDesc.Count = 1;
	//texture2d_desc.SampleDesc.Quality = 0;
	//texture2d_desc.Usage = D3D11_USAGE_IMMUTABLE;
	//texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	hr = DirectX::CreateShaderResourceView(device,
		texture.GetImages(),
		texture.GetImageCount(),
		texture.GetMetadata(),
		ShaderResourceView.GetAddressOf());
	assert(SUCCEEDED(hr));


	//device->CreateTexture2D(&texture2d_desc, srs_d.data(), Texture2D.GetAddressOf());
	// 画像からシェーダリソースView

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	//ZeroMemory(&srvd, sizeof(srvd));
	//srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	//srvd.Texture2DArray.MostDetailedMip = 0;
	//srvd.Texture2DArray.MipLevels = texture2d_desc.MipLevels;
	//srvd.Texture2DArray.ArraySize = texture2d_desc.ArraySize;

	//hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, ShaderResourceView.GetAddressOf());
	//assert(SUCCEEDED(hr));

#else
	HRESULT hr = E_FAIL;

	ID3D11Texture2D* pTexture = NULL;

	D3D11_TEXTURE2D_DESC desc;
	::ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage img;
	bool first_load = true;
	int index = 0;



	for (const wchar_t* filename : list)
	{

		ID3D11Resource* pLoadedRes = NULL;
		ID3D11Texture2D* pLoadedTexture2D = NULL;

		// 画像ファイルを読み込んでテクスチャーを作成する
		hr = LoadFromWICFile(filename, 0, &metadata, img);
		assert(SUCCEEDED(hr));

		// 空テクスチャー作成
		if (first_load)
		{
			// 画像ファイルを読み込んで作成したテクスチャーの情報を取得する

			desc.Width = metadata.width;
			desc.Height = metadata.height;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;

			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.ArraySize = list.size();
			hr = device->CreateTexture2D(&desc, NULL, &pTexture);
			assert(SUCCEEDED(hr));

			first_load = false;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		// ファイルから画像ファイルを読み込んで作成したテクスチャーを、読み込みモードでマップする
		//hr = context->Map(pLoadedTexture2D, NULL, D3D11_MAP_READ, 0, &mappedResource);


		//// 空テクスチャーのサブリソースをファイルから画像ファイルを読み込んで作成したテクスチャーの内容で更新する
		//for (UINT iMip = 0; iMip < desc.MipLevels; iMip++)
		//{
		//	context->UpdateSubresource(pTexture,
		//		D3D11CalcSubresource(iMip, index, desc.MipLevels),
		//		NULL,
		//		img.GetPixels(),
		//		img.GetImages()->rowPitch,
		//		0
		//	);
		//}
		//context->Unmap(pLoadedTexture2D, NULL);

		//SAFE_RELEASE(pLoadedTexture2D);
		//SAFE_RELEASE(pLoadedRes);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
	SRVDesc.Texture2DArray.ArraySize = list.size();
	// シェーダーリソースビューを作成する
	hr = device->CreateShaderResourceView(pTexture, &SRVDesc, ShaderResourceView.GetAddressOf());


	hr = S_OK;

	index++;

#endif

	return true;
}


bool Texture::LoadMipMap(const wchar_t* filename)
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	// 画像ファイル読み込み DirectXTex
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	HRESULT hr = LoadFromWICFile(filename, (DirectX::WIC_FLAGS)0, &metadata, image);
	assert(SUCCEEDED(hr));


	//ミップマップ生成
	DirectX::ScratchImage mipChain;
	DirectX::GenerateMipMaps(
		image.GetImages(),		//元画像
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_DEFAULT,
		(DirectX::TEX_FILTER_FLAGS)0,						//ミップマップをすべて作る
		mipChain);

	// 画像からシェーダリソースView
	hr = CreateShaderResourceView(
		device,
		mipChain.GetImages(),
		mipChain.GetImageCount(),
		mipChain.GetMetadata(),
		ShaderResourceView.GetAddressOf());

	assert(SUCCEEDED(hr));
	//テクスチャデータ取得
	texture2d_desc.Width = metadata.width;
	texture2d_desc.Height = metadata.height;

	return true;
}

//bool Texture::LoadMipMap(const wchar_t* filename)
//{
//	ID3D11Device* device = pSystem->GetDevice();
//	HRESULT hr = S_OK;
//
//	//	シェーダーリソースビュー作成
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
//	//テクスチャ情報取得
//	ComPtr<ID3D11Texture2D> m_pTexture2D;
//	resource.Get()->QueryInterface(m_pTexture2D.GetAddressOf());
//	m_pTexture2D.Get()->GetDesc(&texture2d_desc);
//
//	//	サンプラステート作成
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

void Texture::SetArray(UINT Slot, BOOL flg)
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
	//	テクスチャ作成
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

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
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
	//	テクスチャ作成
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

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(Texture2D.Get(), &dsvd, DepthStencilView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
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
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = miplevel;//★
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

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;//2Dテクスチャ配列
	rtvd.Texture2DArray.FirstArraySlice = 0;
	rtvd.Texture2DArray.ArraySize = 6;
	rtvd.Texture2DArray.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
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
	//	テクスチャ作成
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

	//	デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;//2Dテクスチャ配列
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
	//	テクスチャ作成
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

	//	レンダーターゲットビュー作成
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, RenderTargetView.GetAddressOf());
	assert(SUCCEEDED(hr));

	//	シェーダーリソースビュー作成
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
	//	//ミップマップ生成
	//	DirectX::ScratchImage mipChain;
	//	DirectX::GenerateMipMaps(
	//		image.GetImages(),		//元画像
	//		image.GetImageCount(),
	//		image.GetMetadata(),
	//		DirectX::TEX_FILTER_DEFAULT,
	//		0,						//ミップマップをすべて作る
	//		mipChain);
	//
	//	// 画像からシェーダリソースView
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
