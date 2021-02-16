struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color    : COLOR;
	float3 w_pos	: W_POS;
	float3 w_normal : W_NORMAL;
};

struct VS_IN
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 bone_weights_1to4 : WEIGHTSA;
	float4 bone_weights_5to8 : WEIGHTSB;
	uint4  bone_indices_1to4 : BONESA;
	uint4  bone_indices_5to8 : BONESB;

};

struct CAMERA
{
	float4 position;
	float4 direction;
	row_major float4x4 view;
	row_major float4x4 projection;
	row_major float4x4 view_projection;
	row_major float4x4 inverse_view;
	row_major float4x4 inverse_view_projection;
	row_major float4x4 previous_view_projection;
};


cbuffer CbScene : register(b0)
{
	CAMERA				cam;
	float4				light_direction;
	float4				light_color;
	float3				fog_color;
	float				fog_near;
	float				fog_far;
};

cbuffer CbObject : register(b1)
{
	row_major float4x4	world;
};


#define MAX_BONES 128
cbuffer CbMesh : register(b2)
{
	row_major float4x4	bone_transforms[MAX_BONES];

};

cbuffer CbSubset : register(b3)
{
	float4				material_color;
};
