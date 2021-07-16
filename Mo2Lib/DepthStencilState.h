#pragma once


#include <d3d11.h>
#include <wrl.h>

#include "misc.h"

namespace DS_STATE
{
	enum enumDS_STATE
	{
		D_TRUE_W_TRUE = 0,
		D_TRUE_W_FALSE,
		D_FALSE_W_TRUE,
		D_FALSE_W_FALSE,
		MODE_END,
	};
}

class DepthStencilState
{
public:

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> states[DS_STATE::MODE_END];


	void Create(ID3D11Device* device)
	{

		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;

		HRESULT hr = device->CreateDepthStencilState(&desc, states[DS_STATE::D_TRUE_W_TRUE].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS;

		hr = device->CreateDepthStencilState(&desc, states[DS_STATE::D_TRUE_W_FALSE].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateDepthStencilState(&desc, states[DS_STATE::D_FALSE_W_TRUE].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

		hr = device->CreateDepthStencilState(&desc, states[DS_STATE::D_FALSE_W_FALSE].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}

	DepthStencilState() {}
	~DepthStencilState() {}
};

