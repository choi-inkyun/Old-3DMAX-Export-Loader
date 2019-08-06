/*
제작 : 최인균
제작 시작 : 2003년 1월 2일
쓰임 : 3D MAX EXPORT 플러그인 파일인 CIK 포멧을 불러들여 3D 공간에 출력
최종 수정 : 0000년 0월 0일
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
char buffer[MAX_BUFFER]; //fgets 로 값을 읽어들일 변수
char dust[MAX_BUFFER];   //dust 뜻 그대로 쓰레기. 쓰레기값 넣는 변수
// 자세히 설명 하자면 뭐... buffer 함수에서 fgets 로 불러온 다음에 sscanf 로 불러올때
// 필요없는 값은 dust 를 써서 불러와서 아무것도 처리하지 않는다란 그럼 의미

// 좌표값만 있는 기본적인 형.
struct CUSTOMVERTEX
{
    FLOAT x, y, z;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ)
////////////
// 생성자 //
CCIK::CCIK()
{	
	fp = NULL;	
	TotalM = 0; TotalO = 0;
	Matial = NULL;	
	Object = NULL;		
}
////////////
// 생성자 //
MATERIAL::MATERIAL()
{
	TextureCheck = false;
	memset(TextureName, NULL, MAX_PATH);
}
////////////
// 생성자 //
GEOMOBJECT::GEOMOBJECT()
{
	TotalVertex = NULL;
	TotalFace = NULL;
	TotalTV = NULL;
	REF = NULL;
}
////////////
// 함수명 : void DoLoadCIK(char *FileName) ★ 중요 Start~
// CIK 파일을 로드 하기 위해서 가장 먼저 호출해야할 함수.
// 읽어들일 CIK 파일 이름을 요구한다.  ex : "aaa.cik"
///////////
void CCIK::DoLoadCIK(char *FileName)
{
	DestroyObject(); //이전 객체 제거
	
	fp = fopen(FileName, "r"); //CIK 파일을 열기
	
    //////// Load ////////
	LoadInfo(); //기본적 정보. SCENE 에 해당된다.
	LoadMaterial(); //메트리얼 정보.
	LoadObject(); //오브젝트 정보.
	//////////////////////
	
	int aa= 4;
	int *ab = new int[aa];
	fclose(fp); //CIK 파일을 닫는다
}
////////////
// 함수명 : int LoadInfo()
// DoLoadCIK() 함수에서 먼저 불러지는 함수
// CIK 파일 위에있는 기본적인 정보들을 로드 할 수 있다.
// 어떤 다이얼로그에서 옵션을 선택했던지 불러와지는 함수
////////////
int CCIK::LoadInfo()
{
	/// 맨 윗줄에 있는 다이얼로그박스 체크 정보를 얻어옴 ///
	//방식은 fgets 로 한줄을 읽어온다음 그 것에 대해서 띄어쓰기 단위로 하나씩 다시 읽어들여 각버퍼에 저장한다.
	fgets(buffer, MAX_BUFFER, fp); 
	sscanf(buffer, "%d%d%d%d%d%d%d%d", &CheckOption.IncludeM,    &CheckOption.IncludeMT, 
		&CheckOption.IncludeMTA,  &CheckOption.Controllers,
		&CheckOption.IncludeAKey, &CheckOption.IncludeAForce,
		&CheckOption.IncludeAPhy, &CheckOption.IncludeAAni);
	///////////////////////////////////////////////////////
	// 그 다음 두줄은 그냥 넘깁니다 // 
	// - 3DSMAX_ASCIIEXPORT  'CIK' Format -
	// *버전 :  1.0v *제작자 : 최인균 - "Fri Jan 03 11:16:35 2003"
	fgets(buffer, MAX_BUFFER, fp);
	// 이 파일이 CIK 파일이 맞는지 검사합니다.
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
	//SCENE_BACKGROUND_STATIC SCENE_AMBIENT_STATIC 이지만 장당은 필요없어서 Pass
	fgets(buffer, MAX_BUFFER, fp);
	fgets(buffer, MAX_BUFFER, fp);
	fgets(buffer, MAX_BUFFER, fp);
	/////////////// Scene Load End ////////////////
	return 0;
}
////////////
// 함수명 : int LoadMaterial()
// 함수명에서 보듯이 CIK 포멧안의 메트리얼 정보를 로드하는 함수이다.
// 이것도 LoadInfo() 함수와 같이 어떤 파일이던지 로드가 되어야 할 것이다.
////////////
int CCIK::LoadMaterial()
{
	fgets(buffer, MAX_BUFFER, fp); //여기서 CIK 파일안에있는 메트리얼 갯수랑 오브젝트 갯수를 불러온다
	sscanf(buffer, "%s%d%d", &dust, &TotalM, &TotalO); //중요하고 자주 쓰이니 기억해 둘것
	
	// 딱 보면 알꺼라 생각한다. 위에서 불러들인 메트리얼과 오브젝트 갯수만큼 메모리
	// 할당을 해준다. 그렇게 해야지 불러올수 있기 때문이다. 그리고 사용이 끝난 후에
	// 메모리 유수를 막기위해 메모리 해제를 시켜줘야한다.
	Matial = new MATERIAL[TotalM];
	Object = new GEOMOBJECT[TotalO];	
	
	for(int i = 0; i < TotalM; i++){ //메트리얼 수만큼 for 문을 돌려서 읽어들인다.
		fgets(buffer, MAX_BUFFER, fp); // M 숫자. 번호를 가르킴
		
		fgets(buffer, MAX_BUFFER, fp); //메트리얼 정보. Ambient
		sscanf(buffer, "%f%f%f",   &Matial[i].Material.Ambient.r,
						 		   &Matial[i].Material.Ambient.g,
								   &Matial[i].Material.Ambient.b);
		
		fgets(buffer, MAX_BUFFER, fp); //메트리얼 정보. Diffuse
		sscanf(buffer, "%f%f%f",   &Matial[i].Material.Diffuse.r,
			 					   &Matial[i].Material.Diffuse.g,
								   &Matial[i].Material.Diffuse.b);
		
		fgets(buffer, MAX_BUFFER, fp); //메트리얼 정보. Specular
		sscanf(buffer, "%f%f%f",  &Matial[i].Material.Specular.r,
								  &Matial[i].Material.Specular.g,
								  &Matial[i].Material.Specular.b);
		
		// 밑에 4줄은 일단 필요없어서 보류. 그냥 건너뜀.
		// 나중에 필요할 경우 변수 생성후 읽어 들이면 될것이고
		// 결국 필요없다고 느껴지면 Export 플러그인과 여기서 삭제함.
		fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);
		
		// Texture 정보를 읽어들임.
		// Texture 가 쓰이지 않은 메트리얼 이라면 NULL 값일 것이다. 이것에 대한 처리를 봐야한다.
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%s%s", &dust, Matial[i].TextureName);
		if(strcmp(Matial[i].TextureName, "NULL")) //여기가 체크부분!. 텍스쳐가 있는지 없는지
			Matial[i].TextureCheck = true; //텍스쳐가 있다면 true 로
	}
	return 0;	
}		
///////////
// 함수명 : int LoadObject()
// GEOMOBJECT 를 로드한다. 다이얼로그 박스에서 선택한 옵션에 따라 차이가 날 것이다.
// 그것에 대한 예외처리를 잘 봐주길 바란다.
///////////
int CCIK::LoadObject()
{
	for(int i = 0; i < TotalO; i++){
		fgets(buffer, MAX_BUFFER, fp);
		// 오브젝트의 처음. TM 을 로드함. TM_ROW0
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
		// 이지만 지금은 별로 필요 없어보여서 PASS. 필요할떄 불러서 쓰면 됨.
		fgets(buffer, MAX_BUFFER, fp);	fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);	fgets(buffer, MAX_BUFFER, fp);
		fgets(buffer, MAX_BUFFER, fp);	fgets(buffer, MAX_BUFFER, fp);
		
		// 버텍스 총갯수, Face 갯수, 텍스쳐 좌표 갯수.
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%d%d%d", &Object[i].TotalVertex, &Object[i].TotalFace, &Object[i].TotalTV);
		// Vertex 만큼 Face 만큼 메모리를 할당해줌 
		Object[i].VertexList = new Vertex[Object[i].TotalVertex];
		Object[i].FaceList = new DWORD[Object[i].TotalFace * 3];
		Object[i].UVFaceList = new DWORD[Object[i].TotalFace * 3];
		
		for(int j = 0; j < Object[i].TotalVertex; j++)
		{
			//버텍스를 로드한다. CIK 플러그인에서 이미 Y,Z(왼손좌표계로)가 바뀌어 있기
			//때문에 이부분은 그대로 로드하면 됨.
			fgets(buffer, MAX_BUFFER, fp);
			sscanf(buffer, "%f%f%f", &Object[i].VertexList[j].x,
				&Object[i].VertexList[j].y,
				&Object[i].VertexList[j].z);
		}
		
		for(j = 0; j < Object[i].TotalFace * 3; j+=3)
		{
			//페이스 정보를 로드함
			fgets(buffer, MAX_BUFFER, fp);
			sscanf(buffer, "%d%d%d", &Object[i].FaceList[j + 0],
				 					 &Object[i].FaceList[j + 1],
									 &Object[i].FaceList[j + 2]);
		}
		// 다이얼로그 체크박스에서 텍스쳐가 포함 되어 있었을때면 텍스쳐를 로드함
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
				//페이스 정보를 로드함. 위에서 로드 했으므로 그냥 건너 뜀.
				// 나중에 확인을 해야할..
				fgets(buffer, MAX_BUFFER, fp);
				sscanf(buffer, "%d%d%d", &Object[i].UVFaceList[j + 0],
					 					 &Object[i].UVFaceList[j + 1],
										 &Object[i].UVFaceList[j + 2]);
			}
		}	
		
		// PROP_MOTIONBLUR 등 3개지만. 당장 필요없음을 느낌
		fgets(buffer, MAX_BUFFER, fp);
		// 메트리얼 REF! 이 오브젝트가 어떤 메트리얼을 사용하고 있는지. 적용시키고 있는지
		fgets(buffer, MAX_BUFFER, fp);
		sscanf(buffer, "%d", &Object[i].REF);
	}
	return 0;
}

///////////
// 함수명 : void PrintBuffer()
// 지금 버퍼에 저장되어 있는 값을 다시 Text 파일로 출력해 주는 역활을 한다.
// 그래서 값이 어떻게 되어있나 확인 할 수 있는 것이다. 보면 알것이라 생각합니다.
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
// 함수명 : void DestroyObject()
// 이 함수는 프로그램 맨 마지막에 호출하는 함수로 함수명 그대로 오브젝트들을 없애준다.
// 즉 메모리 해제 시켜줄껀 시켜주고 그런 역활을 하는것이다.
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
// 소멸자 //
GEOMOBJECT::~GEOMOBJECT()
{
	// 버텍스 리스트와 페이스 리스트의 메모리를 삭제한다.
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
// 소멸자 //
CCIK::~CCIK()
{
	DestroyObject(); //메모리 해제 등을 위한..
}

