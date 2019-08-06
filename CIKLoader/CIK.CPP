/*
���� : ���α�
���� ���� : 2003�� 1�� 2��
���� : 3D MAX EXPORT �÷����� ������ CIK ������ �ҷ��鿩 3D ������ ���
���� ���� : 0000�� 0�� 0��
DirectX 9.0
*/
// Include //
#include<d3d9.h>
#include<d3d9types.h>
#include<d3dx9.h>
#include<windows.h>
#include<assert.h>
#include"CIK.H"
#include"math.h"
/////////////
// Define //
#define MAX_BUFFER 250
#define MAX_LINE 40
////////////
extern CCIK cCIK;
////////////
char buffer[MAX_BUFFER]; //fgets �� ���� �о���� ����
char dust[MAX_BUFFER];   //dust �� �״�� ������. �����Ⱚ �ִ� ����
// �ڼ��� ���� ���ڸ� ��... buffer �Լ����� fgets �� �ҷ��� ������ sscanf �� �ҷ��ö�
// �ʿ���� ���� dust �� �Ἥ �ҷ��ͼ� �ƹ��͵� ó������ �ʴ´ٶ� �׷� �ǹ�

// ��ǥ���� �ִ� �⺻���� ��.
struct CUSTOMVERTEX
{
    FLOAT x, y, z;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ)
////////////
// ������ //
CCIK::CCIK()
{	
	fp = NULL;	
	TotalM = 0; TotalO = 0;
	Matial = NULL;	
	Object = NULL;		
}
////////////
// ������ //
MATERIAL::MATERIAL()
{
	TextureCheck = false;
	memset(TextureName, NULL, MAX_PATH);
}
////////////
// ������ //
GEOMOBJECT::GEOMOBJECT()
{
	TotalVertex = NULL;
	TotalFace = NULL;
	TotalTV = NULL;
	REF = NULL;
}
////////////
// �Լ��� : void DoLoadCIK(char *FileName) �� �߿� Start~
// CIK ������ �ε� �ϱ� ���ؼ� ���� ���� ȣ���ؾ��� �Լ�.
// �о���� CIK ���� �̸��� �䱸�Ѵ�.  ex : "aaa.cik"
///////////
void CCIK::DoLoadCIK(char *FileName)
{
	DestroyObject(); //���� ��ü ����
	
	fp = fopen(FileName, "r"); //CIK ������ ����
	
    //////// Load ////////
	LoadInfo(); //�⺻�� ����. SCENE �� �ش�ȴ�.
	LoadMaterial(); //��Ʈ���� ����.
	LoadObject(); //������Ʈ ����.
	//////////////////////
	
	int aa= 4;
	int *ab = new int[aa];
	fclose(fp); //CIK ������ �ݴ´�
}
////////////
// �Լ��� : int LoadInfo()
// DoLoadCIK() �Լ����� ���� �ҷ����� �Լ�
// CIK ���� �����ִ� �⺻���� �������� �ε� �� �� �ִ�.
// � ���̾�α׿��� �ɼ��� �����ߴ��� �ҷ������� �Լ�
////////////
int CCIK::LoadInfo()
{
	/// �� ���ٿ� �ִ� ���̾�α׹ڽ� üũ ������ ���� ///
	//����� fgets �� ������ �о�´��� �� �Ϳ� ���ؼ� ���� ������ �ϳ��� �ٽ� �о�鿩 �����ۿ� �����Ѵ�.
	fgets(buffer, MAX_BUFFER, fp); 
	sscanf(buffer, "%d%d%d%d%d%d%d%d", &CheckOption.IncludeM,    &CheckOption.IncludeMT, 
		&CheckOption.IncludeMTA,  &CheckOption.Controllers,
		&CheckOption.IncludeAKey, &CheckOption.IncludeAForce,
		&CheckOption.IncludeAPhy, &CheckOption.IncludeAAni);
	///////////////////////////////////////////////////////
	// �� ���� ������ �׳� �ѱ�ϴ� // 
	// - 3DSMAX_ASCIIEXPORT  'CIK' Format -
	// *���� :  1.0v *������ : ���α� - "Fri Jan 03 11:16:35 2003"
	fgets(buffer, MAX_BUFFER, fp);
	// �� ������ CIK ������ �´��� �˻��մϴ�.
	assert(buffer != "- 3DSMAX_ASCIIEXPORT  'CIK' Format -");
	//////////////////////////////////
	/*
	SCENE_FIRSTFRAME		SCENE_LASTFRAME		SCENE_FRAMESPEED
	SCENE_TICKSPERFRAME		SCENE_BACKGROUND_STATIC		SCENE_AMBIENT_STATIC
	*/		
	fgets(buffer, MAX_BUFFER, fp);
	fscanf(fp, "%d", &Scene.FirstFrame);		
	fgets(buffer, MAX_BUFFER, fp);
	fscanf(fp, "%d", &Scene.LastFrame);		
	fgets(buffer, MAX_BUFFER, fp);
	fscanf(fp, "%d", &Scene.FreamSpeed);
	fgets(buffer, MAX_BUFFER, fp);
	fscanf(fp, "%d", &Scene.TicksperFrame);
	//SCENE_BACKGROUND_STATIC SCENE_AMBIENT_STATIC ������ ����� �ʿ��� Pass
	fgets(buffer, MAX_BUFFER, fp);
	fgets(buffer, MAX_BUFFER, fp);
	fgets(buffer, MAX_BUFFER, fp);
	/////////////// Scene Load End ////////////////
	return 0;
}
////////////
// �Լ��� : int LoadMaterial()
// �Լ������� ������ CIK ������� ��Ʈ���� ������ �ε��ϴ� �Լ��̴�.
// �̰͵� LoadInfo() �Լ��� ���� � �����̴��� �ε尡 �Ǿ�� �� ���̴�.
////////////
int CCIK::LoadMaterial()
{
	fgets(buffer, MAX_BUFFER, fp); //���⼭ CIK ���Ͼȿ��ִ� ��Ʈ���� ������ ������Ʈ ������ �ҷ��´�
	sscanf(buffer, "%s%d%d", &dust, &TotalM, &TotalO); //�߿��ϰ� ���� ���̴� ����� �Ѱ�
	
	// �� ���� �˲��� �����Ѵ�. ������ �ҷ����� ��Ʈ����� ������Ʈ ������ŭ �޸�
	// �Ҵ��� ���ش�. �׷��� �ؾ��� �ҷ��ü� �ֱ� �����̴�. �׸��� ����� ���� �Ŀ�
	// �޸� ������ �������� �޸� ������ ��������Ѵ�.
	Matial = new MATERIAL[TotalM];
	Object = new GEOMOBJECT[TotalO];	
	
	for(int i = 0; i < TotalM; i++){ //��Ʈ���� ����ŭ for ���� ������ �о���δ�.
		fgets(buffer, MAX_BUFFER, fp); // M ����. ��ȣ�� ����Ŵ
		
		fgets(buffer, MAX_BUFFER, fp); //��Ʈ���� ����. Ambient
		sscanf(buffer, "%f%f%f",   &Matial[i].Material.Ambient.r,
						 		   &Matial[i].Material.Ambient.g,
								   &Matial[i].Material.Ambient.b);
		
		fgets(buffer, MAX_BUFFER, fp); //��Ʈ���� ����. Diffuse
		sscanf(buffer, "%f%f%f",   &Matial[i].Material.Diffuse.r,
			 					   &Matial[i].Material.Diffuse.g,
								   &Matial[i].Material.Diffuse.b);
		
		fgets(buffer, MAX_BUFFER, fp); //��Ʈ���� ����. Specular
		sscanf(buffer, "%f%f%f",  &Matial[i].Material.Specular.r,
								  &Matial[i].Material.Specular.g,
								  &Matial[i].Material.Specular.b);
		
		// �ؿ� 4���� �ϴ� �ʿ��� ����. �׳� �ǳʶ�.
		// ���߿� �ʿ��� ��� ���� ������ �о� ���̸� �ɰ��̰�
		// �ᱹ �ʿ���ٰ� �������� Export �÷����ΰ� ���⼭ ������.
		fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);
		
		// Texture ������ �о����.
		// Texture �� ������ ���� ��Ʈ���� �̶�� NULL ���� ���̴�. �̰Ϳ� ���� ó���� �����Ѵ�.
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%s%s", &dust, Matial[i].TextureName);
		if(strcmp(Matial[i].TextureName, "NULL")) //���Ⱑ üũ�κ�!. �ؽ��İ� �ִ��� ������
			Matial[i].TextureCheck = true; //�ؽ��İ� �ִٸ� true ��
	}
	return 0;	
}		
///////////
// �Լ��� : int LoadObject()
// GEOMOBJECT �� �ε��Ѵ�. ���̾�α� �ڽ����� ������ �ɼǿ� ���� ���̰� �� ���̴�.
// �װͿ� ���� ����ó���� �� ���ֱ� �ٶ���.
///////////
int CCIK::LoadObject()
{
	for(int i = 0; i < TotalO; i++){
		fgets(buffer, MAX_BUFFER, fp);
		// ������Ʈ�� ó��. TM �� �ε���. TM_ROW0
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%f%f%f",  &Object[i].TM._11,
			&Object[i].TM._13,
			&Object[i].TM._12);
		
		// TM_ROW1
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%f%f%f", &Object[i].TM._31,
			&Object[i].TM._33, 
			&Object[i].TM._32);
		
		// TM_ROW2
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%f%f%f", &Object[i].TM._21,
			&Object[i].TM._23,
			&Object[i].TM._22);
		
		// TM_ROW3
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%f%f%f", &Object[i].TM._41,
			&Object[i].TM._43,
			&Object[i].TM._42);
		
		// *TM_POS *TM_ROTAXIS *TM_ROTANGLE *TM_SCALE *TM_SCALEAXIS *TM_SCALEAXISANG
		// ������ ������ ���� �ʿ� ������� PASS. �ʿ��ҋ� �ҷ��� ���� ��.
		fgets(buffer, MAX_BUFFER, fp);	fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);	fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);	fgets(buffer, MAX_BUFFER, fp);
		
		// ���ؽ� �Ѱ���, Face ����, �ؽ��� ��ǥ ����.
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%d%d%d", &Object[i].TotalVertex, &Object[i].TotalFace, &Object[i].TotalTV);
		// Vertex ��ŭ Face ��ŭ �޸𸮸� �Ҵ����� 
		Object[i].VertexList = new Vertex[Object[i].TotalVertex];
		Object[i].FaceList = new DWORD[Object[i].TotalFace * 3];
		Object[i].UVFaceList = new DWORD[Object[i].TotalFace * 3];
		
		for(int j = 0; j < Object[i].TotalVertex; j++)
		{
			//���ؽ��� �ε��Ѵ�. CIK �÷����ο��� �̹� Y,Z(�޼���ǥ���)�� �ٲ�� �ֱ�
			//������ �̺κ��� �״�� �ε��ϸ� ��.
			fgets(buffer, MAX_BUFFER, fp);
			sscanf(buffer, "%f%f%f", &Object[i].VertexList[j].x,
				&Object[i].VertexList[j].y,
				&Object[i].VertexList[j].z);
		}
		
		for(j = 0; j < Object[i].TotalFace * 3; j+=3)
		{
			//���̽� ������ �ε���
			fgets(buffer, MAX_BUFFER, fp);
			sscanf(buffer, "%d%d%d", &Object[i].FaceList[j + 0],
				 					 &Object[i].FaceList[j + 1],
									 &Object[i].FaceList[j + 2]);
		}
		// ���̾�α� üũ�ڽ����� �ؽ��İ� ���� �Ǿ� �־������� �ؽ��ĸ� �ε���
		if(CheckOption.IncludeMT == true || CheckOption.IncludeMTA == true)
		{
			for(j = 0; j < Object[i].TotalTV; j++)
			{
				fgets(buffer, MAX_BUFFER, fp);
				sscanf(buffer, "%f%f%f", &Object[i].VertexList[j].tu,
					&Object[i].VertexList[j].tv,
					&Object[i].VertexList[j].tz);			
			}
			for(j = 0; j < Object[i].TotalFace * 3; j+=3)
			{
				//���̽� ������ �ε���. ������ �ε� �����Ƿ� �׳� �ǳ� ��.
				// ���߿� Ȯ���� �ؾ���..
				fgets(buffer, MAX_BUFFER, fp);
				sscanf(buffer, "%d%d%d", &Object[i].UVFaceList[j + 0],
					 					 &Object[i].UVFaceList[j + 1],
										 &Object[i].UVFaceList[j + 2]);
			}
		}	
		
		// PROP_MOTIONBLUR �� 3������. ���� �ʿ������ ����
		fgets(buffer, MAX_BUFFER, fp);
		// ��Ʈ���� REF! �� ������Ʈ�� � ��Ʈ������ ����ϰ� �ִ���. �����Ű�� �ִ���
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%d", &Object[i].REF);
	}
	return 0;
}

///////////
// �Լ��� : void PrintBuffer()
// ���� ���ۿ� ����Ǿ� �ִ� ���� �ٽ� Text ���Ϸ� ����� �ִ� ��Ȱ�� �Ѵ�.
// �׷��� ���� ��� �Ǿ��ֳ� Ȯ�� �� �� �ִ� ���̴�. ���� �˰��̶� �����մϴ�.
///////////
void CCIK::PrintBuffer(char *FileName)
{
	FILE *fp = fopen(FileName, "w");

	fprintf(fp, "%d %d %d %d %d %d %d %d\n", CheckOption.IncludeM, CheckOption.IncludeMT, CheckOption.IncludeMTA,
										   CheckOption.Controllers, CheckOption.IncludeAKey, CheckOption.IncludeAForce,
										   CheckOption.IncludeAPhy, CheckOption.IncludeAAni);

	fprintf(fp, "%d\n", Scene.FirstFrame);
	fprintf(fp, "%d\n", Scene.LastFrame);
	fprintf(fp, "%d\n", Scene.FreamSpeed);
	fprintf(fp, "%d\n", Scene.TicksperFrame);

	fprintf(fp, "*TotalMO %d %d\n", TotalM, TotalO);

	for(int i = 0; i < TotalM; i++)
	{
		fprintf(fp,"%f%f%f\n"	, Matial[i].Material.Ambient.r, Matial[i].Material.Ambient.g, Matial[i].Material.Ambient.b); 	
		fprintf(fp,"%f%f%f\n"	, Matial[i].Material.Diffuse.r, Matial[i].Material.Diffuse.g, Matial[i].Material.Diffuse.b);	
		fprintf(fp,"%f%f%f\n"	, Matial[i].Material.Specular.r,Matial[i].Material.Specular.g,Matial[i].Material.Specular.b);	

		fprintf(fp,"%d %s\n", Matial[i].TextureCheck, Matial[i].TextureName);
	}

	for(i = 0; i < TotalO; i++)
	{
		fprintf(fp,"%f %f %f\n", Object[i].TM._11, Object[i].TM._12, Object[i].TM._13); 
		fprintf(fp,"%f %f %f\n", Object[i].TM._21, Object[i].TM._22, Object[i].TM._23); 
		fprintf(fp,"%f %f %f\n", Object[i].TM._31, Object[i].TM._32, Object[i].TM._33); 
		fprintf(fp,"%f %f %f\n", Object[i].TM._41, Object[i].TM._42, Object[i].TM._43); 

		fprintf(fp,"%d %d %d\n", Object[i].TotalVertex, Object[i].TotalFace, Object[i].TotalTV);

		for(int j = 0; j < Object[i].TotalVertex; j++)
		{
			fprintf(fp,"%f %f %f\n", Object[i].VertexList[j].x, Object[i].VertexList[j].y, Object[i].VertexList[j].z);
		}
		for(j = 0; j < Object[i].TotalFace * 3; j+=3)
		{
			fprintf(fp, "%d %d %d\n", Object[i].FaceList[j + 0],Object[i].FaceList[j + 1],Object[i].FaceList[j + 2]); 		
		}
	}
	fclose(fp);
}

///////////
// �Լ��� : void DestroyObject()
// �� �Լ��� ���α׷� �� �������� ȣ���ϴ� �Լ��� �Լ��� �״�� ������Ʈ���� �����ش�.
// �� �޸� ���� �����ٲ� �����ְ� �׷� ��Ȱ�� �ϴ°��̴�.
///////////
void CCIK::DestroyObject()
{
	if(Object)
	{
		delete[] Object;
		Object = NULL;
	}
	
	if(Matial)
	{
		delete[] Matial;
		Matial = NULL;
	}
}
////////////
// �Ҹ��� //
GEOMOBJECT::~GEOMOBJECT()
{
	// ���ؽ� ����Ʈ�� ���̽� ����Ʈ�� �޸𸮸� �����Ѵ�.
	if(VertexList)
	{
		delete[] VertexList;
		VertexList = NULL;
	}
	
	if(FaceList)
	{
		delete[] FaceList;
		FaceList = NULL;
	}

	if(UVFaceList)
	{
		delete[] UVFaceList;
		UVFaceList = NULL;
	}
}
////////////
// �Ҹ��� //
CCIK::~CCIK()
{
	DestroyObject(); //�޸� ���� ���� ����..
}
