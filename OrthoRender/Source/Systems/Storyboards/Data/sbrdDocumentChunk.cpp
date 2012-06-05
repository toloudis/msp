/*****************************************************************************
**	sbrdDocumentChunk.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/Storyboards/Data/sbrdDocumentChunk.hpp"


//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual 
const char* sbrdDocumentChunk::GetChunkDesc() const
{
	return "Storyboards";
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
//virtual 
void  sbrdDocumentChunk::Read(	chReader &i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size)
{
	//m_DataList.m_Items.resize(1);
	sbrdDataParser::ReadData( i_Reader, i_Version, i_Size, m_DataList );

	DBG_LOG1("Storyboard object has %d filenames", m_DataList.m_Filenames.size());
	DBG_LOG1("Storyboard object has %d items", m_DataList.m_Items.size());
	//DBG_LOG1("Storyboard object chunk has %d drivers", m_DataList.m_Items[0].m_Drivers.size());
	//DBG_LOG1("Storyboard object chunk has %d channels", m_DataList.m_Items[0].m_Channels.size());

	if (this->m_bActive)
	{
		//	FIX - copied the Read() just to remove this assert.
		//
		//DBG_ASSERT0(xxxObjectMgr::GetNumObjects() == 0, "Read() should be called after Clear(), use Import() to append items");
		sbrdObjectMgr::SetData( m_DataList );
		sbrdDialogUtil::UpdateListDialog();
	}

	m_bDirty = false;
}


