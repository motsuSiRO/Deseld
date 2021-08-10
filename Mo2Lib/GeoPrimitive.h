#pragma once


#include <d3d11.h>
#include <wrl.h>
#include "Vector3D.h"
#include <string>
#include "Texture.h"

class GeoPrimitive
{
public:
	bool is_solid;
	int					numIndices;
	Mo2Lib::Transform transform;
	//DirectX::XMFLOAT3	pos;				//	「位置」
	//DirectX::XMFLOAT3	rotate;				//	「(回転)角度」
	//DirectX::XMFLOAT3	scale;				//	「大きさ」

	DirectX::XMFLOAT4	color;
public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	std::shared_ptr<Texture> texture;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframe_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> solid_rasterizer_state;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

	//texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;


	void render(ID3D11DeviceContext* context);
	void render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection,
		const DirectX::XMFLOAT4& light_dir,
		bool isSolid);

	GeoPrimitive(std::wstring texture = L"");
	virtual ~GeoPrimitive()
	{
		//if (vertex_shader)vertex_shader->Release();
		//if (pixel_shader)pixel_shader->Release();
		//if (input_layout)input_layout->Release();
		//if (vertex_buffer)vertex_buffer->Release();
		//if (index_buffer)index_buffer->Release();
		//if (constant_buffer)constant_buffer->Release();
		//if (wireframe_rasterizer_state)wireframe_rasterizer_state->Release();
		//if (solid_rasterizer_state)solid_rasterizer_state->Release();
		//if (depth_stencil_state)depth_stencil_state->Release();
		//if (shader_resource_view)shader_resource_view->Release();
		//if (sampler_state)sampler_state->Release();
	};

	struct vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4 light_direction;
	};

	DirectX::XMMATRIX BuildWorld();
	bool CreateBuffers(ID3D11Device* device, vertex* vertices, int numV, u_int* indices, int numI);

	bool CreateCube();
	bool CreateBillBoard();
	bool CreateSphere(u_int slices, u_int stacks);
	//bool CreateCylinder(ID3D11Device* device);
	//bool CreateCapsule(ID3D11Device* device);

	void SetTransform(Mo2Lib::Transform t) { transform = t; }
};

class GeoLine
{
protected:
	int					numIndices;
	bool				isPoint = false;
public:
	Mo2Lib::Transform transform;
	//DirectX::XMFLOAT3	pos;				//	「位置」
	//DirectX::XMFLOAT3	rotate;				//	「(回転)角度」
	//DirectX::XMFLOAT3	scale;				//	「大きさ」

	DirectX::XMFLOAT4	color;
public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;


	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframe_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> solid_rasterizer_state;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

	//texture


	void render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection);
	void render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& inv_world, const DirectX::XMFLOAT4X4& view_projection);

	GeoLine(ID3D11Device* device);
	virtual ~GeoLine()
	{
	};

	struct vertex
	{
		DirectX::XMFLOAT3 pos;
	};

	struct cbuffer
	{
		DirectX::XMFLOAT4X4 view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
	};

	DirectX::XMMATRIX BuildWorld();
	DirectX::XMMATRIX BuildWorldST();
	bool CreateBuffers(ID3D11Device* device, vertex* vertices, int numV, u_int* indices, int numI);

	bool CreateLine(ID3D11Device* device);
	bool CreatePoint(ID3D11Device* device);


	void SetTransform(Mo2Lib::Transform t) { transform = t; }
};


