/*****************************************************************************
**	setsDocumentChunk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/Data/setsDocumentChunk.hpp"

#include "Systems/Sets/Data/setsDataParser.hpp"

#include "Core/dbg/dbgMsg.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
setsDocumentChunk::setsDocumentChunk() 
:	m_bDirty(false), 
	m_bActive(false)
{
}

//--------------------------------------------------------------------
//  Clear document data back to initial state
//--------------------------------------------------------------------
void  setsDocumentChunk::Clear()
{
	m_Data		= setsListData();
	m_bDirty	= false;

	if (this->m_bActive)
		setsDataMgr::Clear();
}

//--------------------------------------------------------------------
//  returns chunk name for this data type
//--------------------------------------------------------------------
chDefs::Name  setsDocumentChunk::GetChunkName() const
{
	return setsDataParser::GetChunkName();
}

//--------------------------------------------------------------------
//  returns chunk description for display purposes
//--------------------------------------------------------------------
//virtual 
const char* setsDocumentChunk::GetChunkDesc() const
{
	return "Sets";
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
//virtual 
void setsDocumentChunk::BuildDataList( std::vector<std::string>& o_List )
{
	if (this->m_bActive)
		BuildDataList( setsDataMgr::GetData(), o_List );
	else
		BuildDataList( m_Data, o_List );
}

//--------------------------------------------------------------------
//	remove the items in this chunk by name.  each chunk built the 
//	list using BuildDataList() so each chunk will know what to do
//	with this data.
//--------------------------------------------------------------------
//virtual 
void setsDocumentChunk::RemoveItems( const std::vector<std::string>& i_List )
{
	//DBG_LOG( "Sets removeitems()" );

	// if active, remove items from the manager, otherwise from the data list.
	//
	if ( this->m_bActive )
	{
		//DBG_LOG( "active" );

		// remove each item from the manager
		//
		for ( int i = 0 ; i < i_List.size() ; i++ )
		{
			//DBG_LOG2( "%d - %s", i, i_List[i] );

			//	find the index and then remove it
			//
			int index = setsDataMgr::GetIndexForItem( nameString( i_List[i] ) );
			if ( index >= 0 )
			{
				setsDataMgr::RemoveSetItem( index );
			}
		}
	}
	else
	{
		DBG_LOG( "inactive (" << i_List.size() << ")" );

		//
		//int k;
		//for ( k = 0 ; k < m_Data.m_SetItems.size() ; k++ )
		//{
		//	DBG_LOG2( " before remove Sets %02d %s", k, m_Data.m_SetItems[k].m_Name.c_str() );
		//}

		int j;
		//	search for each item in the list
		//
		for ( j = 0 ; j < i_List.size() ; j++ )
		{
			//DBG_LOG2( " looking for Sets #%d (%s)", j, i_List[j].c_str() );

			//	go through the existing items and find the match
			//
			int w;
			for ( w = 0 ; w < m_Data.m_SetItems.size() ; w++ )
			{
				//DBG_LOG( "   vs [" <<  m_Data.m_SetItems[w].m_Name.c_str() << "]???" );

				//  find the match
				//
				if ( m_Data.m_SetItems[w].m_BaseData.m_Filename.GetString() == i_List[j] )
				{
					//DBG_LOG2( "  removing Sets object %02d [datalist] %s", w, m_Data.m_SetItems[w].m_Name.c_str() );

					//	erase the item in the list
					//
					envSTLHelpers::RemoveOneValue( m_Data.m_SetItems, m_Data.m_SetItems[w] );
					break;
				}
			}
		}

		//
		//for ( j = 0 ; j < m_Data.m_SetItems.size() ; j++ )
		//{
		//	DBG_LOG2( "Sets list after REMOVE = Setss %02d %s", j, m_Data.m_SetItems[j].m_Name.c_str() );
		//}
	}
}

//--------------------------------------------------------------------
//  Import chunk data and add it to existing data
//--------------------------------------------------------------------
void  setsDocumentChunk::Import(chReader &i_Reader,
							  chDefs::Version i_Version,
							  chDefs::Size i_Size)
{
	//	If this is the active chunk, do a merge of the new data 
	//  into the manager
	if (this->m_bActive)
	{
		setsListData new_data;
		setsDataParser::ReadData( i_Reader, i_Version, i_Size, new_data );
		setsDataMgr::MergeData(new_data);

		m_Data = setsDataMgr::GetData();
	}
	else
	{
		// append data to our m_Data field
		setsDataParser::ReadData(i_Reader, i_Version, i_Size, m_Data);
	}
		
	//	if importing data, then this chunk is dirty
	this->DataChanged();
}

//--------------------------------------------------------------------
//  Read chunk data
//--------------------------------------------------------------------
void  setsDocumentChunk::Read( chReader &i_Reader,
							  chDefs::Version i_Version,
							  chDefs::Size i_Size )
{
	setsDataParser::ReadData( i_Reader, i_Version, i_Size, m_Data );

	if (this->m_bActive)
	{
		DBG_ASSERT0(setsDataMgr::GetNumSetItems() == 0, "Read() should be called after Clear(), use Import() to append items");
		setsDataMgr::SetData( m_Data );
	}

	m_bDirty = false;
}

//--------------------------------------------------------------------
//  Write chunk data
//--------------------------------------------------------------------
void  setsDocumentChunk::Write(chWriter &i_Writer, bool i_bResetDirtyFlag )
{
	if (this->m_bActive)
		setsDataParser::WriteData(i_Writer, setsDataMgr::GetData());
	else
		setsDataParser::WriteData(i_Writer, m_Data);

	if ( i_bResetDirtyFlag )
		m_bDirty = false;
}

//--------------------------------------------------------------------
//  Return true if the chunk has been modified since the last
// call to Write()
//--------------------------------------------------------------------
bool  setsDocumentChunk::IsDirty() const
{
	return m_bDirty;
}

//--------------------------------------------------------------------
//  This chunk is currently made active, add data to the scene
// or world
//--------------------------------------------------------------------
void  setsDocumentChunk::SetActive()
{
	// Make our data active
	setsDataMgr::SetData(m_Data);
//	setsDataMgr::AddDataChangedCallback(this);
	m_bActive = true;
}

//--------------------------------------------------------------------
//  this chunk is currently being made in active, remove data
// from scene or world
//--------------------------------------------------------------------
void  setsDocumentChunk::SetInactive( bool i_bUpdateData )
{
	if ( i_bUpdateData )
	{
		// Get current state of data
		//
		m_Data.m_SetItems.clear();
		m_Data = setsDataMgr::GetData();
//		setsDataMgr::RemoveDataChangedCallback(this);
	}

	m_bActive = false;
}

//--------------------------------------------------------------------
// Callback when dialog changes data, sets dirty bit
//--------------------------------------------------------------------
void setsDocumentChunk::DataChanged()
{
	m_bDirty = true;
}

//--------------------------------------------------------------------
//  build a list of "items" in this chunk
//
//	this is used for itemizing of objects in this chunk.  It can
//	also be used to select parts of this chunk as "active" or not.
//--------------------------------------------------------------------
void setsDocumentChunk::BuildDataList( const setsListData& i_List, std::vector<std::string>& o_DataList )
{
	int num = i_List.m_SetItems.size();
	o_DataList.resize( num );

	int i;
	for ( i = 0 ; i < num ; i++ )
	{
		//DBG_LOG3( "sets BDL: %d of %d [%s]", i, num, itStringUtil::GetStdString( i_List.m_SetItems[i].m_Filename ).c_str() );

		o_DataList[i] = i_List.m_SetItems[i].m_BaseData.m_Filename.GetString();
	}
}
