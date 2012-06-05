/*****************************************************************************
**	todDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "todDocumentChunk.hpp"

#include "todTimeOfDayDataParser.hpp"

// library
#include "dayTimeOfDay.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
todDocumentChunk::todDocumentChunk() : m_bDirty(false), m_bActive(false)
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  todDocumentChunk::Clear()
{
	m_Data = todTimeOfDayData();
	m_bDirty = false;

	if (this->m_bActive)
	{
		todDataMgr::Clear();

		dayTimeOfDay::DeInitialize();
		dayTimeOfDay::Initialize();
	}
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  todDocumentChunk::GetChunkName() const
{
	return todTimeOfDayDataParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual 
const char* todDocumentChunk::GetChunkDesc() const
{
	return "Time of Day";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual 
void todDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
	o_List.resize(1);
	o_List[0] = "Time of Day";
}


//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the 
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual 
void todDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
	int i;
	int num = i_List.size();
	for ( i = 0 ; i < num ; i++ )
	{
		if ( i_List[i] == "Time of Day" )
		{
		}
	}
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  todDocumentChunk::Read(chReader &i_Reader,
									  chDefs::Version i_Version,
									  chDefs::Size i_Size)
{
	//	sync up the chunk data with the active data
	if (this->m_bActive)
	{
		m_Data = todDataMgr::GetData();
	}

	todTimeOfDayDataParser::ReadTimeOfDayData(i_Reader, i_Version, i_Size, m_Data);

	if (this->m_bActive)
	{
		todDataMgr::SetData(m_Data);
	}
	m_bDirty = false;
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void  todDocumentChunk::Write(chWriter &i_Writer, bool i_bResetDirtyFlag )
{
	if (this->m_bActive)
		todTimeOfDayDataParser::WriteTimeOfDayData(i_Writer, todDataMgr::GetData());
	else
		todTimeOfDayDataParser::WriteTimeOfDayData(i_Writer, m_Data);

	if ( i_bResetDirtyFlag )
		m_bDirty = false;
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  todDocumentChunk::IsDirty() const
{
	return m_bDirty;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  todDocumentChunk::SetActive()
{
	// Make our data active
	todDataMgr::SetData(m_Data);
	todDataMgr::AddDataChangedCallback(this);
	m_bActive = true;

}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  todDocumentChunk::SetInactive( bool i_bUpdateData )
{
	if ( i_bUpdateData )
	{
		// Get current state of data
		//
		m_Data = todDataMgr::GetData();
		todDataMgr::RemoveDataChangedCallback(this);
	}

	m_bActive = false;

}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void todDocumentChunk::DataChanged()
{
	m_bDirty = true;
}
