#pragma once

//-------------------------------------------------------------------------------------------
//								DirectInput
//-------------------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION		0x0800		// DirectInputのバージョン情報
#include <dinput.h>
#include <Xinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "Vector3D.h"

enum XINPUT_GAMEPAD_BUTTONS
{
	PAD_UP = 0,
	PAD_DOWN,
	PAD_LEFT,
	PAD_RIGHT,
	PAD_START,
	PAD_BACK,
	PAD_LTHUMB,
	PAD_RTHUMB,
	PAD_LSHOULDER,
	PAD_RSHOULDER,
	PAD_A,
	PAD_B,
	PAD_X,
	PAD_Y,
	GAMEPAD_MAX_BUTTON,
};

enum MOUSE_KEY_INPUT
{
	L_CLICK = 256,
	R_CLICK,
	M_CLICK,
};

enum MOUSE_INPUT
{
	LEFT_CLICK,
	RIGHT_CLICK,
	MID_CLICK,
	MAX_MOUSE_INPUT = 3,
};

enum INPUT_STATUS
{
	RELEASED_INPUT = -1,	// Equal
	NON_INPUT = 0,		// Equal
	PRESSED_INPUT,		// Equal
	HOLD_DOWN = 1,		// Greater than || Equal
};

struct Mouse
{
	int d_x;		//displacement_x
	int d_y;		//displacement_y
	int whole;	//Mouse Whole : UP + / DOWN -

	int click[MAX_MOUSE_INPUT];

};

struct Xpad
{
	int index = 0;
	int buttons[GAMEPAD_MAX_BUTTON] = {};
	int TriggerL = 0;
	int TriggerR = 0;
};


class input
{
private:
	input() {}
	~input() {}

public:


	HINSTANCE Hinstance;

	void Init(HINSTANCE ins)
	{
		Hinstance = ins;
	}
	//-------------------------------------------------------------------------------------------
	//								DirectInput
	//-------------------------------------------------------------------------------------------
	LPDIRECTINPUT8       g_pDInput = NULL;	// DirectInputオブジェクト

											// マウス用
	LPDIRECTINPUTDEVICE8 g_pDIMouse = NULL;	// マウスデバイス
	DIMOUSESTATE g_zdiMouseState;			// マウス状態
	LPDIRECTINPUTDEVICE8 g_pDIKeyboard = NULL;	// キーボードデバイス

	BOOL g_bAppActive = FALSE;			// アプリケーションアクティブフラグ

	XINPUT_STATE xpad[XUSER_MAX_COUNT];


	Xpad xinput[XUSER_MAX_COUNT] = {};
	int keys[259] = {};// 256 keys + 3 mouse
	Mouse mouse = {};
	//float x = xpad1.Gamepad.sThumbLX / 32767;
	//float y = xpad1.Gamepad.sThumbLY / 32767;

	//-----------------------------------------------------------------------------
	//
	// DirectInput用関数
	//
	//-----------------------------------------------------------------------------
	bool InitDInput(HINSTANCE hInstApp, HWND hWnd);	// DirectInputの初期化処理
	bool ReleaseDInput(); // DirectInputの終了処理

	//-----------------------------------------------------------------------------
	//
	// DirectInput(Mouse)用関数
	//
	//-----------------------------------------------------------------------------
	bool InitDInputMouse(HWND hWnd);// DirectInputのマウスデバイス用の初期化処理
	bool ReleaseDInputMouse();// DirectInputのマウスデバイス用の解放処理
	void GetMouseState(/*HWND hWnd*/);
	void MouseStateUpdate();
	bool ClickPressed(int mouse_input);
	bool ClickReleased(int mouse_input);
	bool ClickKeeping(int mouse_input);
	// DirectInputのマウスデバイス状態取得処理


	//-----------------------------------------------------------------------------
	//
	// DirectInput(Keyboard)用関数
	//
	//-----------------------------------------------------------------------------
	bool InitDInputKeyboard(HWND hwnd);
	bool ReleaseDInputKeyboard();
	void GetHitKeyStateAll(BYTE* _key);
	void KeyStateUpdate();
	bool KeyPressed(int mouse_input);
	bool KeyReleased(int mouse_input);
	bool KeyKeeping(int mouse_input);

	//------------------------------------------------------------------------------
	//
	// XInput(Xcontrol)用関数
	//
	//------------------------------------------------------------------------------
	void XpadStateUpdate();
	bool LStickDeadzoneX(int deadzone);
	bool LStickDeadzoneY(int deadzone);
	Mo2Lib::Float2 LStickVector();
	float LStickAngle();//Radian
	int LTrigger();
	bool RStickDeadzoneX(int deadzone);
	bool RStickDeadzoneY(int deadzone);
	Mo2Lib::Float2 RStickVector();
	float RStickAngle();//Radian
	int RTrigger();

	static input& getInstance()
	{
		static input ins;
		return ins;
	}

	void ImGui();
};

//static const input& INPUT()
//{
//	return input::getInstance();
//}
#define INPUT input::getInstance()
