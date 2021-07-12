#pragma once

#include <memory>
#include <d3d11.h>
#include "model.h"
#include "GeoPrimitive.h"
#include "Shader.h"
#include <unordered_map>
#include "Singleton.h"
#include "Rasterizer.h"

class ModelRenderer : public Singleton<ModelRenderer>
{
public:
	friend class Singleton<ModelRenderer>; // Singleton でのインスタンス作成は許可
	ID3D11DeviceContext* context;
	std::unique_ptr<Rasterizer> rasterizer;

	void Begin(const DirectX::XMFLOAT3 eye_pos, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction);
	void Draw(ShaderEx* shader, Mo2Lib::Model& model);
	void Draw(ShaderEx* shader, GeoPrimitive& model);
	void End();

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);

	void RSSet(int index);
protected:
private:
	ModelRenderer(){}
	//ModelRenderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~ModelRenderer() {}

	static const int MaxBones = 128;

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

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_input_layout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		m_blend_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	m_depth_stencil_state;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		m_sampler_state;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_dummy_srv;
};

static ModelRenderer& Mo2Render()
{
	return ModelRenderer::GetInstance();
}