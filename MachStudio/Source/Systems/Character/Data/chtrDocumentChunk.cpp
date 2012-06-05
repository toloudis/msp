/*****************************************************************************
**	chtrDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Data/chtrDocumentChunk.hpp"

//#include "Systems/Props/Data/propDocumentChunk.hpp"
#include "Drivers/Attach/tmlnDriverAttachUtil.hpp"


//--------------------------------------------------------------------
// This flag needs to be coordinated with the one in 
// propDocumentChunk.cpp
//--------------------------------------------------------------------
//const bool c_bConvertingProps = true;


//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void chtrDocumentChunk::Clear()
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
//  Read chunk data
//--------------------------------------------------------------------
void chtrDocumentChunk::Read(chReader &i_Reader,
							 chDefs::Version i_Version,
							 chDefs::Size i_Size)
{
	chtrDataParser::ReadData( i_Reader, i_Version, i_Size, m_DataList );

	// If the flag is set, promote all props to characters
	//if (c_bConvertingProps)
	//{
	//	propDocumentChunk::ConvertPropsToCharacters(m_DataList);
	//}

	if (this->m_bActive)
	{
		DBG_ASSERT(chtrObjectMgr::GetNumObjects() == 0, "Read() should be called after Clear(), use Import() to append items");
		chtrObjectMgr::SetData( m_DataList );
		chtrDialogUtil::UpdateListDialog();
	}

	m_bDirty = false;
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual 
const char* chtrDocumentChunk::GetChunkDesc() const
{
	return "Objects";
}
