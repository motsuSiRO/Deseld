#pragma once


// UNIT.07
#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "misc.h"

class Rasterizer
{
public:
	enum RS_STATE
	{
		WIRE_CULLN = 0,
		SOLID_CULLN,
		SOLID_CULLF,
		SOLID_CULLB,
		MODE_END
	};


	Microsoft::WRL::ComPtr<ID3D11RasterizerState> states[MODE_END];

	void Init(ID3D11Device* device)
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

		HRESULT hr = device->CreateRasterizerState(&desc, states[WIRE_CULLN].GetAddressOf());
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

		hr = device->CreateRasterizerState(&desc, states[SOLID_CULLN].GetAddressOf());
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

		hr = device->CreateRasterizerState(&desc, states[SOLID_CULLF].GetAddressOf());
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

		hr = device->CreateRasterizerState(&desc, states[SOLID_CULLB].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
private:
	Rasterizer() {}
	~Rasterizer() {}

public:

	static Rasterizer* GetInstance()
	{
		static Rasterizer ins;
		return &ins;
	}

};

#define Rasterizer Rasterizer::GetInstance()