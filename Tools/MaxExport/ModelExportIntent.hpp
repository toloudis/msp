/**---------------------------------------------------------------------------------------------------------------------------------------------------p0o/*****************************************************************************
** ModelExportIntent.hpp
**
**	Exports max nodes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#ifdef MAXEXP_MODELEXPORTINTENT_HPP
#error MAXEXP_MODELEXPORTINTENT_HPP multuply defined!!
#endif

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MAXEXP_MAXEXPORTERUTILS_HPP
#include "MaxExportUtils.hpp"
#endif
#ifndef MAXEXP_EXPORTINTENT_HPP
#include "ExportIntent.hpp"
#endif

#include <map>
#include <hash_set>
#include <list>
#include <max.h>


//forward declarations
class INode;
class mdlNodeInfo;
class mdlFragInfo;
class maMatrix4x4;


namespace MaxExp
{

	//========================================================================
	// Class for exporting nodes
	//========================================================================
	class ModelExportIntent: public ExportIntent
	{
	public:

	

	public:
		ModelExportIntent( ExportDoc &doc):
			ExportIntent(doc)
			{
			}
	    ~ModelExportIntent(){}
	private:

		//Cleanup book keeping
		void CleanUp();
		void ExportNodes();
		
		void PreExportBookKeeping();
	
		//The core recursive workhorse function that 
		//export the nodes
		void ExportNodesRec( 
			INode *i_CurNode , 
			INode *& io_pExportedRootNode,
			shared_ptr<mdlNodeInfo> &io_MdlNode);

		void PostProcess( shared_ptr<mdlNodeInfo> &io_RootMdlNode);
		//
		//For a given model return the type, typename and the exporter suited
		BaseExporter *GetAppropriateExporter( INode *i_pCurNode, std::string &stypeName, MaxObjectType::TypeVal &objType );

		//Validate the export
		//If not valid dont proceed
		bool PreExportValidation();

		bool GetPathFromRootRec( const shared_ptr< mdlNodeInfo > & i_DesiredNode, const shared_ptr< mdlNodeInfo > &i_CurNode,  std::vector< shared_ptr< mdlNodeInfo > > &o_PathOutput );
		void GetPathFromRoot( const shared_ptr< mdlNodeInfo > & i_DesiredNode, std::vector< shared_ptr< mdlNodeInfo > > &o_PathOutput );

	public:
		INodeVector m_BonesAssociatedWithAllMeshes;
	private:
		shared_ptr< mdlNodeInfo > m_RootMdlNode;
	};

} //namespace MaxExp
