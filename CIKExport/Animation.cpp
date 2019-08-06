#include "CIKExport.h"

#define ALMOST_ZERO 1.0e-3f //애니관련
BOOL EqualPoint3(Point3 p1, Point3 p2);
/////////// 쭉 애니메이션 관련 함수 들 ////////////////////////
void CIKExport::ExportAnimKeys(INode* node, int indentLevel) 
{
	BOOL bDoKeys = FALSE;

	// We can only export keys if all TM controllers are "known" to us.
	// The reason for that is that some controllers control more than what
	// they should. Consider a path position controller, if you turn on
	// follow and banking, this position controller will also control
	// rotation. If a node that had a path position controller also had a
	// TCB rotation controller, the TCB keys would not describe the whole
	// rotation of the node.
	// For that reason we will only export keys if all controllers
	// position, rotation and scale are linear, hybrid (bezier) or tcb.

	//애니메이션..
//	if (!GetIncludeAForce()) {
		Control* pC = node->GetTMController()->GetPositionController();
		Control* rC = node->GetTMController()->GetRotationController();
		Control* sC = node->GetTMController()->GetScaleController();

		if (IsKnownController(pC) && IsKnownController(rC) && IsKnownController(sC)) {
			bDoKeys = TRUE;
		}
//	}

//	if (bDoKeys) {
		// Only dump the track header i6f any of the controllers have keys
		if (node->GetTMController()->GetPositionController()->NumKeys() ||
			node->GetTMController()->GetRotationController()->NumKeys() ||
			node->GetTMController()->GetScaleController()->NumKeys()) {

			fprintf(pStream,"%s {\n", "ID_TM_ANIMATION"); 
			fprintf(pStream,"Name : \"%s\"\n",	FixupName(node->GetName()));

			DumpPosKeys(node->GetTMController()->GetPositionController(), indentLevel);
			DumpRotKeys(node->GetTMController()->GetRotationController(), indentLevel);
			DumpScaleKeys(node->GetTMController()->GetScaleController(), indentLevel);

			fprintf(pStream,"}\n");
		}
//	}
}
void CIKExport::ExportAnimForces(INode* node, int indentLevel) 
{
	BOOL bPosAnim;
	BOOL bRotAnim;
	BOOL bScaleAnim;

	if (CheckForAnimation(node, bPosAnim, bRotAnim, bScaleAnim)) {
		fprintf(pStream,"%s {\n", "ID_TM_ANIMATION"); 
		fprintf(pStream,"\"%s\"\n",	FixupName(node->GetName()));

		if (bPosAnim) {
			DumpPosSample(node, indentLevel);
		}
		if (bRotAnim) {
			DumpRotSample(node, indentLevel);
		}
		if (bScaleAnim) {
			DumpScaleSample(node, indentLevel);
		}

		fprintf(pStream,"}\n");
	}
}

BOOL CIKExport::CheckForAnimation(INode* node, BOOL& bPos, BOOL& bRot, BOOL& bScale)
{
	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end = ip->GetAnimRange().End();
	TimeValue t;
	int delta = GetTicksPerFrame();
	Matrix3 tm;
	AffineParts ap;
	Point3 firstPos;
	float rotAngle, firstRotAngle;
	Point3 rotAxis, firstRotAxis;
	Point3 firstScaleFactor;

	bPos = bRot = bScale = FALSE;

	for (t=start; t<=end; t+=delta) {
		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));

		decomp_affine(tm, &ap);

		AngAxisFromQ(ap.q, &rotAngle, rotAxis);

		if (t != start) {
			if (!bPos) {
				if (!EqualPoint3(ap.t, firstPos)) {
					bPos = TRUE;
					}
				}
			// MAX 2.x:
			// We examine the rotation angle to see if the rotation component
			// has changed.
			// Although not entierly true, it should work.
			// It is rare that the rotation axis is animated without
			// the rotation angle being somewhat affected.
			// MAX 3.x:
			// The above did not work, I have a repro scene that doesn't export a rotation track
			// because of this. I fixed it to also compare the axis.
			if (!bRot) {
				if (fabs(rotAngle - firstRotAngle) > ALMOST_ZERO) {
					bRot = TRUE;
					}
				else if (!EqualPoint3(rotAxis, firstRotAxis)) {
					bRot = TRUE;
					}
				}

			if (!bScale) {
				if (!EqualPoint3(ap.k, firstScaleFactor)) {
					bScale = TRUE;
					}
				}
			}
		else {
			firstPos = ap.t;
			firstRotAngle = rotAngle;
			firstRotAxis = rotAxis;
			firstScaleFactor = ap.k;
			}

		// No need to continue looping if all components are animated
		if (bPos && bRot && bScale)
			break;
		}

	return bPos || bRot || bScale;
}

// Not truly the correct way to compare floats of arbitary magnitude...
BOOL EqualPoint3(Point3 p1, Point3 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return FALSE;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return FALSE;

	return TRUE;
}

void CIKExport::DumpPosSample(INode* node, int indentLevel) 
{	
	fprintf(pStream,"%s {\n", "ID_POS_TRACK");

	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end = ip->GetAnimRange().End();
	TimeValue t;
	int delta = GetTicksPerFrame() * GetIncludeFrame();
	Matrix3 tm;
	AffineParts ap;
	Point3	prevPos;

	for (t=start; t<=end; t+=delta) {
		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));
		decomp_affine(tm, &ap);

		Point3 pos = ap.t;

		if (t!= start && EqualPoint3(pos, prevPos)) {
			// Skip identical keys 
			continue;
		}

		prevPos = pos;

		// Output the sample
		fprintf(pStream, "%s %d\t%s\n",
			"ID_POS_SAMPLE",
			t,
			Format(pos));
	}

	fprintf(pStream,"}\n");
}

void CIKExport::DumpRotSample(INode* node, int indentLevel) 
{	
	fprintf(pStream,"%s {\n", "ID_ROT_TRACK");

	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end = ip->GetAnimRange().End();
	TimeValue t;
	int delta = GetTicksPerFrame() * GetIncludeFrame();
	Matrix3 tm;
	AffineParts ap;
	Quat prevQ;

	prevQ.Identity();

	for (t=start; t<=end; t+=delta) {
		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));

		decomp_affine(tm, &ap);

		// Rotation keys should be relative, so we need to convert these
		// absolute samples to relative values.

		Quat q = ap.q / prevQ;
		prevQ = ap.q;

		if (q.IsIdentity()) {
			// No point in exporting null keys...
			continue;
		}

		// Output the sample
		fprintf(pStream, "%s %d\t%s\n",
			"ID_ROT_SAMPLE",
			t,
			Format(q));
	}

	fprintf(pStream,"}\n");
}

void CIKExport::DumpScaleSample(INode* node, int indentLevel) 
{	
	fprintf(pStream,"%s {\n", "ID_SCALE_TRACK");

	TimeValue start = ip->GetAnimRange().Start();
	TimeValue end = ip->GetAnimRange().End();
	TimeValue t;
	int delta = GetTicksPerFrame() * GetIncludeFrame();
	Matrix3 tm;
	AffineParts ap;
	Point3	prevFac;

	for (t=start; t<=end; t+=delta) {
		tm = node->GetNodeTM(t) * Inverse(node->GetParentTM(t));
		decomp_affine(tm, &ap);

		if (t!= start && EqualPoint3(ap.k, prevFac)) {
			// Skip identical keys 
			continue;
		}

		prevFac = ap.k;

		// Output the sample
		fprintf(pStream, "%s %d\t%s %s\n",
			"ID_SCALE_SAMPLE",
			t,
			Format(ap.k),
			Format(ap.u));
	}

	fprintf(pStream,"}\n");
}

void CIKExport::DumpPosKeys(Control* cont, int indentLevel) 
{
	if (!cont)
		return;
	
	int i;
	IKeyControl *ikc = GetKeyControlInterface(cont);
	
	// TCB position
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_POSITION_CLASS_ID, 0)) {
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) {
			fprintf(pStream,"%s {\n", "ID_CONTROL_POS_TCB"); 
			for (i=0; i<numKeys; i++) {
				ITCBPoint3Key key;
				ikc->GetKey(i, &key);
				fprintf(pStream, "%s %d\t%s",
					"ID_TCB_POS_KEY",
					key.time,
					Format(key.val));
				fprintf(pStream, " %s %s %s %s %s\n", Format(key.tens), Format(key.cont), Format(key.bias), Format(key.easeIn), Format(key.easeOut));
			}
			fprintf(pStream,"}\n");
		}
	}
	// Bezier position
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0))
	{
		int numKeys;
		if(numKeys = ikc->GetNumKeys())
		{
			fprintf(pStream,"%s {\n", "ID_CONTROL_POS_BEZIER"); 
			for (i=0; i<numKeys; i++) {
				IBezPoint3Key key;
				ikc->GetKey(i, &key);
				fprintf(pStream, "%s %d\t%s",
					"ID_BEZIER_POS_KEY",
					key.time,
					Format(key.val));
				fprintf(pStream, " %s %s %d\n", Format(key.intan), Format(key.outtan), key.flags);
			}
			fprintf(pStream,"}\n");
		}
	}
	// Linear position
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0)) {
		int numKeys;
		if(numKeys = ikc->GetNumKeys())
		{
			fprintf(pStream,"%s {\n", "ID_CONTROL_POS_LINEAR"); 
			for (i=0; i<numKeys; i++) {
				ILinPoint3Key key;
				ikc->GetKey(i, &key);
				fprintf(pStream, "%s %d\t%s\n",
					"ID_POS_KEY",
					key.time,
					Format(key.val));
			}
			fprintf(pStream,"}\n");
		}
	}
}

void CIKExport::DumpRotKeys(Control* cont, int indentLevel) 
{
	if (!cont)
		return;
	
	int i;
	IKeyControl *ikc = GetKeyControlInterface(cont);
	
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0)) {
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) {
			fprintf(pStream,"%s {\n", "ID_CONTROL_ROT_TCB"); 
			for (i=0; i<numKeys; i++) {
				ITCBRotKey key;
				ikc->GetKey(i, &key);
				fprintf(pStream, "%s %d\t%s", 
					"ID_TCB_ROT_KEY",
					key.time,
					Format(key.val));
				fprintf(pStream, " %s %s %s %s %s\n", Format(key.tens), Format(key.cont), Format(key.bias), Format(key.easeIn), Format(key.easeOut));
			}
			fprintf(pStream,"}\n");
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0))
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) {
			fprintf(pStream,"%s {\n", "ID_CONTROL_ROT_BEZIER"); 
			for (i=0; i<numKeys; i++) {
				IBezQuatKey key;
				ikc->GetKey(i, &key);
				fprintf(pStream, "%s %d\t%s\n", 
					 // Quaternions are converted to AngAxis when written to file
					"ID_ROT_KEY",
					key.time,
					Format(key.val));
				// There is no intan/outtan for Quat Rotations
			}
			fprintf(pStream,"}\n");
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0)) {
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) {
			fprintf(pStream,"%s {\n", "ID_CONTROL_ROT_LINEAR"); 
			for (i=0; i<numKeys; i++) {
				ILinRotKey key;
				ikc->GetKey(i, &key);
				fprintf(pStream, "%s %d\t%s\n",
					 // Quaternions are converted to AngAxis when written to file
					"ID_ROT_KEY",
					key.time,
					Format(key.val));
			}
			fprintf(pStream,"}\n");
		}
	}
}

void CIKExport::DumpScaleKeys(Control* cont, int indentLevel) 
{
	if (!cont)
		return;
	
	int i;
	IKeyControl *ikc = GetKeyControlInterface(cont);
	
	if (ikc && cont->ClassID() == Class_ID(TCBINTERP_SCALE_CLASS_ID, 0))
	{
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) {
			fprintf(pStream,"%s {\n", "ID_CONTROL_SCALE_TCB"); 
			for (i=0; i<numKeys; i++) {
				ITCBScaleKey key;
				ikc->GetKey(i, &key);
				fprintf(pStream, "%s %d\t%s",
					"ID_TCB_SCALE_KEY",
					key.time,
					Format(key.val));
				fprintf(pStream, " %s %s %s %s %s\n", Format(key.tens), Format(key.cont), Format(key.bias), Format(key.easeIn), Format(key.easeOut));
			}
			fprintf(pStream,"}\n");
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0)) {
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) {
			fprintf(pStream,"%s {\n", "ID_CONTROL_SCALE_BEZIER"); 
			for (i=0; i<numKeys; i++) {
				IBezScaleKey key;
				ikc->GetKey(i, &key);
				fprintf(pStream, "%s %d\t%s",
					"ID_BEZIER_SCALE_KEY",
					key.time,
					Format(key.val));
				fprintf(pStream, "\t%s\t%s\t%d\n", Format(key.intan), Format(key.outtan), key.flags);
			}
			fprintf(pStream,"}\n");
		}
	}
	else if (ikc && cont->ClassID() == Class_ID(LININTERP_SCALE_CLASS_ID, 0)) {
		int numKeys;
		if (numKeys = ikc->GetNumKeys()) {
			fprintf(pStream,"%s {\n", "ID_CONTROL_SCALE_LINEAR"); 
			for (i=0; i<numKeys; i++) {
				ILinScaleKey key;
				ikc->GetKey(i, &key);
				fprintf(pStream, "%s %d\t%s\n",
					"ID_SCALE_KEY",
					key.time,
					Format(key.val));
			}
			fprintf(pStream,"}\n");
		}
	}
}



///////////// 여기까지 애니메이션 관련 ////////////////

CIKExport::IsKnownController(Control* cont)
{
	ulong partA, partB;

	if (!cont)
		return FALSE;

	partA = cont->ClassID().PartA();
	partB = cont->ClassID().PartB();

	if (partB != 0x00)
		return FALSE;

	switch (partA) {
		case TCBINTERP_POSITION_CLASS_ID:
		case TCBINTERP_ROTATION_CLASS_ID:
		case TCBINTERP_SCALE_CLASS_ID:
		case HYBRIDINTERP_POSITION_CLASS_ID:
		case HYBRIDINTERP_ROTATION_CLASS_ID:
		case HYBRIDINTERP_SCALE_CLASS_ID:
		case LININTERP_POSITION_CLASS_ID:
		case LININTERP_ROTATION_CLASS_ID:
		case LININTERP_SCALE_CLASS_ID:
			return TRUE;
	}

	return FALSE;
}

// 다른 애니메이션 /// ANi

void CIKExport::ExportAniObject( INode* node )
{
	ObjectState os = node->EvalWorldState( 0 );
	if( !os.obj )	return;

	// Targets are actually geomobjects, but we will export them
	// from the camera and light objects, so we skip them here.
	if( os.obj->ClassID() == Class_ID( TARGET_CLASS_ID, 0 ) )
	{
		return;
	}

	fprintf( pStream, "*OBJECT	\"%s\"\n", node->GetName() );

	// TM ( Transform Matrix )

	Matrix3 pivot = node->GetNodeTM( 0 );

	Point3	row;
	row = pivot.GetRow( 0 );
	fprintf( pStream, "\t*TM_ROW0	%f %f %f\n", row.x, row.y, row.z );
	row = pivot.GetRow( 1 );
	fprintf( pStream, "\t*TM_ROW1	%f %f %f\n", row.x, row.y, row.z );
	row = pivot.GetRow( 2 );
	fprintf( pStream, "\t*TM_ROW2	%f %f %f\n", row.x, row.y, row.z );
	row = pivot.GetRow( 3 );
	fprintf( pStream, "\t*TM_ROW3	%f %f %f\n", row.x, row.y, row.z );

	// Key Frame Animation ( Sample )

	TimeValue	start = ip->GetAnimRange().Start();
	TimeValue	end   = ip->GetAnimRange().End();
	TimeValue	t;

	int delta = GetTicksPerFrame();

	for( t = start ; t <= end ; t += delta )
	{
		Matrix3 tm = node->GetNodeTM( t ) * Inverse( node->GetParentTM( t ) );

		AffineParts ap;
		decomp_affine( tm, &ap );

		Point3	pos = ap.t;
		Quat	q = ap.q;

		fprintf( pStream, "\t*POS %d %f %f %f\n", t / delta, pos.x, pos.y, pos.z );
		fprintf( pStream, "\t*ROT %d %f %f %f %f\n", t / delta, q.x, q.y, q.z, q.w );

//		Point3 axis;
//		float angle;
//		AngAxisFromQ( q, &angle, axis );
//		fprintf( m_pStream, "\t*ROT %d %f %f %f %f\n", t / delta, axis.x, axis.y, axis.z, angle );
	}
}
