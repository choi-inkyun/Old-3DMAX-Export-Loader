#include "CIKExport.h"
#include "Max.h"
#include "PhyExp.h"

//
// FindPhysiqueModifier
//
Modifier* FindPhysiqueModifier( INode* node )
{
	if( !node )	return NULL;

	Object* obj = node->GetObjectRef();

	if( !obj )	return NULL;

	while( obj->SuperClassID() == GEN_DERIVOB_CLASS_ID && obj )
	{
		IDerivedObject* derivedObj = (IDerivedObject *)obj;

		for( int i = 0 ; i < derivedObj->NumModifiers() ; i++ )
		{
			Modifier* modifier = derivedObj->GetModifier( i );

			if( modifier->ClassID() == Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) )
			{
				return modifier;
			}
		}

		obj = derivedObj->GetObjRef();
	}

	return NULL;
}

//ExportPhysiqueData
void CIKExport::ExportPhysiqueData( INode* node, Modifier* mod )
{
	if( mod->ClassID() != Class_ID( PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B ) )
	{
		return;
	}

	fprintf( pStream, "*OBJECT	%s\n", node->GetName() );

	IPhysiqueExport *phyExport = (IPhysiqueExport*)mod->GetInterface( I_PHYEXPORT );

	IPhyContextExport *mcExport = (IPhyContextExport*)phyExport->GetContextInterface( node );

	mcExport->ConvertToRigid( true );

//	mcExport->AllowBlending( true );
	mcExport->AllowBlending( false );

	int numverts = mcExport->GetNumberVertices();

	fprintf( pStream, "\t*NUMPHYSIQUE	%d\n", numverts );

	for( int i = 0 ; i < numverts ; i++ )
	{
		IPhyVertexExport* vi = mcExport->GetVertexInterface( i, HIERARCHIAL_VERTEX );

		if( vi )
		{
			int type = vi->GetVertexType();

			if( type == RIGID_TYPE )
			{
				IPhyRigidVertex*	v;
				TSTR	nodeName;

				v = (IPhyRigidVertex*)vi;
				nodeName = v->GetNode()->GetName();

				fprintf( pStream, "\t\t*PHYSIQUE %d \"%s\"\n", i, nodeName );
			}

			mcExport->ReleaseVertexInterface( vi );
		}
	}

	phyExport->ReleaseContextInterface( mcExport );

	mod->ReleaseInterface( I_PHYINTERFACE, phyExport );
}
