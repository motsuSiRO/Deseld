//#include "Animation.h"
//#include "Mo2GUI.h"
//
//namespace Mo2Lib
//{
//	void Animator::Initialize()
//	{
//		data.blend_timer = 0.2f;
//	}
//
//	void Animator::PlayAnim(
//		int index,
//		bool l,
//		bool force)
//	{
//		if (current_anim_index == index && !data.end_anim) return;
//		if (current_anim_index == -1 || force)
//		{
//			current_anim_index = index;
//			data.anim_sec = 0.f;
//			data.end_anim = false;
//			data.loop = l;
//		}
//	}
//
//	void Animator::PlayBlendAnim(
//		int index,
//		bool l,
//		bool force)
//	{
//		if (current_anim_index == index && !data.end_anim) return;
//		if (current_anim_index == -1 || force)
//		{
//			prev.anim_index = current_anim_index;
//			current_anim_index = index;
//			data.anim_sec = 0.f;
//			data.end_anim = false;
//			data.loop = l;
//			prev.change_anim = true;
//		}
//
//	}
//
//
//
//	//void Animator::AnimImGui(std::string s)
//	//{
//	//	std::string str = "Animation##" + s;
//
//	//	if (ImGui::CollapsingHeader(str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
//	//	{
//
//
//	//	}
//	//}
//}
//
