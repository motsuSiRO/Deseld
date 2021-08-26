
#include "logger.h"
#include "Mo2Model.h"
#include "misc.h"
#include "Vector3D.h"
#include "framework.h"
#include "Mo2GUI.h"

namespace Mo2Lib
{
	Model::Model(std::shared_ptr<ModelResource>& resource)
	{
		//translate = { 0,0,0 };
		//rotate = { 0,0,0,0 };
		//scale = { 1,1,1 };
		color = { 1,1,1,1 };

		data.blend_timer = 0.2f;

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

	void Model::PlayAnim(int index, bool l, bool force)
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

	void Model::PlayBlendAnim(int index, bool l, bool force)
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

	// アニメーション計算
	void Model::InitializeAnimation(int index)
	{
		const ModelResource::Animation& animation = m_model_resource->GetAnimations()[index];
		const ModelResource::Keyframe& keyframe0 = animation.keyframes[0];
		int node_count = static_cast<int>(m_nodes.size());

		for (int node_index = 0; node_index < node_count; ++node_index)
		{
			const ModelResource::NodeKeyData& key0 = keyframe0.node_keys[node_index];

			Node& node = m_nodes.at(node_index);

			DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&key0.scale);
			DirectX::XMVECTOR r = DirectX::XMLoadFloat4(&key0.rotate);
			DirectX::XMVECTOR t = DirectX::XMLoadFloat3(&key0.translate);

			node.transform.scale = s;
			node.transform.rotate = r;
			node.transform.translate = t;

		}

	}

	void Model::UpdateAnimation(float elapsed_time)
	{



		if (prev.change_anim)
		{
			ChangeAnim(elapsed_time);

			return;
		}
		root_motion = next_root_motion;

		switch (blend_type)
		{
		case ANIM_NORMAL:
		case ANIM_BLEND:
			NormalAnimation(elapsed_time);
			break;
		case ANIM_RATIO:
			NormalAnimation(elapsed_time);
			RatioAnimation(elapsed_time);
			break;
		case ANIM_ADD:
			break;
		default:
			break;
		}
	}

	void Model::AddAnimation(int c_index, float elapsed_time)
	{

	}

	void Model::NormalAnimation(float elapsed_time)
	{
		Mo2Lib::ROOT_MOTION& root = root_motion;

		const ModelResource::Animation& animation = m_model_resource->GetAnimations().at(current_anim_index);
		data.sec_len = animation.seconds_length;

		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		int key_count = static_cast<int>(keyframes.size());
		for (int key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(key_index);
			const ModelResource::Keyframe& keyframe1 = keyframes.at(key_index + 1);
			if (data.anim_sec >= keyframe0.seconds && data.anim_sec < keyframe1.seconds)
			{
				float rate = (data.anim_sec - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
				//int node_count = static_cast<int>(m_nodes.size());
				bool first_bone = true;
				for (size_t node_index = 0; node_index < m_nodes.size(); ++node_index)
				{
					// ２つのキーフレーム間の補完計算
					const ModelResource::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
					const ModelResource::NodeKeyData& key1 = keyframe1.node_keys.at(node_index);

					Node& node = m_nodes.at(node_index);

					//DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
					//DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
					Mo2Lib::Float4 r0 = DirectX::XMLoadFloat4(&key0.rotate);
					Mo2Lib::Float4 r1 = DirectX::XMLoadFloat4(&key1.rotate);
					Mo2Lib::Float3 t0 = DirectX::XMLoadFloat3(&key0.translate);
					Mo2Lib::Float3 t1 = DirectX::XMLoadFloat3(&key1.translate);

					Mo2Lib::Float4 r = r0.QuaternionSlerp(r1, rate);

					Mo2Lib::Float3 t = DirectX::XMVectorLerp(t0.ConvertToXMVECTOR(), t1.ConvertToXMVECTOR(), rate);
					
					if (root ^ Mo2Lib::NO_ROOT_MOTION)
					{
						if (first_bone)
						{
							Mo2Lib::Float3 prev_t = t;
							Mo2Lib::Float3 root_t = {};
							if (root & Mo2Lib::ROOT_MOTION_XZ)
							{
								prev_t.x = node.transform.translate.x;
								prev_t.z = node.transform.translate.z;
								root_t = { prev_t.x - t.x, 0.f, prev_t.z - t.z };
							}
							if (root & Mo2Lib::ROOT_MOTION_Y)
							{
								prev_t.y = node.transform.translate.y;
								root_t.y = prev_t.y - t.y;
							}
							if (root & Mo2Lib::ROOT_MOTION_RY)
							{

							}
							root_trans.translate = root_t;
							t = prev_t;

							first_bone = false;
						}
					}

					//node.transform.scale = s;
					node.transform.rotate = r;
					node.transform.translate = t;

				}
				break;
			}
		}


		// 時間経過
		// 最終フレーム処理
		data.anim_sec += elapsed_time * data.anim_spd;
		//layer.first_rate = anim->data.anim_sec / animation.seconds_length;

		if (data.anim_sec >= animation.seconds_length)
		{
			if (data.loop)
			{
				data.anim_sec -= animation.seconds_length;
			}
			else
			{
				data.anim_sec = animation.seconds_length;
				data.end_anim = true;
			}
		}
	}

	void Model::RatioAnimation(float elapsed_time)
	{
		const ModelResource::Animation& animation = m_model_resource->GetAnimations().at(current_anim_index);
		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		int key_count = static_cast<int>(keyframes.size());
		for (int key_index = 0; key_index < key_count - 1; ++key_index)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(key_index);
			const ModelResource::Keyframe& keyframe1 = keyframes.at(key_index + 1);
			if (data.anim_sec >= keyframe0.seconds && data.anim_sec < keyframe1.seconds)
			{
				float rate = (data.anim_sec - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
				//assert(m_nodes.size() == keyframe0.node_keys.size());
				//assert(m_nodes.size() == keyframe1.node_keys.size());

				//int node_count = static_cast<int>(m_nodes.size());
				//int remove = m_model_resource->GetRemoveNodeCount();
				for (size_t node_index = 0; node_index < m_nodes.size(); ++node_index)
				{
					// ２つのキーフレーム間の補完計算
					const ModelResource::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
					const ModelResource::NodeKeyData& key1 = keyframe1.node_keys.at(node_index);

					//Animator::Node& blend_node = anim->nodes.at(node_index);
					Node& node = m_nodes.at(node_index);

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
							rate);
						t = DirectX::XMVectorLerp(
							node.transform.translate.ConvertToXMVECTOR(),
							DirectX::XMVectorLerp(t0, t1, rate),
							rate);

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

	}

	void Model::ChangeAnim(float elapsed_time)
	{
		Mo2Lib::ROOT_MOTION& root = root_motion;

		const ModelResource::Animation& animation = m_model_resource->GetAnimations().at(current_anim_index);
		const ModelResource::Animation& anim_prev = m_model_resource->GetAnimations().at(prev.anim_index);

		const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
		const std::vector<ModelResource::Keyframe>& keys_prev = anim_prev.keyframes;
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const ModelResource::Keyframe& keyframe0 = keyframes.at(0);
			const ModelResource::Keyframe& key_prev = keys_prev.back();

			float rate = data.anim_sec / data.blend_timer;
			bool first_bone = true;
			for (int node_index = 0; node_index < m_nodes.size(); ++node_index)
			{
				// ２つのキーフレーム間の補完計算
				const ModelResource::NodeKeyData& key0 = keyframe0.node_keys.at(node_index);
				const ModelResource::NodeKeyData& key1 = key_prev.node_keys.at(node_index);

				Node& node = m_nodes[node_index];

				//DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
				//DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
				Mo2Lib::Float4 r0 = DirectX::XMLoadFloat4(&key0.rotate);
				Mo2Lib::Float4 r1 = DirectX::XMLoadFloat4(&key1.rotate);
				Mo2Lib::Float3 t0 = DirectX::XMLoadFloat3(&key0.translate);
				Mo2Lib::Float3 t1 = DirectX::XMLoadFloat3(&key1.translate);

				Mo2Lib::Float4 r = r1.QuaternionSlerp(r0, min(1.0f, rate));
				Mo2Lib::Float3 t = DirectX::XMVectorLerp(t1.ConvertToXMVECTOR(), t0.ConvertToXMVECTOR(), min(1.0f, rate));

				if (root ^ Mo2Lib::NO_ROOT_MOTION)
				{
					if (first_bone)
					{
						Mo2Lib::Float3 prev_t = t;
						Mo2Lib::Float3 root_t = {};
						if (root & Mo2Lib::ROOT_MOTION_XZ)
						{
							prev_t.x = node.transform.translate.x;
							prev_t.z = node.transform.translate.z;
							root_t = { prev_t.x - t.x, 0.f, prev_t.z - t.z };
						}
						if (root & Mo2Lib::ROOT_MOTION_Y)
						{
							prev_t.y = node.transform.translate.y;
							root_t.y = prev_t.y - t.y;
						}
						if (root & Mo2Lib::ROOT_MOTION_RY)
						{

						}
						root_trans.translate = root_t;
						t = prev_t;

						first_bone = false;
					}
				}

				//node.transform.scale = s;
				node.transform.rotate = r;
				node.transform.translate = t;

			}
		}

		data.anim_sec += elapsed_time;
		if (data.anim_sec >= data.blend_timer)
		{
			data.anim_sec = 0.f;
			data.end_anim = false;
			prev.change_anim = false;
		}

	}

	Mo2Lib::Float3 Model::RootAnimTransform(Transform& t)
	{
		DirectX::XMMATRIX S, R;
		S = DirectX::XMMatrixScaling(t.scale.x, t.scale.y, t.scale.z);
		R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&t.rotate));
		DirectX::XMMATRIX m = S * R;
		DirectX::XMMATRIX inv_m = DirectX::XMMatrixInverse(nullptr, m);

		Mo2Lib::Float3 local;
		DirectX::XMVECTOR v = DirectX::XMVectorSet(root_trans.translate.x, root_trans.translate.y, root_trans.translate.z, 1.0f);
		local = DirectX::XMVector3TransformCoord(v, inv_m);
		root_trans = {};

		return local;
	}

	void Model::ImGuiAnim()
	{
		std::string str, s;
		s = "##" + GetNodes().size();
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
		str = "Animation" + s;


		if (ImGui::CollapsingHeader(str.c_str()))
		{
			str = "##Animation" + GetNodes().size();
			const ModelResource::Animation& animation = m_model_resource->GetAnimations().at(current_anim_index);
			

			ImGui::Text(animation.name);
			s = str;
			float length = data.sec_len;
			s = str + "anim_end";
			ImGui::DragFloat(s.c_str(), &length);
			ImGui::SliderFloat(s.c_str(), &data.anim_sec, 0.f, length, "%.1f sec");

		}

	}

	// ローカル変換行列計算
	void Model::CalculateLocalTransform()
	{

		for (Node& node : m_nodes)
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
		for (Node& node : m_nodes)
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


	void ModelLoader::Setup(bool is_save)
	{
		assert(!active && "This loading is Active");

		is_saving = is_save;

		model_name = "";
		meta_data = std::make_shared<ModelData>();
		resource_buff = std::make_shared<ModelResource>();
		active = true;

	}

	void ModelLoader::Load(const char* filename, bool anim)
	{
		if (loaded_bin || is_find)return;
		if (model_name == "")model_name = filename;

		auto it = cache.find(filename);
		if (it != cache.end())
		{

			*resource_buff = *(it->second);
			is_find = true;
			return;
		}

		if (!anim)
		{
			loaded_bin = meta_data->Load(filename, ModelData::BINARY);
		}
		else
		{
			meta_data->AddAnimation(filename);
		}

	}

	std::shared_ptr<ModelResource> ModelLoader::Apply()
	{
		resource_buff->Load(Mo2System->DX11device.Get(), meta_data.get(), false);

		if (is_saving)
		{
			meta_data->SaveToFile(model_name.c_str());
		}

		return resource_buff;
	}

	void ModelLoader::End()
	{
		assert(active && "This loading isn't Active");

		cache[model_name] = resource_buff;

		meta_data.reset();
		resource_buff.reset();

		model_name = "";
		loaded_bin = false;
		is_find = false;
		active = false;
	}


}

