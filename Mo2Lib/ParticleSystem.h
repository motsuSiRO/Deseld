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
		XMFLOAT3 Pos;	//�ʒu
		XMFLOAT3 Normal;//�@��
		XMFLOAT2 Tex;	//UV���W
		XMFLOAT4 Color;	//���_�F
	};

	ParticleData* data; //�p�[�e�B�N���f�[�^
	VERTEX* v;			//���_�f�[�^

	int num_particles;

	// ���_�f�[�^
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
	//�e�N�X�`�����p
	std::shared_ptr<Texture> texture = nullptr;
	// �萔�o�b�t�@�̃f�[�^��`
	struct ConstantBufferForPerFrame {
		XMFLOAT4X4	view;			// �r���[�ϊ��s��
		XMFLOAT4X4	projection;		// �����ϊ��s��
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