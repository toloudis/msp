
/*****************************************************************************
**  HelperExporter.cpp
**
**	Exports nodes containing geometry (poly-mesh, tri-mesh, shapes)
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "HelperExporter.hpp"

#include "MaxExportUtils.hpp"
#include "ExportDoc.hpp"
#include "ExportIntent.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"



#include "MaxCommon.hpp"

#include <iostream>
#include <string>
#include <hash_set>
#include <cmath>
#include <map>
#include <crtdbg.h>


using namespace std;
using namespace stdext;
using ::Mesh;


namespace MaxExp
{	

	//=============================================================================
	//Given the tri-mesh, faceId  of a face of the mesh and 
	//mesh-based vertexId of one of the vertices of that face
	//find the normal, taking into account the smoothing group to which the 
	//face belongs to
	//
	//i_Mesh = the input mesh
	// i_nFaceId = the id of a  face of the mesh
	//i_nVertId =  the id of a vertex (mesh-based vertex id) of the msh
	//				(the vertex should be a part of the face)
	// i_pExportedRootNode, see BaseExporter.hpp for details
	//=============================================================================
	void HelperExporter::Export( 
		INode  *i_pCurNode, 
		ExportIntent &i_Intent, 			  
			  INode *i_pExportedRootNode,	  	  
		shared_ptr< mdlNodeInfo > & o_mdlNode )
	{
		MaxObjectType::TypeVal objType;
		string sTypeName;
		MaxObjectType::Get(  i_pCurNode, OPTS, sTypeName, objType);
		Class_ID cid = i_pCurNode->ClassID();

		i_Intent.ReportProgress( 1.0f );

		if ( i_pCurNode->IsGroupHead() != TRUE )
		{
			const MCHAR *szName = i_pCurNode->GetName();
			//If the grouphead flag is not set,
			//give a warning
			EXPLOG.WriteWarning( "CuriousSighting: object %s of type %s, grouphead flag is not set", MBCSTOLPCSTR( szName ), sTypeName.c_str() );

		} 
	}

} //namespace MaxExp