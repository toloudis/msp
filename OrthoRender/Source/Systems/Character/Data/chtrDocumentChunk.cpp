/*****************************************************************************
**	chtrDocumentChunk.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Character/Data/chtrDocumentChunk.hpp"

#include "Drivers/Attach/tmlnDriverAttachUtil.hpp"

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  chtrDocumentChunk::Clear()
{
	cmmDocumentChunkTemplate<chtrCharactersData, 
									chtrDataParser, 
									chtrObjectMgr, 
									chtrDialogUtil>::Clear();

	if (this->m_bActive)
	{
		// When the name system can handle importing, then
		// this cache clear doesn't need to be here.
		tmlnDriverAttachUtil::ClearCache();
	}
}


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual 
const char* chtrDocumentChunk::GetChunkDesc() const
{
	return "Characters";
}
