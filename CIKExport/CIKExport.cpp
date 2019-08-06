/**********************************************************************
 *<
	FILE: CIKExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: ���α� 

	HISTORY: ������. 2002 �� 12�� 15��

���̾�α� �ڽ����� ������ �ɼ�..
- 3DSMAX_ASCIIEXPORT  'CIK' Format -
*���� :  1.0v *������ : ���α� - ������ ��¥
�����̸�("xx.max")
SCENE_FILENAME
SCENE_FIRSTFRAME
SCENE_LASTFRAME
SCENE_FRAMESPEED
SCENE_TICKSPERFRAME
SCENE_BACKGROUND_STATIC
SCENE_AMBIENT_STATIC
*TOTALMO ��Ʈ�����Ѽ��� ������Ʈ�Ѽ���
M ��ȣ -> �̰͸�ŭ FOR��
AMBIENT
DIFFUSE
SPECULAR
SHINE
SHINE_STRENGTH
TRANSPARENCY
WIRESIZE
TEXTURE
G ��ȣ -> �̰͸�ŭ FOR��
TM_ROW0
TM_ROW1
TM_ROW2
TM_ROW3
TM_POS
TM_ROTAXIS
TM_ROTANGLE
TM_SCALE
TM_SCALEAXIS
TM_SCALEAXISANG
���ý� �� ����
���̽� �� ����
���ý� ������ŭ FOR �� X,Y,Z.
���̽� ������ŭ FOR �� A,B,C 
{Animation - ���ÿɼ�
Key
Force
Phy
ani
}
Ref��
 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "CIKExport.h"

static CIKExportClassDesc CIKExportDesc;
ClassDesc2* GetCIKExportDesc() { return &CIKExportDesc; }

Modifier* FindPhysiqueModifier( INode* node );


BOOL CALLBACK CIKExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static CIKExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (CIKExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;
	}
	return FALSE;
}


//--- CIKExport -------------------------------------------------------
CIKExport::CIKExport()
{
	nStaticFrame = 0;
	Precision = 0;
}

CIKExport::~CIKExport() 
{

}

BOOL CIKExport::TMNegParity(Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

TriObject* CIKExport::GetTriObjectFromNode(INode *node, TimeValue t, int &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) { 
		TriObject *tri = (TriObject *) obj->ConvertToType(t, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}
	else {
		return NULL;
	}
}


BOOL CIKExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;

#define CTL_CHARS  31
#define SINGLE_QUOTE 39

}
// �ǵ帮�� �ʾƵ� ��. //
TCHAR* CIKExport::FixupName(TCHAR* name)
{
	static char buffer[256];
	TCHAR* cPtr;
	
    _tcscpy(buffer, name);
    cPtr = buffer;
	
    while(*cPtr) {
		if (*cPtr == '"')
			*cPtr = SINGLE_QUOTE;
        else if (*cPtr <= CTL_CHARS)
			*cPtr = _T('_');
        cPtr++;
    }
	
	return buffer;
}

DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}

// �ǵ帮�� �ʾƵ� ��. //
void CIKExport::PreProcess(INode* node, int& nodeCount)
{
	nodeCount++;
	// Add the nodes material to out material list
	// Null entries are ignored when added...
	mtlList.AddMtl(node->GetMtl());

	// For each child of this node, we recurse into ourselves 
	// and increment the counter until no more children are found.
	for (int c = 0; c < node->NumberOfChildren(); c++) {
		PreProcess(node->GetChildNode(c), nodeCount);
	}
}

// ���� ó������ export ���� �ҷ��ִ�.. ���� ���
int CIKExport::ExportInfo()
{
	Interval range = ip->GetAnimRange();

	struct tm *newtime;
	time_t aclock;

	time( &aclock );
	newtime = localtime(&aclock);

	TSTR today = _tasctime(newtime);	// The date string has a \n appended.
	today.remove(today.length()-1);		// Remove the \n

	fprintf(pStream, "%s\n", "- 3DSMAX_ASCIIEXPORT  'CIK' Format -");
	fprintf(pStream, "%s - \"%s\"\n", "*���� :  1.0v *������ : ���α�", today);

	//Scene ����
	fprintf(pStream, "%d\n",  range.Start() / GetTicksPerFrame()); // FIRSTFRAME
	fprintf(pStream, "%d\n",  range.End() / GetTicksPerFrame()); // LASTFRAME 
	fprintf(pStream, "%d\n",  GetFrameRate()); // FRAMESPEED
	fprintf(pStream, "%d\n",  GetTicksPerFrame()); // TICKSPERFRAME

	Texmap* env = ip->GetEnvironmentMap();

	Control* ambLightCont;
	Control* bgColCont;
	
	if (env) {
		// Output environment texture map
		ExportTexture(env, Class_ID(0,0), 0, 1.0f, 0);
	}
	else {
		// Output background color
		bgColCont = ip->GetBackGroundController();
		if (bgColCont && bgColCont->IsAnimated()) {
			// background color is animated.
			fprintf(pStream, "{\n");

			DumpPoint3Keys(bgColCont, 0);
				
			fprintf(pStream, "\t}\n");
		}
		else {
			// Background color is not animated
			Color bgCol = ip->GetBackGround(GetStaticFrame(), FOREVER);
			fprintf(pStream, "%s\n", Format(bgCol));
		}
	}
	
	// Output ambient light
	ambLightCont = ip->GetAmbientController();
	if (ambLightCont && ambLightCont->IsAnimated()) {
		// Ambient light is animated.
		fprintf(pStream, "{\n");
		
		DumpPoint3Keys(ambLightCont, 0);
		
		fprintf(pStream, "\t}\n");
	}
	else {
		// Ambient light is not animated
		Color ambLight = ip->GetAmbient(GetStaticFrame(), FOREVER);
		fprintf(pStream, "%s\n", Format(ambLight));
	}

	//Scene ��
	return FALSE;
}
//SCENE_AMBIENT_STATIC  ���� �̾Ƴ��� scene �߿� �Լ�. �� �𸣰���-_-;
void CIKExport::DumpPoint3Keys(Control* cont, int indentLevel) 
{
	if (!cont)	// Bug out if no controller.
		return;
	
	int i;
	TSTR indent = GetIndent(indentLevel);
	IKeyControl *ikc = NULL;

		ikc = GetKeyControlInterface(cont);
	
	// TCB point3
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POINT3_CLASS_ID, 0)) {
		fprintf(pStream,"%s\t\t%s {\n", indent.data(), "*CONTROL_POINT3_TCB"); 
		for (i=0; i<ikc->GetNumKeys(); i++) {
			ITCBPoint3Key key;
			ikc->GetKey(i, &key);
			fprintf(pStream, "%s\t\t\t%s %d\t%s",
				indent.data(),
				"*CONTROL_TCB_POINT3_KEY",
				key.time,
				Format(key.val));
			// Add TCB specific data
			fprintf(pStream, "\t%s\t%s\t%s\t%s\t%s\n", Format(key.tens), Format(key.cont), Format(key.bias), Format(key.easeIn), Format(key.easeOut));
		}
		fprintf(pStream,"%s\t\t}\n", indent.data());
	}
	// Bezier point3
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POINT3_CLASS_ID, 0)) {
		fprintf(pStream,"%s\t\t%s {\n", indent.data(), "*CONTROL_POINT3_BEZIER"); 
		for (i=0; i<ikc->GetNumKeys(); i++) {
			IBezPoint3Key key;
			ikc->GetKey(i, &key);
			fprintf(pStream, "%s\t\t\t%s %d\t%s",
				indent.data(),
				"*CONTROL_BEZIER_POINT3_KEY",
				key.time, 
				Format(key.val));
			fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);
		}
		fprintf(pStream,"%s\t\t}\n", indent.data());
	}
	// Bezier color
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_COLOR_CLASS_ID, 0)) {
		fprintf(pStream,"%s\t\t%s {\n", indent.data(), "*CONTROL_POINT3_BEZIER"); 
		for (i=0; i<ikc->GetNumKeys(); i++) {
			IBezPoint3Key key;
			ikc->GetKey(i, &key);
			fprintf(pStream, "%s\t\t\t%s %d\t%s", 
				indent.data(),
				"*CONTROL_BEZIER_POINT3_KEY",
				key.time,
				Format(key.val));
			fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);
		}
		fprintf(pStream,"%s\t\t}\n", indent.data());
	}
	else {
		
		// Unknown controller, no key interface or sample on demand -
		// This might be a procedural controller or something else we
		// don't know about. The last resort is to get the value from the 
		// controller at every n frames.
		
		TSTR name;
		cont->GetClassName(name);
		fprintf(pStream,"%s\t\t%s \"%s\" {\n", indent.data(), "*CONTROL_POINT3_SAMPLE",
			FixupName(name));
		
		// If it is animated at all...
		if (cont->IsAnimated()) {
			// Get the range of the controller animation 
			Interval range; 
			// Get range of full animation
			Interval animRange = ip->GetAnimRange(); 
			TimeValue t = cont->GetTimeRange(TIMERANGE_ALL).Start();
			Point3 value;
			
			// While we are inside the animation... 
			while (animRange.InInterval(t)) {
				// Sample the controller
				range = FOREVER;
				cont->GetValue(t, &value, range);
				
				// Set time to start of controller validity interval 
				t = range.Start();
				
				// Output the sample
				fprintf(pStream, "%s\t\t\t%s %d\t%s\n",
					indent.data(),
					"*CONTROL_POINT3_KEY",
					t,
					Format(value));
				
				// If the end of the controller validity is beyond the 
				// range of the animation
				if (range.End() > cont->GetTimeRange(TIMERANGE_ALL).End()) {
					break;
				}

			}
		}
		fprintf(pStream,"%s\t\t}\n", indent.data());
	}
}

void CIKExport::ExportTexture(Texmap* tex, Class_ID cid, int subNo, float amt, int indentLevel)
{
//	fprintf(pStream, "*ExportTexture\n");
	TSTR mapName = ((BitmapTex *)tex)->GetMapName();
	fprintf(pStream,"\"%s\"\n", FixupName(mapName));
}

void CIKExport::ExportMaterialList()
{

	int numMtls = mtlList.Count(); //��Ʈ���� ����
	int numChildren = ip->GetRootNode()->NumberOfChildren(); //������Ʈ ����

	fprintf(pStream, "%s %d %d\n", "*TotalMO", numMtls, numChildren); //��Ʈ���� ����, ������Ʈ ����

	for (int i=0; i<numMtls; i++) { //��Ʈ���� ����ŭ FOR~
		DumpMaterial(mtlList.GetMtl(i), i, 0);
	}
}

void CIKExport::DumpMaterial(Mtl* mtl, int mtlID, int indentLevel)
{
	bool check = 0;
	SetStaticFrame(4);
	TimeValue t = GetStaticFrame();

	if (!mtl) return;
	
	fprintf(pStream,"M %d\n", mtlID);

	if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
		StdMat* std = (StdMat*)mtl;

		Color value = std->GetAmbient(t);

		fprintf(pStream,"%s\n", Format(std->GetAmbient(t)));
		fprintf(pStream,"%s\n", Format(std->GetDiffuse(t)));
		fprintf(pStream,"%s\n", Format(std->GetSpecular(t)));
		fprintf(pStream,"%s\n", Format(std->GetShininess(t)));
		fprintf(pStream,"%s\n", Format(std->GetShinStr(t)));
		fprintf(pStream,"%s\n", Format(std->GetXParency(t)));
		fprintf(pStream,"%s\n", Format(std->WireSize(t)));
	}

	fprintf(pStream,"*Texture ");

	for (int i=0; i<mtl->NumSubTexmaps(); i++) {
		Texmap* subTex = mtl->GetSubTexmap(i);
		float amt = 1.0f;
		if (subTex) {
			// If it is a standard material we can see if the map is enabled.
			if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
				if (!((StdMat*)mtl)->MapEnabled(i))
					continue;
				amt = ((StdMat*)mtl)->GetTexmapAmt(i, 0);
				
			}
			ExportTexture(subTex, mtl->ClassID(), i, amt, indentLevel+1);
			check = 1;
		}
	}
	if(check == 0)
		fprintf(pStream,"NULL\n");
	else
		check = 1;
}
// ������Ʈ �κ�
BOOL CIKExport::nodeEnum(INode* node, int indentLevel, int number) 
{
	fprintf(pStream,"G %d\n", number);

	Matrix3 pivot = node->GetNodeTM(GetStaticFrame());
	TSTR indent = GetIndent(indentLevel);
	
	DumpMatrix3(&pivot, indentLevel+2);
	
	ExportGeomObject(node, indentLevel);

	return TRUE;
}
// TM ��..
void CIKExport::DumpMatrix3(Matrix3* m, int indentLevel)
{
	Point3 row;
	// Dump the whole Matrix
	row = m->GetRow(0);
	fprintf(pStream,"%s\n", Format(row));
	row = m->GetRow(1);
	fprintf(pStream,"%s\n", Format(row));
	row = m->GetRow(2);
	fprintf(pStream,"%s\n", Format(row));
	row = m->GetRow(3);
	fprintf(pStream,"%s\n", Format(row));
	
	// Decompose the matrix and dump the contents
	AffineParts ap;
	float rotAngle;
	Point3 rotAxis;
	float scaleAxAngle;
	Point3 scaleAxis;
	
	decomp_affine(*m, &ap);

	// Quaternions are dumped as angle axis.
	AngAxisFromQ(ap.q, &rotAngle, rotAxis);
	AngAxisFromQ(ap.u, &scaleAxAngle, scaleAxis);

	fprintf(pStream,"%s\n", Format(ap.t));
	fprintf(pStream,"%s\n", Format(rotAxis));
	fprintf(pStream,"%s\n", Format(rotAngle));
	fprintf(pStream,"%s\n", Format(ap.k));
	fprintf(pStream,"%s\n", Format(scaleAxis));
	fprintf(pStream,"%s\n", Format(scaleAxAngle));
}

// REF ��
void CIKExport::ExportMaterial(INode* node, int indentLevel)
{
	Mtl* mtl = node->GetMtl();
	
	// If the node does not have a material, export the wireframe color
	if (mtl) {
		int mtlID = mtlList.GetMtlID(mtl);
		if (mtlID >= 0) {
			fprintf(pStream,"%d\n", mtlID);
		}
	}
	else {
		DWORD c = node->GetWireColor();
		fprintf(pStream,"%s\n",
			Format(Color(GetRValue(c)/255.0f, GetGValue(c)/255.0f, GetBValue(c)/255.0f)));
	}
}
// ������Ʈ ��µ� �߿��Ѥ���~
void CIKExport::ExportGeomObject(INode* node, int indentLevel)
{
	ObjectState os = node->EvalWorldState(GetStaticFrame());

	if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		return;
	
	ExportMesh(node, GetStaticFrame(), indentLevel);

	// Node properties (only for geomobjects)
	fprintf(pStream, "%d %d %d\n",node->MotBlur(), node->CastShadows(), node->RcvShadows());

   //�ִϸ��̼�
   // �̷��� ���������� üũ�ڽ��̱⵵ �ϰ�, �ΰ��� �������ְ� �ϱ� ���ؼ�
	if(GetIncludeMTA()){ //�ִϸ��̼Ƿ� �ͽ���Ʈ �ϴ°ſ� üũ�Ǿ�������
		if(GetIncludeAKey()) //keyframe
			ExportAnimKeys(node, indentLevel);
		if(GetIncludeAForce()) //force
			ExportAnimForces(node, indentLevel);
		if(GetIncludeAPhy()){
			Modifier* mod = FindPhysiqueModifier( node );
			if( mod )	ExportPhysiqueData( node, mod );
		}
		if(GetIncludeAAni()){
			Interval range = ip->GetAnimRange();

			fprintf( pStream, "*NAME		%s\n", ip->GetCurFileName() );
			fprintf( pStream, "*LASTFRAME	%d\n", range.End() / GetTicksPerFrame() );
			ExportAniObject( node );
		}
	}
	//REF ��
	ExportMaterial(node, indentLevel);
}
// �������� ������Ʈ�� ���ؽ� ���̽� ���� ����� //
void CIKExport::ExportMesh(INode* node, TimeValue t, int indentLevel)
{
	Mtl* nodeMtl = node->GetMtl();
	Matrix3 tm = node->GetObjTMAfterWSM(t);
	BOOL negScale = TMNegParity(tm);
	int vx1, vx2, vx3;
	TSTR indent;
	
	indent = GetIndent(indentLevel+1);
	ObjectState os = node->EvalWorldState(t);
	if (!os.obj || os.obj->SuperClassID()!=GEOMOBJECT_CLASS_ID) {
		return; // Safety net. This shouldn't happen.
	}
	
	// Order of the vertices. Get 'em counter clockwise if the objects is
	// negatively scaled.
	if (negScale) {
		vx1 = 2;
		vx2 = 1;
		vx3 = 0;
	}
	else {
		vx1 = 0;
		vx2 = 1;
		vx3 = 2;
	}
	
	BOOL needDel;
	TriObject* tri = GetTriObjectFromNode(node, t, needDel);
	if (!tri) {
		return;
	}
	
	Mesh* mesh = &tri->GetMesh();
	
	mesh->buildNormals();

	int numVert = mesh->getNumVerts(); //���ؽ� ����
	int numFace = mesh->getNumFaces(); //���̽� ����
	int numTVx = mesh->getNumTVerts(); //�ؽ��� ���ؽ� ����

	assert(numVert != 0); // ���ؽ��� 0 �̸� ����

	fprintf(pStream, "%d %d %d\n",mesh->getNumVerts(), mesh->getNumFaces(), numTVx);

	// ���ؽ� ������ print ���ش�.//
	for (int i=0; i<numVert; i++) {
		Point3 v = tm * mesh->verts[i];
		fprintf(pStream, "%s\n", Format(v));
	}
    
	// ���̽� ������ print ���ش�.//
	for (i=0; i<numFace; i++) {
		fprintf(pStream,"%d %d %d %d %d %d ",
			mesh->faces[i].v[vx1],
			mesh->faces[i].v[vx2],
			mesh->faces[i].v[vx3],
			mesh->faces[i].getEdgeVis(vx1) ? 1 : 0,
			mesh->faces[i].getEdgeVis(vx2) ? 1 : 0,
			mesh->faces[i].getEdgeVis(vx3) ? 1 : 0);
	// MESH_SMOOTHING 
	for (int j=0; j<32; j++) {
		if (mesh->faces[i].smGroup & (1<<j)) {
			if (mesh->faces[i].smGroup>>(j+1)) {
				fprintf(pStream,"%d,",j+1); // Add extra comma
			} else {
				fprintf(pStream,"%d ",j+1);
				}
			}
		}
	//  MESH_MTLID
		fprintf(pStream,"%d", mesh->faces[i].getMatID());
		
		fprintf(pStream,"\n");
	}
	if(!GetIncludeM()){//mash �� ����ִ°��� �ƴҶ� �ؽ��ĵ�..
		if(numTVx){ //�ؽ��� ���ؽ��� 0 �� �ƴҶ��� �ؽ��� ���� �Ѹ� 
			for (i=0; i<numTVx; i++) { //�ؽ��� ���ؽ� ���� �ѷ���
				UVVert tv = mesh->tVerts[i];
				fprintf(pStream, "%s\n",Format(tv));
			}
			// UV �ؽ��� ��ǥ �����
			for (i=0; i<mesh->getNumFaces(); i++) {
				fprintf(pStream,"%d %d %d\n",
					mesh->tvFace[i].t[vx1],
					mesh->tvFace[i].t[vx2],
					mesh->tvFace[i].t[vx3]);
			}
		}
	}

}

void CIKExport::ExportStart(const TCHAR *name)
{
	pStream = _tfopen(name,_T("wt")); //���Ͽ���
//	pStream = fopen( name, "wb" );
	// ProgressStart()�Լ��� MAX�� Plug-in�� ó���� �� �� Progress bar�� ��� ����ϴ����� ���� �Լ��̴�.
	ip->ProgressStart(GetString(7), TRUE, fn, NULL);
}

// �ͽ���Ʈ�� �������� �Լ� ȣ�� //
void CIKExport::ExportEnd() 
{
	ip->ProgressEnd();

	fclose(pStream);
}	
// export ������ ó�� �ߴ� ���̾�α� �ڽ� ���ν��� //
static INT_PTR CALLBACK ExportDlgProc(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	CIKExport *exp = (CIKExport*)GetWindowLongPtr(hWnd,GWLP_USERDATA); 

	switch (msg) {
	case WM_INITDIALOG: //���̾�α� �����ɶ�
		exp = (CIKExport*)lParam;
		SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam); 
		CenterWindow(hWnd, GetParent(hWnd)); 
		SetDlgItemInt(hWnd, IDC_FRAME, 5, FALSE);
		SetDlgItemInt(hWnd, IDC_DECIMALS, 4, FALSE);
		break;
	case WM_COMMAND: //���̾�α� ��ɾ�
		switch (LOWORD(wParam)) {
		case IDOK: //������ �������� üũ ����� �ȉ�´��� �޾ƿ�
			exp->SetIncludeM(IsDlgButtonChecked(hWnd, IDC_RADIO2)); 
			exp->SetIncludeMT(IsDlgButtonChecked(hWnd, IDC_RADIO3)); 
			exp->SetIncludeMTA(IsDlgButtonChecked(hWnd, IDC_RADIO4)); 
			exp->SetIncludeAKey(IsDlgButtonChecked(hWnd, IDC_KEY)); 
			exp->SetIncludeAPhy(IsDlgButtonChecked(hWnd, IDC_PHY));
			exp->SetIncludeAAni(IsDlgButtonChecked(hWnd, IDC_ANI));
			exp->SetIncludeAForce(IsDlgButtonChecked(hWnd, IDC_FORCE)); 
			exp->SetIncludeFrame(GetDlgItemInt(hWnd, IDC_FRAME, NULL,FALSE));
			exp->SetPrecision(GetDlgItemInt(hWnd, IDC_DECIMALS, NULL,FALSE));
			EndDialog(hWnd, 1);
			break;

		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}
		break;
		default:
			return FALSE;
	}
	return TRUE;
}
// Main �Դϴ�. Export ���� //
int	CIKExport::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	ip = i;
	//�ɼ� ���̾�α� �ڽ� ���
	if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EXPORT_DLG),
		ip->GetMAXHWnd(), ExportDlgProc, (LPARAM)this)) {
			return 1;
	}

	sprintf(szFmtStr, "%%4.%df", Precision); //�ڸ��� ����. �̰� ���� format ���� ����

	ExportStart(name); // Start
	//����ڰ� ���̾�α� �ڽ����� � �ɼ��� �����ߴ��� �����
	fprintf(pStream, "%d %d %d %d %d %d %d %d\n"
		,GetIncludeM(), GetIncludeMT(), GetIncludeMTA(), GetIncludeFrame(),
		GetIncludeAKey(), GetIncludeAForce(),GetIncludeAPhy(),GetIncludeAAni());

	nTotalNodeCount = 0;
	nCurNode = 0;
	PreProcess(ip->GetRootNode(), nTotalNodeCount);
	
	assert(nTotalNodeCount != 0);

	ExportInfo(); //�⺻ ������ Scene print
	ExportMaterialList(); //��Ʈ����

	// ������Ʈ
	int numChildren = ip->GetRootNode()->NumberOfChildren();
	
	assert(numChildren != 0);
	for (int idx=0; idx<numChildren; idx++) { //�ڽ��� ����ŭ. ������Ʈ
		if (ip->GetCancel())
			break;
		nodeEnum(ip->GetRootNode()->GetChildNode(idx), 0, idx);
	}


	ExportEnd();

	return FALSE;
}


// Return an indentation string
TSTR CIKExport::GetIndent(int indentLevel)
{
	TSTR indentString = "";
	for (int i=0; i<indentLevel; i++) {
		indentString += "\t";
	}
	
	return indentString;
}

