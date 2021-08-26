#include "SkyBox.h"
#include <Windows.h>
#include <string>
#include "framework.h"
#include "Mo2GUI.h"
#include "TextureLoader.h"

#ifdef SkyBoxGenerate
void SkyBox::Initialize()
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	const char* fbx_filename = "./Data/Assets/SkyBox/skybox.fbx";
	std::shared_ptr<ModelResource> model_resource = std::make_shared<ModelResource>();
	Mo2Lib::LoadModelResource(model_resource, fbx_filename, Mo2Lib::LOADINGMODE_BIN, LOAD_SKINNED_MODEL);
	//if (!model_resource->LoadFromBinary(device, fbx_filename))
	//{
	//	model_resource->SaveToData(fbx_filename);
	//}
	sky = std::make_unique<Mo2Lib::Model>(model_resource);
	sky->transform.scale = { 50.f, 50.f, 50.f };


	HANDLE hFind;
	//defined at Windwos.h
	WIN32_FIND_DATA win32fd;
	//ファイル名の配列
	std::vector<std::wstring> file_names;

	//探すファイル名指定　ワイルドカードを使用
	std::string search_name = "./Data/Assets/SkyBox\\*.png";
	//ファイル検索
	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	//見つからなかったか
	if (hFind == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("file not found");
	}

	//次のファイルがある限り読み込み続ける
	int index = 0;
	while (FindNextFile(hFind, &win32fd))
	{
		//ディレクトリなら無視
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {}
		else
		{
			//ファイル名をパス付で取得
			std::string filename = win32fd.cFileName;
			filename.insert(0, "./Data/Assets/SkyBox/");
			size_t length;
			wchar_t wfilename[256];
			::mbstowcs_s(&length, wfilename, 256, filename.c_str(), _TRUNCATE);


			Texture::LoadFromFile(device, wfilename, tex_list[index].GetAddressOf(), nullptr);
		}
		++index;
	}
	//閉じる
	FindClose(hFind);

	//if (tex_list.size() > 0)
	//{
	//	Set(0);
	//}
}

bool SkyBox::Set(int index)
{
	if (tex_list.size() <= (size_t)index)return false;

	sky->GetModelResource()->Set(tex_list[index].Get());
}

void SkyBox::ImGui()
{
	if (ImGui::Begin("SkyBox"))
	{
		static int tex_num = 0;
		ImGui::Text("MAX_TEX_NUM : %d", tex_list.size());
		ImGui::InputInt("SetTexNum", &tex_num);

		if (tex_list.size() > 0)
		{
			Mo2Lib::Clamp(tex_num, 0, (int)tex_list.size() - 1);

			if (ImGui::Button("SetTexture"))
			{
				Set(tex_num);
			}
		}
		else
		{
			tex_num = 0;
		}


		ImGui::End();
	}
}

#else

bool SkyBox::CreateBuffers(ID3D11Device* _device, vertex* vertices, int numV, u_int* indices, int numI)
{
	HRESULT hr;

	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA subresource_data;

	//	Vertex Buffer
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	ZeroMemory(&subresource_data, sizeof(subresource_data));

	buffer_desc.ByteWidth = numV * sizeof(vertex);
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = _device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//Index Buffer
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	ZeroMemory(&subresource_data, sizeof(subresource_data));

	buffer_desc.ByteWidth = numI * sizeof(unsigned int);
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = indices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	numIndices = numI;
	hr = _device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


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

		HRESULT hr = _device->CreateBuffer(&desc, 0, m_cb_scene.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// オブジェクト用バッファ
		desc.ByteWidth = sizeof(CbObject);

		hr = _device->CreateBuffer(&desc, 0, m_cb_object.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// メッシュ用バッファ
		desc.ByteWidth = sizeof(CbMesh);

		hr = _device->CreateBuffer(&desc, 0, m_cb_mesh.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// サブセット用バッファ
		desc.ByteWidth = sizeof(CbSubset);

		hr = _device->CreateBuffer(&desc, 0, m_cb_subset.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//hr = _device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());

	return true;
}

bool SkyBox::CreateCube(ID3D11Device* device)
{
	vertex vertices[4 * 6] = {};
	unsigned int indices[3 * 2 * 6] = {};
	//size 大きく
	float half_size = 1.f;
	int numV = 0, numI = 0;

	//	上面
	vertices[numV + 0].pos = DirectX::XMFLOAT3(-half_size, +half_size, +half_size);
	vertices[numV + 1].pos = DirectX::XMFLOAT3(+half_size, +half_size, +half_size);
	vertices[numV + 2].pos = DirectX::XMFLOAT3(-half_size, +half_size, -half_size);
	vertices[numV + 3].pos = DirectX::XMFLOAT3(+half_size, +half_size, -half_size);
	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(0.25f, 0.f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(0.5f, 0.f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(0.25f, 0.333f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(0.5f, 0.333f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	下面
	vertices[numV + 0].pos = DirectX::XMFLOAT3(-half_size, -half_size, +half_size);
	vertices[numV + 1].pos = DirectX::XMFLOAT3(+half_size, -half_size, +half_size);
	vertices[numV + 2].pos = DirectX::XMFLOAT3(-half_size, -half_size, -half_size);
	vertices[numV + 3].pos = DirectX::XMFLOAT3(+half_size, -half_size, -half_size);
	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(0.25f, 1.0f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(0.5f, 1.0f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(0.25f, 0.666f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(0.5f, 0.666f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	//	右面
	vertices[numV + 0].pos = DirectX::XMFLOAT3(+half_size, +half_size, -half_size);
	vertices[numV + 1].pos = DirectX::XMFLOAT3(+half_size, +half_size, +half_size);
	vertices[numV + 2].pos = DirectX::XMFLOAT3(+half_size, -half_size, -half_size);
	vertices[numV + 3].pos = DirectX::XMFLOAT3(+half_size, -half_size, +half_size);
	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(0.5f, 0.334f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(0.75f, 0.334f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(0.5f, 0.666f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(0.75f, 0.666f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	左面
	vertices[numV + 0].pos = DirectX::XMFLOAT3(-half_size, +half_size, -half_size);
	vertices[numV + 1].pos = DirectX::XMFLOAT3(-half_size, +half_size, +half_size);
	vertices[numV + 2].pos = DirectX::XMFLOAT3(-half_size, -half_size, -half_size);
	vertices[numV + 3].pos = DirectX::XMFLOAT3(-half_size, -half_size, +half_size);
	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(0.25f, 0.334f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(0.f, 0.334f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(0.25f, 0.666f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(0.f, 0.666f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	//	後面
	vertices[numV + 0].pos = DirectX::XMFLOAT3(+half_size, -half_size, +half_size);
	vertices[numV + 1].pos = DirectX::XMFLOAT3(+half_size, +half_size, +half_size);
	vertices[numV + 2].pos = DirectX::XMFLOAT3(-half_size, -half_size, +half_size);
	vertices[numV + 3].pos = DirectX::XMFLOAT3(-half_size, +half_size, +half_size);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(1.0f, 0.666f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(1.0f, 0.334f);
	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(0.75f, 0.666f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(0.75f, 0.334f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
		vertices[numV + 2].normal =
		vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
	numV += 4;	numI += 6;

	//	前面
	vertices[numV + 0].pos = DirectX::XMFLOAT3(+half_size, -half_size, -half_size);
	vertices[numV + 1].pos = DirectX::XMFLOAT3(+half_size, +half_size, -half_size);
	vertices[numV + 2].pos = DirectX::XMFLOAT3(-half_size, -half_size, -half_size);
	vertices[numV + 3].pos = DirectX::XMFLOAT3(-half_size, +half_size, -half_size);
	vertices[numV + 0].texcoord = DirectX::XMFLOAT2(0.5f, 0.666f);
	vertices[numV + 1].texcoord = DirectX::XMFLOAT2(0.5f, 0.334f);
	vertices[numV + 2].texcoord = DirectX::XMFLOAT2(0.25f, 0.666f);
	vertices[numV + 3].texcoord = DirectX::XMFLOAT2(0.25f, 0.334f);
	vertices[numV + 0].normal = vertices[numV + 1].normal =
	vertices[numV + 2].normal =
	vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
	indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
	indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
	numV += 4;	numI += 6;

	return CreateBuffers(device, vertices, numV, indices, numI);


}


SkyBox::SkyBox(ID3D11Device* device)
{
	//	情報初期値
	transform.translate = Mo2Lib::Float3(0, 0, 0);
	transform.rotate = Mo2Lib::Float4(0, 0, 0, 1);
	transform.scale = Mo2Lib::Float3(1, 1, 1);


	color = DirectX::XMFLOAT4(1, 1, 1, 1);

	shader = std::make_unique<ShaderEx>();
	shader->Create(L"SkyBox_VS", L"SkyBox_PS");

	HRESULT hr;



	//hr = Texture::LoadFromFile(_device, texture.c_str(), shader_resource_view.GetAddressOf(), &texture2d_desc);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_ANISOTROPIC
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//	ラスタライザーステート(ワイヤーフレーム)の作成
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rsDesc, wireframe_rasterizer_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//	ラスタライザーステート(ソリッド)の作成
	//D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.FrontCounterClockwise = true;
	rsDesc.DepthClipEnable = TRUE;
	hr = device->CreateRasterizerState(&rsDesc, solid_rasterizer_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//	深度ステンシルStateの作成
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = TRUE;						//	深度テスト有効
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dssDesc.StencilEnable = FALSE;					//	ステンシルは無効
	hr = device->CreateDepthStencilState(&dssDesc, depth_stencil_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	CreateCube(device);

	Load(device);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void SkyBox::Load(ID3D11Device* device)
{
	HANDLE hFind;
	//defined at Windwos.h
	WIN32_FIND_DATA win32fd;
	//ファイル名の配列
	std::vector<std::wstring> file_names;

	//探すファイル名指定　ワイルドカードを使用
	std::string search_name = "./Data/Assets/SkyBox\\*.png";
	//ファイル検索
	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	//見つからなかったか
	if (hFind == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("file not found");
	}

	//次のファイルがある限り読み込み続ける
	int index = 0;
	while (FindNextFile(hFind, &win32fd))
	{
		//ディレクトリなら無視
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {}
		else
		{
			//ファイル名をパス付で取得
			std::string filename = win32fd.cFileName;
			filename.insert(0, "./Data/Assets/SkyBox/");
			size_t length;
			wchar_t wfilename[256];
			::mbstowcs_s(&length, wfilename, 256, filename.c_str(), _TRUNCATE);


			//Texture::LoadFromFile(device, wfilename, tex_list[index].GetAddressOf(), &tex_desc[index]);
			tex_list[index] = Mo2Tex().Load(wfilename);
		}
		++index;
	}
	//閉じる
	FindClose(hFind);

}

void SkyBox::Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view_projection, bool isSolid)
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

	context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
	context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());


	//context->Unmap(vertex_buffer.Get(), 0);

	// シーン用定数バッファ更新
	CbScene cb_scene;
	//cb_scene.cam.position = DirectX::XMFLOAT4(eye_pos.x, eye_pos.y, eye_pos.z, 1.0f);;
	cb_scene.cam.view_projection = view_projection;
	//cb_scene.light_direction = light_direction;
	cb_scene.light_color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb_scene.fog_color = { 0.6f, 0.6f, 0.55f };
	cb_scene.fog_near = 10.f;
	cb_scene.fog_far = 9000.f;
	context->UpdateSubresource(m_cb_scene.Get(), 0, 0, &cb_scene, 0, 0);

	shader->Activate();

	CbObject cb_object;
	DirectX::XMStoreFloat4x4(&cb_object.world, transform.BuildWorld());
	cb_object.model_color = color;
	context->UpdateSubresource(m_cb_object.Get(), 0, 0, &cb_object, 0, 0);

	CbSubset cb_subset;
	cb_subset.material_color = color;
	context->UpdateSubresource(m_cb_subset.Get(), 0, 0, &cb_subset, 0, 0);


	// 頂点バッファのバインド
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//	インデックスバッファのバインド
	context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//	プリミティブモードの設定
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	入力レイアウトのバインド

	if (isSolid)
	{
		//	プリミティブモードの設定
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//	ラスタライザーの設定
		context->RSSetState(solid_rasterizer_state.Get());
	}
	else
	{
		//	プリミティブモードの設定
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		//	ラスタライザーの設定
		context->RSSetState(wireframe_rasterizer_state.Get());
	}


	//if (tex_link)
	//{
		//context->PSSetShaderResources(0, 1, tex_list[tex_num].GetAddressOf());
	tex_list[tex_num]->Set(0);
		context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
	//}

	//	深度テストの設定
	context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);

	//	プリミティブの描画(index付き)
	context->DrawIndexed(numIndices, 0, 0);

	shader->Inactivate();
}


bool SkyBox::Set(int index)
{
	if (tex_list.size() <= (size_t)index)return false;

	tex_num = index;
}

void SkyBox::ImGui()
{
	ImGui::Begin("SkyBox");
	{
		static int tex_num = 0;
		ImGui::Text("MAX_TEX_NUM : %d", tex_list.size());
		ImGui::InputInt("SetTexNum", &tex_num);

		if (tex_list.size() > 0)
		{
			Mo2Lib::Clamp(tex_num, 0, (int)tex_list.size() - 1);

			ImGui::Image((void*)(tex_list[tex_num]->GetSRV()), ImVec2(640, 360));
			if (ImGui::Button("SetTexture"))
			{
				Set(tex_num);
			}
		}
		else
		{
			tex_num = 0;
		}


		ImGui::End();
	}
}

#endif

