#pragma once

#include <memory>
//#include <d3d11.h>
#include "model_resource.h"
#include "Animation.h"

namespace Mo2Lib
{
	enum ANIM_TYPE
	{
		ANIM_NORMAL = 0,
		ANIM_BLEND,
		ANIM_RATIO,
		ANIM_ADD,
	};

	class Model
	{
	public:
		Transform transform;
		//DirectX::XMFLOAT3	scale;
		//DirectX::XMFLOAT4	rotate;
		//DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4 color = { 1.f, 1.f, 1.f, 1.f };
		Model(std::shared_ptr<ModelResource>& resource);
		~Model() {}

		//struct Node
		//{

		//	const char* name;
		//	Node* parent;
		//	DirectX::XMFLOAT3	scale;
		//	DirectX::XMFLOAT4	rotate;
		//	DirectX::XMFLOAT3	translate;
		//	DirectX::XMFLOAT4X4	local_transform;
		//	DirectX::XMFLOAT4X4	world_transform;

		//	DirectX::XMFLOAT3 GetWorldPos() { return DirectX::XMFLOAT3(world_transform._41, world_transform._42, world_transform._43); }
		//};

		// アニメーション
		//bool IsPlayAnimation() const { return m_current_anim >= 0; }
		//void PlayAnimation(int animation_index, float anim_rate = 1.0f, bool loop = false);
		//void PlayBlendAnimation(int animation_index, float blend_rate = 1.0f, float anim_rate = 1.0f, bool loop = false);
		//void PlayRatingAnimation(int animation_index1, int animation_index2, float blend_rate = 1.0f, float anim_rate = 1.0f, bool loop = false);
		void PlayAnim(int layer_index, int anim_index, bool loop = false, bool force = false);
		void PlayBlendAnim(int layer_index, int anim_index, bool loop = false, bool force = false);
		void PlayAddAnim(int layer_index, int anim_first, int anim_second, bool loop = false, bool force = false);
		void PlayRatioAnim(int layer_index, int anim_first, int anim_second, float blend_rate, bool force = true);
		void PlayRatioDefAnim(int layer_index, int anim_first, int anim_second, float blend_rate, bool force = true);
		Animator& AddLayer(int first, int last);
		void InitializeAnimation();
		void UpdateAnimation(Animator* anim_data, float elapsed_time);
		void NormalAnimation(Animator* anim_data, float elapsed_time);
		void AddAnimation(Animator* anim_data, int call_index, float elapsed_time);
		void RatioAnimation(Animator* anim_data, float elapsed_time);
		void ChangeAnim(Animator* anim_data, float elapsed_time);
		//void RatingAnim(Animator* anim_data, float elapsed_time);
		// 行列計算
		void CalculateLocalTransform();
		void CalculateWorldTransform();

		std::vector<Animator::Node>& GetNodes() { return m_nodes; }
		ModelResource* GetModelResource() { return m_model_resource.get(); }
		void SetModelColor(DirectX::XMFLOAT4 color) { this->color = color; }
		DirectX::XMMATRIX BulidWorld();
		void SetTransform(Mo2Lib::Float3 pos, Mo2Lib::Float4 orient, Mo2Lib::Float3 sc)
		{
			transform.translate = pos; transform.rotate = orient; transform.scale = sc;
		}
		void SetTransform(Mo2Lib::Transform t)
		{
			transform = t;
		}

		void SetTranslate(Mo2Lib::Float3 pos) { transform.translate = pos; }
		void SetRoatation(Mo2Lib::Float4 orient) { transform.rotate = orient; }
		void SetScale(Mo2Lib::Float3 s) { transform.scale = s; }
	public:
		std::shared_ptr<ModelResource>	m_model_resource;
		std::vector<Animator::Node>				m_nodes;
		//int								m_previous_anim = -1;
		//int								m_current_anim = -1;
		//float							m_current_seconds = 0.0f;
		//float							m_blend_seconds = 0.0f;
		//float							m_anim_rate = 0.0f;
		//float							m_blend_rate = 0.0f;
		//bool							m_loop_anim = false;
		//bool							m_end_anim = false;
		//bool							m_blend_anim = false;
		//bool							m_rating_anim = false;
	};


	enum MODEL_LOAD_MODE
	{
		LOADINGMODE_FBX = 0,
		LOADINGMODE_BIN,
		LOADINGMODE_JSON,
	};

	int LoadModelResource(std::shared_ptr<ModelResource> resource, const char* filename, int mode, int load_type, bool is_save = false);
}