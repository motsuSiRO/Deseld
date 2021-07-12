#include "Sprite.h"

#include "Shader.h"
#include "misc.h"

// UNIT.04
#include "texture.h"

// UNIT.02
//sprite::sprite(ID3D11Device *device)
// UNIT.04
Sprite::Sprite(ID3D11Device* device, const wchar_t* file_name)
{
	HRESULT hr = S_OK;

	vertex vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0) },
	};
	D3D11_BUFFER_DESC buffer_desc = {};
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC; // UNIT.03
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // UNIT.03
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
	subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// UNIT.04
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = create_vs_from_cso(device, "./Data/System/ShaderObjects/Sprite_VS.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	hr = create_ps_from_cso(device, "./Data/System/ShaderObjects/Sprite_PS.cso", pixel_shader.GetAddressOf());

	D3D11_RASTERIZER_DESC rasterizer_desc = {}; //https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx
	rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizer_desc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0; //https://msdn.microsoft.com/en-us/library/windows/desktop/cc308048(v=vs.85).aspx
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = FALSE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// UNIT.04
	hr = Texture::LoadFromFile(device, file_name, &shader_resource_view, &texture2d_desc);

	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_ANISOTROPIC
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//Unit.06
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_NEVER;
	depth_stencil_desc.StencilEnable = FALSE;

	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//D3D11_BLEND_DESC blend_desc;

	//blend_desc.AlphaToCoverageEnable = TRUE;
	//blend_desc.IndependentBlendEnable = FALSE;
	//blend_desc.RenderTarget[0].BlendEnable = true;
	//blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	//blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	//blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	//blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//hr = device->CreateBlendState(&blend_desc, blend_state.GetAddressOf());

	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

// UNIT.02
//void sprite::render(ID3D11DeviceContext *immediate_context) const
// UNIT.03
//void sprite::render(ID3D11DeviceContext *immediate_context, float dx, float dy, float dw, float dh, float angle/*degree*/, float r, float g, float b, float a) const
// UNIT.04
void Sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float cx, float cy, float angle/*degree*/, float r, float g, float b, float a) const
// dx, dy : Coordinate of sprite's left-top corner in screen space 
// dw, dh : Size of sprite in screen space 
// sx, sy : Coordinate of sprite's left-top corner in texture space 
// sw, sh : Size of sprite in texture space 
// angle : Raotation angle (Rotation centre is sprite's centre), unit is degree
// r, g, b : Color of sprite's each vertices 
{
	// UNIT.03
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	immediate_context->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	// Set each sprite's vertices coordinate to screen space
	// left-top
	float x0 = dx;
	float y0 = dy;
	// right-top
	float x1 = dx + dw;
	float y1 = dy;
	// left-bottom
	float x2 = dx;
	float y2 = dy + dh;
	// right-bottom
	float x3 = dx + dw;
	float y3 = dy + dh;

	// Translate sprite's centre to origin (rotate centre)
	float mx = dx + dw * 0.5f;
	float my = dy + dh * 0.5f;
	x0 -= mx;
	y0 -= my;
	x1 -= mx;
	y1 -= my;
	x2 -= mx;
	y2 -= my;
	x3 -= mx;
	y3 -= my;

	// Rotate each sprite's vertices by angle
	float rx, ry;
	float cos = cosf(angle * 0.01745f);
	float sin = sinf(angle * 0.01745f);
	rx = x0;
	ry = y0;
	x0 = cos * rx + -sin * ry;
	y0 = sin * rx + cos * ry;
	rx = x1;
	ry = y1;
	x1 = cos * rx + -sin * ry;
	y1 = sin * rx + cos * ry;
	rx = x2;
	ry = y2;
	x2 = cos * rx + -sin * ry;
	y2 = sin * rx + cos * ry;
	rx = x3;
	ry = y3;
	x3 = cos * rx + -sin * ry;
	y3 = sin * rx + cos * ry;

	// Translate sprite's centre to original position
	x0 += mx;
	y0 += my;
	x1 += mx;
	y1 += my;
	x2 += mx;
	y2 += my;
	x3 += mx;
	y3 += my;

	// Convert to NDC space
	x0 = 2.0f * x0 / screen_width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / screen_height;
	x1 = 2.0f * x1 / screen_width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / screen_height;
	x2 = 2.0f * x2 / screen_width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / screen_height;
	x3 = 2.0f * x3 / screen_width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / screen_height;

	HRESULT hr = S_OK;
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = immediate_context->Map(vertex_buffer.Get(), 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices = static_cast<vertex*>(mapped_buffer.pData);
	vertices[0].position.x = x0;
	vertices[0].position.y = y0;
	vertices[1].position.x = x1;
	vertices[1].position.y = y1;
	vertices[2].position.x = x2;
	vertices[2].position.y = y2;
	vertices[3].position.x = x3;
	vertices[3].position.y = y3;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0.0f;

	DirectX::XMFLOAT4 color(r, g, b, a);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

	vertices[0].texcoord.x = static_cast<FLOAT>(sx) / texture2d_desc.Width;
	vertices[0].texcoord.y = static_cast<FLOAT>(sy) / texture2d_desc.Height;
	vertices[1].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2d_desc.Width;
	vertices[1].texcoord.y = static_cast<FLOAT>(sy) / texture2d_desc.Height;
	vertices[2].texcoord.x = static_cast<FLOAT>(sx) / texture2d_desc.Width;
	vertices[2].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2d_desc.Height;
	vertices[3].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2d_desc.Width;
	vertices[3].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2d_desc.Height;


	immediate_context->Unmap(vertex_buffer.Get(), 0);


	// UNIT.02
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(input_layout.Get());

	immediate_context->RSSetState(rasterizer_state.Get());

	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	// UNIT.04
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());


	immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

	//immediate_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);

	immediate_context->Draw(4, 0);
}

void Sprite::textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a) const
{
	float sw = static_cast<float>(texture2d_desc.Width / 16);
	float sh = static_cast<float>(texture2d_desc.Height / 16);
	float cursor = 0;
	for (auto c : s)
	{
		LONG sx = c % 0x0F;
		render(immediate_context, x + cursor, y, w, h, sw * (c & 0x0F), sh * (c >> 4), sw, sh, 0, r, g, b, a);
		cursor += w;
	}
}

//distance field font