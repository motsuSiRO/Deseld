#include "Mo2GUI.h"
#include "Input.h"

void Mo2GUI::Initialize()
{
	FPS = 0.f;
	msdt = 0.f;
#if _DEBUG
	win_showFPS = true;
#else
	win_showFPS = false;
#endif
	win_option = false;
	win_input = false;
	win_camera = false;
	win_pl = false;
	win_reticule = false;
	win_cursor = true;
	visible_collider = false;
}

void Mo2GUI::ShowFPS()
{
	const float DISTANCE = 50.0f;
	static int corner = 0;
	//ImGui::InputInt("##suincorner", &corner);
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("Simple overlay", 0, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		ImGui::Text("   FPS : %.3f", FPS);
		ImGui::Text("FrameTime :%.3f (ms)", msdt);
	}
	ImGui::End();

}

void Mo2GUI::ImGuiShowDemo()
{
	ImGui::ShowDemoWindow();
}

void Mo2GUI::SetNextWindowOnce(Mo2Lib::Float2 left_up, Mo2Lib::Float2 _size)
{
	ImVec2 pos;
	pos.x = left_up.x;
	pos.y = left_up.y;
	ImVec2 size;
	size.x = _size.x;
	size.y = _size.y;
	ImGui::SetNextWindowPos(pos, ImGuiCond_Once);
	ImGui::SetNextWindowSize(size, ImGuiCond_Once);
}

void Mo2GUI::ImGui()
{
	if (ImGui::Button("showFPS"))
	{
		win_showFPS ^= (bool)1;
	}
	if (ImGui::Button("OPTION"))
	{
		win_option ^= (bool)1;
	}
	if (ImGui::Button("INPUT"))
	{
		win_input ^= (bool)1;
	}
	if (ImGui::Button("CAMERA"))
	{
		win_camera ^= (bool)1;
	}
	if (ImGui::Button("Player"))
	{
		win_pl ^= (bool)1;
	}
	if (ImGui::Button("Reticule"))
	{
		win_reticule ^= (bool)1;
	}
	if (ImGui::Button("Show_Collider"))
	{
		visible_collider ^= (bool)1;
	}
	ImGui::SameLine();
	ImGui::Checkbox("Collider", &visible_collider);

	if (INPUT.KeyPressed(DIK_F1) || ImGui::Button("Show_Cursor"))
	{
		win_cursor ^= (bool)1;
	}
	ImGui::Checkbox("Cursor##mo2Gui", &win_cursor);


}