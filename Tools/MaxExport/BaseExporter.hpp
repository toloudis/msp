/*****************************************************************************
**  BaseExporter.hpp
**
**	Base class of all component types (eg: MtlExporter, MashExporter etc)
**  Also defines IgnoreExporter, ErrorExporter and TrivialExporter
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MAXEXP_BASEEXPORTER_HPP
#error MAXEXP_BASEEXPORTER_HPP multiply defined!!
#endif
#define MAXEXP_BASEEXPORTER_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#include <string>
#include <list>
#include <hash_set>

//forward declarations
class INode;
class mdlNodeInfo;

class maMatrix4x4;
namespace MaxExp
{
	class ExportDoc;
	class ExportIntent;
}


namespace MaxExp
{

	//=============================================================================
	// An exporter class exports a specific kind of the 3d-content of the 
	// 3ds max file. Eg: CameraAnimExporter exports camera animation. We create one instance of 
	// each type of exporter as a member of ExportDoc. So the exporter class
	// should be able to export a number of entities of the same kind.
	//=============================================================================

	class  BaseExporter
	{
	public:
		BaseExporter( ExportDoc &exportDoc):m_pExportDoc(&exportDoc){}
		virtual ~BaseExporter(){}		
		
		// export the curent max node into the mdlNodeInfo node provided
			
		
		//i_pExportedRootNode:
		//            SceneRoot
		//             /     \
		//            A       B
		//          /  \     /  \
		//         C   D    E   F
		//        / \
		//       G   H
		// If we select the C and B nodes and export
		// the exportedRootNode for the C subhierarchy nodes is C
		// and the exportedRootNode for the B syb hierarchy is B

		virtual void Export( 
			INode *i_pCurNode, 
			ExportIntent &i_Intent, 
			INode *i_pExportedRootNode,
			shared_ptr<mdlNodeInfo> &o_MdlNodeInfo )
		{};
		//export the info associated pertaining to a  connection
		// between this node and  the nodes connected to this
		//Eg: skin info
		virtual void ExportConnectedInfo( INode *i_pCurNode ){}
        //Enumerate the nodes connected to this node
		virtual void  ComputeConnectedNodesToBeExported( INode * i_pCurNode, stdext::hash_set<INode *> &io_ForcedNodes )
		{
		}
		ExportDoc *m_pExportDoc;
	};


	//=============================================================================
	// Upon export a export_ignore exception is thrown
	//=============================================================================

	class  IgnoreExporter : public BaseExporter
	{
	public:
		IgnoreExporter( ExportDoc &exportDoc):BaseExporter(exportDoc){}
		~IgnoreExporter(){}		
		
		// throw am export_ignore exception
		// i_pExportedRootNode, see BaseExporter.hpp for details
		void Export( 
			INode *i_pCurNode, 
			ExportIntent &i_Intent, 	
			INode *i_pExportedRootNode,
			shared_ptr<mdlNodeInfo> &o_MdlNodeInfo );
	};

	
	
	//=============================================================================
	// Upon export a export_error exception is thrown
	//=============================================================================
	class  ErrorExporter : public BaseExporter
	{
	public:
		ErrorExporter( ExportDoc &exportDoc):BaseExporter(exportDoc){}
		~ErrorExporter(){}		
		
		// throw an export error exception
		// i_pExportedRootNode, see BaseExporter.hpp for details
		void Export( 
			INode *i_pCurNode, 
			ExportIntent &i_Intent, 
			INode *i_pExportedRootNode,
			shared_ptr<mdlNodeInfo> &o_MdlNodeInfo );
	};


	//=============================================================================
	// Basically a no-op
	//=============================================================================
	class  TrivialExporter : public BaseExporter
	{
	public:
		TrivialExporter( ExportDoc &exportDoc):BaseExporter(exportDoc){}
		~TrivialExporter(){}		
		//dont do anything
		void Export( 
			INode *i_pCurNode, 
			ExportIntent &i_Intent, 	
			INode *i_pExportedRootNode,
			shared_ptr<mdlNodeInfo> &o_MdlNodeInfo );
	};
} //namespace MaxExp
