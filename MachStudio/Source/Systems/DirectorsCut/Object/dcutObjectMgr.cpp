/*****************************************************************************
**	dcutObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"

#include "Systems/DirectorsCut/Object/dcutDirectorsCutObject.hpp"
#include "Systems/DirectorsCut/Object/dcutScriptObject.hpp"
#include "Systems/DirectorsCut/Timeline/dcutChannelCapture.hpp"

#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/tmln/tmlnTimeInOutMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoPickRay.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include <vector>


namespace
{
	std::vector<dcutScriptObject*> l_DirectorsCuts;
	int l_ObjectCounter = 0;

	//--------------------------------------------------------------------
	void clear_cameras()
	{
		envSTLHelpers::DeleteContainer(l_DirectorsCuts);
	}

	//--------------------------------------------------------------------
	//	return true if the filename is NOT a dupe
	bool verify_nodupe_name( char* i_Name )
	{
		for (int i=0; i<l_DirectorsCuts.size(); i++)
		{
			if ( l_DirectorsCuts[i]->GetName().GetString() == i_Name )
			{
				return false;
			}
		}
		return true;
	}

	//--------------------------------------------------------------------
	void create_default_name( const itString& i_Filename, nameString& o_NameString )
	{
		char strName[64];
		char filename_base[64];
		char *filename;
		strcpy( filename_base, itStringUtil::GetStdString( i_Filename ).c_str() );
		filename = strtok( filename_base, "." );

		do
		{
			sprintf( strName, "%s%02d", filename, l_ObjectCounter+1 );	// start out at 1 instead of 0
			o_NameString.SetString( strName );
			l_ObjectCounter++;
		} while ( !verify_nodupe_name(strName) );

		//DBG_LOG( "Added -- name (" << strName << ")"  );
	}

	//--------------------------------------------------------------------
	void create_directors_cut(const dcutScriptData &i_Data)
	{
		// create object for camera
		dcutScriptObject *cut = new dcutScriptObject(i_Data);
		l_DirectorsCuts.push_back(cut);

		if (i_Data.m_BaseData.m_Name.GetString().empty())
		{
			//	set a default name for the prop
			nameString objName;
			create_default_name( itString("CUT"), objName );
			cut->SetName( objName );
		}

		dcutDirectorsCutObject *pObject = cut->GetPickObject();
		camsDirectorsCutMgr::AddDirectorsCut( pObject->GetName(), 
			pObject->GetPropertyDescription().GetValue(),
			pObject);
	}

}	// end of namespace


//--------------------------------------------------------------------
//  Clear
//--------------------------------------------------------------------
void  dcutObjectMgr::Clear()
{
	camsDirectorsCutMgr::Clear();
	clear_cameras();
}

//--------------------------------------------------------------------
// Update individual base data
//--------------------------------------------------------------------
void dcutObjectMgr::SetBaseData(int i_Index, const dcutCueData& i_Data)
{
	l_DirectorsCuts[i_Index]->SetBaseData(i_Data);
}
dcutCueData dcutObjectMgr::GetBaseData(int i_Index)
{
	return l_DirectorsCuts[i_Index]->GetBaseData();
}

//--------------------------------------------------------------------
// Update individual camera data
//--------------------------------------------------------------------
void dcutObjectMgr::SetScriptData(int i_Index, const dcutScriptData& i_Data)
{
	DBG_ASSERT0( (i_Index >= 0 && i_Index < l_DirectorsCuts.size()), "index out of range" );

	l_DirectorsCuts[i_Index]->SetScriptData(i_Data);
	camsDirectorsCutMgr::SetDirectorsCutName(i_Index, i_Data.m_BaseData.m_Name.GetValue());
	camsDirectorsCutMgr::SetDirectorsCutDescription(i_Index, i_Data.m_BaseData.m_Description.GetValue());
	
	// notify_callbacks
}
dcutScriptData dcutObjectMgr::GetScriptData(int i_Index)
{
	return l_DirectorsCuts[i_Index]->GetScriptData();
}

//--------------------------------------------------------------------
//  Access to whole data as one structure for easy display and
// parsing
//--------------------------------------------------------------------
dcutCuesData dcutObjectMgr::GetData()
{
	dcutCuesData data;
	for (int i=0; i<l_DirectorsCuts.size(); i++)
	{
		data.m_Items.push_back( l_DirectorsCuts[i]->GetScriptData() );
	}
	return data;
}

//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void dcutObjectMgr::SetData(const dcutCuesData &i_Data)
{
	//camsDirectorsCutMgr::SetEditorCamera();
	clear_cameras();
	for (int i=0; i<i_Data.m_Items.size(); i++)
	{
		create_directors_cut(i_Data.m_Items[i]);
	}
}

//--------------------------------------------------------------------
// Append objects from this data into list, not allowing duplicates
//--------------------------------------------------------------------
void dcutObjectMgr::MergeData(const dcutCuesData &i_Data)
{
	const int num_items = i_Data.m_Items.size();
	for (int i=0; i<num_items; i++)
	{
		bool bDuplicate = false;
		const int num_objects = l_DirectorsCuts.size();
		for (int j=0; j<num_objects; j++)
		{
			if ( i_Data.m_Items[i].m_BaseData.m_Name.GetString() == 
					l_DirectorsCuts[j]->GetName().GetString() )
			{
				// found a duplicate name, don't add this object
				bDuplicate = true;
				break;
			}
		
		}
		if (!bDuplicate)
		{
			create_directors_cut( i_Data.m_Items[i] );
		}
	}
}

//--------------------------------------------------------------------
//  Get number of cameras
//--------------------------------------------------------------------
int dcutObjectMgr::GetNumObjects()
{
	return l_DirectorsCuts.size();
}

//--------------------------------------------------------------------
//  Add new camera to world
//--------------------------------------------------------------------
int  dcutObjectMgr::AddObject(const dcutScriptData& i_Data)
{
	create_directors_cut(i_Data);

	int index = l_DirectorsCuts.size() - 1;
	return index;
}

//--------------------------------------------------------------------
//  Select camera with given index
//--------------------------------------------------------------------
void  dcutObjectMgr::SelectObject(int i_Index, bool i_bAppend)
{
	DBG_ASSERT0( (i_Index >= 0 && i_Index < l_DirectorsCuts.size()), "index out of range" );

	if (sel3dMgr::GetSelected() != l_DirectorsCuts[i_Index]->GetPickObject())
	{
		if (i_bAppend)
			sel3dMgr::AddToSelection(l_DirectorsCuts[i_Index]->GetPickObject());
		else
			sel3dMgr::Select(l_DirectorsCuts[i_Index]->GetPickObject());
	}
}

//--------------------------------------------------------------------
//  Remove camera with given index from selection 
//--------------------------------------------------------------------
void  dcutObjectMgr::DeselectObject(int i_Index)
{
	DBG_ASSERT0( (i_Index >= 0 && i_Index < l_DirectorsCuts.size()), "index out of range" );

	if (sel3dMgr::GetSelected() != l_DirectorsCuts[i_Index]->GetPickObject())
	{
		sel3dMgr::RemoveFromSelection(l_DirectorsCuts[i_Index]->GetPickObject());
	}
}

//--------------------------------------------------------------------
//  Delete camera with given index
//--------------------------------------------------------------------
void  dcutObjectMgr::DeleteObject(int i_Index)
{
	DBG_ASSERT0( (i_Index >= 0 && i_Index < l_DirectorsCuts.size()), "index out of range" );

	// shift follow index down when deleting camera
	if (camsFollowUtil::GetFollowIndex() > i_Index)
		camsFollowUtil::SetFollowIndex(camsFollowUtil::GetFollowIndex()-1);
	else if (camsFollowUtil::GetFollowIndex() == i_Index)
		camsFollowUtil::SetEditorCamera();

	//DBG_LOG2("deleting dir-cut #%d %s", i_Index, l_DirectorsCuts[i_Index]->GetPickObject()->GetName().GetString().c_str() );

	sel3dMgr::ClearSelection();

	camsDirectorsCutMgr::RemoveDirectorsCut(i_Index);

	delete l_DirectorsCuts[i_Index];
	l_DirectorsCuts.erase(l_DirectorsCuts.begin() + i_Index);
}

//--------------------------------------------------------------------
// return index of given camera, returns -1 if not found
//--------------------------------------------------------------------
int dcutObjectMgr::GetIndexForObject(dcutDirectorsCutObject* i_pObject)
{
	for (int i=0; i<l_DirectorsCuts.size(); i++)
		if (l_DirectorsCuts[i]->GetPickObject() == i_pObject)
			return i;
	return -1;
}

//--------------------------------------------------------------------
// return index of given Name, returns -1 if not found
//--------------------------------------------------------------------
int dcutObjectMgr::GetIndexForObject( const nameString& i_Name )
{
	for (int i=0; i<l_DirectorsCuts.size(); i++)
	{
		if (l_DirectorsCuts[i]->GetPickObject()->GetName() == i_Name)
		{
			return i;
		}
	}
	return -1;
}


//--------------------------------------------------------------------
//  Get the camera
//--------------------------------------------------------------------
dcutScriptObject* dcutObjectMgr::GetObject(int i_Index)
{
	DBG_ASSERT0( (i_Index >= 0 && i_Index < l_DirectorsCuts.size()), "index out of range" );
	return l_DirectorsCuts[i_Index];
}
dcutDirectorsCutObject* dcutObjectMgr::GetPickObject(int i_Index)
{
	DBG_ASSERT0( (i_Index >= 0 && i_Index < l_DirectorsCuts.size()), "index out of range" );
	return l_DirectorsCuts[i_Index]->GetPickObject();
}
dcutScriptObject* dcutObjectMgr::GetObject( const nameString& i_Name )
{
	int index = dcutObjectMgr::GetIndexForObject( i_Name );

	if ( index != -1 )
		return l_DirectorsCuts[ index ];
	else
		return 0;
}

//--------------------------------------------------------------------
//	build the time in/out lists for all the cameras
//--------------------------------------------------------------------
void dcutObjectMgr::BuildTimeInOutLists()
{
	//	Get the in/out time for the cameras
	//
	for (int i=0; i<l_DirectorsCuts.size(); i++)
	{
		std::string& camname = l_DirectorsCuts[i]->GetName().GetString();
		//DBG_LOG("building IN-OUT lists for dir-cut " << camname.c_str() );
		dcutScriptObject* pObject = GetObject(l_DirectorsCuts[i]->GetName());
		if (pObject != 0)
		{
			pObject->CaptureChannel().GetInOutTimes(tmlnTimeInOutMgr::GetDataList(camname));
		}

		// DEBUG ONLY
		//
		//tmlnTimeInOutDataList tiolist = tmlnTimeInOutMgr::GetDataList( camname );
		//tiolist.Debug_DisplayData();
	}
}


