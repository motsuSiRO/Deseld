#include "pointSprite.hlsli"

//--------------------------------------------
//	���_�V�F�[�_�[
//--------------------------------------------
GSInput main(VSInput input)
{
	uint type = input.Color.r;
	if (type < 0)discard;
	else return input;
}
