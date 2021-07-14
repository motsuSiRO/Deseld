
#include "logger.h"
#include "model_renderer.h"
#include "misc.h"
#include "Blender.h"


void ModelRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* _context)
{
	context = _context;
	// 頂点シェーダー
	{
		// ファイルを開く
		FILE* fp = nullptr;
		fopen_s(&fp, "./Data/System/ShaderObjects/model_vs.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		std::unique_ptr<u_char[]> cso_data = std::make_unique<u_char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		// 頂点シェーダー生成
		HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, m_vertex_shader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTSA",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTSB",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONESA",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONESB",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		hr = device->CreateInputLayout(input_element_desc, ARRAYSIZE(input_element_desc), cso_data.get(), cso_sz, m_input_layout.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// ピクセルシェーダー
	{
		// ファイルを開く
		FILE* fp = nullptr;
		fopen_s(&fp, "./Data/System/ShaderObjects/model_ps.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ファイルのサイズを求める
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// メモリ上に頂点シェーダーデータを格納する領域を用意する
		std::unique_ptr<u_char[]> cso_data = std::make_unique<u_char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		// ピクセルシェーダー生成
		HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, m_pixel_shader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// 定数バッファ
	{
		// シーン用バッファ
		D3D11_BUFFER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbScene);
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, m_cb_scene.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// オブジェクト用バッファ
		desc.ByteWidth = sizeof(CbObject);

		hr = device->CreateBuffer(&desc, 0, m_cb_object.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// メッシュ用バッファ
		desc.ByteWidth = sizeof(CbMesh);

		hr = device->CreateBuffer(&desc, 0, m_cb_mesh.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// サブセット用バッファ
		desc.ByteWidth = sizeof(CbSubset);

		hr = device->CreateBuffer(&desc, 0, m_cb_subset.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// ブレンドステート
	{
		D3D11_BLEND_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = device->CreateBlendState(&desc, m_blend_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// 深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;

		HRESULT hr = device->CreateDepthStencilState(&desc, m_depth_stencil_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	 //ラスタライザーステート
	//{
	//	D3D11_RASTERIZER_DESC desc;
	//	::memset(&desc, 0, sizeof(desc));
	//	desc.FrontCounterClockwise = true;
	//	desc.DepthBias = 0;
	//	desc.DepthBiasClamp = 0;
	//	desc.SlopeScaledDepthBias = 0;
	//	desc.DepthClipEnable = true;
	//	desc.ScissorEnable = false;
	//	desc.MultisampleEnable = true;
	//	desc.FillMode = D3D11_FILL_SOLID;
	//	desc.CullMode = D3D11_CULL_BACK;
	//	desc.AntialiasedLineEnable = false;

	//	HRESULT hr = device->CreateRasterizerState(&desc, m_rasterizer_state.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	rasterizer = std::make_unique<Rasterizer>();
	rasterizer->Create(device);

	// サンプラステート
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, m_sampler_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// ダミーテクスチャ
	{
		const int width = 8;
		const int height = 8;
		UINT pixels[width * height];
		::memset(pixels, 0xFF, sizeof(pixels));

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));
		data.pSysMem = pixels;
		data.SysMemPitch = width;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
		HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateShaderResourceView(texture.Get(), nullptr, m_dummy_srv.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void ModelRenderer::RSSet(int index)
{
	if (index >= Rasterizer::MODE_END)return;
	context->RSSetState(rasterizer->states[index].Get());

}

// 描画開始
void ModelRenderer::Begin(const DirectX::XMFLOAT3 eye_pos, const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& light_direction)
{

	ID3D11Buffer* constant_buffers[] =
	{
		m_cb_scene.Get(),
		m_cb_object.Get(),
		m_cb_mesh.Get(),
		m_cb_subset.Get()
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->PSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);

	const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->OMSetBlendState(m_blend_state.Get(), blend_factor, 0xFFFFFFFF);
	RSSet(D3D11_CULL_BACK);
	context->OMSetDepthStencilState(m_depth_stencil_state.Get(), 0);
	context->PSSetSamplers(0, 1, m_sampler_state.GetAddressOf());

	// シーン用定数バッファ更新
	CbScene cb_scene;
	cb_scene.cam.position = DirectX::XMFLOAT4(eye_pos.x, eye_pos.y, eye_pos.z, 1.0f);;
	cb_scene.cam.view_projection = view_projection;
	cb_scene.light_direction = light_direction;
	cb_scene.light_color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb_scene.fog_color = { 0.6f, 0.6f, 0.55f };
	cb_scene.fog_near = 10.f;
	cb_scene.fog_far = 9000.f;
	context->UpdateSubresource(m_cb_scene.Get(), 0, 0, &cb_scene, 0, 0);
}

// 描画
void ModelRenderer::Draw(ShaderEx* shader, Mo2Lib::Model& model)
{
	shader->Activate();

	model.CalculateLocalTransform();
	model.CalculateWorldTransform();


	const ModelResource* model_resource = model.GetModelResource();
	const std::vector<Mo2Lib::Animator::Node>& nodes = model.GetNodes();

	//オブジェクト用定数バッファ更新w
	CbObject cb_object;
	DirectX::XMStoreFloat4x4(&cb_object.world, model.BulidWorld());
	cb_object.model_color = model.color;
	context->UpdateSubresource(m_cb_object.Get(), 0, 0, &cb_object, 0, 0);

	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// メッシュ用定数バッファ更新
		CbMesh cb_mesh;
		::memset(&cb_mesh, 0, sizeof(cb_mesh));
		if (mesh.node_indices.size() > 0)
		{
			for (size_t i = 0; i < mesh.node_indices.size(); ++i)
			{
				DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(&mesh.inverse_transforms.at(i));
				DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);
				DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
				DirectX::XMStoreFloat4x4(&cb_mesh.bone_transforms[i], bone_transform);
			}
		}
		else
		{
			//DirectX::XMStoreFloat4x4(&cb_mesh.bone_transforms[0], DirectX::XMMatrixIdentity());
			DirectX::XMStoreFloat4x4(&cb_mesh.bone_transforms[0], model.BulidWorld());
			//cb_mesh.bone_transforms[0] = nodes.at(mesh.node_index).world_transform;
		}
		context->UpdateSubresource(m_cb_mesh.Get(), 0, 0, &cb_mesh, 0, 0);

		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			CbSubset cb_subset;
			cb_subset.material_color = subset.material->color;
			context->UpdateSubresource(m_cb_subset.Get(), 0, 0, &cb_subset, 0, 0);
			context->PSSetShaderResources(0, 1, subset.material->shader_resource_view.Get() ? subset.material->shader_resource_view.GetAddressOf() : m_dummy_srv.GetAddressOf());
			context->PSSetSamplers(0, 1, m_sampler_state.GetAddressOf());
			context->DrawIndexed(subset.index_count, subset.start_index, 0);
		}
	}

	shader->Inactivate();
}

void ModelRenderer::Draw(ShaderEx* shader, GeoPrimitive& model)
{
	shader->Activate();

	CbObject cb_object;
	DirectX::XMStoreFloat4x4(&cb_object.world, model.transform.BuildWorld());
	context->UpdateSubresource(m_cb_object.Get(), 0, 0, &cb_object, 0, 0);


	UINT stride = sizeof(GeoPrimitive::vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, model.vertex_buffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(model.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		CbSubset cb_subset;
		cb_subset.material_color = model.color;
		context->UpdateSubresource(m_cb_subset.Get(), 0, 0, &cb_subset, 0, 0);
		context->PSSetShaderResources(0, 1, model.shader_resource_view.Get() ? model.shader_resource_view.GetAddressOf() : m_dummy_srv.GetAddressOf());
		context->PSSetSamplers(0, 1, m_sampler_state.GetAddressOf());
		//context->DrawIndexed(model.numIndices, 0, 0);
	}

	model.render(context, true);

	shader->Inactivate();
}


// 描画終了
void ModelRenderer::End()
{

}
