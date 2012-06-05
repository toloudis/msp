/*****************************************************************************
**	ptltDocumentChunk.hpp
**
**	 Derived chunk for point lights
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_DOCUMENTCHUNK_HPP
#error ptltDocumentChunk.hpp multiply included
#endif
#define PTLT_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef PTLT_DATAPARSER_HPP
#include "Systems/PtLt/Data/ptltDataParser.hpp"
#endif
#ifndef PTLT_DIALOGUTIL_HPP
#include "Systems/PtLt/GUI/ptltDialogUtil.hpp"
#endif
#ifndef PTLT_OBJECTMGR_HPP
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"
#endif

//============================================================================
//============================================================================
class ptltDocumentChunk : 
	public cmmDocumentChunkTemplate<ptltPointLightsData, 
									ptltDataParser, 
									ptltObjectMgr, 
									ptltDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
