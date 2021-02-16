#include "Animation.h"
#include "Mo2GUI.h"

namespace Mo2Lib
{
	void Animator::PlayAnim(
		int index,
		bool l,
		bool force)
	{
		//if (current_index == -1 || force)
		//{
		//	current_index = index;
		//	data[index].anim_sec = 0.f;
		//	data[index].end_anim = false;
		//	data[index].loop = l;
		//}
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
		int index)
	{
		//if (current_index != index)
		//{
		//	current_index = index;
		//	data[index].anim_sec = 0.f;
		//	data[index].end_anim = false;
		//	change_anim = true;
		//	copying_bones = true;
		//}

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

	void Animator::CallAnim(int layer_index, int anim_index, bool loop, bool force)
	{
		AnimationLayer& layer = layers[layer_index];
		AnimCall& called = layer.call[0];
		data[anim_index].first_call = true;
		if (called.first != anim_index)
		{
			if (force || called.first == -1 || data[anim_index].priority > data[called.first].priority)
			{
				called.blend_type = ANIM_NORMAL;
				called.first = anim_index;
				layer.called = anim_index;
				layer.anim1_sec = 0.f;
				layer.anim2_sec = 0.f;
				data[anim_index].loop = loop;
				data[anim_index].anim_sec = 0.f;
				data[anim_index].end_anim = false;
				data[anim_index].anim_spd = anim_spd;
			}
		}
		else
		{
			data[anim_index].anim_spd = anim_spd;
		}
		call_count++;
	}

	void Animator::CallBlendAnim(int layer_index, int anim_index, bool loop, bool force)
	{
		AnimationLayer& layer = layers[layer_index];
		AnimCall& called = layer.call[0];
		layer.called = anim_index;
		this->anim_spd = anim_spd;
		data[anim_index].first_call = true;

		if (called.first != anim_index)
		{
			if (force || called.first == -1/* || data[anim_index].priority > data[called.first].priority*/)
			{
				called.blend_type = ANIM_BLEND;
				called.first = anim_index;
				data[anim_index].loop = loop;
				data[anim_index].anim_sec = 0.f;
				data[anim_index].end_anim = false;

				layer.anim1_sec = 0.f;
				layer.anim2_sec = 0.f;
				layer.change_anim = true;
				layer.copied_bones = true;
			}
		}
		data[anim_index].anim_spd = anim_spd;
		call_count++;
	}


	void Animator::CallAddAnim(int layer_index, int anim_first, int anim_second, bool loop, bool force)
	{
		AnimationLayer& layer = layers[layer_index];
		layer.call.emplace_back(AnimCall());
		AnimCall& called = layer.call.back();
		layer.called = anim_first;

		if (called.first != anim_first || called.second != anim_second)
		{
			//if (force || data[anim_first].priority > data[called.first].priority)
			{
				called.blend_type = ANIM_ADD;
				called.first = anim_first;
				called.second = anim_second;
				data[anim_first].loop = loop;
				data[anim_first].anim_sec = 0.f;
				data[anim_first].end_anim = false;
				//data[anim_first].anim_spd = anim_spd;
				data[anim_second].loop = loop;
				data[anim_second].anim_sec = 0.f;
				data[anim_second].end_anim = false;
				//data[anim_second].anim_spd = anim_spd;


				//layer.change_anim = true;
				//layer.copied_bones = true;
			}
		}
		data[anim_first].anim_spd = anim_spd;
		data[anim_second].anim_spd = anim_spd;

		//call_count++;
	}

	void Animator::CallRatioAnim(int layer_index, int anim_first, int anim_second, float blend_rate, bool force)
	{
		AnimationLayer& layer = layers[layer_index];
		AnimCall& called = layer.call[0];
		layer.called = anim_first;
		layer.blend_rate = blend_rate;
		data[anim_first].first_call = true;
		data[anim_second].first_call = true;

		if (called.first != anim_first || called.second != anim_second)
		{
			if (force || data[anim_first].priority > data[called.first].priority)
			{
				called.blend_type = ANIM_RATIO;
				called.first = anim_first;
				called.second = anim_second;
				data[anim_first].loop = true;
				data[anim_first].anim_sec = 0.f;
				data[anim_first].end_anim = false;
				data[anim_second].loop = true;
				data[anim_second].anim_sec = 0.f;
				data[anim_second].end_anim = false;


				layer.anim1_sec = 0.f;
				layer.anim2_sec = 0.f;
				layer.change_anim = true;
				layer.copied_bones = true;
			}
		}
		data[anim_first].anim_spd = anim_spd;
		data[anim_second].anim_spd = anim_spd;


		call_count++;
	}

	void Animator::CallRatioDefAnim(int layer_index, int anim_first, int anim_second, float blend_rate, bool force)
	{
		AnimationLayer& layer = layers[layer_index];
		AnimCall& called = layer.call[0];
		layer.called = anim_first;
		layer.blend_rate = blend_rate;

		if (called.first != anim_first || called.second != anim_second)
		{
			if (force || data[anim_first].priority > data[called.first].priority)
			{
				called.blend_type = ANIM_RATIO;
				called.first = anim_first;
				called.second = anim_second;
				data[anim_first].loop = true;
				//data[anim_first].anim_sec = 0.f;
				data[anim_first].end_anim = false;
				data[anim_first].anim_spd = anim_spd;
				data[anim_second].loop = true;
				//data[anim_second].anim_sec = 0.f;
				data[anim_second].end_anim = false;
				data[anim_second].anim_spd = anim_spd;


				//layer.change_anim = true;
				//layer.copied_bones = true;
			}
		}
		data[anim_first].anim_spd = anim_spd;
		data[anim_second].anim_spd = anim_spd;


		call_count++;
	}


	void Animator::AddLayer(int first, int last)
	{
		AnimationLayer layer;
		layer.call.emplace_back(AnimCall());
		layer.first_bone = first;
		layer.last_bone = last;
		layers.emplace_back(layer);
	}


	void Animator::AnimImGui(std::string s)
	{
		std::string str = "Animation##" + s;

		if (ImGui::CollapsingHeader(str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Animation : %d", (int)data.size());
			static int current_anim = -1;
			int index = 0;
			ImGui::BeginTabBar("Anim_Tab");
			if (ImGui::BeginTabItem("Anim_Data"))
			{
				ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
				for (auto& it : data)
				{
					if (ImGui::Button("anim"))
					{
						current_anim = index;
					}
					ImGui::SameLine();
					ImGui::Text("%d", index);
					index++;
				}
				ImGui::EndChild();

				//if (current_anim >= 0)
				//{
				//	ImGui::Text("current : %d", current_anim);

				//	Anim_Data& anim_d = data[current_anim];

				//	str = "anim_seconds ##" + s;
				//	ImGui::SliderFloat(str.c_str(), &anim_d.anim_sec, 0.0f, anim_d.sec_len);

				//	str = "anim_priority ##" + s;
				//	ImGui::Text(str.c_str());
				//	ImGui::Text("%d", anim_d.priority);

				//	str = "influence_bone_num ##" + s;
				//	ImGui::Text(str.c_str());

				//	str = "Looping##" + s;
				//	ImGui::Checkbox(str.c_str(), &anim_d.loop);

				//	str = "end_anim##" + s;
				//	ImGui::Checkbox(str.c_str(), &anim_d.end_anim);

				//}
				ImGui::EndTabItem();
			}


			if (ImGui::BeginTabItem("Layers"))
			{
				static int layer_index = 0;
				index = 0;
				//ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
				//for (auto& it : layers)
				//{
				//	if (ImGui::Button("layer"))
				//	{
				//		layer_index = index;
				//	}
				//	ImGui::SameLine();
				//	ImGui::Text("%d", index);
				//	index++;
				//}
				//ImGui::EndChild();

				for(size_t i = 0; i < layers.size(); ++i)
				{
					AnimationLayer& layer = layers[i];
					current_anim = layer.call[0].first;
					//ImGui::Text("current_layer : %d", layer_index);
					ImGui::Text("current : %d", current_anim);

					if (current_anim < 0) continue;
					Anim_Data& anim_d = data[current_anim];
					ImGui::Text("BLEND_TYPE");
					switch (layer.call[0].blend_type)
					{
					case Mo2Lib::Animator::ANIM_NORMAL:
						ImGui::Text("ANIM_NORMAL");
						break;
					case Mo2Lib::Animator::ANIM_BLEND:
						ImGui::Text("ANIM_BLEND");
						break;
					case Mo2Lib::Animator::ANIM_RATIO:
						ImGui::Text("ANIM_RATIO");
						break;
					case Mo2Lib::Animator::ANIM_ADD:
						ImGui::Text("ANIM_ADD");
						break;
					default:
						break;
					}
					s = s + std::to_string(i);

					str = "anim_spd##" + s;
					ImGui::DragFloat(str.c_str(), &anim_d.anim_spd, 0.0f, anim_d.sec_len);

					str = "anim_seconds##" + s;
					ImGui::SliderFloat(str.c_str(), &layer.anim1_sec, 0.0f, anim_d.sec_len);

					str = "anim_priority##" + s;
					ImGui::Text(str.c_str());
					ImGui::Text("%d", anim_d.priority);

					str = "influence_bone_num##" + s;
					ImGui::Text(str.c_str());

					str = "Looping##" + s;
					ImGui::Checkbox(str.c_str(), &anim_d.loop);

					str = "end_anim##" + s;
					ImGui::Checkbox(str.c_str(), &anim_d.end_anim);

					str = "change_anim##" + s;
					ImGui::Checkbox(str.c_str(), &layer.change_anim);


					if (layer.call[0].blend_type == 2)
					{
						current_anim = layer.call[0].second;
						//ImGui::Text("current_layer : %d", layer_index);
						ImGui::Text("current : %d", current_anim);

						if (current_anim < 0) continue;

						Anim_Data& anim_d = data[current_anim];
						ImGui::Text("BLEND_TYPE");
						switch (layer.call[0].blend_type)
						{
						case Mo2Lib::Animator::ANIM_NORMAL:
							ImGui::Text("ANIM_NORMAL");
							break;
						case Mo2Lib::Animator::ANIM_BLEND:
							ImGui::Text("ANIM_BLEND");
							break;
						case Mo2Lib::Animator::ANIM_RATIO:
							ImGui::Text("ANIM_RATIO");
							break;
						case Mo2Lib::Animator::ANIM_ADD:
							ImGui::Text("ANIM_ADD");
							break;
						default:
							break;
						}

						str = "anim_spd##" + s;
						ImGui::DragFloat(str.c_str(), &anim_d.anim_spd, 0.0f, anim_d.sec_len);

						//if(anim_d)
						str = "anim_seconds##" + s;
						ImGui::SliderFloat(str.c_str(), &layer.first_rate, 0.0f, 1.0f);

						str = "anim_priority##" + s;
						ImGui::Text(str.c_str());
						ImGui::Text("%d", anim_d.priority);

						str = "influence_bone_num##" + s;
						ImGui::Text(str.c_str());

						str = "Looping##" + s;
						ImGui::Checkbox(str.c_str(), &anim_d.loop);

						str = "end_anim##" + s;
						ImGui::Checkbox(str.c_str(), &anim_d.end_anim);

						str = "change_anim##" + s;
						ImGui::Checkbox(str.c_str(), &layer.change_anim);

					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
			//ImGui::Text("current : %d", current_index);

			//Anim_Data& anim_d = data[current_index];

			//str = "anim_seconds ##" + s;
			//ImGui::SliderFloat(str.c_str(), &anim_d.anim_sec, 0.0f, sec_len);

			//str = "anim_priority ##" + s;
			//ImGui::Text(str.c_str());
			//ImGui::Text("%d", anim_d.priority);

			//str = "influence_bone_num ##" + s;
			//ImGui::Text(str.c_str());
			//ImGui::Text("< %d ~ %d >", anim_d.begin_bone, anim_d.end_bone);

			//str = "Looping" + s;
			//ImGui::Checkbox(str.c_str(), &anim_d.loop);

			//str = "end_anim" + s;
			//ImGui::Checkbox(str.c_str(), &anim_d.end_anim);


		}
	}
}

