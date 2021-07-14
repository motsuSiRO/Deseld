#include "GameOption.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <string>
#include "FrameWork.h"

GameOption::GameOption()
{
}


GameOption::~GameOption()
{
}


void GameOption::ImGui()
{

	ImGui::Begin("GameOption");

	if (ImGui::BeginTabBar("DetailTab##GameOption"))
	{
		if (ImGui::BeginTabItem("Video##GameOption"))
		{
			const char* win_mode_name[] = { "FullScreen", "Borderless", "Windowed" };
			ImGui::Combo("Windows_Mode##GameOption", &windows_mode, win_mode_name, IM_ARRAYSIZE(win_mode_name));

			ImGui::NewLine();

			if (ImGui::Button("Apply"))
			{
				LONG style;

				switch (windows_mode)
				{
				case 0:
					Mo2System->swap_chain->SetFullscreenState(TRUE, nullptr);
					break;
				case 1:
					Mo2System->swap_chain->SetFullscreenState(FALSE, nullptr);
					ChangeDisplaySettings(NULL, 0);
					SetWindowLongPtr(Mo2System->hwnd, GWL_STYLE, WS_POPUPWINDOW);
					SetWindowPos(Mo2System->hwnd, HWND_TOPMOST, 0, 0, Mo2System->screen_width, Mo2System->screen_height, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
					break;
				case 2:
					Mo2System->swap_chain->SetFullscreenState(FALSE, nullptr);
					ChangeDisplaySettings(NULL, 0);
					SetWindowLongPtr(Mo2System->hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME);
					SetWindowPos(Mo2System->hwnd, HWND_TOPMOST, 0, 0, Mo2System->screen_width, Mo2System->screen_height, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
					break;
				default:
					break;
				}

				ShowWindow(Mo2System->hwnd, SW_SHOW);

			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}



	ImGui::End();
}