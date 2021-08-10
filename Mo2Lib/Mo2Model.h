#pragma once

#include <memory>
//#include <d3d11.h>
#include "model_resource.h"
#include "Animation.h"
#include "Singleton.h"

namespace Mo2Lib
{
	class Model
	{
	public:
		Transform transform;
		DirectX::XMFLOAT4 color = { 1.f, 1.f, 1.f, 1.f };
		Model(std::shared_ptr<ModelResource>& resource);
		~Model() {}

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

		// アニメーション
		//bool IsPlayAnimation() const { return m_current_anim >= 0; }
		//void PlayAnimation(int animation_index, float anim_rate = 1.0f, bool loop = false);
		//void PlayBlendAnimation(int animation_index, float blend_rate = 1.0f, float anim_rate = 1.0f, bool loop = false);
		//void PlayRatingAnimation(int animation_index1, int animation_index2, float blend_rate = 1.0f, float anim_rate = 1.0f, bool loop = false);
		//void PlayAnim(int layer_index, int anim_index, bool loop = false, bool force = false);
		//void PlayBlendAnim(int layer_index, int anim_index, bool loop = false, bool force = false);
		//void PlayAddAnim(int layer_index, int anim_first, int anim_second, bool loop = false, bool force = false);
		//void PlayRatioAnim(int layer_index, int anim_first, int anim_second, float blend_rate, bool force = true);
		//void PlayRatioDefAnim(int layer_index, int anim_first, int anim_second, float blend_rate, bool force = true);
		void Initialize();
		void PlayAnim(int animation_index, bool loop = false, bool force = true);
		void PlayBlendAnim(int index, bool l = true, bool force = true);
		bool IsPlaying() { return !data.end_anim; }


		int current_anim_index;
		int blend_type;
		Transform root_trans = {};
		ROOT_MOTION root_motion;
		Anim_Data data;
		Anim_Prev prev;

		
		void InitializeAnimation(int anim_index = 0);
		void UpdateAnimation(float elapsed_time);
		void NormalAnimation(float elapsed_time);
		void AddAnimation(int call_index, float elapsed_time);
		void RatioAnimation(float elapsed_time);
		void ChangeAnim(float elapsed_time);
		//void RatingAnim(Animator* anim_data, float elapsed_time);
		Mo2Lib::Float3 RootAnimTransform(Transform& transform);
		
		// 行列計算
		void CalculateLocalTransform();
		void CalculateWorldTransform();

		std::vector<Node>& GetNodes() { return m_nodes; }
		Node* GetNodes(int index) { return &m_nodes[index]; }
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
		std::vector<Node>				m_nodes;
	};


	enum MODEL_LOAD_MODE
	{
		LOADINGMODE_FBX = 0,
		LOADINGMODE_BIN,
		LOADINGMODE_JSON,
	};

	class ModelLoader : public Singleton<ModelLoader>
	{
	private:
		std::unordered_map<std::string, std::shared_ptr<ModelResource>> cache;
		std::shared_ptr<ModelData> meta_data;
		std::shared_ptr<ModelResource> resource_buff;
		std::string model_name;
		bool is_saving;
		bool active = false;
		bool loaded_bin = false;
		bool is_find = false;


	public:
		void Setup(bool is_save = false);

		void Load(const char* filename, bool is_animation = false);

		std::shared_ptr<ModelResource> Apply();

		void End();

	};
}