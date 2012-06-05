/*****************************************************************************
**	skyDocumentChunk.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "skyDocumentChunk.hpp"

#include "skySkyDataParser.hpp"
#include "skyDialogUtil.hpp"

// library
#include "daySkyMgr.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
skyDocumentChunk::skyDocumentChunk() : m_bDirty(false), m_bActive(false)
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  skyDocumentChunk::Clear()
{
	m_Data.clear();
	m_bDirty = false;

	if (this->m_bActive)
	{
		skyDataMgr::Clear();
		skyDialogUtil::UpdateDialog();
	}
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  skyDocumentChunk::GetChunkName() const
{
	return skySkyDataParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual 
const char* skyDocumentChunk::GetChunkDesc() const
{
	return "Sky";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual 
void skyDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
	o_List.resize(1);
	o_List[0] = "Sky";
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the 
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual 
void skyDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  skyDocumentChunk::Read(chReader &i_Reader,
							  chDefs::Version i_Version,
							  chDefs::Size i_Size)
{
	//	sync up the chunk data with the active data
	if (this->m_bActive)
	{
		//m_Data = daySkyMgr::GetData();
	}

	skySkyDataParser::ReadSkyData(i_Reader, i_Version, i_Size, m_Data);

	if (this->m_bActive)
	{
		daySkyMgr::Add( m_Data );
		//daySkyMgr::SetListData(m_Data);

		skyDialogUtil::UpdateDialog();
	}
	m_bDirty = false;
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void  skyDocumentChunk::Write(chWriter &i_Writer, bool i_bResetDirtyFlag )
{
	if (this->m_bActive)
		skySkyDataParser::WriteSkyData(i_Writer, daySkyMgr::GetListData());
	else
		skySkyDataParser::WriteSkyData(i_Writer, m_Data);

	if ( i_bResetDirtyFlag )
		m_bDirty = false;
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  skyDocumentChunk::IsDirty() const
{
	return m_bDirty;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  skyDocumentChunk::SetActive()
{
	// Make our data active
	daySkyMgr::SetListData(m_Data);
	skyDataMgr::AddDataChangedCallback(this);
	m_bActive = true;

}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  skyDocumentChunk::SetInactive( bool i_bUpdateData )
{
	if ( i_bUpdateData )
	{
		// Get current state of data
		//
		m_Data.clear();
		m_Data = daySkyMgr::GetListData();
		skyDataMgr::RemoveDataChangedCallback(this);
	}

	m_bActive = false;
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void skyDocumentChunk::DataChanged()
{
	m_bDirty = true;
}
