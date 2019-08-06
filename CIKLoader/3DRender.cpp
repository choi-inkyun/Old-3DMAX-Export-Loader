/*
3DRender.cpp
이곳에서 하는일은 3D Render 에 관련된 함수들을 제공하는 것입니다.
사용자는 이곳에 있는 함수들로 손쉽게 프로그래밍을 할 수 있을 것입니다.
DirectX 9.0
제작자 : 최인균
제작   : 2003년 1월 4일
수정   :
*/

#include "3DRender.h"



// 가장 기본적으로 필요한 인터페이스들.. 헤더파일에서 extern 으로 정의해준다.
LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;   
LPDIRECT3DTEXTURE9	    pTexture[8];		 // 텍스쳐..8개가 최고
// D3D 인터페이스 Create 등 시에 필요. //
D3DDISPLAYMODE d3ddm;
D3DPRESENT_PARAMETERS d3dpp; 
////////////
// 생성자 //
Render::Render()
{
	
}
////////////////////
// 함수명 : int Create3DWindow(HWND hWnd)
// 3D 인터페이스, 디바이스를 윈도우 모드로 생성하게 된다.
////////////////////
int Render::Create3DWindow(HWND hWnd)
{
	// 가장 기초 적인 부분이니 다 알꺼라 생각한다. 모른다면 샘플의 튜토리얼을 참고 하기 바란다.
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
// 함수명 : int Create3DFull(HWND hWnd, int flag)
// 3D 인터페이스, 디바이스를 풀 스크린 모드로 생성하게 된다.
// int flag 에 들어가는 int 가 값은 소스를 보면 알곘지만. 백 버퍼의 크기를 결정하는 인자이다.
// 안넣을수는 없겠지만 이상한 인자를 넣어도 된다. 1,2를 넣으면 다른 버퍼 크기로 결정.!.기본은 640 * 480
///////////////////
int Render::Create3DFull(HWND hWnd, int flag)
{
	// 가장 기초 적인 부분이니 다 알꺼라 생각한다. 모른다면 샘플의 튜토리얼을 참고 하기 바란다.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return 1;
	
	g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = FALSE; //이부분이 위에랑 다르다
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format; //변경
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; //추가
	d3dpp.EnableAutoDepthStencil = TRUE; //추가
	//이부분이 Window 모드에서 추가된.. //
	d3dpp.BackBufferWidth = 640; 
	d3dpp.BackBufferHeight = 480;
	if(flag == 1) //보면 알꺼라 생각한다.!
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
// 함수명 : VOID Cleanup()
// 기본적으로 선언되어 있는 D3D 객체들을 삭제한다.
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
// 소멸자 //
Render::~Render()
{
	Cleanup();
}