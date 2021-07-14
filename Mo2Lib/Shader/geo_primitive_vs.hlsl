//#include "geo_primitive.hlsli"
#include "model.hlsli"

VS_OUT main(VS_IN vin)
{
	VS_OUT vout;
	float3 blended_position = vin.position;

	vout.position = mul(float4(blended_position, 1.0), cam.view_projection);

	float4 N = normalize(mul(float4(vin.normal, 0), world));
	float4 L = normalize(-light_direction);

	vout.color = material_color * max(0.5, dot(L, N));
	vout.color.a = material_color.a;

	//vout.texcoord = texcoord;

	return vout;
}