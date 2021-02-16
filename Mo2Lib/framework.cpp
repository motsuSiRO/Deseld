#include "framework.h"
#include "Scene.h"
#include "Mo2GUI.h"
#include "Blender.h"


#include "Input.h"



FrameWork* FrameWork::ins = nullptr;

bool FrameWork::Initialize()
{
	HRESULT hr = S_OK;

	// ��ʂ̃T�C�Y���擾����B
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
		INPUT.ReleaseDInputMouse();	// DirectInput(Mouse)�I�u�W�F�N�g�̊J��
		INPUT.ReleaseDInput();		// DirectInput�I�u�W�F�N�g�̊J��

		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KILLFOCUS:
		INPUT.ReleaseDInputMouse();	// DirectInput(Mouse)�I�u�W�F�N�g�̊J��
		INPUT.ReleaseDInputKeyboard();
		INPUT.ReleaseDInput();		// DirectInput�I�u�W�F�N�g�̊J��
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


	INPUT.ReleaseDInputMouse();	// DirectInput(Mouse)�I�u�W�F�N�g�̊J��
	INPUT.ReleaseDInputKeyboard();
	INPUT.ReleaseDInput();		// DirectInput�I�u�W�F�N�g�̊J��
}

void FrameWork::Update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	SceneMgr.Update(elapsed_time);
}

void FrameWork::Render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr = S_OK;

	FLOAT color[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA(0.0�`1.0)
	// �����_�[�^�[�Q�b�g�r���[��ʂ��ăo�b�N�o�b�t�@�e�N�X�`���̐F��h��Ԃ��B
	DX11context->ClearRenderTargetView(render_target_view.Get(), color);
	// �[�x�X�e���V���r���[��ʂ��Đ[�x�X�e���V���e�N�X�`���̐[�x���ƃX�e���V�������N���A����B
	DX11context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// ���ꂩ��`�悷����̏������ސ��ݒ肷��B
	DX11context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
	

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	SceneMgr.Render();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// �o�b�N�o�b�t�@�ɕ`�悵�������ʂɕ\������B
	swap_chain->Present(0, 0);
}

HRESULT FrameWork::CreateDevice(BOOL windowed)
{
	HRESULT hr = S_OK;

	// �f�o�C�X���X���b�v�`�F�[���̐���
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
		

		// �X���b�v�`�F�[�����쐬���邽�߂̐ݒ�I�v�V����
		DXGI_SWAP_CHAIN_DESC swap_chain_desc;
		{
			swap_chain_desc.BufferDesc.Width = screen_width;
			swap_chain_desc.BufferDesc.Height = screen_height;
			swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
			swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
			swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 1�s�N�Z��������̊e�F(RGBA)��8bit(0�`255)�̃e�N�X�`��(�o�b�N�o�b�t�@)���쐬����B
			swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			swap_chain_desc.SampleDesc.Count = 1;
			swap_chain_desc.SampleDesc.Quality = 0;
			swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_desc.BufferCount = 1;		// �o�b�N�o�b�t�@�̐�
			swap_chain_desc.OutputWindow = hwnd;	// DirectX�ŕ`�������\������E�C���h�E
			swap_chain_desc.Windowed = TRUE;		// �E�C���h�E���[�h���A�t���X�N���[���ɂ��邩�B
			swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swap_chain_desc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		}
		
		D3D_FEATURE_LEVEL feature_level;

		// �f�o�C�X���X���b�v�`�F�[���̐���
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,						// �ǂ̃r�f�I�A�_�v�^���g�p���邩�H����Ȃ��nullptr�ŁAIDXGIAdapter�̃A�h���X��n���B
			D3D_DRIVER_TYPE_HARDWARE,		// �h���C�o�̃^�C�v��n���BD3D_DRIVER_TYPE_HARDWARE �ȊO�͊�{�I�Ƀ\�t�g�E�F�A�����ŁA���ʂȂ��Ƃ�����ꍇ�ɗp����B
			nullptr,						// ��L��D3D_DRIVER_TYPE_SOFTWARE�ɐݒ肵���ۂɁA���̏������s��DLL�̃n���h����n���B����ȊO���w�肵�Ă���ۂɂ͕K��nullptr��n���B
			create_device_flags,			// ���炩�̃t���O���w�肷��B�ڂ�����D3D11_CREATE_DEVICE�񋓌^�Ō����B
			feature_levels,					// D3D_FEATURE_LEVEL�񋓌^�̔z���^����Bnullptr�ɂ��邱�Ƃł���Lfeature�Ɠ����̓��e�̔z�񂪎g�p�����B
			ARRAYSIZE(feature_levels),		// feature_levels�z��̗v�f����n���B
			D3D11_SDK_VERSION,				// SDK�̃o�[�W�����B�K�����̒l�B
			&swap_chain_desc,				// �����Őݒ肵���\���̂ɐݒ肳��Ă���p�����[�^��SwapChain���쐬�����B
			swap_chain.GetAddressOf(),		// �쐬�����������ꍇ�ɁASwapChain�̃A�h���X���i�[����|�C���^�ϐ��ւ̃A�h���X�B�����Ŏw�肵���|�C���^�ϐ��o�R��SwapChain�𑀍삷��B
			DX11device.GetAddressOf(),			// �쐬�����������ꍇ�ɁADevice�̃A�h���X���i�[����|�C���^�ϐ��ւ̃A�h���X�B�����Ŏw�肵���|�C���^�ϐ��o�R��Device�𑀍삷��B
			&feature_level,					// �쐬�ɐ�������D3D_FEATURE_LEVEL���i�[���邽�߂�D3D_FEATURE_LEVEL�񋓌^�ϐ��̃A�h���X��ݒ肷��B
			DX11context.GetAddressOf()// �쐬�����������ꍇ�ɁAContext�̃A�h���X���i�[����|�C���^�ϐ��ւ̃A�h���X�B�����Ŏw�肵���|�C���^�ϐ��o�R��Context�𑀍삷��B
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

	// �����_�[�^�[�Q�b�g�r���[�̐���
	{
		// �X���b�v�`�F�[������o�b�N�o�b�t�@�e�N�X�`�����擾����B
		// ���X���b�v�`�F�[���ɓ����Ă���o�b�N�o�b�t�@�e�N�X�`����'�F'���������ރe�N�X�`���B
		Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
		hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// �o�b�N�o�b�t�@�e�N�X�`���ւ̏������݂̑����ƂȂ郌���_�[�^�[�Q�b�g�r���[�𐶐�����B
		hr = DX11device->CreateRenderTargetView(back_buffer.Get(), nullptr, render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	// �[�x�X�e���V���r���[�̐���
	{
		// �[�x�X�e���V�������������ނ��߂̃e�N�X�`�����쐬����B
		D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc;
		depth_stencil_buffer_desc.Width = screen_width;
		depth_stencil_buffer_desc.Height = screen_height;
		depth_stencil_buffer_desc.MipLevels = 1;
		depth_stencil_buffer_desc.ArraySize = 1;
		depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 1�s�N�Z��������A�[�x����24Bit / �X�e���V������8bit�̃e�N�X�`�����쐬����B
		depth_stencil_buffer_desc.SampleDesc.Count = 1;
		depth_stencil_buffer_desc.SampleDesc.Quality = 0;
		depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;		// �[�x�X�e���V���p�̃e�N�X�`�����쐬����B
		depth_stencil_buffer_desc.CPUAccessFlags = 0;
		depth_stencil_buffer_desc.MiscFlags = 0;
		hr = DX11device->CreateTexture2D(&depth_stencil_buffer_desc, nullptr, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// �[�x�X�e���V���e�N�X�`���ւ̏������݂ɑ����ɂȂ�[�x�X�e���V���r���[���쐬����B
		hr = DX11device->CreateDepthStencilView(depth_stencil_buffer.Get(), nullptr, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// �r���[�|�[�g�̐ݒ�
	{
		// ��ʂ̂ǂ̗̈��DirectX�ŕ`�������\�����邩�̐ݒ�B
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