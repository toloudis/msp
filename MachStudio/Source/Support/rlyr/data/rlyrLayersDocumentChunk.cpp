/*****************************************************************************
**	rlyrLayersDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005-9 - All Rights Reserved
\****************************************************************************/
#include "Support/rlyr/data/rlyrLayersDocumentChunk.hpp"

#include "Support/mnm/mnmPaths.hpp"
#include "Support/rlyr/data/rlyrLayersDataParser.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"

#include <assert.h>
#include <string>


//============================================================================
//============================================================================
namespace
{
	rlyrLayersData l_TempLayers;
}

//============================================================================
// static variable
//============================================================================
rlyrLayersDocumentChunk* rlyrLayersDocumentChunk::sm_pActiveChunk = NULL;


//--------------------------------------------------------------------
// Call DataChanged function on the active document chunk
//--------------------------------------------------------------------
//static 
void rlyrLayersDocumentChunk::ActiveDataChanged()
{
	if (sm_pActiveChunk != NULL)
		sm_pActiveChunk->DataChanged();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rlyrLayersDocumentChunk::rlyrLayersDocumentChunk() 
:	m_bDirty(false), 
	m_bActive(false)
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  rlyrLayersDocumentChunk::Clear()
{
	m_bDirty = false;

	if (this->m_bActive)
	{
		
	}
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  rlyrLayersDocumentChunk::GetChunkName() const
{
	return rlyrLayersDataParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual
const char* rlyrLayersDocumentChunk::GetChunkDesc() const
{
	return "Render Layers";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual
void rlyrLayersDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
	o_List.clear();

	std::string item;
	for( int i = 0; i < m_LayerData.m_Layers.size(); i++)
	{
		item = m_LayerData.m_Layers[i].m_Name.GetValue();
		o_List.push_back( item );
	}
	l_TempLayers = m_LayerData;
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual
void rlyrLayersDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
	// remove items from the manager
	//
	std::vector<rlyrLayerDataItem>::iterator it;
	for (int i = 0; i < i_List.size(); ++i)
	{
		for(it = m_LayerData.m_Layers.begin(); it != m_LayerData.m_Layers.end(); ++it)
		{		
			if(i_List[i] == (*it).m_Name.GetValue())
			{
				m_LayerData.m_Layers.erase(it);
				break;
			}
		}
	}
	l_TempLayers = m_LayerData;
}

//--------------------------------------------------------------------
//  Import chunk data and add it to existing data
//--------------------------------------------------------------------
void  rlyrLayersDocumentChunk::Import(chReader &i_Reader,
							  chDefs::Version i_Version,
							  chDefs::Size i_Size)
{
	//	If this is the active chunk and there is already data,
	//	then do a merge of the new data into the manager
	// 
	if (this->m_bActive)
	{
		rlyrRenderLayerMgr::MergeData(l_TempLayers);
		l_TempLayers.m_Layers.clear();
	
		//reset the data in the manager
		rlyrRenderLayerMgr::GetData(m_LayerData);
		rlyrRenderLayerMgr::SetData(m_LayerData);
	}
	else
	{}

	//	if importing data, then this chunk is dirty
	this->DataChanged();
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  rlyrLayersDocumentChunk::Read(	chReader &i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size )
{
	//update the current manager in case this is an import read
	rlyrRenderLayerMgr::Update();

	//clear the list and read in new data
	m_LayerData.m_Layers.clear();
	rlyrLayersDataParser::ReadData( i_Reader, i_Version, i_Size, m_LayerData );
	if (this->m_bActive)
	{
		rlyrRenderLayerMgr::SetData(m_LayerData);	
	}
	else
	{}

	m_bDirty = false;
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void  rlyrLayersDocumentChunk::Write( chWriter &i_Writer, bool i_bResetDirtyFlag )
{
	if (this->m_bActive)
	{
		//in case there is some data that hasn't been populated through the manager
		rlyrRenderLayerMgr::Update();
		rlyrRenderLayerMgr::GetData(m_LayerData);
		
		rlyrLayersDataParser::WriteData( i_Writer, m_LayerData );
	}
	else
	{
		rlyrLayersDataParser::WriteData( i_Writer, m_LayerData );
	}

	if ( i_bResetDirtyFlag )
		m_bDirty = false;
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  rlyrLayersDocumentChunk::IsDirty() const
{
	return m_bDirty;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  rlyrLayersDocumentChunk::SetActive()
{
	m_bActive = true;
	sm_pActiveChunk = this;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  rlyrLayersDocumentChunk::SetInactive( bool i_bUpdateData )
{
	m_bActive = false;
	if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void rlyrLayersDocumentChunk::DataChanged()
{
	m_bDirty = true;
}


