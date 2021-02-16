#include "input.h"

#include <math.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

//input::input(HINSTANCE instance): Hinstance(instance)
//{
//}
//
//
//input::~input()
//{
//}

//-----------------------------------------------------------------------------
//
// DirectInput用関数
//
//-----------------------------------------------------------------------------

bool input::InitDInput(HINSTANCE hInstApp, HWND hWnd)	// DirectInputの初期化処理
{
	HRESULT ret = DirectInput8Create(hInstApp, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDInput, NULL);
	if (FAILED(ret)) {
		return false;	// 作成に失敗
	}
	return true;
}

bool input::ReleaseDInput() // DirectInputの終了処理
{
	// DirectInputのデバイスを開放
	if (g_pDInput) {
		g_pDInput->Release();
		g_pDInput = NULL;
	}

	return true;
}

//-----------------------------------------------------------------------------
//
// DirectInput(Mouse)用関数
//
//-----------------------------------------------------------------------------
bool input::InitDInputMouse(HWND hWnd)// DirectInputのマウスデバイス用の初期化処理
{
	HRESULT ret = S_FALSE;
	if (g_pDInput == NULL) {
		return false;
	}

	// マウス用にデバイスオブジェクトを作成
	ret = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDIMouse, NULL);
	if (FAILED(ret)) {
		// デバイスの作成に失敗
		return false;
	}

	// データフォーマットを設定
	ret = g_pDIMouse->SetDataFormat(&c_dfDIMouse);	// マウス用のデータ・フォーマットを設定
	if (FAILED(ret)) {
		// データフォーマットに失敗
		return false;
	}

	// モードを設定（フォアグラウンド＆非排他モード）
	ret = g_pDIMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(ret)) {
		// モードの設定に失敗
		return false;
	}

	// デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）

	ret = g_pDIMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(ret)) {
		// デバイスの設定に失敗
		return false;
	}

	// 入力制御開始
	g_pDIMouse->Acquire();

	return true;
}

bool input::ReleaseDInputMouse()// DirectInputのマウスデバイス用の解放処理
{
	// DirectInputのデバイスを開放
	if (g_pDIMouse) {
		g_pDIMouse->Release();
		g_pDIMouse = NULL;
	}

	return true;

}

void input::GetMouseState(/*HWND hWnd*/)// DirectInputのマウスデバイス状態取得処理
{
	if (g_pDIMouse == NULL) {
		// オブジェクト生成前に呼ばれたときはここで生成させる
		//InitDInputMouse(hWnd);
		return;
	}

	// 読取前の値を保持します
	DIMOUSESTATE g_zdiMouseState_bak;	// マウス情報(変化検知用)
	memcpy(&g_zdiMouseState_bak, &g_zdiMouseState, sizeof(g_zdiMouseState_bak));

	// ここから、DirectInputで必要なコード -->
	// マウスの状態を取得します
	HRESULT	hr = g_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &g_zdiMouseState);
	if (hr == DIERR_INPUTLOST) {
		g_pDIMouse->Acquire();
		hr = g_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &g_zdiMouseState);
	}
	// --> ここまで、DirectInputで必要なコード

	//if (memcmp(&g_zdiMouseState_bak, &g_zdiMouseState, sizeof(g_zdiMouseState_bak)) != 0)
	//{
	//	// 確認用の処理、ここから -->
	//	// 値が変わったら表示します
	//	char buf[128];
	//	wsprintf(buf, "(%5d, %5d, %5d) %s %s %s\n",
	//		g_zdiMouseState.lX, g_zdiMouseState.lY, g_zdiMouseState.lZ,
	//		(g_zdiMouseState.rgbButtons[0] & 0x80) ? "Left" : "--",
	//		(g_zdiMouseState.rgbButtons[1] & 0x80) ? "Right" : "--",
	//		(g_zdiMouseState.rgbButtons[2] & 0x80) ? "Center" : "--");
	//	OutputDebugString(buf);
	//	// --> ここまで、確認用の処理
	//}
}


void input::MouseStateUpdate()
{
	GetMouseState();


	mouse.d_x = (int)g_zdiMouseState.lX;
	mouse.d_y = (int)g_zdiMouseState.lY;
	mouse.whole = (int)g_zdiMouseState.lZ;

	
	if (g_zdiMouseState.rgbButtons[LEFT_CLICK] > 0)
	{
		if (mouse.click[LEFT_CLICK] == 0)
		{
			keys[256 + LEFT_CLICK] = 1;
			mouse.click[LEFT_CLICK] = 1;
		}
		else if (mouse.click[LEFT_CLICK] >= 1)
		{
			mouse.click[LEFT_CLICK]++;
			keys[256 + LEFT_CLICK]++;
		}
	}
	else
	{
		if (mouse.click[LEFT_CLICK] > 0)
		{
			mouse.click[LEFT_CLICK] = -1;
			keys[256 + LEFT_CLICK] = -1;
		}
		else
		{
			mouse.click[LEFT_CLICK] = 0;
			keys[256 + LEFT_CLICK] = 0;
		}
	}


	if (g_zdiMouseState.rgbButtons[RIGHT_CLICK] > 0)
	{
		if (mouse.click[RIGHT_CLICK] == 0)
		{
			mouse.click[RIGHT_CLICK] = 1;
			keys[256 + RIGHT_CLICK] = 1;
		}
		else if (mouse.click[RIGHT_CLICK] >= 1)
		{
			mouse.click[RIGHT_CLICK]++;
			keys[256 + RIGHT_CLICK]++;
		}
	}
	else
	{
		if (mouse.click[RIGHT_CLICK] > 0)
		{
			mouse.click[RIGHT_CLICK] = -1;
			keys[256 + RIGHT_CLICK] = -1;
		}
		else
		{
			mouse.click[RIGHT_CLICK] = 0;
			keys[256 + RIGHT_CLICK] = 0;
		}
	}


	if (g_zdiMouseState.rgbButtons[MID_CLICK] > 0)
	{
		if (mouse.click[MID_CLICK] == 0)
		{
			mouse.click[MID_CLICK] = 1;
			keys[256 + MID_CLICK] = 1;
		}
		else if (mouse.click[MID_CLICK] >= 1)
		{
			mouse.click[MID_CLICK]++;
			keys[256 + MID_CLICK]++;
		}
	}
	else
	{
		if (mouse.click[MID_CLICK] > 0)
		{
			mouse.click[MID_CLICK] = -1;
			keys[256 + MID_CLICK] = -1;
		}
		else
		{
			mouse.click[MID_CLICK] = 0;
			keys[256 + MID_CLICK] = 0;
		}
	}

}

bool input::ClickPressed(int m)
{
	if (mouse.click[m] == PRESSED_INPUT)
	{
		return true;
	}
	return false;
}

bool input::ClickReleased(int m)
{
	if (mouse.click[m] == RELEASED_INPUT)
	{
		return true;
	}
	return false;
}

bool input::ClickKeeping(int m)
{
	if (mouse.click[m] >= HOLD_DOWN)
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------
//
// DirectInput(Keyboard)用関数
//
//-----------------------------------------------------------------------------
bool input::InitDInputKeyboard(HWND hwnd)
{
	HRESULT ret = S_FALSE;
	if (g_pDInput == NULL)
	{
		return false;
	}

	// IDirectInputDevice8の取得
	ret = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIKeyboard, NULL);
	if (FAILED(ret))
	{
		return false;

	}
	// 入力データ形式のセット
	ret = g_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(ret))
	{
		return false;
	}

	// 排他制御のセット
	ret = g_pDIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(ret))
	{
		return false;
	}

	// 動作開始
	g_pDIKeyboard->Acquire();

	return true;
}

bool input::ReleaseDInputKeyboard()
{
	// DirectInputのデバイスを開放
	if (g_pDIKeyboard) {
		g_pDIKeyboard->Release();
		g_pDIKeyboard = NULL;
	}

	return true;
}

void input::GetHitKeyStateAll(BYTE* _key)
{
	if (!g_pDIKeyboard)return;
	HRESULT ret;
	// キーの入力
	//BYTE key[256];
	ZeroMemory(_key, sizeof(_key));
	ret = g_pDIKeyboard->GetDeviceState(sizeof(BYTE) * 256, _key);
	if (FAILED(ret))
	{
		// 失敗なら再開させてもう一度取得
		g_pDIKeyboard->Acquire();
		g_pDIKeyboard->GetDeviceState(sizeof(BYTE) * 256, _key);
	}
}

void input::KeyStateUpdate()
{
	static BYTE buf[256];

	GetHitKeyStateAll(buf);
	for (int i = 0; i < 256; i++)
	{
		if (buf[i])
		{
			if (keys[i] == 0)
			{
				keys[i] = 1;
			}
			else if (keys[i] >= 1)
			{
				keys[i]++;
			}
		}
		else
		{
			if (keys[i] > 0)
			{
				keys[i] = -1;
			}
			else
			{
				keys[i] = 0;
			}
		}
	}

	for (int i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		//XInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &xpad[i]);
		XInputGetState(i, &xpad[i]);
	}

	XpadStateUpdate();

}


bool input::KeyPressed(int i)
{
	if (i > 0 && keys[i] == PRESSED_INPUT)
	{
		return true;
	}
	return false;
}

bool input::KeyReleased(int i)
{
	if (i > 0 && keys[i] == RELEASED_INPUT)
	{
		return true;
	}
	return false;
}

bool input::KeyKeeping(int i)
{
	if (i > 0 && keys[i] >= HOLD_DOWN)
	{
		return true;
	}
	return false;
}



void input::XpadStateUpdate()
{

	
	for (int i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		xinput[i].index = i;
		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
		{
			if (xinput[i].buttons[PAD_UP] == 0)
			{
				xinput[i].buttons[PAD_UP] = 1;
			}
			else if (xinput[i].buttons[PAD_UP] >= 1)
			{
				xinput[i].buttons[PAD_UP]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_UP] > 0)
			{
				xinput[i].buttons[PAD_UP] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_UP] = 0;
			}
		}

		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
		{
			if (xinput[i].buttons[PAD_DOWN] == 0)
			{
				xinput[i].buttons[PAD_DOWN] = 1;
			}
			else if (xinput[i].buttons[PAD_DOWN] >= 1)
			{
				xinput[i].buttons[PAD_DOWN]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_DOWN] > 0)
			{
				xinput[i].buttons[PAD_DOWN] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_DOWN] = 0;
			}
		}

		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			if (xinput[i].buttons[PAD_LEFT] == 0)
			{
				xinput[i].buttons[PAD_LEFT] = 1;
			}
			else if (xinput[i].buttons[PAD_LEFT] >= 1)
			{
				xinput[i].buttons[PAD_LEFT]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_LEFT] > 0)
			{
				xinput[i].buttons[PAD_LEFT] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_LEFT] = 0;
			}
		}

		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
		{
			if (xinput[i].buttons[PAD_RIGHT] == 0)
			{
				xinput[i].buttons[PAD_RIGHT] = 1;
			}
			else if (xinput[i].buttons[PAD_RIGHT] >= 1)
			{
				xinput[i].buttons[PAD_RIGHT]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_RIGHT] > 0)
			{
				xinput[i].buttons[PAD_RIGHT] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_RIGHT] = 0;
			}
		}

		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_START)
		{
			if (xinput[i].buttons[PAD_START] == 0)
			{
				xinput[i].buttons[PAD_START] = 1;
			}
			else if (xinput[i].buttons[PAD_START] >= 1)
			{
				xinput[i].buttons[PAD_START]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_START] > 0)
			{
				xinput[i].buttons[PAD_START] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_START] = 0;
			}
		}

		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
		{
			if (xinput[i].buttons[PAD_BACK] == 0)
			{
				xinput[i].buttons[PAD_BACK] = 1;
			}
			else if (xinput[i].buttons[PAD_BACK] >= 1)
			{
				xinput[i].buttons[PAD_BACK]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_BACK] > 0)
			{
				xinput[i].buttons[PAD_BACK] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_BACK] = 0;
			}
		}


		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
		{
			if (xinput[i].buttons[PAD_LTHUMB] == 0)
			{
				xinput[i].buttons[PAD_LTHUMB] = 1;
			}
			else if (xinput[i].buttons[PAD_LTHUMB] >= 1)
			{
				xinput[i].buttons[PAD_LTHUMB]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_LTHUMB] > 0)
			{
				xinput[i].buttons[PAD_LTHUMB] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_LTHUMB] = 0;
			}
		}


		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
		{
			if (xinput[i].buttons[PAD_RTHUMB] == 0)
			{
				xinput[i].buttons[PAD_RTHUMB] = 1;
			}
			else if (xinput[i].buttons[PAD_RTHUMB] >= 1)
			{
				xinput[i].buttons[PAD_RTHUMB]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_RTHUMB] > 0)
			{
				xinput[i].buttons[PAD_RTHUMB] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_RTHUMB] = 0;
			}
		}

		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
		{
			if (xinput[i].buttons[PAD_LSHOULDER] == 0)
			{
				xinput[i].buttons[PAD_LSHOULDER] = 1;
			}
			else if (xinput[i].buttons[PAD_LSHOULDER] >= 1)
			{
				xinput[i].buttons[PAD_LSHOULDER]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_LSHOULDER] > 0)
			{
				xinput[i].buttons[PAD_LSHOULDER] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_LSHOULDER] = 0;
			}
		}


		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
		{
			if (xinput[i].buttons[PAD_RSHOULDER] == 0)
			{
				xinput[i].buttons[PAD_RSHOULDER] = 1;
			}
			else if (xinput[i].buttons[PAD_RSHOULDER] >= 1)
			{
				xinput[i].buttons[PAD_RSHOULDER]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_RSHOULDER] > 0)
			{
				xinput[i].buttons[PAD_RSHOULDER] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_RSHOULDER] = 0;
			}
		}


		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_A)
		{
			if (xinput[i].buttons[PAD_A] == 0)
			{
				xinput[i].buttons[PAD_A] = 1;
			}
			else if (xinput[i].buttons[PAD_A] >= 1)
			{
				xinput[i].buttons[PAD_A]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_A] > 0)
			{
				xinput[i].buttons[PAD_A] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_A] = 0;
			}
		}


		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_B)
		{
			if (xinput[i].buttons[PAD_B] == 0)
			{
				xinput[i].buttons[PAD_B] = 1;
			}
			else if (xinput[i].buttons[PAD_B] >= 1)
			{
				xinput[i].buttons[PAD_B]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_B] > 0)
			{
				xinput[i].buttons[PAD_B] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_B] = 0;
			}
		}


		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_X)
		{
			if (xinput[i].buttons[PAD_X] == 0)
			{
				xinput[i].buttons[PAD_X] = 1;
			}
			else if (xinput[i].buttons[PAD_X] >= 1)
			{
				xinput[i].buttons[PAD_X]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_X] > 0)
			{
				xinput[i].buttons[PAD_X] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_X] = 0;
			}
		}


		if (xpad[i].Gamepad.wButtons & XINPUT_GAMEPAD_Y)
		{
			if (xinput[i].buttons[PAD_Y] == 0)
			{
				xinput[i].buttons[PAD_Y] = 1;
			}
			else if (xinput[i].buttons[PAD_Y] >= 1)
			{
				xinput[i].buttons[PAD_Y]++;
			}
		}
		else
		{
			if (xinput[i].buttons[PAD_Y] > 0)
			{
				xinput[i].buttons[PAD_Y] = -1;
			}
			else
			{
				xinput[i].buttons[PAD_Y] = 0;
			}
		}

		xinput[i].TriggerL = xpad[i].Gamepad.bLeftTrigger;
		xinput[i].TriggerR = xpad[i].Gamepad.bRightTrigger;

	}
}

bool input::LStickDeadzoneX(int dead_zone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
{
	return xpad[0].Gamepad.sThumbLX > dead_zone || xpad[0].Gamepad.sThumbLX < -dead_zone ? true : false;
}

bool input::LStickDeadzoneY(int dead_zone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
{
	return xpad[0].Gamepad.sThumbLY > dead_zone || xpad[0].Gamepad.sThumbLY < -dead_zone ? true : false;
}

Mo2Lib::Float2 input::LStickVector()
{
	Mo2Lib::Float2 vec2 = {};

	vec2.x = static_cast<float>(xpad[0].Gamepad.sThumbLX) / 32767;
	vec2.y = static_cast<float>(xpad[0].Gamepad.sThumbLY) / 32767;

	vec2.Normalize();

	return vec2;
}

float input::LStickAngle()
{
	float angle;
	Mo2Lib::Float2 zero = Mo2Lib::Float2(0, 1.f);
	angle = acosf(LStickVector().Dot(zero));

	if (LStickVector().Cross(zero) < 0.f)
	{
		angle = DirectX::XM_2PI - angle;
	}

	return angle;
}

int input::LTrigger()
{
	return xinput->TriggerL;
}

bool input::RStickDeadzoneX(int dead_zone)
{
	return xpad[0].Gamepad.sThumbRX > dead_zone || xpad[0].Gamepad.sThumbRX < -dead_zone ? true : false;
}

bool input::RStickDeadzoneY(int dead_zone)
{
	return xpad[0].Gamepad.sThumbRY > dead_zone || xpad[0].Gamepad.sThumbRY < -dead_zone ? true : false;
}

Mo2Lib::Float2 input::RStickVector()
{
	Mo2Lib::Float2 vec2 = {};

	vec2.x = static_cast<float>(xpad[0].Gamepad.sThumbRX) / 32767;
	vec2.y = static_cast<float>(xpad[0].Gamepad.sThumbRY) / 32767;

	float len = sqrtf(vec2.x * vec2.x + vec2.y * vec2.y);
	float mag = 1 / len;
	vec2.x *= mag;
	vec2.y *= mag;

	return vec2;
}

float input::RStickAngle()
{
	float angle;
	angle = acosf(RStickVector().y / RStickVector().Length());

	return angle;
}

int input::RTrigger()
{
	return xinput->TriggerR;
}


void input::ImGui()
{

	ImGui::Begin("User_Input");

	static int i = 0;
	ImGui::InputInt("Index", &i);
	if (i < 0)i = 0;
	else if (i >= XUSER_MAX_COUNT)i = XUSER_MAX_COUNT - 1;

	ImGui::Text("%d", INPUT.xpad[i].Gamepad.sThumbLX);
	ImGui::Text("%d", INPUT.xpad[i].Gamepad.sThumbLY);
	ImGui::Text("vec_x : %.2f", INPUT.LStickVector().x);
	ImGui::Text("vec_y : %.2f", INPUT.LStickVector().y);
	ImGui::Text("angle : %.2f", INPUT.LStickAngle());

	ImGui::Text("%d", INPUT.xpad[i].Gamepad.sThumbRX);
	ImGui::Text("%d", INPUT.xpad[i].Gamepad.sThumbRY);
	ImGui::Text("vec_x : %.2f", INPUT.RStickVector().x);
	ImGui::Text("vec_y : %.2f", INPUT.RStickVector().y);
	ImGui::Text("angle : %.2f", INPUT.RStickAngle());

	
	for (int i = 0; i < GAMEPAD_MAX_BUTTON; i++)
	{
		ImGui::Text("%d", INPUT.xinput[i].buttons[i]);
	}


	ImGui::Text("%d", INPUT.mouse.d_x);
	ImGui::Text("%d", INPUT.mouse.d_y);

	//for (int i = 0; i < MAX_MOUSE_INPUT; i++)
	//{
	//	
	//}

	ImGui::End();
}