/*****************************************************************************
**	rptReportUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Features/Reports/rptReportUtil.hpp"

#include "Features/Reports/rptReportData.hpp"

#include "Tool/doc/docDocumentWithChunks.hpp"
#include "Tool/doc/docDocumentChunk.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include <string>


//
namespace rptReportUtil
{
	//------------------------------------------------------------------------
	//	Build a data list from the passed in doc.
	//------------------------------------------------------------------------
	void BuildInvDataList( docDocument* i_pDoc, rptReportData& o_Data )
	{
		docDocumentWithChunks *pDoc = dynamic_cast<docDocumentWithChunks*>(i_pDoc);
		DBG_ASSERT( pDoc != NULL, "Need a doc to build a datalist" );

		docDocumentChunk* pChunk;
		int i;
		int numchunks;

		numchunks = pDoc->GetNumDocumentChunks();
		o_Data.m_Chunks.resize( numchunks );

		for ( i = 0; i < numchunks ; i++ )
		{
			pChunk = pDoc->GetDocumentChunk( i );

			//	get the chunk description
			o_Data.m_Chunks[i].m_Desc = pChunk->GetChunkDesc();

			//	now, have the chunk fill in it's item list however
			//	it needs to.
			//
			pChunk->BuildDataList( o_Data.m_Chunks[i].m_Items );
		}
	}
}
