//3D Render �� ��� ����
#ifndef __3D_RENDER_H__
#define __3D_RENDER_H__

#define WIN32_LEAN_AND_MEAN 
#define NODEFAULTLIB 

#include <d3d9.h>
#include <d3d9types.h>
#include <d3dx9.h>
#include <tchar.h>
#include <windowsx.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <basetsd.h>
#include <dxerr9.h>
#include <malloc.h>
#include "CIK.h"

extern LPDIRECT3D9             g_pD3D;
extern LPDIRECT3DDEVICE9       g_pd3dDevice; 
extern LPDIRECT3DVERTEXBUFFER9 g_pVB;   
extern LPDIRECT3DTEXTURE9	   pTexture[8];		 // �ؽ���..8���� �ְ�


class Render
{
	public:
		int Create3DWindow(HWND hWnd);
		int Create3DFull(HWND hWnd, int flag);
		
		Render();
		~Render();
	protected:

	private:
		VOID Cleanup(); //�Ҹ��ڿ��� �ѹ��� �ҷ��� �ָ� �ǹǷ�. �ٸ����� ���� ����
};

#endif