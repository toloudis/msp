/*****************************************************************************
**  ExportDoc.hpp
**
**	The main document containing/coordinating all 
**	the book keeping of the 3ds max exporter
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#ifdef MAXEXP_EXPORTDOC_HPP
#error MAXEXP_EXPORTDOC_HPP multiply defined!!
#endif
#define MAXEXP_EXPORTDOC_HPP

#include "Graphics/mdl/mdlMatInfoTable.hpp"
#include "MaxCommon.hpp"
#include "MaxExportOptions.hpp"

#include <cstdio>
#include <string>
#include <list>
#include <tchar.h>

//forward declarations
class fsXMLWriter;
class chBinWriter;
class mdlNodeInfo;

namespace MaxExp
{
	class ExportIntent;
	class MtlExporter;
	class MeshExporter;
	class HelperExporter;
	class VertAnimExporter;
	class IgnoreExporter;
	class ErrorExporter;
	class PhysiqueExporter;
	class SkinModExporter;
	class TrivialExporter;
	class CameraAnimExporter;
    class SgpuInstanceMgr;
}

	class SgpuExportOptions;


namespace MaxExp
{

	//========================================================================
	//	Main class, which serves as the document king-pin 
	//	for all the book-keeping
	//========================================================================
	class ExportDoc
	{
	public:
		ExportDoc( const MCHAR *fname, SgpuExportOptions &options );
		~ExportDoc();

		void Do(const std::list<INode *> &suggestedNodes );
		
		void RegisterWriter( chBinWriter *pChBinWriter)
		{
			m_pWriter = pChBinWriter;
		}
		void UnregsiterWriter()
		{
			m_pWriter=NULL;
		}

		chBinWriter *GetWriter() const
		{
			if( NULL ==  m_pWriter)
			{	
				throw std::runtime_error( "accessing file writer before allocating it!");
			}
			assert( m_pWriter );
			return m_pWriter;
		}

		const char * GetExporterVersion();

			//Calculate the worldtransform of a node
		//(use the cache if possible)
		const Matrix3 & GetWordTransform( INode * i_CurNode );


		//Calculate the local (object level) transform of a node
		//(use the cache if possible)
		const Matrix3 & GetLocalTranform( INode * i_CurNode );
	private:  
		void LogPreExportStatistics() const;  
		void LogPostExportStatistics() const;


	public:
		std::wstring			m_FileName;

	public:
		//different  exporters
		ExportIntent *m_pExportIntent;
		MtlExporter	 *m_pMtlExporter;
		MeshExporter *m_pMeshExporter;
		mdlMatInfoTable *m_pMdlMatInfoTable;
		HelperExporter *m_pHelperExporter;
		VertAnimExporter *m_pVertAnimExporter;
		IgnoreExporter *m_pIgnoreExporter;
		ErrorExporter *m_pErrorExporter;
		PhysiqueExporter *m_pPhysiqueExporter;
		SkinModExporter *m_pSkinModExporter;
		TrivialExporter *m_pTrivialExporter;
		CameraAnimExporter *m_pCameraAnimExporter;
		struct FileWriterLifeTimeKeeper;
		shared_ptr< SgpuInstanceMgr > m_InstanceMgr;
		SgpuExportOptions m_Options;
		std::list< INode * > m_SuggestedNodesTobeExported;
	private:
		chBinWriter *m_pWriter;		
		//cached transformations of verious nodes
		std::map< INode *, Matrix3 > m_WorldTransformsCache;
		std::map< INode *, Matrix3 > m_LocalTransformsCache;

	};
	
#define OPTS m_pExportDoc->m_Options
} //namespace MaxExp
