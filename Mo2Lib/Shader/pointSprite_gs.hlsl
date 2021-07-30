#include "pointSprite.hlsli"

//--------------------------------------------
//	ジオメトリシェーダー
//--------------------------------------------

[maxvertexcount(4)]
void main(point GSInput In[1],
	inout TriangleStream<PSInput> ParticleStream)
{
	// 座標変換　(ワールド座標系　→　ビュー座標系)
	float4 pos = mul(float4(In[0].Position, 1.0), View);
	float w = In[0].Size.x;
	float h = In[0].Size.y;
	float c = cos(In[0].Size.z);
	float s = sin(In[0].Size.z);

	//rx = x0;
	//ry = y0;
	//x0 = cos * rx + -sin * ry;
	//y0 = sin * rx + cos * ry;
	//rx = x1;
	//ry = y1;
	//x1 = cos * rx + -sin * ry;
	//y1 = sin * rx + cos * ry;
	//rx = x2;
	//ry = y2;
	//x2 = cos * rx + -sin * ry;
	//y2 = sin * rx + cos * ry;
	//rx = x3;
	//ry = y3;
	//x3 = cos * rx + -sin * ry;
	//y3 = sin * rx + cos * ry;

	// 点を面にする
	float4 posLT = pos + float4(-w, h, 0.0, 0.0);
	float4 posLB = pos + float4(-w, -h, 0.0, 0.0);
	float4 posRT = pos + float4(w, h, 0.0, 0.0);
	float4 posRB = pos + float4(w, -h, 0.0, 0.0);

	
	// 左上の点の位置(射影座標系)・UV・色を計算して出力
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

	// 右上の点の位置(射影座標系)・UV・色を計算して出力
	Out.Position = mul(posRT, Projection);
	Out.Tex = float3(u + slice, v, t);
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	ParticleStream.Append(Out);

	// 左下の点の位置(射影座標系)・UV・色を計算して出力
	Out.Position = mul(posLB, Projection);
	Out.Tex = float3(u, v + slice, t);
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	ParticleStream.Append(Out);

	// 右下の点の位置(射影座標系)・UV・色を計算して出力
	Out.Position = mul(posRB, Projection);
	Out.Tex = float3(u + slice, v + slice, t);
	Out.Color = float4(1, 1, 1, In[0].Color.a);
	ParticleStream.Append(Out);

	ParticleStream.RestartStrip();
}
