/*****************************************************************************
** CameraAnimExportIntent.hpp
**
**	Exports max nodes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#ifdef MAXEXP_CAMERAANIMEXPORTINTENT_HPP
#error MAXEXP_CAMERAANIMEXPORTINTENT_HPP multuply defined!!
#endif

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef MAXEXP_EXPORTINTENT_HPP
#include "ExportIntent.hpp"
#endif
#ifndef MAXEXP_MAXMESHUTILS_HPP
#include "MaxMeshUtils.hpp"
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
class chBinWriter;

namespace MaxExp
{


	//========================================================================
	// Class for exporting nodes
	//========================================================================
	class CameraAnimExportIntent: public ExportIntent
	{
	public:
		CameraAnimExportIntent( ExportDoc &doc):
			ExportIntent(doc)
			{
			}
	    ~CameraAnimExportIntent(){}
	private:

		//Cleanup book keeping
		void CleanUp();

		//The core recursive workhorse function that 
		//export the nodes
		void ExportNodesRec( 
			INode *i_CurNode,   			
			INode *& io_pExportedRootNode,
			shared_ptr<mdlNodeInfo> &io_MdlNode);
			
		//Estimate the size of the file
		//and whether it is greater than
		//the limit
		bool PreExportValidation();
		
		void PreExportBookKeeping(){};
		//After the export,
		//before writing,
		//post process the mdl hierarchy
		void PostProcess( shared_ptr<mdlNodeInfo> &io_RootMdlNode){};
		void ExportNodes();
		void ExportPrefaceChunks(  shared_ptr<mdlNodeInfo> &i_RootMdlNode );
		void ExportEndChunks( shared_ptr<mdlNodeInfo> &i_RootMdlNode );
#if DELME
		void ExtractUserDefinedProps( INode *i_pCurNode, shared_ptr< mdlNodeInfo >  &io_mdlNodeInfo ){};

#endif

		//
		//For a given model return the type, typename and the exporter suited
		BaseExporter *GetAppropriateExporter( INode *i_pCurNode, std::string &stypeName, MaxObjectType::TypeVal &objType );
		
	};

} //namespace MaxExp
