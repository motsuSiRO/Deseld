#include "pointSprite.hlsli"

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
GSInput main(VSInput input)
{
	uint type = input.Color.r;
	if (type < 0)discard;
	else return input;
}
