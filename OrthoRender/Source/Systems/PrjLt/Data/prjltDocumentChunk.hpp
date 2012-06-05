/*****************************************************************************
**	prjltDocumentChunk.hpp
**
**	 Derived chunk for projected lights
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_DOCUMENTCHUNK_HPP
#error prjltDocumentChunk.hpp multiply included
#endif
#define PRJLT_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef PRJLT_DATAPARSER_HPP
#include "Systems/PrjLt/Data/prjltDataParser.hpp"
#endif
#ifndef PRJLT_DIALOGUTIL_HPP
#include "Systems/PrjLt/GUI/prjltDialogUtil.hpp"
#endif
#ifndef PRJLT_OBJECTMGR_HPP
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#endif

//============================================================================
//============================================================================
class prjltDocumentChunk : 
	public cmmDocumentChunkTemplate<prjltProjectLightsData, 
									prjltDataParser, 
									prjltObjectMgr, 
									prjltDialogUtil>
{
public:
	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
