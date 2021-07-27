#include "ParticleSystem.h"
#include "framework.h"
#include "Blender.h"
#include "Mo2Func.h"
cParticleSystem::cParticleSystem(int num)
{
	ID3D11Device* device = Mo2System->DX11device.Get();

	data = new ParticleData[num];
	ZeroMemory(data, sizeof(ParticleData)*num);
	v = new VERTEX[num];
	ZeroMemory(v, sizeof(VERTEX)*num);

	num_particles = num;
	for (int i = 0; i < num_particles; i++) { data[i].type = -1;}
	//パーティクル作成と画像ロード
	texture = std::make_unique<Texture>();
	//std::vector<const wchar_t*> filename;
	//filename.push_back(L"./Data/Assets/Effect/Electric.png");
	//filename.push_back(L"./Data/Assets/Effect/Electric2.png");
	//texture->LoadArray({ L"./Data/Assets/Effect/Flash.png",
	//	L"./Data/Assets/Effect/Smork.png" });
	texture->Load(L"./Data/Assets/Effect/Flash.png");


	//頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//頂点数分のバッファ
	bd.ByteWidth = sizeof(VERTEX)* num_particles; 
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 定数バッファ生成
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&cbd, nullptr, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

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
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		HRESULT hr = device->CreateSamplerState(&desc, sampler_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


}
cParticleSystem::cParticleSystem(std::shared_ptr<Texture> texture,int num)
{
	data = new ParticleData[num];
	ZeroMemory(data, sizeof(ParticleData)*num);
	v = new VERTEX[num];
	ZeroMemory(v, sizeof(VERTEX)*num);

	num_particles = num;
	for (int i = 0; i < num_particles; i++) { data[i].type = -1; }
	//パーティクル作成と画像設定
	//this->texture = texture;

	ID3D11Device* device = Mo2System->DX11device.Get();

	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	//頂点数分のバッファ
	bd.ByteWidth = sizeof(VERTEX)* num_particles;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = device->CreateBuffer(&bd, NULL, VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	// 定数バッファ生成
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBufferForPerFrame);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;

	hr = device->CreateBuffer(&cbd, nullptr, ConstantBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

}


void cParticleSystem::Update()
{
	float time = Mo2System->delta_time;
	for (int i = 0; i < num_particles; i++) {
		if (data[i].type < 0) continue;

		data[i].vx += data[i].ax * time;
		data[i].vy += data[i].ay * time;
		data[i].vz += data[i].az * time;

		data[i].x += data[i].vx * time;
		data[i].y += data[i].vy * time;
		data[i].z += data[i].vz * time;

		data[i].timer -= Mo2System->delta_time;
		data[i].anim_timer += Mo2System->delta_time;
		static float next_timer = 0.05f;
		//if(data[i].anim_timer > next_timer)
		//{
		//	data[i].type++;
		//	if (data[i].type > 12)data[i].type = 0;
		//	data[i].anim_timer -= next_timer;
		//}
		data[i].alpha = data[i].timer*data[i].timer*data[i].timer*data[i].timer;
		if (data[i].timer <= 0) data[i].type = -1;
	}
}

void cParticleSystem::Render(Shader* shader, const XMMATRIX* view, const XMMATRIX* projection)
{

	ID3D11DeviceContext* device_context = Mo2System->DX11context.Get();
	//シェーダ有効化
	shader->Activate();

	//定数バッファの更新
	ConstantBufferForPerFrame cb;
	XMFLOAT4X4 fv,fp;
	XMStoreFloat4x4(&fv, *view);
	cb.view = fv;
	XMStoreFloat4x4(&fp, *projection);
	cb.projection = fp;
	cb.size = 1.1f;
	device_context->UpdateSubresource(ConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->GSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());

	//ブレンドステート設定
	Mo2System->SetBlendState(BLEND_STATE::ADD);
	//ラスタライザ―設定
	Mo2System->SetRSState(RS_STATE::RS_CULL_FRONT);
	//デプスステンシルステート設定
	Mo2System->SetDSState(DS_STATE::D_TRUE_W_FALSE);
	//プリミティブ・トポロジーをセット
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	device_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
	//テクスチャ設定
	if (texture) texture->SetArray(0);

	int n = 0; //パーティクル発生数
	for (int i = 0; i < num_particles; i++)
	{
		if (data[i].type < 0) continue;

		v[n].Pos.x = data[i].x;
		v[n].Pos.y = data[i].y;
		v[n].Pos.z = data[i].z;
		v[n].Tex.x = data[i].w;
		v[n].Tex.y = data[i].h;		  
		v[n].Color.x = (float)data[i].type;
		v[n].Color.y = v[n].Color.z = 1.0f;
		v[n].Color.w = data[i].alpha;
		++n;
	}
	//頂点データ更新
	device_context->UpdateSubresource(VertexBuffer.Get(), 0, nullptr, v, 0, 0);

	//バーテックスバッファーをセット
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);

	//レンダリング(インデックス付き)
	device_context->Draw(n, 0);

	//デプスステンシルステート設定
	//device_context->OMSetDepthStencilState(, 1);

	//シェーダ無効化
	shader->Inactivate();
}

void cParticleSystem::Set(int type,float timer, DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v, DirectX::XMFLOAT3 f, DirectX::XMFLOAT2 size)
{
	for (int i = 0; i < num_particles; i++) {
		if (data[i].type >= 0) continue;
		data[i].type = type;
		data[i].x = p.x;
		data[i].y = p.y;
		data[i].z = p.z;
		data[i].vx = v.x;
		data[i].vy = v.y;
		data[i].vz = v.z;
		data[i].ax = f.x;
		data[i].ay = f.y;
		data[i].az = f.z;
		data[i].w = size.x;
		data[i].h = size.y;
		data[i].alpha = 1.0f;
		data[i].timer = timer;
		break;
	}
}

void cParticleSystem::Snow(DirectX::XMFLOAT3 pos, /*float r,*/ int max)
{
	for (int i = 0; i < max; i++) {
		DirectX::XMFLOAT3 p;
		p.x = pos.x + (rand() % 1001 - 500) * 0.1f;
		p.y = pos.y;
		p.z = pos.z + (rand() % 1001 - 500) * 0.1f;

		DirectX::XMFLOAT3 v = { 0,0,0 };
		v.y = -(rand() % 1001)*0.002f - 0.002f;
		DirectX::XMFLOAT3 f = { 0,0,0 };
		DirectX::XMFLOAT2 s = { 1.f, 1.f };

		Set(Mo2Lib::RandInt(0, 10), 10, p, v, f, s);

	}
}

void cParticleSystem::Spark(DirectX::XMFLOAT3 pos, int max)
{
	for (int i = 0; i < max; i++) {
		DirectX::XMFLOAT3 p;
		p.x = pos.x;
		p.y = pos.y;
		p.z = pos.z;

		DirectX::XMFLOAT3 v = { 0,0,0 };
		FLOAT rad = Mo2Lib::RandFloat() * 6.28f;
		v.x = cosf(rad) * 5.f;
		v.y = (rand() % 1001)*0.5f + 0.01f;
		v.z = sinf(rad) * 5.f;

		DirectX::XMFLOAT3 f = { -v.x * .7f,-v.y * 2.f, -v.z * .7f };
		DirectX::XMFLOAT2 s = { 5.5f, 5.5f };

		Set(1, 0.5f, p, v, f, s);

	}
}
