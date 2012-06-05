/*****************************************************************************
**	giDocumentChunk.hpp
**
**	 Derived chunk for gi system
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GI_DOCUMENTCHUNK_HPP
#error giDocumentChunk.hpp multiply included
#endif
#define GI_DOCUMENTCHUNK_HPP

#ifndef CMM_DOCUMENTCHUNKSINGLEITEMTEMPLATE_HPP
#include "Systems/Common/Templates/cmmDocumentChunkSingleItemTemplate.hpp"
#endif

#ifndef GI_GIDATAPARSER_HPP
#include "Systems/GlobalIllumination/Data/giGIDataParser.hpp"
#endif
#ifndef GI_DIALOGUTIL_HPP
#include "Systems/GlobalIllumination/GUI/giDialogUtil.hpp"
#endif
#ifndef GI_OBJECTMGR_HPP
#include "Systems/GlobalIllumination/Object/giObjectMgr.hpp"
#endif

class giDocumentChunk : 	
	public cmmDocumentChunkSingleItemTemplate<giScriptData, 
									giGIDataParser, 
									giObjectMgr, 
									giDialogUtil>
{
public:

	//--------------------------------------------------------------------
	//  returns chunk description for display purposes
	//--------------------------------------------------------------------
	virtual const char* GetChunkDesc() const;
};
