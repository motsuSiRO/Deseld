#include "ComponentObject.h"
#include "GameSystem.h"


void Component::ImGui()
{
	std::string s;
	std::string name(typeid(*this).name());
	s = "##" + name;
	s += parent->GetID();
	ImGui::Checkbox(s.c_str(), &is_visible);
}


void Object::Start()
{
	for (auto com : component_list)
	{
		com->Start();
	}
}


void Object::Update(float elapsed)
{
	delta_time = elapsed;
	auto buff = component_list;
	for (auto com : buff)
	{
		com->Update();
	}

}

void Object::Draw()
{
	for (auto com : component_list)
	{
		if (!com->is_visible)continue;
		com->Draw();
	}
}

void Object::ImGui()
{
	if (selected)
	{
		ImGui::SetNextWindowPos(ImVec2(1420.f, 0.f), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(500.f, 1000.f), ImGuiCond_Once);
		char _tag[128];
		strcpy_s(_tag, 128, tag);
		std::string str = "##";
		str += tag;
		if (ImGui::Begin(tag))
		{
			if (ImGui::InputText(str.c_str(), _tag, size_t(128), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				strcpy_s(tag, 128, _tag);
			}

			for (auto& com : component_list)
			{
				com->ImGui();
			}
		}
		ImGui::End();
	}
}

void Object::Destroy()
{
	destroy = true;
}