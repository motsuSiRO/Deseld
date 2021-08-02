// **************************************************
//
//				�p�[�e�B�N���̕`�揈��
//
// **************************************************
//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);


SamplerState DecalSampler : register(s0);


//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float3 Size     : TEXCOORD;
	float4 Color    : COLOR;
};

struct GSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float3 Size     : TEXCOORD;
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
	row_major matrix	View;			// �r���[�ϊ��s��
	row_major matrix	Projection;		// �����ϊ��s��


};

#define PI 3.14159265

//#define type 7
//#define size 0.1

