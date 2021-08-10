#pragma once

namespace Mo2Lib
{
	enum ROOT_MOTION
	{
		NO_ROOT_MOTION = 0b0000,
		ROOT_MOTION_XZ = 0b0001,
		ROOT_MOTION_Y = 0b0010,
		ROOT_MOTION_RY = 0b0100,
	};

	enum ANIM_TYPE
	{
		ANIM_NORMAL = 0,
		ANIM_BLEND,
		ANIM_RATIO,
		ANIM_ADD,
	};

	struct Anim_Data//アニメーション単位のデータ
	{
		float anim_sec;
		float anim_spd = 1.0f; 
		float blend_timer = 0.1f;
		float sec_len;
		bool loop = false;
		bool end_anim = true;
	};
	struct Anim_Prev
	{
		bool change_anim;
		int anim_index;
	};


}
