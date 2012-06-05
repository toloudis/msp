/*****************************************************************************
**	lsetDocumentChunk.hpp
**
**	 Derived chunk for lset system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef LSET_DOCUMENTCHUNK_HPP
#error lsetDocumentChunk.hpp multiply included
#endif
#define LSET_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkTemplate.hpp"
#endif

#ifndef LSET_LIGHTSETSDATAPARSER_HPP
#include "Systems/LightSets/Data/lsetLightSetsDataParser.hpp"
#endif
#ifndef LSET_DIALOGUTIL_HPP
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"
#endif
#ifndef LSET_OBJECTMGR_HPP
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"
#endif


class lsetDocumentChunk : 
	public cmmDocumentChunkTemplate<lsetLightSetsData, 
									lsetLightSetsDataParser, 
									lsetObjectMgr, 
									lsetDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;

};
