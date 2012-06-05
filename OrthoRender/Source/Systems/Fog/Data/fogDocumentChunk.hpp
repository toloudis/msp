/*****************************************************************************
**	fogDocumentChunk.hpp
**
**	 Derived chunk for fog system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FOG_DOCUMENTCHUNK_HPP
#error fogDocumentChunk.hpp multiply included
#endif
#define FOG_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKSINGLEITEMTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkSingleItemTemplate.hpp"
#endif

#ifndef FOG_FOGDATAPARSER_HPP
#include "Systems/Fog/Data/fogFogDataParser.hpp"
#endif
#ifndef FOG_DIALOGUTIL_HPP
#include "Systems/Fog/GUI/fogDialogUtil.hpp"
#endif
#ifndef FOG_OBJECTMGR_HPP
#include "Systems/Fog/Object/fogObjectMgr.hpp"
#endif

class fogDocumentChunk : 	
	public cmmDocumentChunkSingleItemTemplate<fogScriptData, 
									fogFogDataParser, 
									fogObjectMgr, 
									fogDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;
};
