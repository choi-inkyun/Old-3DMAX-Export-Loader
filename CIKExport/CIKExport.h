/**********************************************************************
 *<
	FILE: CIKExport.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __CIKEXPORT__H
#define __CIKEXPORT__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#define CIKEXPORT_CLASS_ID Class_ID(0x435610fd, 0x16c174bd)

class MtlKeeper {
public:
	BOOL	AddMtl(Mtl* mtl);
	int		GetMtlID(Mtl* mtl);
	int		Count();
	Mtl*	GetMtl(int id);

	Tab<Mtl*> mtlTab;
};

class CIKExport : public SceneExport {
	public:


		FILE		*pStream; // ���� ��ü �ְ�
		Interface*	ip;
		void		ExportStart(const TCHAR *name); //���ϸ��� ���ڷ�..
		int			ExportInfo();
		void		ExportEnd();
		TCHAR*		FixupName(TCHAR* name);//�����̸� ������ "xxx.max"
		void		ExportMaterialList();
		void		PreProcess(INode* node, int& nodeCount); //�Ѹ�Ʈ���� ���� �˾ƿ���?
		int			nTotalNodeCount; //�˼����º���. ���� �Լ��� 2���� ���ڿ� ��
		int			nCurNode;
		void		DumpMaterial(Mtl* mtl, int mtlID, int indentLevel);
		TimeValue	nStaticFrame;
		void		ExportTexture(Texmap* tex, Class_ID cid, int subNo, float amt, int indentLevel);
		void		DumpPoint3Keys(Control* cont, int indentLevel) ;
		void		ExportAnimForces(INode* node, int indentLevel); //�ִϸ��̼�
		void		ExportPhysiqueData( INode* node, Modifier* mod ); //����ũ
		inline		TimeValue GetStaticFrame()		{ return nStaticFrame; }
	
		inline BOOL	GetIncludeM()					{ return bIncludeM; } //�޽�����
		inline BOOL	GetIncludeMT()					{ return bIncludeMT; } //�޽��� �ؽ���
		inline BOOL	GetIncludeMTA()					{ return bIncludeMTA; } //�޽����ؽ��Ķ� �ִ�
		inline BOOL	GetIncludeFrame()				{ return bIncludeFrame; } //�����Ӹ�
		inline BOOL	GetIncludeAKey()				{ return bIncludeAKey; } //Ű�ִ�
		inline BOOL	GetIncludeAForce()				{ return bIncludeAForce; } //���������ִ�
		inline BOOL	GetIncludeAPhy()				{ return bIncludeAPhy; } //����ũ�ִ�
		inline BOOL	GetIncludeAAni()				{ return bIncludeAAni; } //�ִ�
		inline int	GetPrecision()				    { return Precision; } // �ڸ���
		
		inline void	SetIncludeM(BOOL val)			{ bIncludeM = val; }
		inline void	SetIncludeMT(BOOL val)			{ bIncludeMT = val; }
		inline void	SetIncludeMTA(BOOL val)			{ bIncludeMTA = val; }
		inline void	SetIncludeFrame(int val)		{ bIncludeFrame = val; }
		inline void	SetIncludeAKey(BOOL val)		{ bIncludeAKey = val; }
		inline void	SetIncludeAForce(int val)		{ bIncludeAForce = val; }
		inline void	SetIncludeAPhy(int val)		    { bIncludeAPhy = val; }
		inline void	SetIncludeAAni(int val)		    { bIncludeAAni = val; }
		inline void SetStaticFrame(TimeValue val)	{ nStaticFrame = val; }
		inline void	SetPrecision(int val)		    { Precision = val; }
		
		TSTR		GetIndent(int indentLevel);
		void		CommaScan(TCHAR* buf);
		BOOL		nodeEnum(INode* node, int indentLevel, int number);
		void		ExportGeomObject(INode* node, int indentLevel);
		void		DumpMatrix3(Matrix3* m, int indentLevel);
		void		ExportMesh(INode* node, TimeValue t, int indentLevel);
		BOOL		TMNegParity(Matrix3 &m);
		TriObject*	GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt);
		void		ExportMaterial(INode* node, int indentLevel);
					IsKnownController(Control* cont);
		void		ExportAnimKeys(INode* node, int indentLevel); 
		void		DumpScaleKeys(Control* cont, int indentLevel);
		void		DumpRotKeys(Control* cont, int indentLevel);
		void  	    DumpPosKeys(Control* cont, int indentLevel);
		BOOL		CheckForAnimation(INode* node, BOOL& bPos, BOOL& bRot, BOOL& bScale);
		void		DumpPosSample(INode* node, int indentLevel);
		void		DumpRotSample(INode* node, int indentLevel);
		void		DumpScaleSample(INode* node, int indentLevel);
		void		ExportAniObject( INode* node ); //�̰͵� �ִ�
		// A collection of overloaded value to string converters.
		TSTR		Format(int value);
		TSTR		Format(float value);
		TSTR		Format(Color value);
		TSTR		Format(Point3 value); 
		TSTR		Format(AngAxis value); 
		TSTR		Format(Quat value);
		TSTR		Format(ScaleValue value);
			
		
		static HWND hParams;

		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);


		
		//Constructor/Destructor

		CIKExport();
		~CIKExport();		
	private:

	BOOL	bIncludeM;
	BOOL	bIncludeMT;
	BOOL	bIncludeMTA;
	BOOL	bIncludeAKey;
	BOOL	bIncludeAForce;
	BOOL	bIncludeAPhy;
	BOOL	bIncludeAAni;
	int     bIncludeFrame;
	int     Precision;

	MtlKeeper	mtlList;
	TCHAR		szFmtStr[16];

};


class CIKExportClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new CIKExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return CIKEXPORT_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("CIKExport"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};


#endif // __CIKEXPORT__H
