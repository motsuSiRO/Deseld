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
	win_cursor = false;
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
		Mo2Gui()->win_showFPS ^= 1;
	}
	if (ImGui::Button("OPTION"))
	{
		Mo2Gui()->win_option ^= 1;
	}
	if (ImGui::Button("INPUT"))
	{
		Mo2Gui()->win_input ^= 1;
	}
	if (ImGui::Button("CAMERA"))
	{
		Mo2Gui()->win_camera ^= 1;
	}
	if (ImGui::Button("Player"))
	{
		Mo2Gui()->win_pl ^= 1;
	}
	if (ImGui::Button("Reticule"))
	{
		Mo2Gui()->win_reticule ^= 1;
	}
	if (ImGui::Button("Show_Collider"))
	{
		visible_collider ^= 1;
	}
	if (INPUT.KeyPressed(DIK_F1))
	{
		Mo2Gui()->win_cursor ^= 1;
	}

}