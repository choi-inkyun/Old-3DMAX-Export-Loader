/*
CIK 포멧 Read 테스트 프로그램 입니다.
CIK 포멧을 테스트 해볼수 있습니다.
3D 테스트 버전..
*/
#include <Windows.h>
#include "3DRender.h" //"CIK.H" 를 써줄 필요 없이. 3DRender 에서 모든 h 파일 연결.
HWND hWnd; //핸들값~*

Render Ren;
CCIK CIK[5];

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    FLOAT x, y, z;      // The untransformed, 3D position for the vertex
    DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

float xRot = 0.0f, yRot = 0.0f;
//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Turn off culling, so we see the front and back of the triangle
 //   g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    // Turn off D3D lighting, since we are providing our own vertex colors
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	CIK[0].DoLoadCIK("aaa.cik");
	int VertexSize = (CIK[0].Object[0].TotalFace * 3) * sizeof(CUSTOMVERTEX);

    CUSTOMVERTEX *g_Vertices = new CUSTOMVERTEX[ CIK[0].Object[0].TotalFace * 3 ];

	for(int i = 0; i < CIK[0].Object[0].TotalFace; i++)
	{
		g_Vertices[i*3].x   = CIK[0].Object[0].VertexList[CIK[0].Object[0].FaceList[i * 3 + 0]].x;
		g_Vertices[i*3].z   = CIK[0].Object[0].VertexList[CIK[0].Object[0].FaceList[i * 3 + 0]].y;
		g_Vertices[i*3].y   = CIK[0].Object[0].VertexList[CIK[0].Object[0].FaceList[i * 3 + 0]].z;
		g_Vertices[i*3].color = 0xffff00ff;

		g_Vertices[i*3+1].x = CIK[0].Object[0].VertexList[CIK[0].Object[0].FaceList[i * 3 + 2]].x;
		g_Vertices[i*3+1].z = CIK[0].Object[0].VertexList[CIK[0].Object[0].FaceList[i * 3 + 2]].y;
		g_Vertices[i*3+1].y = CIK[0].Object[0].VertexList[CIK[0].Object[0].FaceList[i * 3 + 2]].z;
		g_Vertices[i*3+1].color = 0xff00ffff;

		g_Vertices[i*3+2].x = CIK[0].Object[0].VertexList[CIK[0].Object[0].FaceList[i * 3 + 1]].x;
		g_Vertices[i*3+2].z = CIK[0].Object[0].VertexList[CIK[0].Object[0].FaceList[i * 3 + 1]].y;
		g_Vertices[i*3+2].y = CIK[0].Object[0].VertexList[CIK[0].Object[0].FaceList[i * 3 + 1]].z;
		g_Vertices[i*3+2].color = 0xff000000;

/*		FILE *fp2 = fopen("abc.txt", "a");
		fprintf(fp2, "%f %f %f\n", g_Vertices[i].x, g_Vertices[i].y, g_Vertices[i].z);
		fclose(fp2);*/
	}

    // Create the vertex buffer.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( VertexSize,
												  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( g_pVB->Lock( 0, VertexSize, (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, VertexSize );
    g_pVB->Unlock();

	if(g_Vertices)
	{
		delete[] g_Vertices;
		g_Vertices = NULL;
	}
  
   // Initialize three vertices for rendering a triangle
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    // For our world matrix, we will just rotate the object about the y-axis.
    D3DXMATRIXA16 matWorld, matX, matY;

//    UINT  iTime  = timeGetTime() % 5000;
//    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 5000.0f;
	  D3DXMatrixRotationY( &matX, yRot/1000.0f );
	  D3DXMatrixRotationX( &matY, xRot/1000.0f);

	  matWorld = matX * matY;

      g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    D3DXVECTOR3 vEyePt( 0.0f, 300.0f, -300.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 5.0f, 1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // Clear the backbuffer to a black color
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET || D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(199,199,199), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        // Setup the world, view, and projection matrices
        SetupMatrices();

		// Render the vertex buffer contents
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
//		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, CIK[0].Object[0].TotalFace);

        // End the scene
        g_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_UP:
			xRot += 100.0f;
			break;
		case VK_DOWN:
			xRot -= 100.0f;
			break;
		case VK_LEFT:
			yRot += 100.0f;
			break;
		case VK_RIGHT:
			yRot -= 100.0f;
			break;
		}
		return 0;
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "D3D Tutorial", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "D3D Tutorial", "D3D Tutorial 03: Matrices",
                              WS_OVERLAPPEDWINDOW, 100, 100, 256, 256,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        // Create the scene geometry
        if( SUCCEEDED( InitGeometry() ) )
        {
            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof(msg) );
            while( msg.message!=WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}



