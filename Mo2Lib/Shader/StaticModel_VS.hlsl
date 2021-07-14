
#include "Structures.hlsli"

VS_OUT main(VS_IN vin)
{

	float3 blended_position = mul(vin.position, world).xyz;
	float3 blended_normal = vin.normal;

	VS_OUT vout;
	vout.position = mul(float4(blended_position, 1.0), cam.view_projection);

	float3 N = normalize(mul(world, blended_normal));
	//float3 L = normalize(-light_direction.xyz);
	//float d = dot(L, N);
	vout.w_pos = float4(blended_position, 1.0f);
	vout.w_normal = float4(blended_normal, 0.0f);
	vout.color.xyz = material_color.xyz /**max(0, d)*/;
	vout.color.w = material_color.w;
	vout.texcoord = vin.texcoord;

	return vout;
}
