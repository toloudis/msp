/*****************************************************************************
**  BaseExporter.hpp
**
**	Base class of all component types (eg: MtlExporter, MashExporter etc)
**	Also defines IgnoreExporter, ErrorExporter and TrivialExporter
**	Extra Large Technology
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "BaseExporter.hpp"
#include "ExportDoc.hpp"
#include "MaxExportUtils.hpp"
#include "ExportIntent.hpp"

#include <string>



using namespace std;


namespace MaxExp
{
	void ErrorExporter::Export( 
		INode *i_pCurNode, 
		ExportIntent &i_Intent,		
		INode *i_pExportedRootNode,
		shared_ptr<mdlNodeInfo> &o_MdlNodeInfo )
		{
			const MCHAR *szNodeName = i_pCurNode->GetName();
			string stypeName;
			MaxObjectType::TypeVal tval;
			MaxObjectType::Get( i_pCurNode, OPTS, stypeName, tval );
			EXPLOG.WriteError( "node %s of type %s not exported", MBCSTOLPCSTR( szNodeName ),  stypeName.c_str() ); 
			
			//report progress
			i_Intent.ReportProgress( 1.0f );

			throw export_failure();
			return;
		}

		  // i_pExportedRootNode, see BaseExporter.hpp for details
	void IgnoreExporter::Export( 
		INode *i_pCurNode, 
		ExportIntent &i_Intent, 		
		INode *i_pExportedRootNode,
		shared_ptr<mdlNodeInfo> &o_MdlNodeInfo )
		{
			const MCHAR *szNodeName = i_pCurNode->GetName();
			string stypeName;
			MaxObjectType::TypeVal tobject;
			MaxObjectType::Get( i_pCurNode, OPTS, stypeName, tobject );
			EXPLOG.WriteInfo("Export Of node %s if type %s is ignored", MBCSTOLPCSTR( szNodeName ),  stypeName.c_str() );

			i_Intent.ReportProgress( 1.0f );
			throw export_ignore();
			return;
		}

		  // i_pExportedRootNode, see BaseExporter.hpp for details
	void TrivialExporter::Export( 
		INode *i_pCurNode, 
		ExportIntent &i_Intent, 		
		INode *i_pExportedRootNode,
		shared_ptr<mdlNodeInfo> &o_MdlNodeInfo )
		{
			const MCHAR *szNodeName = i_pCurNode->GetName();
			string stypeName;
			MaxObjectType::TypeVal tobject;
			MaxObjectType::Get( i_pCurNode, OPTS, stypeName, tobject );			
			i_Intent.ReportProgress( 1.0f );
			return;
		}

}  //MaxExp