/*
3DRender.cpp
�̰����� �ϴ����� 3D Render �� ���õ� �Լ����� �����ϴ� ���Դϴ�.
����ڴ� �̰��� �ִ� �Լ���� �ս��� ���α׷����� �� �� ���� ���Դϴ�.
DirectX 9.0
������ : ���α�
����   : 2003�� 1�� 4��
����   :
*/

#include "3DRender.h"



// ���� �⺻������ �ʿ��� �������̽���.. ������Ͽ��� extern ���� �������ش�.
LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;   
LPDIRECT3DTEXTURE9	    pTexture[8];		 // �ؽ���..8���� �ְ�
// D3D �������̽� Create �� �ÿ� �ʿ�. //
D3DDISPLAYMODE d3ddm;
D3DPRESENT_PARAMETERS d3dpp; 
////////////
// ������ //
Render::Render()
{
	
}
////////////////////
// �Լ��� : int Create3DWindow(HWND hWnd)
// 3D �������̽�, ����̽��� ������ ���� �����ϰ� �ȴ�.
////////////////////
int Render::Create3DWindow(HWND hWnd)
{
	// ���� ���� ���� �κ��̴� �� �˲��� �����Ѵ�. �𸥴ٸ� ������ Ʃ�丮���� ���� �ϱ� �ٶ���.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return 1;
	
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; 
	d3dpp.EnableAutoDepthStencil = TRUE;
	
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
    {
        return 2;
    }
	
    return 0;
}
////////////////////
// �Լ��� : int Create3DFull(HWND hWnd, int flag)
// 3D �������̽�, ����̽��� Ǯ ��ũ�� ���� �����ϰ� �ȴ�.
// int flag �� ���� int �� ���� �ҽ��� ���� �ˁ�����. �� ������ ũ�⸦ �����ϴ� �����̴�.
// �ȳ������� �������� �̻��� ���ڸ� �־ �ȴ�. 1,2�� ������ �ٸ� ���� ũ��� ����.!.�⺻�� 640 * 480
///////////////////
int Render::Create3DFull(HWND hWnd, int flag)
{
	// ���� ���� ���� �κ��̴� �� �˲��� �����Ѵ�. �𸥴ٸ� ������ Ʃ�丮���� ���� �ϱ� �ٶ���.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return 1;
	
	g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = FALSE; //�̺κ��� ������ �ٸ���
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format; //����
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; //�߰�
	d3dpp.EnableAutoDepthStencil = TRUE; //�߰�
	//�̺κ��� Window ��忡�� �߰���.. //
	d3dpp.BackBufferWidth = 640; 
	d3dpp.BackBufferHeight = 480;
	if(flag == 1) //���� �˲��� �����Ѵ�.!
	{
		d3dpp.BackBufferWidth = 800;
		d3dpp.BackBufferHeight = 600;
	}
	else if(flag == 2)
	{
		d3dpp.BackBufferWidth = 1024;
		d3dpp.BackBufferHeight = 768;
	}	
	//////////////////////////////////////
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
    {
        return 3;
    }
	
    return 0;
}
////////////////////
// �Լ��� : VOID Cleanup()
// �⺻������ ����Ǿ� �ִ� D3D ��ü���� �����Ѵ�.
////////////////////
VOID Render::Cleanup()
{
    if( g_pVB != NULL )
        g_pVB->Release();
	
    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();
	
    if( g_pD3D != NULL )
        g_pD3D->Release();
}

////////////
// �Ҹ��� //
Render::~Render()
{
	Cleanup();
}