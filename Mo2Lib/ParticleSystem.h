#pragma once
#include <d3d11.h>

#include "Texture.h"
#include "Shader.h"

class cParticleSystem
{
private:

	struct ParticleData
	{
		float x, y, z;
		float w, h;
		float vx, vy, vz;
		float ax, ay, az;
		float alpha;
		float timer;
		int type;
	};

	struct VERTEX
	{
		XMFLOAT3 Pos;	//位置
		XMFLOAT3 Normal;//法線
		XMFLOAT2 Tex;	//UV座標
		XMFLOAT4 Color;	//頂点色
	};

	ParticleData* data; //パーティクルデータ
	VERTEX* v;			//頂点データ

	int num_particles;

	// 頂点データ
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	//テクスチャ利用
	std::shared_ptr<Texture> texture = nullptr;
	// 定数バッファのデータ定義
	struct ConstantBufferForPerFrame {
		XMFLOAT4X4	view;			// ビュー変換行列
		XMFLOAT4X4	projection;		// 透視変換行列
		float		size;
		float		dumy0;
		float		dumy1;
		float		dumy2;
	};

private:
	
public:

	cParticleSystem(int num = 1000);
	cParticleSystem(std::shared_ptr<Texture> texture, int num = 1000);
	~cParticleSystem() { delete[] data; delete[] v; }
	void Update();
	void Render(Shader* shader, const XMMATRIX* v, const XMMATRIX* p);

	void Set(
		int type,
		float timer,
		DirectX::XMFLOAT3 p,
		DirectX::XMFLOAT3 v = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(1.0f, 1.0f)
	);

	void cParticleSystem::Snow(DirectX::XMFLOAT3 pos, float r, int max);
	void cParticleSystem::Spark(DirectX::XMFLOAT3 pos, int max);

};