/*****************************************************************************
** ParticleExportIntent.hpp
**
**	Exports max nodes
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#ifdef MAXEXP_PARTICLEEXPORTINTENT_HPP
#error MAXEXP_PARTICLEEXPORTINTENT_HPP multuply defined!!
#endif

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
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
	class ParticleExportIntent: public ExportIntent
	{
	public:
		ParticleExportIntent( ExportDoc &doc):
			ExportIntent(doc)
			{
			}
	    ~ParticleExportIntent(){}
		void Do();
	private:

		//Cleanup book keeping
		void CleanUp();
		
		void ExportNodes(){}
		//The core recursive workhorse function that 
		//export the nodes
		void ExportNodesRec( 
			INode *i_CurNode , 		
			INode *& io_pExportedRootNode,
			shared_ptr<mdlNodeInfo> &io_MdlNode)
		{}

		virtual void PostProcess( shared_ptr<mdlNodeInfo> &io_RootMdlNode)
		{}
		
		void PreExportBookKeeping(){}
		//
		//For a given model return the type, typename and the exporter suited
		BaseExporter *GetAppropriateExporter( INode *i_pCurNode, std::string &stypeName, MaxObjectType::TypeVal &objType )
		{
			throw std::runtime_error(std::string("not implemented"));
			return NULL;
		}

	};

} //namespace MaxExp
