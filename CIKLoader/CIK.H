////////////
// define //
#ifndef _CIK_H_
#define _CIK_H_
/////////////
// include //
#include<stdio.h>
#include<d3d9types.h> //메트리얼 구조체 등을 쓰기위함
#include<d3dx9math.h> //D3DXMATRIX 구조체 등을 사용하기 위함
/////////////
// lib 파일 링크. 나중에 옮길 필요성이..
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx9dt.lib")
#pragma comment(lib, "d3dxof.lib")
////////////


/*
CIK Export 할때 다이얼로그박스에서 선택했던 옵션의 정보.
CIK 포멧 가장 위에 있으며, Load 할때나 Render 할때 꼭 필요한 정보.
보기쉽고 사용하기 편하게 구조체로 묶음
*/
struct DigCheck{
	bool IncludeM; //Mash
	bool IncludeMT; //Mash & Texture
	bool IncludeMTA; //Mash & Texture & Animation
	int  Controllers; //fream?.
	// Animation Check = True
	bool IncludeAKey; //Key Animation, 3D MAX 
	bool IncludeAForce; //Force Animation, 3D MAX
	bool IncludeAPhy; //Phy, 피지크 
	bool IncludeAAni; //Ani, 아샬님 플러그인
};

// Vertex 는 x,y,z 좌표, Normal 좌표, 텍스쳐 좌표를 한번에 묶은 구조체.
typedef struct tagVertex
{
	float x,y,z;
	float nx, ny, nz;
	float tu, tv, tz;
	
	tagVertex()
	{
		x = y = z = 0;
		nx = ny = nz = 0;
		tu = tv = tz = 0;
	}
}Vertex;

// Face 값을 저장할 구조체~
// 이구조체는 일단 쓰지않고.!
typedef struct tagFace
{
	int a, b, c;
	
	tagFace()
	{
		a = b = c = 0;
	}
}Face;

/*
ASE 파일안에 있는 SCENE를 따서 씀. 그안에 있는 정보 간추린것.
CCIK 클레스에서 이 클레스 변수가 있으며 그 클레스 안에 있는 변수로 처리하게됨
*/
class SCENE
{
public:
	int FirstFrame;
	int LastFrame;
	int FreamSpeed;
	int TicksperFrame;
	
protected:

private:
};

/*
ASE 파일안에있는 메트리얼을 따서 만든 클레스
그 안에 있는 간추린 정보를 불러 들여 처리한다
이것역시 CCIK 에서 변수로 받아서 모든걸 처리한다
*/
class MATERIAL
{
	public:
		D3DMATERIAL9 Material;				// 매터리얼 : Diffuse, Ambient 등이 들어가는 구조체. 자세한건 도큐멘트 참고
		bool 		 TextureCheck;          // 텍스쳐가 적용됬는지 안됬는지. 
		char		 TextureName[MAX_PATH];	// 텍스쳐 화일명

		MATERIAL();
	protected:

	private:
};
/*
ASE 파일안에있는 GEOMOBJECT 를 따서만든 클레스
이것 역시 CCIK 클레스에서 변수로 해서 처리하며, 실제적으로 가장 중요한
부분이라고도 할 수있다.실제 버텍스 위치와 텍스쳐, 애니메이션 등의 값이 들어가는곳
*/
class GEOMOBJECT
{
	public:
		D3DXMATRIX	TM;	//TM (TM_ROW)
		Vertex		*VertexList;	// 버텍스(Vertex, Normal, tu, tv) 리스트
//		Face		*FaceList;		// 페이스 리스트		
		DWORD		*FaceList;
		DWORD		*UVFaceList;
		int		  	TotalVertex;
		int		    TotalFace;
		int			TotalTV;
		int         REF;
		GEOMOBJECT();
		~GEOMOBJECT();
	protected:
		
	private:
};

class CCIK
{
	public:
		/* 함수들 */
		void DoLoadCIK(char *FileName);
		int  LoadInfo();
		int  LoadMaterial();
		int  LoadObject();
		void DestroyObject();
		void PrintBuffer(char *FileName);			
		CCIK(); //생성자
		~CCIK(); //소멸자

		/* 변수들 */
		int TotalM;	int TotalO; //*TotalMO 스크립트 뒤에있는 순서대로. 메트리얼과 지오브젝트를 말함~
		FILE *fp;
		DigCheck CheckOption;
		// 클레스 변수 // 
		SCENE	    Scene;
		MATERIAL   *Matial;		
		GEOMOBJECT *Object;		
	protected:

	private:
};


#endif

