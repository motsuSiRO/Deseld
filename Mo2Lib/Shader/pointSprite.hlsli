// **************************************************
//
//				�p�[�e�B�N���̕`�揈��
//
// **************************************************
//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2DArray<float4> DiffuseTexture : register(t0);

SamplerState DecalSampler : register(s0);


//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Size     : TEXCOORD;
	float4 Color    : COLOR;
};

struct GSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Size     : TEXCOORD;
	float4 Color    : COLOR;
};


struct PSInput
{
	float4 Position  : SV_POSITION;     // ���_���W(�ˉe���W�n)
	float3 Tex  : TEXCOORD;        // �e�N�X�`�����W
	float4 Color : COLOR0;
};

//--------------------------------------------
//	�O���[�o���ϐ�
//--------------------------------------------

cbuffer CBPerFrame : register(b0)
{
	matrix	View;			// �r���[�ϊ��s��
	matrix	Projection;		// �����ϊ��s��


};

//#define type 7
//#define size 0.1

