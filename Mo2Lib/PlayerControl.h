#pragma once
#include <string>
#include <unordered_map>
#include "ComponentObject.h"

struct InputAction
{
	int key_1;
	int key_2;
	int pad;
	
	bool Pressed();
	bool Released();
	bool HoldDown();
};

class PlayerControl : public Component
{
	std::unordered_map<std::string, InputAction> i_action_map;
public:
	bool permit;
	Mo2Lib::Float2 dir_vec;
	void Start();
	void Update();
	void ImGui();

	PlayerControl& AddIAction(std::string _tag, int _key1 = -1, int _key2 = -1, int _pad = -1);

	bool Pressed(const char* _tag);
	bool Released(const char* _tag);
	bool Holddown(const char* _tag);
};

