#pragma once

#if 0
#include <d3d11.h>

namespace Texture
{

	HRESULT LoadFromFile(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);

}

#else

#pragma once
#include <wrl\client.h>
#include "DirectXTex.h"
class Texture
{
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView = nullptr;

	// テクスチャ情報
	D3D11_TEXTURE2D_DESC texture2d_desc;

public:
	Texture();
	virtual ~Texture();
	bool Load(const wchar_t* filename = nullptr);
	bool LoadArray(std::vector<const wchar_t*>& flinames);
	bool LoadMipMap(const wchar_t* filename = nullptr);

	void Set(UINT Slot = 0, BOOL flg = TRUE);
	void SetArray(UINT Slot = 0, BOOL flg = TRUE);

	UINT GetWidth() { return texture2d_desc.Width; }
	UINT GetHeight() { return texture2d_desc.Height; }
	bool Create(u_int width, u_int height, DXGI_FORMAT format);
	bool CreateMipMap(u_int width, u_int height, DXGI_FORMAT format);
	// 深度ステンシルバッファ作成
	bool CreateDepth(u_int width, u_int height, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);


	ID3D11RenderTargetView* GetRTV() { return RenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDSV() { return DepthStencilView.Get(); }
	ID3D11ShaderResourceView* GetSRV() { return ShaderResourceView.Get(); }

	//CubeMap用
	bool CreateCube(u_int width, u_int height, DXGI_FORMAT format, int miplevel = 1);
	bool CreateCubeDepthStencil(u_int width, u_int height);


};


#endif