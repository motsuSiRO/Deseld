#include "pointSprite.hlsli"

//--------------------------------------------
//	�W�I���g���V�F�[�_�[
//--------------------------------------------

[maxvertexcount(4)]
void main(point GSInput In[1],
	inout TriangleStream<PSInput> ParticleStream)
{
	// ���W�ϊ��@(���[���h���W�n�@���@�r���[���W�n)
	float4 pos = mul(View, float4(In[0].Position, 1.0));

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
	float u = (type % 4)*0.25f;
	float v = (type / 4)*0.25f;
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	Out.Position = mul(Projection, posLT);
	Out.Tex = float2(u, v);
	ParticleStream.Append(Out);

	// �E��̓_�̈ʒu(�ˉe���W�n)�EUV�E�F���v�Z���ďo��
	Out.Position = mul(Projection, posRT);
	Out.Tex = float2(u + 0.25, v);
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	ParticleStream.Append(Out);

	// �����̓_�̈ʒu(�ˉe���W�n)�EUV�E�F���v�Z���ďo��
	Out.Position = mul(Projection, posLB);
	Out.Tex = float2(u, v + 0.25);
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	ParticleStream.Append(Out);

	// �E���̓_�̈ʒu(�ˉe���W�n)�EUV�E�F���v�Z���ďo��
	Out.Position = mul(Projection, posRB);
	Out.Tex = float2(u + 0.25, v + 0.25);
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	ParticleStream.Append(Out);

	ParticleStream.RestartStrip();
}
