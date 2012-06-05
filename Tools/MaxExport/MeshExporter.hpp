
/*****************************************************************************
**  MeshExporter.hpp
**
**	Exports nodes containing geometry (poly-mesh, tri-mesh, shapes)
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_MESHEXPORTER_HPP
#error MAXEXP_MESHEXPORTER_HPP multuply defined!!
#endif
#define MAXEXP_MESHEXPORTER_HPP


#ifndef MAXEXP_BASEEXPORTER_HPP
#include "BaseExporter.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#ifndef MAXEXP_MAXEXPORTERUTILS_HPP
#include "MaxExportUtils.hpp"
#endif

#include <list>
#include <hash_set>
#include <max.h>
//forward declarations
class INode;
class mdlNodeInfo;
class mdlFragInfo;
class mdlSubdivInfo;
class maMatrix4x4;
namespace MaxExp
{
	class ExportDoc;	
}


namespace MaxExp
{


	//========================================================================
	// Mesh Exporter Class
	//	
	//========================================================================
	class MeshExporter: public BaseExporter
	{
	public:
		MeshExporter( ExportDoc &doc):
		  BaseExporter( doc ){}
		  ~MeshExporter(){}
		  // export the curent max node into the mdlNodeInfo node provided
		  // i_pExportedRootNode, see BaseExporter.hpp for details
		  void Export( 
			  INode *i_pCurNode, 
			  ExportIntent &i_ExportIntent, 	  
			  INode *i_pExportedRootNode,
			  shared_ptr<mdlNodeInfo> &o_MdlNodeInfo );
		  //export the info associated with this node
		  //and connected nodes
		  //Eg: skin info
		  void ExportSkinInfo( INode *i_pCurNode , const  INodeVector &i_AllBones, shared_ptr< mdlNodeInfo > &o_mdlNode  );

		  void  ComputeConnectedNodesToBeExported( INode * i_pCurNode, stdext::hash_set<INode *> &io_NodeCont );
			
		  void  GetBonesAssociatedWithThisMesh( INode * i_pCurNode, INodeHS &o_BonesAssociatdWithThisMesh );


		  //Apply a (Rotation + Tranmslation) transformation to the vertoces and normals
		  static void ApplyRTTransformationToMesh( const maMatrix4x4 &i_Tm, mdlFragInfo &io_MdlMeshInfo );
		  //If the transformation potentially takes it to LH Coordinate system
		  //then invert the vertex order
		  static void ApplyCorrectionToVertexOrderForPotentiallyLeftHandedGeometry( const maMatrix4x4 &i_Tm, mdlFragInfo &io_MdlMeshInfo );
		  //If the transformation potentially takes it to LH Coordinate system
		  //then invert the vertex order
		  static void ApplyCorrectionToVertexOrderForPotentiallyLeftHandedGeometry( const maMatrix4x4 &i_Tm, mdlSubdivInfo &io_MdlSubdivInfo );
		  //creates basis vectors (T,N,B vectors) for each vertex of
		  // the  mesh
		  static void CreateBasisVectors( mdlFragInfo &io_MdlMeshInfo );
		
		  void CheckOnObject( INode  *i_pCurNode );

	};

	 
} //namespace MaxExp