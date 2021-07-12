#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <unordered_map>
#include <string>
#include "Shader.h"

//#define SkyBoxGenerate
#ifdef SkyBoxGenerate
#include "model.h"
class SkyBox
{
public:
	Mo2Lib::Transform transform;
	std::shared_ptr<Mo2Lib::Model> sky;
	//Mo2Lib::Float4 light_dir;
	//Mo2Lib::Float4 light_color;

	void Initialize();
	void SetCamPos(Mo2Lib::Float3 pos) { transform.translate = pos; }
	bool Set(int index);
	void ImGui();

private:
	typedef std::unordered_map<int, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> TexMap;
	TexMap tex_list;
};

#else
class SkyBox
{
public:
	SkyBox(ID3D11Device* _device);
	Mo2Lib::Transform transform;
	DirectX::XMFLOAT4 color;
	Mo2Lib::Float4 light_dir;
	//Mo2Lib::Float4 light_color;

	void Load(ID3D11Device* device);
	void SetCamPos(Mo2Lib::Float3 pos) { transform.translate = pos; }
	bool Set(int index);
	void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection, bool isSolid);
	void ImGui();

private:
	int	numIndices;
	
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;


	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframe_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> solid_rasterizer_state;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

	//texture
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;

	typedef std::unordered_map<int, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> TexMap;
	TexMap tex_list;
	typedef std::unordered_map<int, D3D11_TEXTURE2D_DESC> TexDesc;
	TexDesc tex_desc;
	int tex_num = 0;

	//shader
	std::unique_ptr<ShaderEx> shader;
public:
	struct vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	struct CAMERA
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 direction;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4X4 view_projection;
		DirectX::XMFLOAT4X4 inverse_view;
		DirectX::XMFLOAT4X4 inverse_view_projection;
		DirectX::XMFLOAT4X4 previous_view_projection;
	};


	struct CbScene
	{
		CAMERA				cam;
		DirectX::XMFLOAT4	light_direction;
		DirectX::XMFLOAT4	light_color;
		DirectX::XMFLOAT3	fog_color;
		float				fog_near;
		float				fog_far;
		float pad[3];

	};

	struct CbObject
	{
		DirectX::XMFLOAT4X4	 world;
		DirectX::XMFLOAT4	model_color;
	};
#define MaxBones 128
	struct CbMesh
	{
		DirectX::XMFLOAT4X4	bone_transforms[MaxBones];
	};

	struct CbSubset
	{
		DirectX::XMFLOAT4	material_color;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_cb_scene;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_cb_object;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_cb_mesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_cb_subset;

	//DirectX::XMMATRIX BuildWorld();
	bool CreateBuffers(ID3D11Device* device, vertex* vertices, int numV, u_int* indices, int numI);

	bool CreateCube(ID3D11Device* device);
};


#endif