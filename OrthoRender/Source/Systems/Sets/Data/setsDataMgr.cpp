/*****************************************************************************
**	setsDataMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/Data/setsDataMgr.hpp"

#include "Systems/Sets/Data/setsScriptData.hpp"
#include "Systems/Sets/GUI/setsGeomList.hpp"
#include "Systems/Sets/Data/setsObject.hpp"

#include "Tool/api3d/api3dImport.hpp"
#include "Tool/api3d/api3dObject.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/geo/geoPickRay.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include <assert.h>


//============================================================================
//============================================================================
namespace setsDataMgr
{
	namespace
	{
		setsListData l_Data;
		std::vector<setsObject*> l_Objects; // owns objects pointed to
		std::vector<DataChangedCallback*> l_CallbackFuncs;

		api3dObject* load_item(itString i_Filename, fsLocator &o_Fullpath)
		{
			//DBG_LOG1( "load_item [%s]", itStringUtil::GetStdString( i_Filename ).c_str() );

			//	get the file path
			//
			fsysFileList file_list;
			setsGeomList::BuildFileList(file_list);
			file_list.GetFilePath(i_Filename, o_Fullpath);
			o_Fullpath.Push(i_Filename);

			api3dObject *obj = api3dImport::LoadObject(o_Fullpath);
			return obj;
		}

		void notify_callbacks()
		{
			static bool notifying = false;
			if (notifying) return;

			notifying = true;
			for (int i=0; i<l_CallbackFuncs.size(); i++)
				l_CallbackFuncs[i]->DataChanged();
			notifying = false;
		}

		void create_object(setsScriptData &i_Item)
		{
			fsLocator fullpath;
			api3dObject* pObject = load_item(i_Item.m_BaseData.m_Filename.GetValue(), fullpath);
			setsObject *pObj = new setsObject(pObject, fullpath);
			l_Objects.push_back(pObj);

			// construct name from filename
			nameString name(i_Item.m_BaseData.m_Filename.GetString());
			name.SetUID( i_Item.m_BaseData.m_Name.GetUID() );
			pObj->SetName(name);
			i_Item.m_BaseData.m_Name = name;

			//	register the name
			i_Item.m_BaseData.m_Name.RegisterName();

			l_Data.m_SetItems.push_back(i_Item);

			pObj->SetMaterialData( i_Item.m_Materials );
			itString modelName = i_Item.m_BaseData.m_Filename.GetValue();
			modelName.StripExtension();
			pObj->SetFragmentData( i_Item.m_Fragments, i_Item.m_AOData, itStringUtil::GetStdString(modelName) );
		}

		void remove_all_objects()
		{
			for (int i=0; i<l_Objects.size(); i++)
			{
				delete l_Objects[i];
			}
			l_Objects.clear();
		}

		void remove_object(int index)
		{
			delete l_Objects[index];
			l_Objects.erase(l_Objects.begin() + index);
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// Clear all items
	//--------------------------------------------------------------------
	void  Clear()
	{
		sel3dMgr::ClearSelection();
		l_Data = setsListData();
		remove_all_objects();
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Add new item to set
	//--------------------------------------------------------------------
	void  AddSetItem(setsScriptData &i_Item)
	{
		try
		{
			create_object( i_Item );
		}
		catch( fsFileDoesntExistX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			DBG_WARNING1( "cannot open file %s", filename.c_str() );

			std::string msg = "Cannot find file\n" + filename;
			guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
			//assert(false);
		}

		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Removes an item from the set
	//--------------------------------------------------------------------
	void  RemoveSetItem(const setsScriptData &i_Item)
	{
		for (int i=0; i<l_Data.m_SetItems.size(); i++)
		{
			if (l_Data.m_SetItems[i] == i_Item)
			{
				RemoveSetItem(i);
				return;
			}
		}
	}
	void  RemoveSetItem(int i_Index)
	{
		sel3dMgr::ClearSelection();
		remove_object(i_Index);
		l_Data.m_SetItems.erase(l_Data.m_SetItems.begin() + i_Index);
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Return number of set items
	//--------------------------------------------------------------------
	int  GetNumSetItems()
	{
		return l_Data.m_SetItems.size();
	}

	//--------------------------------------------------------------------
	//  Get data for a specfic item
	//--------------------------------------------------------------------
	const setsData& GetBaseData(int i_Index)
	{
		// No need to sync base data
		return l_Data.m_SetItems[i_Index].m_BaseData;
	}
	const setsScriptData & GetItemData(int i_Index)
	{
		// sync up material info from object to data
		l_Objects[i_Index]->GetMaterialData( l_Data.m_SetItems[i_Index].m_Materials );

		return l_Data.m_SetItems[i_Index];
	}

	//--------------------------------------------------------------------
	// return index of given Name, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForItem( const nameString& i_Name )
	{
		for (int i=0; i<l_Objects.size(); i++)
		{
			if ( l_Objects[i]->GetName().GetString() == i_Name.GetString() )
			{
				return i;
			}
		}
		return -1;
	}

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const setsListData & GetData()
	{
		// sync up material and info from object to data
		for (int i=0; i<l_Objects.size(); i++)
		{
			l_Objects[i]->GetMaterialData( l_Data.m_SetItems[i].m_Materials );
			l_Objects[i]->GetFragmentData( l_Data.m_SetItems[i].m_Fragments, l_Data.m_SetItems[i].m_AOData );
		}

		return l_Data;
	}

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	void SetData(setsListData &i_Data)
	{
		Clear();

		for (int i=0; i<i_Data.m_SetItems.size(); i++)
		{
			//DBG_LOG3( "creating set %d of %d [%s]", i, i_Data.m_SetItems.size(), itStringUtil::GetStdString( i_Data.m_SetItems[i].m_BaseData.m_Filename ).c_str() );

			// could call add item here, but i want to control the number of
			// dirty callbacks executed
			create_object( i_Data.m_SetItems[i] );
		}
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	// Append objects from this data into list, not allowing duplicates
	//--------------------------------------------------------------------
	void MergeData(setsListData &i_Data)
	{
		for (int i=0; i<i_Data.m_SetItems.size(); i++)
		{
			bool bDuplicate = false;
			for (int j=0; j<l_Data.m_SetItems.size(); j++)
			{
				if ( i_Data.m_SetItems[i] == l_Data.m_SetItems[j] )
				{
					// found a duplicate name, don't add this object
					bDuplicate = true;
					break;
				}
			
			}
			if (!bDuplicate)
			{
				create_object( i_Data.m_SetItems[i] );

				//DBG_LOG2( "Character Set Data %02d (%s)", i_Data.m_CharacterItems[i].m_BaseData.m_Name.GetUID(), i_Data.m_CharacterItems[i].m_BaseData.m_Name.GetString().c_str() );
			}
		}
		notify_callbacks();
	}

	//--------------------------------------------------------------------
	//  Changes visible state of set item with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible)
	{
		l_Data.m_SetItems[i_Index].m_BaseData.m_bEditorVisible = i_bVisible;
		l_Objects[i_Index]->SetEditorVisible(i_bVisible);
	}

	//--------------------------------------------------------------------
	// Make sure that all geometry is visible for rendering
	//--------------------------------------------------------------------
	void ConfirmGeometryVisible()
	{
		for (int i=0; i<l_Data.m_SetItems.size(); i++)
		{
			SetEditorVisible(i, true);
		}
	}

	//--------------------------------------------------------------------
	// This callback will be called when the data changes
	//--------------------------------------------------------------------
	void AddDataChangedCallback(DataChangedCallback *i_Func)
	{
		l_CallbackFuncs.push_back(i_Func);

	}
	void RemoveDataChangedCallback(DataChangedCallback *i_Func)
	{
		envSTLHelpers::RemoveOneValue(l_CallbackFuncs, i_Func);
	}

	//--------------------------------------------------------------------
	//	Ray pick occluders returning a modified tVal
	//--------------------------------------------------------------------
	bool OccluderPick( geoPickRay& i_Ray, float &o_tVal )
	{
		bool bIntersection = false;
		for ( int i=0; i < l_Objects.size(); ++i )
		{
			bIntersection |= l_Objects[i]->RayPick( i_Ray.GetRayStart(),
												 i_Ray.GetRayEnd(),
												 o_tVal);
		}

		return bIntersection;
	}

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode)
	{
		for ( int i=0; i < l_Objects.size(); ++i )
		{
			if (l_Objects[i]->MatchPickCode( i_PickCode ) )
				return l_Objects[i];
		}
		return NULL;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	setsObject* GetObject(int i_Index)
	{
		DBG_ASSERT0( (i_Index >= 0) && (i_Index < l_Objects.size()), "Invalid index to GetObject call" );

		return l_Objects[i_Index];
	}

	//--------------------------------------------------------------------
	//	Select Object from Placed
	//--------------------------------------------------------------------
	void SelectObject(int i_Index, bool i_bAppend)
	{
		if (sel3dMgr::GetSelected() != l_Objects[i_Index])
		{
			sel3dMgr::CreateUndoOperation();
			if (i_bAppend)
				sel3dMgr::AddToSelection(l_Objects[i_Index]);
			else
				sel3dMgr::Select(l_Objects[i_Index]);
		}
		
	}

	//--------------------------------------------------------------------
	//	Remove Object from Selection
	//--------------------------------------------------------------------
	void  DeselectObject(int i_Index)
	{
		if (sel3dMgr::GetSelected() != l_Objects[i_Index])
		{
			sel3dMgr::CreateUndoOperation();
			sel3dMgr::RemoveFromSelection(l_Objects[i_Index]);
		}
		
	}


}	// end of namespace
