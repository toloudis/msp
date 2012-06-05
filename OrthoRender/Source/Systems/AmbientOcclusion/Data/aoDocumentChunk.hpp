/*****************************************************************************
**	aoDocumentChunk.hpp
**
**	 Derived chunk for ao system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef AO_DOCUMENTCHUNK_HPP
#error aoDocumentChunk.hpp multiply included
#endif
#define AO_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKSINGLEITEMTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkSingleItemTemplate.hpp"
#endif

#ifndef AO_AODATAPARSER_HPP
#include "Systems/AmbientOcclusion/Data/aoAODataParser.hpp"
#endif
#ifndef AO_DIALOGUTIL_HPP
#include "Systems/AmbientOcclusion/GUI/aoDialogUtil.hpp"
#endif
#ifndef AO_OBJECTMGR_HPP
#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"
#endif

class aoDocumentChunk : 	
	public cmmDocumentChunkSingleItemTemplate<aoScriptData, 
									aoAODataParser, 
									aoObjectMgr, 
									aoDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;
};
