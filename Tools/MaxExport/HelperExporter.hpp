
/*****************************************************************************
**  HelperExporter.hpp
**
**	Exports nodes containing geometry (poly-mesh, tri-mesh, shapes)
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAXEXP_HELPEREXPORTER_HPP
#error MAXEXP_HELPEREXPORTER_HPP multuply defined!!
#endif
#define MAXEXP_HELPEREXPORTER_HPP


#ifndef MAXEXP_BASEEXPORTER_HPP
#include "BaseExporter.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 


#include <list>
#include <max.h>
//forward declarations
class INode;
class mdlNodeInfo;
class mdlFragInfo;
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
	class HelperExporter: public BaseExporter
	{
	public:
		HelperExporter( ExportDoc &doc):
		  BaseExporter( doc ){}
		  ~HelperExporter(){}
		  // export the curent max node into the mdlNodeInfo node provided
		  // i_pExportedRootNode, see BaseExporter.hpp for details
		  void Export( 
			  INode *i_pCurNode, 
			  ExportIntent &i_Intent, 	  
			  INode *i_pExportedRootNode,
			  shared_ptr<mdlNodeInfo> &o_MdlNodeInfo );
	
	};
} //namespace MaxExp