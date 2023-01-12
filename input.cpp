//==========================================================================================
//
//入力プログラム[input.cpp]
//Author:石原颯馬
//
//==========================================================================================
//Ver.2.2.1（プロジェクト制作中のもの）
//使用可能な入力:キーボード・マウス（カーソル位置・移動量）・ゲームパッド（1~4台）
//==========================================================================================
#include "input.h"

//マクロ定義
#define NUM_KEY_MAX			(256)	//キーの最大数
#define MOUSE_BUTTON_MAX	(3)		//マウスボタンの最大数
#define REPEATE_TIME		(150)	//リピートの間隔
#define GAMEPAD_BUTTON_NUM	(14)	//ゲームパッドのボタン数
#define STICK_DEADZONE		(655)	//遊び

//グローバル変数
//キーボード部
LPDIRECTINPUT8 g_pInputKeyboard = NULL;
LPDIRECTINPUTDEVICE8 g_pDevKeyboard = NULL;
BYTE g_aKeyState[NUM_KEY_MAX];				//プレス情報
BYTE g_akeyStateTrigger[NUM_KEY_MAX];		//トリガー情報
BYTE g_akeyStateRelease[NUM_KEY_MAX];		//リリース情報
BYTE g_akeyStateRepeate[NUM_KEY_MAX];		//リピート情報
DWORD g_aKeyboardCurrentTime[NUM_KEY_MAX];			//現在の時間（リピート使用）
DWORD g_aKeyboardExecLastTime[NUM_KEY_MAX];			//最後にtrueを返した時間（リピート使用)

//ゲームパッド（XInput使用）部
XINPUT_STATE g_XinputState[MAX_USE_GAMEPAD];					//XInputの状態
WORD g_wButtonTrigger[MAX_USE_GAMEPAD];
WORD g_wButtonRelease[MAX_USE_GAMEPAD];
WORD g_wButtonRepeate[MAX_USE_GAMEPAD];
DWORD g_GamepadCurrentTime[MAX_USE_GAMEPAD];
DWORD g_GamepadExecLastTime[MAX_USE_GAMEPAD];
bool g_bUseGamepad[MAX_USE_GAMEPAD];

//マウス部
LPDIRECTINPUT8 g_pInputMouse = NULL;
LPDIRECTINPUTDEVICE8 g_pDevMouse = NULL;
DIMOUSESTATE g_MouseState;						//マウスの情報（プレス）
BYTE g_aMouseStateTrigger[MOUSE_BUTTON_MAX];	//ボタンのトリガー情報
BYTE g_aMouseStateRelease[MOUSE_BUTTON_MAX];	//ボタンのリリース情報
BYTE g_aMouseStateRepeate[MOUSE_BUTTON_MAX];	//リピート情報
DWORD g_aMouseCurrentTime[NUM_KEY_MAX];			//現在の時間（リピート使用）
DWORD g_aMouseExecLastTime[NUM_KEY_MAX];		//最後にtrueを返した時間（リピート使用)
D3DXVECTOR3 g_posPoint;							//マウス座標
D3DXVECTOR3 g_moveMouse;						//マウス移動量

//========================
//入力初期化処理
//========================
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	int nCntInit;		//カウンタ

	//キーボード部
	//DireceInputオブジェクトの生成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&g_pInputKeyboard, NULL)))
	{
		return E_FAIL;
	}

	//デバイス取得
	if (FAILED(g_pInputKeyboard->CreateDevice(GUID_SysKeyboard, &g_pDevKeyboard, NULL)))
	{
		return E_FAIL;
	}

	//データフォーマット設定
	if (FAILED(g_pDevKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//協調モード設定
	if (FAILED(g_pDevKeyboard->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//キーボードへのアクセス権を獲得
	g_pDevKeyboard->Acquire();


	//マウス部
	//DireceInputオブジェクトの生成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&g_pInputMouse, NULL)))
	{
		return E_FAIL;
	}

	//デバイス取得
	if (FAILED(g_pInputMouse->CreateDevice(GUID_SysMouse, &g_pDevMouse, NULL)))
	{
		return E_FAIL;
	}

	//データフォーマット設定
	if (FAILED(g_pDevMouse->SetDataFormat(&c_dfDIMouse)))
	{
		return E_FAIL;
	}

	//協調モード設定
	if (FAILED(g_pDevMouse->SetCooperativeLevel(hWnd,
		(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//マウスへのアクセス権を獲得
	g_pDevMouse->Acquire();


	//時間初期化
	for (nCntInit = 0; nCntInit < NUM_KEY_MAX; nCntInit++)
	{
		g_aKeyboardCurrentTime[nCntInit] = 0;
		g_aKeyboardExecLastTime[nCntInit] = timeGetTime() - REPEATE_TIME;
	}
	for (nCntInit = 0; nCntInit < MOUSE_BUTTON_MAX; nCntInit++)
	{
		g_aMouseCurrentTime[nCntInit] = 0;
		g_aMouseExecLastTime[nCntInit] = timeGetTime() - REPEATE_TIME;
	}

	//ゲームパッド類
	for (int nCntGPad = 0; nCntGPad < MAX_USE_GAMEPAD; nCntGPad++)
	{
		g_GamepadCurrentTime[nCntGPad] = 0;
		g_GamepadExecLastTime[nCntGPad] = timeGetTime();
		g_bUseGamepad[nCntGPad] = false;
	}

	return S_OK;
}

//========================
//入力終了処理
//========================
void UninitKeyboard(void)
{
	//入力デバイスの破棄
	if (g_pDevKeyboard != NULL)
	{
		g_pDevKeyboard->Unacquire();
		g_pDevKeyboard->Release();
		g_pDevKeyboard = NULL;
	}
	if (g_pDevMouse != NULL)
	{
		g_pDevMouse->Unacquire();
		g_pDevMouse->Release();
		g_pDevMouse = NULL;
	}

	//DirectInputオブジェクトの破棄
	if (g_pInputKeyboard != NULL)
	{
		g_pInputKeyboard->Release();
		g_pInputKeyboard = NULL;
	}
	if (g_pInputMouse != NULL)
	{
		g_pInputMouse->Release();
		g_pInputMouse = NULL;
	}

	//XInput終了
	XInputEnable(false);
}

//========================
//入力更新処理
//========================
void UpdateKeyboard(void)
{
	BYTE aKeyState[NUM_KEY_MAX];
	DIMOUSESTATE MouseState;
	XINPUT_STATE xInputState;
	POINT point;
	int nCntKey;

	//入力デバイスからデバイスを取得
	if (SUCCEEDED(g_pDevKeyboard->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++) 
		{
			//トリガーとリリース
			g_akeyStateTrigger[nCntKey] = (g_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];
 			g_akeyStateRelease[nCntKey] = (g_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & ~aKeyState[nCntKey];

			//リピート
			g_aKeyboardCurrentTime[nCntKey] = timeGetTime();
			if (aKeyState[nCntKey] && (g_aKeyboardCurrentTime[nCntKey] - g_aKeyboardExecLastTime[nCntKey]) > REPEATE_TIME)
			{
				g_aKeyboardExecLastTime[nCntKey] = g_aKeyboardCurrentTime[nCntKey];
				g_akeyStateRepeate[nCntKey] = aKeyState[nCntKey];
			}
			else
			{
				g_akeyStateRepeate[nCntKey] = 0;
			}
			//プレス
			g_aKeyState[nCntKey] = aKeyState[nCntKey];	//キーボードのプレス情報を保存
		}
	}
	else
	{
		g_pDevKeyboard->Acquire();
	}


	//マウス取得
	if (SUCCEEDED(g_pDevMouse->GetDeviceState(sizeof(DIMOUSESTATE), &MouseState)))
	{
		for (nCntKey = 0; nCntKey < MOUSE_BUTTON_MAX; nCntKey++)
		{
			//トリガーとリリース
			g_aMouseStateTrigger[nCntKey] = (g_MouseState.rgbButtons[nCntKey] ^ MouseState.rgbButtons[nCntKey]) & MouseState.rgbButtons[nCntKey];
			g_aMouseStateRelease[nCntKey] = (g_MouseState.rgbButtons[nCntKey] ^ MouseState.rgbButtons[nCntKey]) & ~MouseState.rgbButtons[nCntKey];

			//リピート
			g_aMouseCurrentTime[nCntKey] = timeGetTime();
			if (MouseState.rgbButtons[nCntKey] && (g_aMouseCurrentTime[nCntKey] - g_aMouseExecLastTime[nCntKey]) > REPEATE_TIME)
			{
				g_aMouseExecLastTime[nCntKey] = g_aMouseCurrentTime[nCntKey];
				g_aMouseStateRepeate[nCntKey] = MouseState.rgbButtons[nCntKey];
			}
			else
			{
				g_aMouseStateRepeate[nCntKey] = 0;
			}
		}

		//プレス
		g_MouseState = MouseState;	//キーボードのプレス情報を保存

		//移動量
		g_moveMouse = D3DXVECTOR3((float)g_MouseState.lX, (float)g_MouseState.lY, 0.0f);
	}
	else
	{
		g_pDevMouse->Acquire();
	}

	//マウス座標取得
	GetCursorPos(&point);
	ScreenToClient(FindWindowA(CLASS_NAME, nullptr), &point);
	g_posPoint.x = (float)point.x;
	g_posPoint.y = (float)point.y;


	//ゲームパッドから情報取得
	for (int nCntGPad = 0; nCntGPad < MAX_USE_GAMEPAD; nCntGPad++)
	{
		if (XInputGetState(nCntGPad, &xInputState) == ERROR_SUCCESS)
		{
			//使ってるよ
			g_bUseGamepad[nCntGPad] = true;

			//ボタントリガー情報取得
			g_wButtonTrigger[nCntGPad] = (g_XinputState[nCntGPad].Gamepad.wButtons ^ xInputState.Gamepad.wButtons) & xInputState.Gamepad.wButtons;

			//ボタンリリース情報取得
			g_wButtonRelease[nCntGPad] = (g_XinputState[nCntGPad].Gamepad.wButtons ^ xInputState.Gamepad.wButtons) & ~xInputState.Gamepad.wButtons;

			//リピート情報生成
			g_GamepadCurrentTime[nCntGPad] = timeGetTime();
			for (nCntKey = 0; nCntKey < GAMEPAD_BUTTON_NUM; nCntKey++)
			{
				if (g_XinputState[nCntGPad].Gamepad.wButtons & 1 << nCntKey && (g_GamepadCurrentTime[nCntGPad] - g_GamepadExecLastTime[nCntGPad]) > REPEATE_TIME)
				{
					g_GamepadExecLastTime[nCntGPad] = g_GamepadCurrentTime[nCntGPad];
					g_wButtonRepeate[nCntGPad] += 1 << nCntKey;
				}
			}

			//プレス情報その他もろもろ設定
			g_XinputState[nCntGPad] = xInputState;
		}
		else
		{
			//使ってないよ
			g_bUseGamepad[nCntGPad] = false;
		}

		
	}
}

//========================
//プレス情報を返す処理
//========================
bool GetKeyboardPress(int nKey)
{
	return (g_aKeyState[nKey] & 0x80) ? true : false;	//?が条件式作ることになってるみたいよ
}

//========================
//トリガー情報を返す処理
//========================
bool GetKeyboardTrigger(int nKey)
{
	return (g_akeyStateTrigger[nKey] & 0x80) ? true : false;
}

//========================
//リリース情報を返す処理
//========================
bool GetKeyboardRelease(int nKey)
{
	return (g_akeyStateRelease[nKey] & 0x80) ? true : false;
}

//========================
//リピート情報を返す処理
//========================
bool GetKeyboardRepeate(int nKey)
{
 	return (g_akeyStateRepeate[nKey] & 0x80) ? true : false;
}



//========================
//ゲームパッドのプレス情報を返す処理
//=======================
bool GetGamepadPress(int nPadNum, int nButton)
{
	if (g_bUseGamepad[nPadNum] == true)
	{
		return g_XinputState[nPadNum].Gamepad.wButtons & nButton ? true : false;
	}
	else
	{
		return false;
	}
}

//========================
//ゲームパッドのトリガー情報を返す処理
//=======================
bool GetGamepadTrigger(int nPadNum, int nButton)
{
	if (g_bUseGamepad[nPadNum] == true)
	{
		return g_wButtonTrigger[nPadNum] & nButton ? true : false;
	}
	else
	{
		return false;
	}
}

//========================
//ゲームパッドのリリース情報を返す処理
//=======================
bool GetGamepadRelease(int nPadNum, int nButton)
{
	if (g_bUseGamepad[nPadNum] == true)
	{
		return g_wButtonRelease[nPadNum] & nButton ? true : false;
	}
	else
	{
		return false;
	}
}

//========================
//ゲームパッドのリピート情報を返す処理
//=======================
bool GetGamepadRepeate(int nPadNum, int nButton)
{
	if (g_bUseGamepad[nPadNum] == true)
	{
		return g_wButtonRepeate[nPadNum] & nButton ? true : false;
	}
	else
	{
		return false;
	}
}

//========================
//左スティックの横軸を返す処理
//=======================
SHORT GetLStickX(int nPadNum)
{
	if (g_bUseGamepad[nPadNum] == true)
	{
		//-値最大なら+値最大に合わせる
		if (g_XinputState[nPadNum].Gamepad.sThumbLX < -STICK_MAX)
		{
			g_XinputState[nPadNum].Gamepad.sThumbLX = -STICK_MAX;
		}

		if (abs(g_XinputState[nPadNum].Gamepad.sThumbLX) > STICK_DEADZONE)
		{
			return g_XinputState[nPadNum].Gamepad.sThumbLX;
		}
		else
		{
			return 0;
		}
	}
	else
	{//使っていなかったら0返す
		return 0;
	}
}

//========================
//左スティックの横軸を返す処理
//=======================
SHORT GetLStickY(int nPadNum)
{
	if (g_bUseGamepad[nPadNum] == true)
	{
		//-値最大なら+値最大に合わせる
		if (g_XinputState[nPadNum].Gamepad.sThumbLY < -STICK_MAX)
		{
			g_XinputState[nPadNum].Gamepad.sThumbLY = -STICK_MAX;
		}

		if (abs(g_XinputState[nPadNum].Gamepad.sThumbLY) > STICK_DEADZONE)
		{
			return g_XinputState[nPadNum].Gamepad.sThumbLY;
		}
		else
		{
			return 0;
		}
	}
	else
	{//使っていなかったら0返す
		return 0;
	}
}

//========================
//右スティックの横軸を返す処理
//=======================
SHORT GetRStickX(int nPadNum)
{
	if (g_bUseGamepad[nPadNum] == true)
	{
		//-値最大なら+値最大に合わせる
		if (g_XinputState[nPadNum].Gamepad.sThumbRX < -STICK_MAX)
		{
			g_XinputState[nPadNum].Gamepad.sThumbRX = -STICK_MAX;
		}

		if (abs(g_XinputState[nPadNum].Gamepad.sThumbRX) > STICK_DEADZONE)
		{
			return g_XinputState[nPadNum].Gamepad.sThumbRX;
		}
		else
		{
			return 0;
		}
	}
	else
	{//使っていなかったら0返す
		return 0;
	}
}
//========================
//右スティックの横軸を返す処理
//=======================
SHORT GetRStickY(int nPadNum)
{
	if (g_bUseGamepad[nPadNum] == true)
	{
		//-値最大なら+値最大に合わせる
		if (g_XinputState[nPadNum].Gamepad.sThumbRY < -STICK_MAX)
		{
			g_XinputState[nPadNum].Gamepad.sThumbRY = -STICK_MAX;
		}

		if (abs(g_XinputState[nPadNum].Gamepad.sThumbRY) > STICK_DEADZONE)
		{
			return g_XinputState[nPadNum].Gamepad.sThumbRY;
		}
		else
		{
			return 0;
		}
	}
	else
	{//使っていなかったら0返す
		return 0;
	}
}

//========================
//ゲームパッド使用しているか返す処理
//=======================
bool GetUseGamepad(int nPadNum)
{
	if (g_bUseGamepad[nPadNum] == true)
	{
		return g_bUseGamepad[nPadNum];
	}
	else
	{
		return false;
	}
}

//========================
//マウスのプレス情報を返す処理
//=======================
bool GetMouseClickPress(int nButton)
{
	return g_MouseState.rgbButtons[nButton] & 0x80 ? true : false;
}

//========================
//マウスのトリガー情報を返す処理
//=======================
bool GetMouseClickTrigger(int nButton)
{
	return g_aMouseStateTrigger[nButton] & 0x80 ? true : false;
}

//========================
//マウスのリピート情報を返す処理
//=======================
bool GetMouseClickRepeate(int nButton)
{
	return g_aMouseStateRepeate[nButton] & 0x80 ? true : false;
}

//========================
//マウスのカーソル位置を返す処理
//=======================
D3DXVECTOR3 GetMousePos(void)
{
	return g_posPoint;
}

//========================
//マウスのカーソル移動量を返す処理
//=======================
D3DXVECTOR3 GetMouseMove(void)
{
	return g_moveMouse;
}