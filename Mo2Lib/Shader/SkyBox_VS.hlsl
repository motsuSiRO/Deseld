#include "Structures.hlsli"

VS_OUT main(VS_IN vin) 
{
	float3 blended_position = mul(vin.position, world).xyz;

	VS_OUT vout;

	vout.position = mul(float4(blended_position, 1.0), cam.view_projection);
	vout.color.xyz = material_color.xyz;
	vout.color.w = material_color.w;
	vout.texcoord = vin.texcoord;


	return vout;
}