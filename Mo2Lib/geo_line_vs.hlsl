#include "geo_line.hlsli"

VS_OUT main( float4 pos : POSITION)
{
	VS_OUT vout;
	vout.position = mul(pos, mul(world, view_projection));


	vout.color = material_color;
	vout.color.a = material_color.a;


	return vout;
}