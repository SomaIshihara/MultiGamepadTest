//==========================================
//
//デバッグ表示プログラム[debugproc.cpp]
//Author:石原颯馬
//
//==========================================
#include "main.h"
#include <stdarg.h>

//マクロ
#define DEBPRO_MAX_STR	(2048)			//確保する文字数

//グローバル変数
LPD3DXFONT g_pFont = NULL;				//フォントへのポインタ
char g_aStrPrint[DEBPRO_MAX_STR];		//表示文字バッファ

//========================
//初期化処理
//========================
void InitDebugProc(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//デバイスの取得

	//デバッグ表示用フォント生成
	D3DXCreateFont(pDevice, 18, 0, 0, 0, FALSE,
		SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		"Terminal", &g_pFont);

	//バッファクリア
	memset(&g_aStrPrint, 0, sizeof g_aStrPrint);
}

//========================
//終了処理
//========================
void UninitDebugProc(void)
{
	//フォント破棄
	if (g_pFont != NULL)
	{
		g_pFont->Release();
		g_pFont = NULL;
	}
}

//========================
//描画処理
//========================
void DrawDebugProc(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	//デバイスの取得
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	
	//テキスト描画
	g_pFont->DrawText(NULL, &g_aStrPrint[0], -1, &rect, DT_LEFT,
		D3DCOLOR_RGBA(255, 255, 255, 255));

	//バッファクリア
	memset(&g_aStrPrint, 0, sizeof g_aStrPrint);
}

//========================
//入力取り込み処理
//========================
void PrintDebugProc(const char *fmt, ...)
{
	int nCount;		//カウンタ
	va_list args;	//可変引数の中身

	//出力開始
	va_start(args, fmt);

	for (nCount = 0; *fmt != '\0'; fmt++)
	{
		if (*fmt == '%')
		{
			char aStr[10] = {};
			switch (*(fmt + 1))
			{
			case 'd':
				sprintf(&aStr[0], "%d", va_arg(args, int));
				snprintf(&g_aStrPrint[0], sizeof g_aStrPrint, "%s%s", &g_aStrPrint[0], &aStr[0]);
				fmt++;
				break;
			case 'f':
				sprintf(&aStr[0], "%.2f", va_arg(args, double));
				snprintf(&g_aStrPrint[0], sizeof g_aStrPrint, "%s%s", &g_aStrPrint[0], &aStr[0]);
				fmt++;
				break;
			case 'c':
				strcat(&g_aStrPrint[0], &va_arg(args, char));
				fmt++;
				break;
			case 's':
				strcat(&g_aStrPrint[0], va_arg(args, const char*));
				fmt++;
				break;
			default:
				strcat(&g_aStrPrint[0], "< ERROR >");
				break;
			}
		}
		else
		{
			char cData = *fmt;
			snprintf(&g_aStrPrint[0], sizeof g_aStrPrint, "%s%c", &g_aStrPrint[0], cData);
		}
	}

	va_end(args);
}