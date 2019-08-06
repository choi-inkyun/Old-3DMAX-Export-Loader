////////////
// define //
#ifndef _CIK_H_
#define _CIK_H_
/////////////
// include //
#include<stdio.h>
#include<d3d9types.h> //��Ʈ���� ����ü ���� ��������
#include<d3dx9math.h> //D3DXMATRIX ����ü ���� ����ϱ� ����
/////////////
// lib ���� ��ũ. ���߿� �ű� �ʿ伺��..
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx9dt.lib")
#pragma comment(lib, "d3dxof.lib")
////////////


/*
CIK Export �Ҷ� ���̾�α׹ڽ����� �����ߴ� �ɼ��� ����.
CIK ���� ���� ���� ������, Load �Ҷ��� Render �Ҷ� �� �ʿ��� ����.
���⽱�� ����ϱ� ���ϰ� ����ü�� ����
*/
struct DigCheck{
	bool IncludeM; //Mash
	bool IncludeMT; //Mash & Texture
	bool IncludeMTA; //Mash & Texture & Animation
	int  Controllers; //fream?.
	// Animation Check = True
	bool IncludeAKey; //Key Animation, 3D MAX 
	bool IncludeAForce; //Force Animation, 3D MAX
	bool IncludeAPhy; //Phy, ����ũ 
	bool IncludeAAni; //Ani, �Ƽ��� �÷�����
};

// Vertex �� x,y,z ��ǥ, Normal ��ǥ, �ؽ��� ��ǥ�� �ѹ��� ���� ����ü.
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

// Face ���� ������ ����ü~
// �̱���ü�� �ϴ� �����ʰ�.!
typedef struct tagFace
{
	int a, b, c;
	
	tagFace()
	{
		a = b = c = 0;
	}
}Face;

/*
ASE ���Ͼȿ� �ִ� SCENE�� ���� ��. �׾ȿ� �ִ� ���� ���߸���.
CCIK Ŭ�������� �� Ŭ���� ������ ������ �� Ŭ���� �ȿ� �ִ� ������ ó���ϰԵ�
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
ASE ���Ͼȿ��ִ� ��Ʈ������ ���� ���� Ŭ����
�� �ȿ� �ִ� ���߸� ������ �ҷ� �鿩 ó���Ѵ�
�̰Ϳ��� CCIK ���� ������ �޾Ƽ� ���� ó���Ѵ�
*/
class MATERIAL
{
	public:
		D3DMATERIAL9 Material;				// ���͸��� : Diffuse, Ambient ���� ���� ����ü. �ڼ��Ѱ� ��ť��Ʈ ����
		bool 		 TextureCheck;          // �ؽ��İ� �������� �ȉ����. 
		char		 TextureName[MAX_PATH];	// �ؽ��� ȭ�ϸ�

		MATERIAL();
	protected:

	private:
};
/*
ASE ���Ͼȿ��ִ� GEOMOBJECT �� �������� Ŭ����
�̰� ���� CCIK Ŭ�������� ������ �ؼ� ó���ϸ�, ���������� ���� �߿���
�κ��̶���� �� ���ִ�.���� ���ؽ� ��ġ�� �ؽ���, �ִϸ��̼� ���� ���� ���°�
*/
class GEOMOBJECT
{
	public:
		D3DXMATRIX	TM;	//TM (TM_ROW)
		Vertex		*VertexList;	// ���ؽ�(Vertex, Normal, tu, tv) ����Ʈ
//		Face		*FaceList;		// ���̽� ����Ʈ		
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
		/* �Լ��� */
		void DoLoadCIK(char *FileName);
		int  LoadInfo();
		int  LoadMaterial();
		int  LoadObject();
		void DestroyObject();
		void PrintBuffer(char *FileName);			
		CCIK(); //������
		~CCIK(); //�Ҹ���

		/* ������ */
		int TotalM;	int TotalO; //*TotalMO ��ũ��Ʈ �ڿ��ִ� �������. ��Ʈ����� ��������Ʈ�� ����~
		FILE *fp;
		DigCheck CheckOption;
		// Ŭ���� ���� // 
		SCENE	    Scene;
		MATERIAL   *Matial;		
		GEOMOBJECT *Object;		
	protected:

	private:
};


#endif
