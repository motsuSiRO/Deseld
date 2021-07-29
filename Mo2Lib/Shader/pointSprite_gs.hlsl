#include "pointSprite.hlsli"

//--------------------------------------------
//	�W�I���g���V�F�[�_�[
//--------------------------------------------

[maxvertexcount(4)]
void main(point GSInput In[1],
	inout TriangleStream<PSInput> ParticleStream)
{
	// ���W�ϊ��@(���[���h���W�n�@���@�r���[���W�n)
	float4 pos = mul(float4(In[0].Position, 1.0), View);

	// �_��ʂɂ���
	float w = In[0].Size.x;
	float h = In[0].Size.y;
	float4 posLT = pos + float4(-w, h, 0.0, 0.0);
	float4 posLB = pos + float4(-w, -h, 0.0, 0.0);
	float4 posRT = pos + float4(w, h, 0.0, 0.0);
	float4 posRB = pos + float4(w, -h, 0.0, 0.0);

	
	// ����̓_�̈ʒu(�ˉe���W�n)�EUV�E�F���v�Z���ďo��
	PSInput Out = (PSInput)0;


	uint type = In[0].Color.r;
	float t = 0;
	float slice = 0.2;

	float u = (type % 4)*slice;
	float v = (type / 4)*slice;
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	Out.Position = mul(posLT, Projection);
	Out.Tex = float3(u, v, t);
	ParticleStream.Append(Out);

	// �E��̓_�̈ʒu(�ˉe���W�n)�EUV�E�F���v�Z���ďo��
	Out.Position = mul(posRT, Projection);
	Out.Tex = float3(u + slice, v, t);
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	ParticleStream.Append(Out);

	// �����̓_�̈ʒu(�ˉe���W�n)�EUV�E�F���v�Z���ďo��
	Out.Position = mul(posLB, Projection);
	Out.Tex = float3(u, v + slice, t);
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	ParticleStream.Append(Out);

	// �E���̓_�̈ʒu(�ˉe���W�n)�EUV�E�F���v�Z���ďo��
	Out.Position = mul(posRB, Projection);
	Out.Tex = float3(u + slice, v + slice, t);
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	ParticleStream.Append(Out);

	ParticleStream.RestartStrip();
}
