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
		int priority;
		float anim_sec;
		float anim_spd = 1.0f; 
		float blend_spd = 10.f;
		float sec_len;
		bool loop = false;
		bool end_anim = true;
		bool first_call = true;
	};

	struct AnimCall
	{
		int first = -1;
		int second = -1;
		int blend_type = 0;
	};

	struct AnimationLayer //レイヤー単位でアニメーしょん適応
	{
		std::vector<AnimCall> call;
		int called = -1;
		float anim1_sec;
		float anim2_sec;
		int first_bone;
		int last_bone;
		bool change_anim =false;
		bool copied_bones = false;
		float anim_spd;
		float first_rate;
		float blend_rate;
	};

	class Animator
	{
	public:
		void Initialize();
		void PlayAnim(int animation_index, bool loop = false, bool force = false);
		void PlayBlendAnim(int next_anim_index);
		
		struct Node
		{
			const char* name;
			Node* parent;
			Mo2Lib::Transform transform;
			DirectX::XMFLOAT4X4	local_transform;
			DirectX::XMFLOAT4X4	world_transform;

			Mo2Lib::Float3 GetWorldPos() { return Mo2Lib::Float3(world_transform._41, world_transform._42, world_transform._43); }
			void SetTransform(Mo2Lib::Transform t) { transform = t; }
		};

		enum ANIM_TYPE
		{
			ANIM_NORMAL = 0,
			ANIM_BLEND,
			ANIM_RATIO,
			ANIM_ADD,
		};

		std::vector<Node> prev_nodes;
		std::vector<Anim_Data> data;
		std::vector<AnimationLayer> layers;

		int current_layer;
		float anim_spd = 1.f;
		int call_count = 0;
		//void CallAnim(int index, bool loop = false, float anim_spd = 1.0f);
		//void CallAddAnim(int index, bool loop = false, float anim_spd = 1.0f);
		void CallAnim(int layer_index, int anim_index, bool loop = false, bool force = false);
		void CallBlendAnim(int layer_index, int anim_index, bool loop = false, bool force = false);
		void CallAddAnim(int layer_index, int anim_first, int anim_second, bool loop = false, bool force = false);
		void CallRatioAnim(int layer_index, int anim_first, int anim_second, float blend_rate, bool force = true);
		void CallRatioDefAnim(int layer_index, int anim_first, int anim_second, float blend_rate, bool force = true);

		void AddLayer(int first, int last);
		//int GetCurrentIndex() { return current_index; }
		//Anim_Data& GetCurrentAnim() { return data[current_index]; }
		AnimationLayer& GetAnimLayer() { return layers[current_layer]; }
		
		void AnimImGui(std::string str);
	};

}
