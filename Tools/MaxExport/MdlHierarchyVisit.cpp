/*****************************************************************************
**  MdlHierarchyVisit.cpp
**
**	Exports max nodes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MdlHierarchyVisit.hpp"


#include "ExportDoc.hpp"
#include "MaxExportUtils.hpp"
#include "MtlExporter.hpp"
#include "MeshExporter.hpp"
#undef CreateFile
#undef DeleteFile
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "MaxCommon.hpp"

#include <string>




using namespace std;
using namespace stdext;

namespace MaxExp
{

	namespace
	{
		const chDefs::Name c_EXPV = chDefs::MakeName('E', 'X', 'P', 'V');

	}



   //=============================================================================
	// This is a visit structure  for the procedure 'VisitMdlHierarchyRec'
	// Please see the details of 'VisitMdlHierarchyRec' in MaxExportUtils.hpp
	//
	// In summary, FlattenVisit applies the cumulative transform acting upon a node
	// to its vertices and normals
	//
	// FlattenVisit's stack_type is maMatrix4x4,
	// and its stores the cumulative transform in m_Stack
	//
	// FlattenVisit, on 'Pre' will compute the crrent cumulativeTransform 
	// taking into account its own transform and then
	// apply it to the  vertices and normals of the mesh
	//=============================================================================

		FlattenVisit::FlattenVisit()
		{
			m_Stack.Identity();
		}


		void FlattenVisit::Pre( shared_ptr< mdlNodeInfo > &node )
		{
			maMatrix4x4 curCumulativeTransform =  (node->m_Transform * m_Stack);
			if( node->m_MeshInfo.get() )
			{
				MeshExporter::ApplyRTTransformationToMesh( curCumulativeTransform, *(node->m_MeshInfo)  );
			}	
			node->m_Transform.Identity();
			m_Stack = curCumulativeTransform;
		}


	//=============================================================================
	// This is a visit structure  for the procedure 'VisitMdlHierarchyRec'
	// Please see the details of 'VisitMdlHierarchyRec' in MaxExportUtils.hpp
	//
	// In summary, CorrectLeftHandednessVisit, computes the cumulative transform 
	//	acting upon a node and if the cumulative tranform is left-handed
	//  it flips the vertex ordeer in the index list
	//
	// CorrectLeftHandednessVisit's stack_type is maMatrix4x4,
	// and its stores the cumulative transform in m_Stack
	//
	// CorrectLeftHandednessVisit, on 'Pre' will compute the crrent 
	// cumulativeTransform taking into account its own transform and then
	// flips the vertex order in the indices if necessary
	//=============================================================================
	
		void CorrectLeftHandednessVisit::Pre( shared_ptr< mdlNodeInfo > &node )
		{
			maMatrix4x4 curCumulativeTransform =  (node->m_Transform * m_Stack);
			if( node->m_MeshInfo.get() )
			{
				MeshExporter::ApplyCorrectionToVertexOrderForPotentiallyLeftHandedGeometry( curCumulativeTransform, *(node->m_MeshInfo)  );
			} else if ( node->m_SubdivInfo.get() )
			{
				MeshExporter::ApplyCorrectionToVertexOrderForPotentiallyLeftHandedGeometry( curCumulativeTransform, *(node->m_SubdivInfo)  );
			}
			m_Stack = curCumulativeTransform;
		}


	//=============================================================================
	// This is a visit structure  for the procedure 'VisitMdlHierarchyRec'
	// Please see the details of 'VisitMdlHierarchyRec' in MaxExportUtils.hpp
	//
	// In summary, CreateBasisVectorVisit, computes the cumulative transform 
	//	acting upon a node and if the cumulative tranform is left-handed
	//  it flips the vertex ordeer in the index list
	//
	// CreateBasisVectorVisit's stack_type is a dummy stack_type
	//
	// CreateBasisVectorVisit, on 'Pre' will compute  the basis vectors
	// of the  mesh corresponding to the node that is currently visited 
	//=============================================================================

		void CreateBasisVectorsVisit::Pre( shared_ptr< mdlNodeInfo > &node )
		{
			if( node->m_MeshInfo.get() )
			{
				mdlFragUtil::CreateBasisVectors( *(node->m_MeshInfo)  );
			}	
		}



	//=============================================================================
	// This is a visit structure  for the procedure 'VisitMdlHierarchyRec'
	// Please see the details of 'VisitMdlHierarchyRec' in MaxExportUtils.hpp
	//
	// In summary, SetTransformNull, sets the transform of all the mdl nodes
	// in the hierarchy to identity.
	//=============================================================================

		void SetTransformIdentity::Pre( shared_ptr< mdlNodeInfo > &node )
		{
			
			node->m_Transform.Identity();
		}

} //maxExp