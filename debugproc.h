//==========================================
//
//デバッグ表示プログラムのヘッダ[debugproc.h]
//Author:石原颯馬
//
//==========================================
#ifndef _DEBUGPROC_H_
#define _DEBUGPROC_H_

//プロトタイプ宣言
void InitDebugProc(void);
void UninitDebugProc(void);
void DrawDebugProc(void);
void PrintDebugProc(const char *fmt, ...);

#endif // !_DEBUGPROC_H_
