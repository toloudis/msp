/*****************************************************************************
**  MdlHierarchyVisit.hpp
****
**	Base of the exporter class which signifies the intention
**	of the export (eg: whether this is a model export, animation export)
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MAXEXP_MDLHIERARCHYVISIT_HPP
#error MAXEXP_MDLHIERARCHYVISIT_HPP multiply defined!!
#endif
#define MAXEXP_MDLHIERARCHYVISIT_HPP


#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#include <hash_set>
#include <map>

//forward declarations
class Matrix3;
class mdlNodeInfo;
class INode;
class gfFileBin;
class chBinWriter;
namespace MaxExp
{
	class ExportDoc;
	class BaseExporter;
	namespace MaxObjectType
	{
		enum TypeVal;
	}
}

namespace MaxExp
{


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
	

	struct FlattenVisit
	{
		FlattenVisit();
		typedef maMatrix4x4 stack_type;
		void Pre( shared_ptr< mdlNodeInfo > &node );
		void Post( shared_ptr<mdlNodeInfo > &node ){}
		stack_type m_Stack;	
	};




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
	
	struct CorrectLeftHandednessVisit
	{
		typedef maMatrix4x4 stack_type;
		void Pre( shared_ptr< mdlNodeInfo > &node );
		void Post( shared_ptr<mdlNodeInfo > &node ){}
		stack_type m_Stack;	
	};


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
	
	struct CreateBasisVectorsVisit
	{
		typedef int stack_type;
		void Pre( shared_ptr< mdlNodeInfo > &node );
		void Post( shared_ptr<mdlNodeInfo > &node ){}
		stack_type m_Stack;	
	};

	//=============================================================================
	// This is a visit structure  for the procedure 'VisitMdlHierarchyRec'
	// Please see the details of 'VisitMdlHierarchyRec' in MaxExportUtils.hpp
	//
	// In summary, SetTransformNull, sets the transform of all the mdl nodes
	// in the hierarchy to identity.
	//=============================================================================
	
	struct SetTransformIdentity
	{
		typedef int stack_type;
		void Pre( shared_ptr< mdlNodeInfo > &node );
		void Post( shared_ptr<mdlNodeInfo > &node ){}
		stack_type m_Stack;	
	};




} //namespace MaxExp
