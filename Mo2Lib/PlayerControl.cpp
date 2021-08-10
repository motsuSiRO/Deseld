#include "PlayerControl.h"
#include "Mo2GUI.h"
#include "Input.h"


void PlayerControl::Start()
{
	permit = true;

	AddIAction("Empty");

	AddIAction("Firstary", L_CLICK);
	AddIAction("Secondary", R_CLICK);
	AddIAction("Skill1", M_CLICK, DIK_Q);
	AddIAction("Front", DIK_W, DIK_UP);
	AddIAction("Back", DIK_S, DIK_DOWN);
	AddIAction("Left", DIK_A, DIK_LEFT);
	AddIAction("Right", DIK_D, DIK_RIGHT);
	AddIAction("Dodge", DIK_LSHIFT);
}

void PlayerControl::Update()
{
	if (Holddown("Front"))
	{
		dir_vec.y = 1;
	}
	else if (Holddown("Back"))
	{
		dir_vec.y = -1;
	}
	else
	{
		dir_vec.y = 0;
	}

	if (Holddown("Left"))
	{
		dir_vec.x = -1;
	}
	else if (Holddown("Right"))
	{
		dir_vec.x = 1;
	}
	else
	{
		dir_vec.x = 0;
	}


	if (INPUT.KeyPressed(DIK_F5))
	{
		permit ^= (bool)1;
	}
}

void PlayerControl::ImGui()
{

	std::string str, s;
	s = "##" + parent->GetID();
	ImGui::SetNextTreeNodeOpen(false, ImGuiCond_Appearing);
	str = "PlayerControl" + s;
	if (ImGui::CollapsingHeader(str.c_str()))
	{
		ImGui::Checkbox("Control : permit", &permit);

		for (auto act : i_action_map)
		{
			ImGui::Text(act.first.c_str());

		}
	}
}

PlayerControl& PlayerControl::AddIAction(std::string _tag, int _key1, int _key2, int _pad)
{
	InputAction action;
	action.key_1 = _key1;
	action.key_2 = _key2;
	action.pad = _pad;
	i_action_map.try_emplace(_tag, action).second;

	return *this;
}

bool PlayerControl::Pressed(const char* _tag)
{
	if (i_action_map.find(_tag) != i_action_map.end())
	{
		return permit && i_action_map[_tag].Pressed();
	}
	return false;
}

bool PlayerControl::Released(const char* _tag)
{
	if (i_action_map.find(_tag) != i_action_map.end())
	{
		return permit && i_action_map[_tag].Released();
	}
	return false;
}

bool PlayerControl::Holddown(const char* _tag)
{
	if (i_action_map.find(_tag) != i_action_map.end())
	{
		return permit && i_action_map[_tag].HoldDown();
	}
	return false;
}


bool InputAction::Pressed()
{
	if (INPUT.KeyPressed(key_1) || INPUT.KeyPressed(key_2))
	{
		return true;
	}
	return false;
}

bool InputAction::Released()
{
	if (INPUT.KeyReleased(key_1) || INPUT.KeyReleased(key_2))
	{
		return true;
	}
	return false;
}

bool InputAction::HoldDown()
{
	if (INPUT.KeyKeeping(key_1) || INPUT.KeyKeeping(key_2))
	{
		return true;
	}
	return false;
}