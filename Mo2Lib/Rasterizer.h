#pragma once


// UNIT.07
#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "misc.h"
//#include "Singleton.h"

namespace RS_STATE
{
	enum enumRS_STATE
	{
		RS_WIREFRAME = 0,
		RS_CULL_NONE,
		RS_CULL_FRONT,
		RS_CULL_BUCK,
		MODE_END
	};

}

class Rasterizer 
{
public:

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> states[RS_STATE::MODE_END];


	void Create(ID3D11Device* device)
	{

		//Mode : Wireframe  Culling : NONE
		D3D11_RASTERIZER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = false;

		HRESULT hr = device->CreateRasterizerState(&desc, states[RS_STATE::RS_WIREFRAME].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//Mode : Solid  Culling : NONE
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = false;

		hr = device->CreateRasterizerState(&desc, states[RS_STATE::RS_CULL_NONE].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//Mode : Solid  Culling : FRONT
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		desc.AntialiasedLineEnable = false;

		hr = device->CreateRasterizerState(&desc, states[RS_STATE::RS_CULL_FRONT].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//Mode : Solid  Culling : BACK
		::memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.AntialiasedLineEnable = false;

		hr = device->CreateRasterizerState(&desc, states[RS_STATE::RS_CULL_BUCK].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

public:
	Rasterizer() {}
	~Rasterizer() {}

};

