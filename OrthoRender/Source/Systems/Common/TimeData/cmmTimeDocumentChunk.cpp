/*****************************************************************************
**	cmmTimeDocumentChunk.cpp
**
**	 Derived chunk for first system test
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/TimeData/cmmTimeDocumentChunk.hpp"

#include "Systems/Common/TimeData/cmmTimeDataParser.hpp"

#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"


// static variable
cmmTimeDocumentChunk* cmmTimeDocumentChunk::sm_pActiveChunk = NULL;

//--------------------------------------------------------------------
// Return pointer to currently active chunk, may return NULL
//--------------------------------------------------------------------
cmmTimeDocumentChunk* cmmTimeDocumentChunk::GetActiveChunk()
{
	return sm_pActiveChunk;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmTimeDocumentChunk::cmmTimeDocumentChunk() : m_bDirty(false), m_bActive(false)
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  cmmTimeDocumentChunk::Clear()
{
	m_Data = cmmTimeData(); // reinit data values
	m_bDirty = false;

	if (this->m_bActive)
	{
		PrefsData& data = PrefsMgr::Data();
		tmlnTimeLine::SetTimeRange( 0.0f, (float)data.m_ChannelEditor_DefaultTime.GetValue() );
	}
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  cmmTimeDocumentChunk::GetChunkName() const
{
	return cmmTimeDataParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual 
const char* cmmTimeDocumentChunk::GetChunkDesc() const
{
	return "Time";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual 
void cmmTimeDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the 
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual 
void cmmTimeDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  cmmTimeDocumentChunk::Read( chReader &i_Reader,
								  chDefs::Version i_Version,
								  chDefs::Size i_Size)
{
	//	sync up the chunk data with the active data
	if (this->m_bActive)
	{
		//m_Data = ptltLightMgr::GetData();
	}

	cmmTimeDataParser::ReadData(i_Reader, i_Version, i_Size, m_Data);

	if (this->m_bActive)
	{
		tmlnTimeLine::SetTimeRange(m_Data.m_MinimumTime, m_Data.m_MaximumTime);
	}

	m_bDirty = false;
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void  cmmTimeDocumentChunk::Write(chWriter &i_Writer, bool i_bResetDirtyFlag )
{
	if (this->m_bActive)
	{
		m_Data.m_MinimumTime = tmlnTimeLine::GetMinimum();
		m_Data.m_MaximumTime = tmlnTimeLine::GetMaximum();
	}

	cmmTimeDataParser::WriteData(i_Writer, m_Data);

	if ( i_bResetDirtyFlag )
		m_bDirty = false;
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  cmmTimeDocumentChunk::IsDirty() const
{
	return m_bDirty;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  cmmTimeDocumentChunk::SetActive()
{
	// Make our data active
	tmlnTimeLine::SetTimeRange(m_Data.m_MinimumTime, m_Data.m_MaximumTime);

	sm_pActiveChunk = this;
	m_bActive = true;

}

//--------------------------------------------------------------------
//  this chunk is currently being made inactive, remove data
// from scene or world
//--------------------------------------------------------------------
void  cmmTimeDocumentChunk::SetInactive( bool i_bUpdateData )
{
	if ( i_bUpdateData )
	{
		// Get current state of data
		//
		m_Data.m_MinimumTime = tmlnTimeLine::GetMinimum();
		m_Data.m_MaximumTime = tmlnTimeLine::GetMaximum();
	}

	if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
	m_bActive = false;
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void cmmTimeDocumentChunk::DataChanged()
{
	m_bDirty = true;
}
