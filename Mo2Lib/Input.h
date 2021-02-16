#pragma once

//-------------------------------------------------------------------------------------------
//								DirectInput
//-------------------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION		0x0800		// DirectInput�̃o�[�W�������
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
	LPDIRECTINPUT8       g_pDInput = NULL;	// DirectInput�I�u�W�F�N�g

											// �}�E�X�p
	LPDIRECTINPUTDEVICE8 g_pDIMouse = NULL;	// �}�E�X�f�o�C�X
	DIMOUSESTATE g_zdiMouseState;			// �}�E�X���
	LPDIRECTINPUTDEVICE8 g_pDIKeyboard = NULL;	// �L�[�{�[�h�f�o�C�X

	BOOL g_bAppActive = FALSE;			// �A�v���P�[�V�����A�N�e�B�u�t���O

	XINPUT_STATE xpad[XUSER_MAX_COUNT];


	Xpad xinput[XUSER_MAX_COUNT] = {};
	int keys[259] = {};// 256 keys + 3 mouse
	Mouse mouse = {};
	//float x = xpad1.Gamepad.sThumbLX / 32767;
	//float y = xpad1.Gamepad.sThumbLY / 32767;

	//-----------------------------------------------------------------------------
	//
	// DirectInput�p�֐�
	//
	//-----------------------------------------------------------------------------
	bool InitDInput(HINSTANCE hInstApp, HWND hWnd);	// DirectInput�̏���������
	bool ReleaseDInput(); // DirectInput�̏I������

	//-----------------------------------------------------------------------------
	//
	// DirectInput(Mouse)�p�֐�
	//
	//-----------------------------------------------------------------------------
	bool InitDInputMouse(HWND hWnd);// DirectInput�̃}�E�X�f�o�C�X�p�̏���������
	bool ReleaseDInputMouse();// DirectInput�̃}�E�X�f�o�C�X�p�̉������
	void GetMouseState(/*HWND hWnd*/);
	void MouseStateUpdate();
	bool ClickPressed(int mouse_input);
	bool ClickReleased(int mouse_input);
	bool ClickKeeping(int mouse_input);
	// DirectInput�̃}�E�X�f�o�C�X��Ԏ擾����


	//-----------------------------------------------------------------------------
	//
	// DirectInput(Keyboard)�p�֐�
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
	// XInput(Xcontrol)�p�֐�
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
