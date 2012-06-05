/*****************************************************************************
**	dirltDocumentChunk.hpp
**
**	 Derived chunk for dirlt system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef DIRLT_DOCUMENTCHUNK_HPP
#error dirltDocumentChunk.hpp multiply included
#endif
#define DIRLT_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "cmmDocumentChunkTemplate.hpp"
#endif

#ifndef DIRLT_DATAPARSER_HPP
#include "dirltDataParser.hpp"
#endif
#ifndef DIRLT_DIALOGUTIL_HPP
#include "dirltDialogUtil.hpp"
#endif
#ifndef DIRLT_OBJECTMGR_HPP
#include "dirltObjectMgr.hpp"
#endif

//============================================================================
//============================================================================
class dirltDocumentChunk : 
	public cmmDocumentChunkTemplate<dirltDirLightsData, 
									dirltDataParser, 
									dirltObjectMgr, 
									dirltDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
