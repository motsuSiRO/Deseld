#pragma once

// UNIT.02
#include <d3d11.h>
#include <wrl.h>
//#include <directxmath.h>
#include "Vector3D.h"
#include <string>
#include "Texture.h"
//#include "GeometricPrimitive.h"

class Sprite
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;

	//UNIT.04
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	//D3D11_TEXTURE2D_DESC texture2d_desc;
	std::shared_ptr<Texture> texture;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;

	//Unit.06
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
	//Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;

public:
	Mo2Lib::Float4 color = { 1.f, 1.f, 1.f ,1.f };

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		// UNIT.04
		DirectX::XMFLOAT2 texcoord;
	};

	// UNIT.02
	//sprite(ID3D11Device *device);
	// UNIT.04
	Sprite(ID3D11Device* device, const wchar_t* file_name);
	~Sprite()
	{

	}

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float cx = 0.0f, float cy = 0.0f, float angle = 0/*degree*/, float r = 1, float g = 1, float b = 1, float a = 1) const;

	void textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a) const;

	void RenderSprite(ID3D11DeviceContext* immediate_context, float dest_x, float dest_y, int width, int height, bool reverse_x = false, bool reverse_y = false)
	{
		render(immediate_context, dest_x + (float)(reverse_x * width), dest_y + (float)(reverse_y * height),
			(float)(width - (reverse_x * width * 2)), (float)(height - (reverse_y * height * 2)), 0.f, 0.f, (float)(width), (float)(height),
			0.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
	}
	void RenderRectSprite(ID3D11DeviceContext* immediate_context, float dest_x, float dest_y, int src_x, int src_y, int width, int height, bool reverse_x = false, bool reverse_y = false)
	{
		render(immediate_context, dest_x + (float)(reverse_x * width), dest_y + (float)(reverse_y * height),
			(float)(width - (reverse_x * width * 2)), (float)(height - (reverse_y * height * 2)), (float)(src_x), (float)(src_y), (float)(width), (float)(height),
			0.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
	}
	void RenderRotaSprite(ID3D11DeviceContext* immediate_context, float dest_x, float dest_y, int width, int height, float angle, bool reverse_x = false, bool reverse_y = false)
	{
		render(immediate_context, dest_x + (float)(reverse_x * width), dest_y + (float)(reverse_y * height),
			(float)(width - (reverse_x * width * 2)), (float)(height - (reverse_y * height * 2)), 0.0f, 0.0f, (float)(width), (float)(height),
			0.0f, 0.0f, angle, color.x, color.y, color.z, color.w);
	}
	void RenderRotaSprite2(ID3D11DeviceContext* immediate_context, float dest_x, float dest_y, int width, int height, float cx, float cy, float angle, bool reverse_x = false, bool reverse_y = false)
	{
		render(immediate_context, dest_x + (float)(reverse_x * width), dest_y + (float)(reverse_y * height),
			(float)(width - (reverse_x * width * 2)), (float)(height - (reverse_y * height * 2)), 0.0f, 0.0f, (float)(width), (float)(height),
			cx, cy, angle, color.x, color.y, color.z, color.w);
	}
	void RenderExtendSprite(ID3D11DeviceContext* immediate_context, float dest_x, float dest_y, int width, int height, float rate_x, float rate_y, bool reverse_x = false, bool reverse_y = false)
	{
		render(immediate_context, dest_x + (float)(reverse_x * width), dest_y + (float)(reverse_y * height),
			(float)(width - (reverse_x * width * 2)) * rate_x, (float)(height - (reverse_y * height * 2)) * rate_y, 0.0f, 0.0f, (float)(width), (float)(height),
			0.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
	}

	void RenderRotaExtendSprite(ID3D11DeviceContext* immediate_context, float dest_x, float dest_y, int width, int height, float rate_x, float rate_y, float dgree, bool reverse_x = false, bool reverse_y = false)
	{
		render(immediate_context, dest_x + (float)(reverse_x * width), dest_y + (float)(reverse_y * height),
			(float)(width - (reverse_x * width * 2)) * rate_x, (float)(height - (reverse_y * height * 2)) * rate_y, 0.0f, 0.0f, (float)(width), (float)(height),
			0.0f, 0.0f, dgree, color.x, color.y, color.z, color.w);
	}

	void RenderRectExtendSprite(ID3D11DeviceContext* immediate_context, float dest_x, float dest_y, int src_x, int src_y, int width, int height, float rate_x, float rate_y, bool reverse_x = false, bool reverse_y = false)
	{
		render(immediate_context, dest_x + (float)(reverse_x * width), dest_y + (float)(reverse_y * height),
			(float)(width - (reverse_x * width * 2)) * rate_x, (float)(height - (reverse_y * height * 2)) * rate_y, (float)(src_x), (float)(src_y), (float)(width), (float)(height),
			0.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w);
	}

};