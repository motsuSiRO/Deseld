#include "Structures.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * model_color;

	color *= pin.color;


	return color;
}