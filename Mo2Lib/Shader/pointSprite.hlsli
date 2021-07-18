// **************************************************
//
//				パーティクルの描画処理
//
// **************************************************
//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2DArray<float4> DiffuseTexture : register(t0);

SamplerState DecalSampler : register(s0);


//--------------------------------------------
//	データーフォーマット
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
	float4 Position  : SV_POSITION;     // 頂点座標(射影座標系)
	float3 Tex  : TEXCOORD;        // テクスチャ座標
	float4 Color : COLOR0;
};

//--------------------------------------------
//	グローバル変数
//--------------------------------------------

cbuffer CBPerFrame : register(b0)
{
	matrix	View;			// ビュー変換行列
	matrix	Projection;		// 透視変換行列


};

//#define type 7
//#define size 0.1

