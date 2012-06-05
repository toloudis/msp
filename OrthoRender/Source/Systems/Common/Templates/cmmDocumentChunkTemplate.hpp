/*****************************************************************************
**	cmmDocumentChunkTemplate.hpp
**
**	 Document chunk for systems with object managers and lists of objects
**	that can be imported separately.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_DOCUMENTCHUNKTEMPLATE_HPP
#error cmmDocumentChunkTemplate.hpp multiply included
#endif
#define CMM_DOCUMENTCHUNKTEMPLATE_HPP

#ifndef DBG_ASSERT_HPP
#include "Core/dbg/dbgAssert.hpp"
#endif
#ifndef DOC_DOCUMENTCHUNK_HPP
#include "Tool/doc/docDocumentChunk.hpp"
#endif
#ifndef ENV_STLHELPERS_HPP
#include "Core/env/envSTLHelpers.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif 


//============================================================================
//============================================================================
template<class xxxListData, 
		 class xxxDataParser, 
		 class xxxObjectMgr, 
		 class xxxDialogUtil>
class cmmDocumentChunkTemplateBase : public docDocumentChunk
{
public:
	//--------------------------------------------------------------------
	// Call DataChanged function on the active document chunk
	//--------------------------------------------------------------------
	static void ActiveDataChanged()
	{
		if (sm_pActiveChunk != NULL)
			sm_pActiveChunk->DataChanged();
	}

public:

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmmDocumentChunkTemplateBase()
		: m_bDirty(false), m_bActive(false)
	{
	}

	//--------------------------------------------------------------------
	//  Clear document data back to initial state
	//--------------------------------------------------------------------
	virtual void  Clear()
	{
		m_DataList.Clear();
		m_bDirty = false;

		if (this->m_bActive)
		{
			xxxObjectMgr::Clear();
			xxxDialogUtil::UpdateListDialog();
		}
	}

	//--------------------------------------------------------------------
	//  returns chunk name for this data type
	//--------------------------------------------------------------------
	virtual chDefs::Name  GetChunkName() const
	{
		return xxxDataParser::GetChunkName();
	}

	//--------------------------------------------------------------------
	//  build a list of resources used in this chunk
	//
	//	this is used for keeping track of what resources are used in
	//	this chunk of a document.
	//--------------------------------------------------------------------
	virtual void GetResourceList( fsResourceTrackerData& io_List )
	{
		xxxObjectMgr::GetResourceList( io_List );
	}

	//--------------------------------------------------------------------
	//  Import chunk data and add it to existing data
	//--------------------------------------------------------------------
	virtual void  Import(chReader &i_Reader,
						 chDefs::Version i_Version,
						 chDefs::Size i_Size)
	{
		//	If this is the active chunk, do a merge of the new data 
		//  into the manager
		//
		if (this->m_bActive)
		{
			xxxListData new_data;
			xxxDataParser::ReadData( i_Reader, i_Version, i_Size, new_data );
			xxxObjectMgr::MergeData(new_data);

			m_DataList = xxxObjectMgr::GetData();

			xxxDialogUtil::UpdateListDialog();
		}
		else
		{
			// append data to our m_Data field
			xxxDataParser::ReadData(i_Reader, i_Version, i_Size, m_DataList);
		}
			
		//	if importing data, then this chunk is dirty
		this->DataChanged();
	}


	//--------------------------------------------------------------------
	//  Read chunk data
	//--------------------------------------------------------------------
	virtual void  Read(chReader &i_Reader,
					   chDefs::Version i_Version,
					   chDefs::Size i_Size)
	{
		xxxDataParser::ReadData( i_Reader, i_Version, i_Size, m_DataList );

		if (this->m_bActive)
		{
			DBG_ASSERT0(xxxObjectMgr::GetNumObjects() == 0, "Read() should be called after Clear(), use Import() to append items");
			xxxObjectMgr::SetData( m_DataList );
			xxxDialogUtil::UpdateListDialog();
		}

		m_bDirty = false;
	}


	//--------------------------------------------------------------------
	//  Write chunk data
	//--------------------------------------------------------------------
	virtual void  Write(chWriter &i_Writer, bool i_bResetDirtyFlag = true)
	{
		if (this->m_bActive)
		{
			xxxDataParser::WriteData( i_Writer, xxxObjectMgr::GetData() );
		}
		else
		{
			ClearNameUIDs();
			xxxDataParser::WriteData( i_Writer, m_DataList );
		}

		if ( i_bResetDirtyFlag )
			m_bDirty = false;
	}


	//--------------------------------------------------------------------
	//  Return true if the chunk has been modified since the last
	// call to Write()
	//--------------------------------------------------------------------
	virtual bool  IsDirty() const
	{
		return m_bDirty;
	}

	//--------------------------------------------------------------------
	//  This chunk is currently made active, add data to the scene
	// or world
	//--------------------------------------------------------------------
	virtual void  SetActive()
	{
		// Make our data active
		xxxObjectMgr::SetData(m_DataList);

		m_bActive = true;
		sm_pActiveChunk = this;
	}

	//--------------------------------------------------------------------
	//  this chunk is currently being made in active, remove data
	// from scene or world
	//--------------------------------------------------------------------
	virtual void  SetInactive( bool i_bUpdateData = true )
	{
		if ( i_bUpdateData )
		{
			// Get current state of data
			//
			m_DataList.Clear();
			m_DataList = xxxObjectMgr::GetData();
		}

		m_bActive = false;
		if (sm_pActiveChunk == this) sm_pActiveChunk = NULL;
	}

	//--------------------------------------------------------------------
	// Callback when dialog changes data, sets dirty bit
	//--------------------------------------------------------------------
	virtual void DataChanged()
	{
		m_bDirty = true;
	}


	//--------------------------------------------------------------------
	//  build a list of "items" in this chunk
	//
	//	this is used for itemizing of objects in this chunk.  It can
	//	also be used to select parts of this chunk as "active" or not.
	//--------------------------------------------------------------------
	virtual void BuildDataList( std::vector<std::string>& o_List )
	{
		if (this->m_bActive)
		{
			BuildDataList( xxxObjectMgr::GetData(), o_List );
		}
		else
		{
			BuildDataList( m_DataList, o_List );
		}
	}

	//--------------------------------------------------------------------
	//	remove the items in this chunk by name.  each chunk built the 
	//	list using BuildDataList() so each chunk will know what to do
	//	with this data.
	//--------------------------------------------------------------------
	virtual void RemoveItems( const std::vector<std::string>& i_List )
	{
		// if active, remove items from the manager, otherwise from the data list.
		if ( this->m_bActive )
		{
			// remove each item from the manager
			for ( int i = 0 ; i < i_List.size() ; i++ )
			{
				//	find the index and then remove it
				int index = xxxObjectMgr::GetIndexForObject( nameString( i_List[i] ) );
				if ( index >= 0 )
				{
					xxxObjectMgr::DeleteObject( index );
				}
			}
		}
		else
		{
			//	search for each item in the list
			for ( int j = 0 ; j < i_List.size() ; j++ )
			{
				//	go through the existing items and find the match
				for ( int w = 0 ; w < m_DataList.m_Items.size() ; w++ )
				{
					//  find the match
					if ( GetItemName(m_DataList, w) == i_List[j] )
					{
						//	erase the item in the list
						envSTLHelpers::RemoveOneValue( m_DataList.m_Items, m_DataList.m_Items[w] );
						break;
					}
				}
			}
		}
	}


private:
	//--------------------------------------------------------------------
	//  build a list of "items" in this chunk
	//
	//	this is used for itemizing of objects in this chunk.  It can
	//	also be used to select parts of this chunk as "active" or not.
	//--------------------------------------------------------------------
	void BuildDataList( const xxxListData& i_DataList, std::vector<std::string>& o_DataList )
	{
		const int num_items = i_DataList.m_Items.size();
		o_DataList.resize( num_items );
		for (int i = 0 ; i < num_items ; i++ )
		{
			o_DataList[i] = GetItemName(i_DataList, i).GetString();
		}
	}


	//--------------------------------------------------------------------
	//	ClearNameUIDs() - set all the name UIDs to be invalid.  this 
	//	is important when importing data.
	//--------------------------------------------------------------------
	void ClearNameUIDs()
	{
		int num = m_DataList.m_Items.size();

		int i;
		for ( i = 0 ; i < num ; i++ )
		{
			GetItemName(m_DataList, i).SetUID( nameString::e_InvalidUID );
		}
	}

protected:
	//--------------------------------------------------------------------
	//	Accessors to name string from an index into the list.
	//	Some data structs have the name directly, some have m_BaseData.
	//--------------------------------------------------------------------
	virtual const prtyName& GetItemName(const xxxListData& i_DataList, int i_Index) const = 0;
	virtual prtyName& GetItemName(xxxListData& i_DataList, int i_Index) const = 0;

	//--------------------------------------------------------------------
	// protected data
	//--------------------------------------------------------------------
	xxxListData m_DataList;
	mutable bool m_bDirty;
	bool m_bActive;				
	static cmmDocumentChunkTemplateBase* sm_pActiveChunk;

};

//============================================================================
// DocumentChunk class that has script and base data
//============================================================================
template<class xxxListData, 
		 class xxxDataParser, 
		 class xxxObjectMgr, 
		 class xxxDialogUtil>
class cmmDocumentChunkTemplate : public cmmDocumentChunkTemplateBase<xxxListData,xxxDataParser,xxxObjectMgr,xxxDialogUtil>
{
private:
	//--------------------------------------------------------------------
	//	Accessors to name string from an index into the list.
	//	Some data structs have the name directly, some have m_BaseData.
	//--------------------------------------------------------------------
	virtual const prtyName& GetItemName(const xxxListData& i_DataList, int i_Index) const
	{
		return i_DataList.m_Items[i_Index].m_BaseData.m_Name;
	}
	virtual prtyName& GetItemName(xxxListData& i_DataList, int i_Index) const
	{
		return i_DataList.m_Items[i_Index].m_BaseData.m_Name;
	}

};

//============================================================================
// DocumentChunk class that has only a single data type.
//============================================================================
template<class xxxListData, 
		 class xxxDataParser, 
		 class xxxObjectMgr, 
		 class xxxDialogUtil>
class cmmDocumentChunkTemplateSimple : public cmmDocumentChunkTemplateBase<xxxListData,xxxDataParser,xxxObjectMgr,xxxDialogUtil>
{
private:
	//--------------------------------------------------------------------
	//	Accessors to name string from an index into the list.
	//	Some data structs have the name directly, some have m_BaseData.
	//--------------------------------------------------------------------
	virtual const prtyName& GetItemName(const xxxListData& i_DataList, int i_Index) const
	{
		return i_DataList.m_Items[i_Index].m_Name;
	}
	virtual prtyName& GetItemName(xxxListData& i_DataList, int i_Index) const
	{
		return i_DataList.m_Items[i_Index].m_Name;
	}

};

//============================================================================
// Initialiazing static members
//============================================================================
template<class xxxListData, class xxxDataParser, class xxxObjectMgr, class xxxDialogUtil>
cmmDocumentChunkTemplateBase<xxxListData, xxxDataParser, xxxObjectMgr, xxxDialogUtil>* cmmDocumentChunkTemplateBase<xxxListData, xxxDataParser, xxxObjectMgr, xxxDialogUtil>::sm_pActiveChunk = NULL;
