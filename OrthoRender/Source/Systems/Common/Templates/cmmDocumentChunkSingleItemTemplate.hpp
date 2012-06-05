/*****************************************************************************
**	cmmDocumentChunkSingleItemTemplate.hpp
**
**	 Document chunk for systems with object managers and a single object
**	that can be imported separately.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_DOCUMENTCHUNKSINGLEITEMTEMPLATE_HPP
#error cmmDocumentChunkSingleItemTemplate.hpp multiply included
#endif
#define CMM_DOCUMENTCHUNKSINGLEITEMTEMPLATE_HPP

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
template<class xxxScriptData, 
		 class xxxDataParser, 
		 class xxxObjectMgr, 
		 class xxxDialogUtil>
class cmmDocumentChunkSingleItemTemplateBase : public docDocumentChunk
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
	cmmDocumentChunkSingleItemTemplateBase()
		: m_bDirty(false), m_bActive(false)
	{
	}

	//--------------------------------------------------------------------
	//  Clear document data back to initial state
	//--------------------------------------------------------------------
	virtual void  Clear()
	{
		// reset to default constructor
		m_Data = xxxScriptData();
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
			xxxScriptData new_data;
			xxxDataParser::ReadData( i_Reader, i_Version, i_Size, new_data );
			xxxObjectMgr::MergeData(new_data);

			m_Data = xxxObjectMgr::GetData();

			xxxDialogUtil::UpdateListDialog();
		}
		else
		{
			// append data to our m_Data field
			xxxDataParser::ReadData(i_Reader, i_Version, i_Size, m_Data);
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
		xxxDataParser::ReadData( i_Reader, i_Version, i_Size, m_Data );

		if (this->m_bActive)
		{
			DBG_ASSERT0(xxxObjectMgr::GetNumObjects() == 1, "Singleton template object mgr must have exactly one object.");
			xxxObjectMgr::SetData( m_Data );
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
			xxxDataParser::WriteData( i_Writer, m_Data );
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
		xxxObjectMgr::SetData(m_Data);

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
			m_Data = xxxObjectMgr::GetData();
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
		o_List.resize(1);
		if (this->m_bActive)
		{
			o_List[0] = GetItemName(xxxObjectMgr::GetData()).GetString();
		}
		else
		{
			o_List[0] = GetItemName(m_Data).GetString();
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
			xxxObjectMgr::DeleteObject( );
		}
		else
		{
			// can't remove the data object, so
			// just revert the data to default state.
			m_Data = xxxScriptData();
		}
	}


private:
	//--------------------------------------------------------------------
	//	ClearNameUIDs() - set all the name UIDs to be invalid.  this 
	//	is important when importing data.
	//--------------------------------------------------------------------
	void ClearNameUIDs()
	{
		GetItemName(m_Data).SetUID( nameString::e_InvalidUID );
	}

protected:
	//--------------------------------------------------------------------
	//	Accessors to name string from an index into the list.
	//	Some data structs have the name directly, some have m_BaseData.
	//--------------------------------------------------------------------
	virtual const prtyName& GetItemName(const xxxScriptData& i_Data) const = 0;
	virtual prtyName& GetItemName(xxxScriptData& i_Data) const = 0;

	//--------------------------------------------------------------------
	// protected data
	//--------------------------------------------------------------------
	xxxScriptData m_Data;
	mutable bool m_bDirty;
	bool m_bActive;				
	static cmmDocumentChunkSingleItemTemplateBase* sm_pActiveChunk;

};

//============================================================================
// DocumentChunk class that has script and base data
//============================================================================
template<class xxxScriptData, 
		 class xxxDataParser, 
		 class xxxObjectMgr, 
		 class xxxDialogUtil>
class cmmDocumentChunkSingleItemTemplate : public cmmDocumentChunkSingleItemTemplateBase<xxxScriptData,xxxDataParser,xxxObjectMgr,xxxDialogUtil>
{
private:
	//--------------------------------------------------------------------
	//	Accessors to name string from an index into the list.
	//	Some data structs have the name directly, some have m_BaseData.
	//--------------------------------------------------------------------
	virtual const prtyName& GetItemName(const xxxScriptData& i_Data) const
	{
		return i_Data.m_BaseData.m_Name;
	}
	virtual prtyName& GetItemName(xxxScriptData& i_Data) const
	{
		return i_Data.m_BaseData.m_Name;
	}

};

//============================================================================
// DocumentChunk class that has only a single data type.
//============================================================================
template<class xxxScriptData, 
		 class xxxDataParser, 
		 class xxxObjectMgr, 
		 class xxxDialogUtil>
class cmmDocumentChunkSingleItemTemplateSimple : public cmmDocumentChunkSingleItemTemplateBase<xxxScriptData,xxxDataParser,xxxObjectMgr,xxxDialogUtil>
{
private:
	//--------------------------------------------------------------------
	//	Accessors to name string from an index into the list.
	//	Some data structs have the name directly, some have m_BaseData.
	//--------------------------------------------------------------------
	virtual const prtyName& GetItemName(const xxxScriptData& i_Data) const
	{
		return i_Data.m_Name;
	}
	virtual prtyName& GetItemName(xxxScriptData& i_Data) const
	{
		return i_DataList.m_Name;
	}

};

//============================================================================
// Initialiazing static members
//============================================================================
template<class xxxScriptData, class xxxDataParser, class xxxObjectMgr, class xxxDialogUtil>
cmmDocumentChunkSingleItemTemplateBase<xxxScriptData, xxxDataParser, xxxObjectMgr, xxxDialogUtil>* cmmDocumentChunkSingleItemTemplateBase<xxxScriptData, xxxDataParser, xxxObjectMgr, xxxDialogUtil>::sm_pActiveChunk = NULL;
