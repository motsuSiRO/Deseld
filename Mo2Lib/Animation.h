#pragma once
//#include <string>
#include <vector>
#include <string>
//#include "model.h"
#include "Vector3D.h"

namespace Mo2Lib
{

	struct Anim_Data//アニメーション単位のデータ
	{
		float anim_sec;
		float anim_spd = 1.0f; 
		float blend_timer = 0.1f;
		float sec_len;
		bool loop = false;
		bool end_anim = true;
		bool first_call = true;
	};
	struct Anim_Prev
	{
		bool change_anim;
		float anim_index;
	};


	class Animator
	{
	public:
		void Initialize();
		void PlayAnim(int animation_index, bool loop = false, bool force = true);
		void PlayBlendAnim(int index, bool l = true, bool force = true);
		bool IsPlaying() { return !data.end_anim; }
		struct Node
		{
			const char* name;
			Node* parent;
			Mo2Lib::Transform transform;
			DirectX::XMFLOAT4X4	local_transform;
			DirectX::XMFLOAT4X4	world_transform;

			Mo2Lib::Float3 GetWorldPos() { return Mo2Lib::Float3(world_transform._41, world_transform._42, world_transform._43); }
			Mo2Lib::Float3 GetPos() { return Mo2Lib::Float3(transform.translate.x, transform.translate.y, transform.translate.z); }
			void SetTransform(Mo2Lib::Transform t) { transform = t; }
		};

		enum ANIM_TYPE
		{
			ANIM_NORMAL = 0,
			ANIM_BLEND,
			ANIM_RATIO,
			ANIM_ADD,
		};
		int current_anim_index;
		int blend_type;
		Anim_Data data;
		Anim_Prev prev;
		
		void AnimImGui(std::string str);
	};

}
