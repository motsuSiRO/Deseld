#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>

#include "misc.h"
#include "high_resolution_timer.h"

#include <d3d11.h>
#include <wrl.h>

#include <memory>

#define DIRECTINPUT_VERSION 0x0800
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



class FrameWork
{
public:
	static FrameWork* ins;

	
	HWND hwnd;//‹¤—LƒNƒ‰ƒX‰»ŒŸ“¢
	HINSTANCE histance;
	UINT screen_width;//
	UINT screen_height;//
	BOOL current_windowed = FALSE;//
	float delta_time;

	Microsoft::WRL::ComPtr<ID3D11Device>			DX11device;//
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		DX11context;//
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swap_chain;//
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	render_target_view;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depth_stencil_buffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depth_stencil_view;
	//Microsoft::WRL::ComPtr<IXAudio2>				xa2;
	//Microsoft::WRL::ComPtr<IXAudio2MasteringVoice>	xa2_master;
	FrameWork(HWND hwnd, HINSTANCE hins) : hwnd(hwnd), histance(hins)
	{
		ins = this;
	}
	~FrameWork()
	{

	}

	int Run();

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	bool Initialize();
	void Finalize();
	void Update(float elapsed_time/*Elapsed seconds from last frame*/);
	void Render(float elapsed_time/*Elapsed seconds from last frame*/);
	void ImGuiInitialize();
	HRESULT CreateDevice(BOOL windowed);
	void CleanupDevice();
	HRESULT CreateRendertargetView();
	void CleanupRendertargetView();
	HRESULT CreateDepthStencilView();
	void CleanupDepthStencilView();

public:
	static FrameWork* GetInstance()
	{
		return ins;
	}

private:
	high_resolution_timer timer;
	void calculate_frame_stats();
};

#define Mo2System FrameWork::GetInstance()