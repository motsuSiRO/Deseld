#pragma once
#include <string>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Vector3D.h"

class Mo2GUI
{
public:
	std::string str;
	float FPS;
	float msdt;
	bool win_ShowDemo;
	bool win_showFPS;
	bool win_option;
	bool win_input;
	bool win_camera;
	bool win_pl;
	bool win_reticule;
	bool win_cursor;
	bool visible_collider;

	void Initialize();
	void ShowFPS();
	void SetNextWindowOnce(Mo2Lib::Float2 left_up, Mo2Lib::Float2 size);
	void ImGuiShowDemo();
	void ImGui();

	static Mo2GUI* GetInstance()
	{
		static Mo2GUI ins;
		return &ins;
	}

private:
	Mo2GUI() {}
	~Mo2GUI() {}

};

static Mo2GUI* Mo2Gui()
{
	return Mo2GUI::GetInstance();
}
