
#include "logger.h"
#include "Mo2Model.h"
#include "misc.h"
#include "Vector3D.h"
#include "framework.h"

namespace Mo2Lib
{
	Model::Model(std::shared_ptr<ModelResource>& resource)
	{
		//translate = { 0,0,0 };
		//rotate = { 0,0,0,0 };
		//scale = { 1,1,1 };
		color = { 1,1,1,1 };


		m_model_resource = resource;

		// ノード
		const std::vector<ModelResource::Node>& res_nodes = resource->GetNodes();

		m_nodes.resize(res_nodes.size());
		for (size_t node_index = 0; node_index < m_nodes.size(); ++node_index)
		{
			auto&& src = res_nodes.at(node_index);
			auto&& dst = m_nodes.at(node_index);

			dst.name = src.name.c_str();
			dst.parent = src.parent_index >= 0 ? &m_nodes.at(src.parent_index) : nullptr;
			dst.transform.scale = Mo2Lib::Float3(1.f, 1.f, 1.f);
			dst.transform.rotate = Mo2Lib::Float4(0.f, 0.f, 0.f, 1.f);
			dst.transform.translate = Mo2Lib::Float3(0.f, 0.f, 0.f);
		}

	}


	// アニメーション計算
	void Model::InitializeAnimation()
	{
		const ModelResource::Animation& animation = m_model_resource->GetAnimations()[0];
		const ModelResource::Keyframe& keyframe0 = animation.keyframes[0];
		int node_count = static_cast<int>(m_nodes.size());

		int remove = m_model_resource->GetRemoveNodeCount();
		for (int node_index = remove; node_index < node_count; ++node_index)
		{
			const ModelResource::NodeKeyData& key0 = keyframe0.node_keys[node_index - remove];

			Animator::Node& node = m_nodes.at(node_index);

			DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&key0.scale);
			DirectX::XMVECTOR r = DirectX::XMLoadFloat4(&key0.rotate);
			DirectX::XMVECTOR t = DirectX::XMLoadFloat3(&key0.translate);

			node.transform.scale = s;
			node.transform.rotate = r;
			node.transform.translate = t;

		}

	}

	void Model::UpdateAnimation(Animator* anim, float elapsed_time)
	{
		if (!anim)
		{
			return;
		}


		anim->current_layer = 0;
		for (;(size_t)anim->current_layer < anim->layers.size();++anim->current_layer)
		{
			auto& layer = anim->GetAnimLayer();
			if (layer.call.size() <= 0)
			{
				continue;
			}

			if (layer.change_anim)
			{
				if (layer.copied_bones)
				{
					for (int i = layer.first_bone; i < layer.last_bone; ++i)
					{
						anim->prev_nodes[i] = m_nodes[i];

						//anim->prev_nodes.assign(m_nodes.begin() + layer.first_bone, m_nodes.begin() + layer.last_bone);
					}
					//std::copy(m_nodes.begin() + layer.first_bone, m_nodes.begin() + m_nodes[layer.last_bone], anim->prev_nodes[layer.first_bone]);
					layer.copied_bones = false;
				}
				ChangeAnim(anim, elapsed_time);
				continue;
			}

			int c_index = 0;
			for (auto& c : layer.call)
			{
				if (c.first <= -1)continue;

				switch (c.blend_type)
				{
				case Animator::ANIM_NORMAL:
				case Animator::ANIM_BLEND:
					NormalAnimation(anim, elapsed_time);
					break;
				case Animator::ANIM_RATIO:
					NormalAnimation(anim, elapsed_time);
					RatioAnimation(anim, elapsed_time);
					break;
				case Animator::ANIM_ADD:
					AddAnimation(anim, c_index, elapsed_time);
					break;
				default:
					break;
				}
				++c_index;
			}

		}
	}

	void Model::AddAnimation(Animator* anim, int c_index, float elapsed_time)
	{
		AnimationLayer& layer = anim->GetAnimLayer();
		AnimCall& call = layer.call[c_index];
		Anim_Data& data1 = anim->data[call.first];
		Anim_Data& data2 = anim->data[call.second];

		const ModelResource::Animation& animation1 = m_model_resource->GetAnimations().at(call.first);
		const ModelResource::Animation& animation2 = m_model_resource->GetAnimations().at(call.second);

		const std::vector<ModelResource::Keyframe>& keyframes1 = animation1.keyframes;
		const std::vector<ModelResource::Keyframe>& keyframes2 = animation2.keyframes;

		int key_count = static_cast<int>(keyframes2.size());
		for (int key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const ModelResource::Keyframe& keyframe10 = keyframes1.at(key_index);
			const ModelResource::Keyframe& keyframe11 = keyframes1.at(key_index + 1);
			const ModelResource::Keyframe& keyframe20 = keyframes2.at(key_index);
			const ModelResource::Keyframe& keyframe21 = keyframes2.at(key_index + 1);
			if (data1.anim_sec >= keyframe10.seconds && data1.anim_sec < keyframe11.seconds)
			{
				float rate = (data1.anim_sec - keyframe10.seconds) / (keyframe11.seconds - keyframe10.seconds);

				//int node_count = static_cast<int>(m_nodes.size());
				//int remove = m_model_resource->GetRemoveNodeCount();
				for (int node_index = layer.first_bone; node_index < layer.last_bone; ++node_index)
				{
					// ２つのキーフレーム間の補完計算
					const ModelResource::NodeKeyData& key10 = keyframe10.node_keys.at(0);
					const ModelResource::NodeKeyData& key11 = keyframe11.node_keys.at(1);
					const ModelResource::NodeKeyData& key20 = keyframe20.node_keys.at(node_index);
					const ModelResource::NodeKeyData& key21 = keyframe21.node_keys.at(node_index);

					Animator::Node& node = m_nodes.at(node_index);

					//DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
					//DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR r10 = DirectX::XMLoadFloat4(&key10.rotate);
					DirectX::XMVECTOR r11 = DirectX::XMLoadFloat4(&key11.rotate);
					DirectX::XMVECTOR t10 = DirectX::XMLoadFloat3(&key10.translate);
					DirectX::XMVECTOR t11 = DirectX::XMLoadFloat3(&key11.translate);

					DirectX::XMVECTOR r20 = DirectX::XMLoadFloat4(&key20.rotate);
					DirectX::XMVECTOR r21 = DirectX::XMLoadFloat4(&key21.rotate);
					DirectX::XMVECTOR t20 = DirectX::XMLoadFloat3(&key20.translate);
					DirectX::XMVECTOR t21 = DirectX::XMLoadFloat3(&key21.translate);

					//DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
					DirectX::XMVECTOR r1 = DirectX::XMQuaternionSlerp(r10, r11, rate);
					DirectX::XMVECTOR t1 = DirectX::XMVectorLerp(t10, t11, rate);

					DirectX::XMVECTOR r2 = DirectX::XMQuaternionSlerp(r20, r21, rate);
					DirectX::XMVECTOR t2 = DirectX::XMVectorLerp(t20, t21, rate);

					r1 = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(r1), r2);

					t1 = DirectX::XMVectorSubtract(t1, t2);
					//node.transform.scale = s;
					//node.transform.rotate *= 2;
					//node.transform.translate *= 2;
					node.transform.rotate *= r1;

					//node.transform.translate -= t;
					//node.transform.rotate /= 2;
					//node.transform.translate /= 2;


				}
				break;
			}
		}


		// 時間経過
		// 最終フレーム処理
		data2.anim_sec += elapsed_time * data2.anim_spd;
		//data1.first_call = false;

		if (data2.anim_sec >= animation2.seconds_length)
		{
			if (data2.loop)
			{
				data2.anim_sec -= animation1.seconds_length;
			}
			else
			{
				data1.anim_sec = animation1.seconds_length;
				data2.anim_sec = animation2.seconds_length;
				data1.end_anim = true;
				data2.end_anim = true;
				layer.call.pop_back();
				c_index--;
			}
		}

	}

	void Model::NormalAnimation(Animator* anim, float elapsed_time)
	{
		AnimationLayer& layer = anim->GetAnimLayer();
		Anim_Data& it = anim->data[layer.call[0].first];

		const ModelResource::Animation& animation = m_model_resource->GetAnimations().at(layer.call[0].first);
		it.sec_len = animation.seconds_length;

		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		int key_count = static_cast<int>(keyframes.size());
		for (int key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(key_index);
			const ModelResource::Keyframe& keyframe1 = keyframes.at(key_index + 1);
			if (layer.anim1_sec >= keyframe0.seconds && layer.anim1_sec < keyframe1.seconds)
			{
				float rate = (layer.anim1_sec - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
				//int node_count = static_cast<int>(m_nodes.size());
				for (int node_index = layer.first_bone; node_index < layer.last_bone; ++node_index)
				{
					// ２つのキーフレーム間の補完計算
					const ModelResource::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
					const ModelResource::NodeKeyData& key1 = keyframe1.node_keys.at(node_index);

					Animator::Node& node = m_nodes.at(node_index);

					//DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
					//DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
					DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

					//DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
					DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
					DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

					//node.transform.scale = s;
					node.transform.rotate = r;
					node.transform.translate = t;

				}
				break;
			}
		}


		// 時間経過
		// 最終フレーム処理
		layer.anim1_sec += elapsed_time * it.anim_spd;
		layer.first_rate = layer.anim1_sec / animation.seconds_length;
//		it.first_call = false;
		
		if (layer.anim1_sec >= animation.seconds_length)
		{
			if (it.loop)
			{
				layer.anim1_sec -= animation.seconds_length;
			}
			else
			{
				layer.anim1_sec = animation.seconds_length;
				layer.call[0].first = -1;
				it.end_anim = true;
			}
		}
	}

	void Model::RatioAnimation(Animator* anim, float elapsed_time)
	{
		AnimationLayer& layer = anim->GetAnimLayer();
		Anim_Data& it = anim->data[layer.call[0].second];

		const ModelResource::Animation& animation = m_model_resource->GetAnimations().at(layer.call[0].second);
		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		int key_count = static_cast<int>(keyframes.size());
		for (int key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(key_index);
			const ModelResource::Keyframe& keyframe1 = keyframes.at(key_index + 1);
			if (layer.anim2_sec >= keyframe0.seconds && layer.anim2_sec < keyframe1.seconds)
			{
				float rate = (layer.anim2_sec - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
				//assert(m_nodes.size() == keyframe0.node_keys.size());
				//assert(m_nodes.size() == keyframe1.node_keys.size());

				//int node_count = static_cast<int>(m_nodes.size());
				//int remove = m_model_resource->GetRemoveNodeCount();
				for (int node_index = layer.first_bone; node_index < layer.last_bone; ++node_index)
				{
					// ２つのキーフレーム間の補完計算
					const ModelResource::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
					const ModelResource::NodeKeyData& key1 = keyframe1.node_keys.at(node_index);

					//Animator::Node& blend_node = anim->nodes.at(node_index);
					Animator::Node& node = m_nodes.at(node_index);

					DirectX::XMVECTOR s, r, t;
					//if (anim->next_index >= 0)
					{
						//DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
						//DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
						DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
						DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
						DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
						DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

						//s = DirectX::XMVectorLerp(
						//	node.transform.scale.ConvertToXMVECTOR(),
						//	s0, anim->blend_sec);
						r = DirectX::XMQuaternionSlerp(
							node.transform.rotate.ConvertToXMVECTOR(),
							DirectX::XMQuaternionSlerp(r0, r1, rate),
							layer.blend_rate);
						t = DirectX::XMVectorLerp(
							node.transform.translate.ConvertToXMVECTOR(),
							DirectX::XMVectorLerp(t0, t1, rate),
							layer.blend_rate);

					}

					//node.transform.scale = s;
					node.transform.rotate = r;
					node.transform.translate = t;

					//DirectX::XMStoreFloat3(&node.transform.scale, s);
					//DirectX::XMStoreFloat4(&node.transform.rotate, r);
					//DirectX::XMStoreFloat3(&node.transform.translate, t);

				}
			}
		}

		layer.anim2_sec = layer.first_rate * animation.seconds_length;

		//if (it.anim_sec >= animation.seconds_length)
		//{
		//	if (it.loop)
		//	{
		//		it.anim_sec -= animation.seconds_length;
		//	}
		//	else
		//	{
		//		it.anim_sec = animation.seconds_length;
		//		it.end_anim = true;
		//	}
		//}
	}

	void Model::ChangeAnim(Animator* anim, float elapsed_time)
	{
		AnimationLayer& layer = anim->GetAnimLayer();
		Anim_Data& it = anim->data[layer.call[0].first];

		const ModelResource::Animation& animation = m_model_resource->GetAnimations().at(layer.call[0].first);
		it.sec_len = animation.seconds_length;

		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		//int key_count = static_cast<int>(keyframes.size());
		//for (int key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(0);
			//const ModelResource::Keyframe& keyframe1 = keyframes.at(key_index + 1);
			//if (anim->current_sec >= keyframe0.seconds && anim->current_sec < keyframe1.seconds)
			{
				float rate = layer.anim1_sec / it.sec_len;

				//assert(m_nodes.size() == keyframe0.node_keys.size());
				//assert(m_nodes.size() == keyframe1.node_keys.size());

				//int node_count = static_cast<int>(m_nodes.size());
				for (int node_index = layer.first_bone; node_index < layer.last_bone; ++node_index)
				{
					// ２つのキーフレーム間の補完計算
					const ModelResource::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
					//const ModelResource::NodeKeyData& key1 = keyframe1.node_keys.at(node_index - remove);

					Animator::Node& blend_node = anim->prev_nodes[node_index];
					Animator::Node& node = m_nodes[node_index];

					DirectX::XMVECTOR s, r, t;
					//if (anim->next_index >= 0)
					{
						//DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
						//DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
						DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
						//DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
						DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
						//DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

						//s = DirectX::XMVectorLerp(
						//	node.transform.scale.ConvertToXMVECTOR(),
						//	s0, anim->blend_sec);
						r = DirectX::XMQuaternionSlerp(
							blend_node.transform.rotate.ConvertToXMVECTOR(),
							r0, min(1.0f, rate));
						t = DirectX::XMVectorLerp(
							blend_node.transform.translate.ConvertToXMVECTOR(),
							t0, min(1.0f, rate));

					}

					//node.transform.scale = s;
					node.transform.rotate = r;
					node.transform.translate = t;

					//DirectX::XMStoreFloat3(&node.transform.scale, s);
					//DirectX::XMStoreFloat4(&node.transform.rotate, r);
					//DirectX::XMStoreFloat3(&node.transform.translate, t);

				}
			}
		}

		layer.anim1_sec += elapsed_time * it.blend_spd * it.anim_spd;
		//it.first_call = false;
		if (layer.anim1_sec >= 1.0f)
		{
			layer.anim1_sec = 0.f;
			it.end_anim = false;
			layer.change_anim = false;
		}

	}




	// ローカル変換行列計算
	void Model::CalculateLocalTransform()
	{
		
		for (Animator::Node& node : m_nodes)
		{
			DirectX::XMMATRIX scale, rotate, translate, local;
			scale = DirectX::XMMatrixScaling(node.transform.scale.x, node.transform.scale.y, node.transform.scale.z);
			rotate = DirectX::XMMatrixRotationQuaternion(node.transform.rotate.ConvertToXMVECTOR());
			translate = DirectX::XMMatrixTranslation(node.transform.translate.x, node.transform.translate.y, node.transform.translate.z);

			local = scale * rotate * translate;
			DirectX::XMStoreFloat4x4(&node.local_transform, local);
		}
	}

	// ワールド変換行列計算
	void Model::CalculateWorldTransform()
	{
		DirectX::XMMATRIX world = BulidWorld();
		for (Animator::Node& node : m_nodes)
		{
			if (node.parent != nullptr)
			{
				DirectX::XMMATRIX local_transform = DirectX::XMLoadFloat4x4(&node.local_transform);
				DirectX::XMMATRIX parent_world_transform = DirectX::XMLoadFloat4x4(&node.parent->world_transform);
				DirectX::XMStoreFloat4x4(&node.world_transform, local_transform * parent_world_transform);
			}
			else
			{
				DirectX::XMMATRIX local_transform = DirectX::XMLoadFloat4x4(&node.local_transform);
				DirectX::XMStoreFloat4x4(&node.world_transform, local_transform * world);
			}
		}
	}


	DirectX::XMMATRIX Model::BulidWorld()
	{
		return transform.BuildWorld();
	}




	int LoadModelResource(std::shared_ptr<ModelResource> resource, const char* filename, int mode, int load_datatype, bool is_save)
	{
		static std::unordered_map<std::string, std::shared_ptr<ModelResource>> cache;

		auto it = cache.find(filename);
		if (it != cache.end())
		{
			
			*resource = *(it->second);
			return 1;
		}

		std::shared_ptr<ModelData> m_data = std::make_shared<ModelData>();
		//*resource = std::make_shared<ModelResource>();

		switch (mode)
		{
		case Mo2Lib::LOADINGMODE_FBX:
			m_data->Load(filename, ModelData::BINARY);
			resource->Load(Mo2System->DX11device.Get(), m_data.get(), load_datatype);
			break;
		case Mo2Lib::LOADINGMODE_BIN:
			m_data->Load(filename, ModelData::BINARY);
			resource->Load(Mo2System->DX11device.Get(), m_data.get(), load_datatype);

			cache[filename] = resource;
			break;
		case Mo2Lib::LOADINGMODE_JSON:
			m_data->Load(filename, ModelData::JSON);
			resource->Load(Mo2System->DX11device.Get(), m_data.get(), load_datatype);

			cache[filename] = resource;
			break;
		default:
			break;
		}

		if (is_save)
		{
			m_data->SaveToFile();
		}

		return 0;

	}
}

