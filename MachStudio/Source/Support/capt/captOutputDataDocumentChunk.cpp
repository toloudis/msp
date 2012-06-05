/*****************************************************************************
**	captOutputDataDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005-9 - All Rights Reserved
\****************************************************************************/
#include "Support/capt/captOutputDataDocumentChunk.hpp"

#include "Support/capt/captOutputDataParser.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"

#include <assert.h>
#include <string>


//============================================================================
//============================================================================
namespace
{
	captRenderOutputData l_TempData;
}

//============================================================================
// static variable
//============================================================================
captOutputDataDocumentChunk* captOutputDataDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Call DataChanged function on the active document chunk
//--------------------------------------------------------------------
//static 
void captOutputDataDocumentChunk::ActiveDataChanged()
{
	if (sm_pActiveChunk != NULL)
		sm_pActiveChunk->DataChanged();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
captOutputDataDocumentChunk::captOutputDataDocumentChunk() 
:	m_bDirty(false), 
	m_bActive(false)
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  captOutputDataDocumentChunk::Clear()
{
	m_bDirty = false;

	if (this->m_bActive)
	{
		
	}
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  captOutputDataDocumentChunk::GetChunkName() const
{
	return captOutputDataParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* captOutputDataDocumentChunk::GetChunkDesc() const
{
	return "Capture Output Data";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void captOutputDataDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
	o_List.clear();
	std::string item;

	item = "Capture Options";
	o_List.push_back( item );

	l_TempData = m_CaptureData;
	l_TempData.m_bChunkData.SetValue(true);
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void captOutputDataDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
	// if active, remove items from the manager, otherwise from the data list.
	//
	if ( this->m_bActive )
	{
		if( i_List.size() > 0 )
			l_TempData.m_bChunkData.SetValue(false);
	}

	else
	{}
}

//--------------------------------------------------------------------
//  Import chunk data and add it to existing data
//--------------------------------------------------------------------
void  captOutputDataDocumentChunk::Import(chReader &i_Reader,
							  chDefs::Version i_Version,
							  chDefs::Size i_Size)
{
	//	If this is the active chunk and there is already data,
	//	then do a merge of the new data into the manager
	// 
	if (this->m_bActive)
	{
		if(l_TempData.m_bChunkData.GetValue() == true)
		{		
			captRenderOutputData& data = captRenderOutputDataUtil::Data();
			l_TempData.m_bChunkData.SetValue(false);
			data = l_TempData;
		}	
	}
	else
	{}

	//	if importing data, then this chunk is dirty
	//this->DataChanged();
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  captOutputDataDocumentChunk::Read(	chReader &i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size )
{

	m_LayerData.m_Layers.clear();
	captOutputDataParser::ReadData( i_Reader, i_Version, i_Size, m_CaptureData, m_LayerData );
	if (this->m_bActive)
	{
		// render layers will become its own document chunk
		if(i_Version == 1)
			rlyrRenderLayerMgr::SetData(m_LayerData);
	
		captRenderOutputDataUtil::CleanUp();
		if(m_CaptureData.m_bChunkData.GetValue())
		{		
			captRenderOutputData& data = captRenderOutputDataUtil::Data();
			m_CaptureData.m_bChunkData.SetValue(false);
			//this needs to get set since it isn't written to file
			m_CaptureData.m_RenderPosTime.SetValue(captRenderOutputData::c_InitRenderPosTime);
			data = m_CaptureData;
		}		
	}

	m_bDirty = false;
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void  captOutputDataDocumentChunk::Write( chWriter &i_Writer, bool i_bResetDirtyFlag )
{
	if (this->m_bActive)
	{
		m_LayerData.m_Layers.clear();
		//rlyrRenderLayerMgr::GetData(m_LayerData);
		m_CaptureData = captRenderOutputDataUtil::Data();
		m_CaptureData.m_bChunkData.SetValue(true);
		
		captOutputDataParser::WriteData( i_Writer, m_CaptureData );
		m_CaptureData.m_bChunkData.SetValue(false);
	}
	else
	{
		captOutputDataParser::WriteData( i_Writer, m_CaptureData );
	}

	if ( i_bResetDirtyFlag )
		m_bDirty = false;
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  captOutputDataDocumentChunk::IsDirty() const
{
	return m_bDirty;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  captOutputDataDocumentChunk::SetActive()
{
	m_bActive = true;
	sm_pActiveChunk = this;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  captOutputDataDocumentChunk::SetInactive( bool i_bUpdateData )
{
	m_bActive = false;
	if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void captOutputDataDocumentChunk::DataChanged()
{
	m_bDirty = true;
}


