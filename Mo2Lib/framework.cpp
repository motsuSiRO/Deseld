#include "framework.h"
#include "Scene.h"
#include "Mo2GUI.h"
#include "Blender.h"


#include "Input.h"



FrameWork* FrameWork::ins = nullptr;

bool FrameWork::Initialize()
{
	HRESULT hr = S_OK;

	// 画面のサイズを取得する。
	RECT rc;
	GetClientRect(hwnd, &rc);
	screen_width = rc.right - rc.left;
	screen_height = rc.bottom - rc.top;

	

	hr = CreateDevice(current_windowed);

	Blender->Init(DX11device.Get());

	SceneMgr.ChangeScene(new SceneTitle);

	return true;
}

int FrameWork::Run()
{
	MSG msg = {};

	if (!Initialize()) return 0;

	{
		// Setup Dear ImGui DX11context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls



		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.6f;
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(DX11device.Get(), DX11context.Get());
	}
	//Set up Input
	{
		INPUT.InitDInput(histance, hwnd);
		INPUT.InitDInputMouse(hwnd);
		INPUT.InitDInputKeyboard(hwnd);
	}


	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.tick();
			calculate_frame_stats();
			INPUT.KeyStateUpdate();
			INPUT.MouseStateUpdate();
			delta_time = timer.time_interval();
			Update(delta_time);
			Render(delta_time);
		}
	}
	Finalize();
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK FrameWork::handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return true;


	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY:
		INPUT.ReleaseDInputMouse();	// DirectInput(Mouse)オブジェクトの開放
		INPUT.ReleaseDInput();		// DirectInputオブジェクトの開放

		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KILLFOCUS:
		INPUT.ReleaseDInputMouse();	// DirectInput(Mouse)オブジェクトの開放
		INPUT.ReleaseDInputKeyboard();
		INPUT.ReleaseDInput();		// DirectInputオブジェクトの開放
		break;
	case WM_SETFOCUS:

		INPUT.InitDInput(histance, hwnd);
		INPUT.InitDInputMouse(hwnd);
		INPUT.InitDInputKeyboard(hwnd);

		break;

	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.start();
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}


void FrameWork::Finalize()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	INPUT.ReleaseDInputMouse();	// DirectInput(Mouse)オブジェクトの開放
	INPUT.ReleaseDInputKeyboard();
	INPUT.ReleaseDInput();		// DirectInputオブジェクトの開放
}

void FrameWork::Update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	SceneMgr.Update(elapsed_time);
}

void FrameWork::Render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr = S_OK;

	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA(0.0～1.0)
	// レンダーターゲットビューを通してバックバッファテクスチャの色を塗りつぶす。
	DX11context->ClearRenderTargetView(render_target_view.Get(), color);
	// 深度ステンシルビューを通して深度ステンシルテクスチャの深度情報とステンシル情報をクリアする。
	DX11context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// これから描画する情報の書き込む先を設定する。
	DX11context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
	

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	SceneMgr.Render();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// バックバッファに描画した画を画面に表示する。
	swap_chain->Present(0, 0);
}

HRESULT FrameWork::CreateDevice(BOOL windowed)
{
	HRESULT hr = S_OK;

	// デバイス＆スワップチェーンの生成
	{
		UINT create_device_flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL feature_levels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};
		

		// スワップチェーンを作成するための設定オプション
		DXGI_SWAP_CHAIN_DESC swap_chain_desc;
		{
			swap_chain_desc.BufferDesc.Width = screen_width;
			swap_chain_desc.BufferDesc.Height = screen_height;
			swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
			swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
			swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 1ピクセルあたりの各色(RGBA)を8bit(0～255)のテクスチャ(バックバッファ)を作成する。
			swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			swap_chain_desc.SampleDesc.Count = 1;
			swap_chain_desc.SampleDesc.Quality = 0;
			swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_desc.BufferCount = 1;		// バックバッファの数
			swap_chain_desc.OutputWindow = hwnd;	// DirectXで描いた画を表示するウインドウ
			swap_chain_desc.Windowed = TRUE;		// ウインドウモードか、フルスクリーンにするか。
			swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swap_chain_desc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		}
		
		D3D_FEATURE_LEVEL feature_level;

		// デバイス＆スワップチェーンの生成
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,						// どのビデオアダプタを使用するか？既定ならばnullptrで、IDXGIAdapterのアドレスを渡す。
			D3D_DRIVER_TYPE_HARDWARE,		// ドライバのタイプを渡す。D3D_DRIVER_TYPE_HARDWARE 以外は基本的にソフトウェア実装で、特別なことをする場合に用いる。
			nullptr,						// 上記をD3D_DRIVER_TYPE_SOFTWAREに設定した際に、その処理を行うDLLのハンドルを渡す。それ以外を指定している際には必ずnullptrを渡す。
			create_device_flags,			// 何らかのフラグを指定する。詳しくはD3D11_CREATE_DEVICE列挙型で検索。
			feature_levels,					// D3D_FEATURE_LEVEL列挙型の配列を与える。nullptrにすることでも上記featureと同等の内容の配列が使用される。
			ARRAYSIZE(feature_levels),		// feature_levels配列の要素数を渡す。
			D3D11_SDK_VERSION,				// SDKのバージョン。必ずこの値。
			&swap_chain_desc,				// ここで設定した構造体に設定されているパラメータでSwapChainが作成される。
			swap_chain.GetAddressOf(),		// 作成が成功した場合に、SwapChainのアドレスを格納するポインタ変数へのアドレス。ここで指定したポインタ変数経由でSwapChainを操作する。
			DX11device.GetAddressOf(),			// 作成が成功した場合に、Deviceのアドレスを格納するポインタ変数へのアドレス。ここで指定したポインタ変数経由でDeviceを操作する。
			&feature_level,					// 作成に成功したD3D_FEATURE_LEVELを格納するためのD3D_FEATURE_LEVEL列挙型変数のアドレスを設定する。
			DX11context.GetAddressOf()// 作成が成功した場合に、Contextのアドレスを格納するポインタ変数へのアドレス。ここで指定したポインタ変数経由でContextを操作する。
			);

		
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	hr = CreateRendertargetView();

	

	return hr;
}

void FrameWork::CleanupDevice()
{
	CleanupRendertargetView();
	swap_chain.Reset();
	DX11device.Reset();
	DX11context.Reset();
}

HRESULT FrameWork::CreateRendertargetView()
{
	HRESULT hr = S_OK;

	// レンダーターゲットビューの生成
	{
		// スワップチェーンからバックバッファテクスチャを取得する。
		// ※スワップチェーンに内包されているバックバッファテクスチャは'色'を書き込むテクスチャ。
		Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
		hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// バックバッファテクスチャへの書き込みの窓口となるレンダーターゲットビューを生成する。
		hr = DX11device->CreateRenderTargetView(back_buffer.Get(), nullptr, render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	// 深度ステンシルビューの生成
	{
		// 深度ステンシル情報を書き込むためのテクスチャを作成する。
		D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc;
		depth_stencil_buffer_desc.Width = screen_width;
		depth_stencil_buffer_desc.Height = screen_height;
		depth_stencil_buffer_desc.MipLevels = 1;
		depth_stencil_buffer_desc.ArraySize = 1;
		depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 1ピクセルあたり、深度情報を24Bit / ステンシル情報を8bitのテクスチャを作成する。
		depth_stencil_buffer_desc.SampleDesc.Count = 1;
		depth_stencil_buffer_desc.SampleDesc.Quality = 0;
		depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// 深度ステンシル用のテクスチャを作成する。
		depth_stencil_buffer_desc.CPUAccessFlags = 0;
		depth_stencil_buffer_desc.MiscFlags = 0;
		hr = DX11device->CreateTexture2D(&depth_stencil_buffer_desc, nullptr, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 深度ステンシルテクスチャへの書き込みに窓口になる深度ステンシルビューを作成する。
		hr = DX11device->CreateDepthStencilView(depth_stencil_buffer.Get(), nullptr, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// ビューポートの設定
	{
		// 画面のどの領域にDirectXで描いた画を表示するかの設定。
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(screen_width);
		viewport.Height = static_cast<float>(screen_height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		DX11context->RSSetViewports(1, &viewport);
	}

	//XAudio2Create(xa2.GetAddressOf());

	//xa2->CreateMasteringVoice(xa2_master.GetAddressOf());


	return hr;
}

void FrameWork::CleanupRendertargetView()
{
	render_target_view.Reset();
	depth_stencil_view.Reset();
}

void FrameWork::calculate_frame_stats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((timer.time_stamp() - time_tlapsed) >= 1.0f)
	{
		Mo2Gui()->FPS = static_cast<float>(frames); // fps = frameCnt / 1
		Mo2Gui()->msdt = 1000.0f / Mo2Gui()->FPS;
		//std::ostringstream outs;
		//outs.precision(6);
		//outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		//SetWindowTextA(hwnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}


void FrameWork::ImGuiInitialize()
{

}