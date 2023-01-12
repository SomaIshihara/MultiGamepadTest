//==========================================
//
//�Q�[���S�ʂ��Ǘ�����v���O�����̃w�b�_[main.h]
//Author:�Ό��D�n
//
//==========================================
#ifndef _MAIN_H_
#define _MAIN_H_

#include <Windows.h>
#include <XInput.h>
#include <string.h>
#include <stdio.h>
#include "d3dx9.h"
#define DIRECTINPUT_VERSION	(0x0800)	//�r���h���̌x���Ώ�
#include "dinput.h"
#include "xaudio2.h"

//���C�u�����̃����N
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"xinput.lib")

//�}�N����`
#define CLASS_NAME			"WindowClass"
#define SCREEN_WIDTH		(1280)		//�N���C�A���g�̈�̕�
#define SCREEN_HEIGHT		(720)		//�N���C�A���g�̈�̍���
#define MAX_FPS				(60)		//�ő�t���[�����[�g
#define FVF_VERTEX_2D		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)	//2D
#define FVF_VERTEX_3D		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)	//3D

//���_���[2D]
typedef struct
{
	D3DXVECTOR3 pos;	//���_���W
	float rhw;			//���W�ϊ��W���i1.0f�ŌŒ�j
	D3DCOLOR col;		//���_�J���[
	D3DXVECTOR2 tex;	//�e�N�X�`�����W
} VERTEX_2D;

//���_���[3D]
typedef struct
{
	D3DXVECTOR3 pos;	//�ʒu
	D3DXVECTOR3 nor;	//�@���x�N�g��
	D3DCOLOR col;		//�J���[
	D3DXVECTOR2 tex;	//�e�N�X�`��
} VERTEX_3D;

//�v���g�^�C�v�錾
LPDIRECT3DDEVICE9 GetDevice(void);

#endif // !_MAIN_H_