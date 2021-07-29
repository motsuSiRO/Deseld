#include "Animation.h"
#include "Mo2GUI.h"

namespace Mo2Lib
{
	void Animator::Initialize()
	{
		data.blend_timer = 0.2f;
	}

	void Animator::PlayAnim(
		int index,
		bool l,
		bool force)
	{
		if (current_anim_index == index && !data.end_anim) return;
		if (current_anim_index == -1 || force)
		{
			current_anim_index = index;
			data.anim_sec = 0.f;
			data.end_anim = false;
			data.loop = l;
		}
	}

	//void Animator::PlayAddAnim(int animation_index)
	//{
	//	if (!add_flg)
	//	{
	//		add_flg = true;
	//		add_index = animation_index;
	//		add_sec = 0.f;
	//	}
	//}

	void Animator::PlayBlendAnim(
		int index,
		bool l,
		bool force)
	{
		if (current_anim_index == index && !data.end_anim) return;
		if (current_anim_index == -1 || force)
		{
			prev.anim_index = current_anim_index;
			current_anim_index = index;
			data.anim_sec = 0.f;
			data.end_anim = false;
			data.loop = l;
			prev.change_anim = true;
		}

	}

	//void Animator::PlayBlendRateAnim(
	//	int first_index, int second_index,
	//	float rate)
	//{
	//	if(rate_index[0] != first_index || rate_index[1] != second_index)
	//	{
	//		rate_index[0] = first_index;
	//		rate_index[1] = second_index;
	//		//ratio_sec[0] = 0.f;
	//		//ratio_sec[1] = 0.f;
	//		loop = true;

	//		next_index = -1;
	//		blend_rate = rate;

	//		end_anim = false;
	//		if (!ratio_flg)
	//		{
	//			blended_flg = true;
	//			ratio_flg = true;
	//		}
	//			ratio_end = false;
	//		blend_sec = 0.0f;
	//	}
	//	else
	//	{
	//		blend_rate = rate;
	//	}
	//	//call_count++;
	//}


	//void Animator::CallAnim(int index, bool loop, float anim_spd)
	//{
	//	if (data[index].priority > data[called].priority)
	//	{
	//		called = index;
	//		 = anim_spd;

	//		data[called].loop = loop;
	//		data[called].type = ANIM_BLEND;
	//		data[called].end_anim = false;
	//	}

	//}

	//void Animator::CallAddAnim(int index, bool loop, float anim_spd)
	//{
	//	//if (data[index].priority > data[called].priority)
	//	{
	//		//index = index;
	//		animation_speed = anim_spd;

	//		data[index].anim_sec = 0.f;
	//		data[index].loop = loop;
	//		data[index].type = ANIM_ADD;
	//		data[index].end_anim = false;
	//		copying_bones = true;
	//	}
	//}



	void Animator::AnimImGui(std::string s)
	{
		std::string str = "Animation##" + s;

		if (ImGui::CollapsingHeader(str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{


		}
	}
}

