/*****************************************************************************
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
#include "ExportIntent.hpp"

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
		ModelExportIntent( ExportDoc &doc):
			ExportIntent(doc)
			{
			}
	    ~ModelExportIntent(){}
	private:

		//Cleanup book keeping
		void CleanUp();

		//The core recursive workhorse function that 
		//export the nodes
		void ExportNodesRec( INode *i_CurNode , shared_ptr<mdlNodeInfo> &io_MdlNode);

	};

} //namespace MaxExp
