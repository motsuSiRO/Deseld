
#include "Structures.hlsli"

VS_OUT main(VS_IN vin)
{



	float3 blended_position = { 0, 0, 0 };
	float3 blended_normal = { 0, 0, 0 };
	for (int i = 0; i < 4; i++)
	{
		blended_position += (vin.bone_weights_1to4[i] * mul(vin.position, bone_transforms[vin.bone_indices_1to4[i]])).xyz;
		blended_position += (vin.bone_weights_5to8[i] * mul(vin.position, bone_transforms[vin.bone_indices_5to8[i]])).xyz;

		blended_normal += (vin.bone_weights_1to4[i] * mul(float4(vin.normal.xyz, 0), bone_transforms[vin.bone_indices_1to4[i]])).xyz;
		blended_normal += (vin.bone_weights_5to8[i] * mul(float4(vin.normal.xyz, 0), bone_transforms[vin.bone_indices_5to8[i]])).xyz;
	}

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
