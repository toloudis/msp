/*****************************************************************************
**	sbrdObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"

#include "Systems/Storyboards/Timeline/sbrdDriverCreator.hpp"
#include "Systems/Storyboards/GUI/sbrdGeomList.hpp"
#include "Systems/Storyboards/Data/sbrdListData.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

#include "Drivers/Enable/tmlnDriverEnable.hpp"
#include "Drivers/FileName/tmlnDriverAnimatedFileName.hpp"
#include "Drivers/Position/tmlnDriverPosition.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/tmln/tmlnChannelBoolean.hpp"
#include "Support/tmln/tmlnChannelFileName.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameMgr.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dBillboard.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"


#include <vector>


//============================================================================
//============================================================================
namespace
{
	int l_ObjectCounter = 0;

	//	The data is not part of the object list is kept here and added
	//	on get/set since this is not standard to the objectmgr template
	//
	sbrdListData l_StoryboardData;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void create_default_name( const itString& i_Filename, nameString& o_NameString )
	{
		char strName[64];
		char filename_base[64];
		char *filename;
		strcpy( filename_base, itStringUtil::GetStdString( i_Filename ).c_str() );
		filename = strtok( filename_base, "." );

		do
		{
			sprintf( strName, "%s-%03d", filename, l_ObjectCounter );
			o_NameString.SetString( strName );
			l_ObjectCounter++;
		} while ( !sbrdObjectMgr::VerifyNodupeName(strName) );

		//DBG_LOG( "Added -- name (" << strName << ")" );
	}

}	// end of namespace


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdScriptObject* sbrdObjectCreator::Create(const sbrdScriptData &i_Data)
{
	//DBG_LOG("Creator Create!");

	if (sbrdObjectMgr::GetNumObjects() > 0)
		return 0;

	//matTexture *pTexture = matTextureMgr::LoadTexture(tex_loc);
	matTexture* pTexture = NULL;

	// create actual point Billboard
	api3dBillboard *p3dBillboard = new api3dBillboard(pTexture);
	p3dBillboard->SetRenderable(false);
	api3dScene::AddObject( p3dBillboard );

	sbrdScriptObject *pObject = new sbrdScriptObject( p3dBillboard );

	pObject->SetScriptData( i_Data );
	p3dBillboard->SetOrientToCamera(false);

	if (i_Data.m_Drivers.size() > 0)
		pObject->GetPickObject()->SetLayerVisible(true);

	// Maintain the current icon visibility
	pObject->ShowIcons(sbrdObjectMgr::IconsVisible());

	if (i_Data.m_BaseData.m_Name.GetString().empty())
	{
		//	set a default name for the object
		nameString objName;
		//create_default_name( i_Data.m_BaseData.m_Filename.GetValue(), objName );
		create_default_name( itString("Storyboards"), objName );
		pObject->SetName( objName );
	}

	l_StoryboardData.m_Items.resize(1);
	l_StoryboardData.m_Items[0] = i_Data;

	return pObject;
}

//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  sbrdObjectMgr::Init()
{
	//sbrdDataMgr::AddDataChangedCallback(&l_DataChangedObj);
}

//--------------------------------------------------------------------
//  Clean up
//--------------------------------------------------------------------
void  sbrdObjectMgr::CleanUp()
{
	//DBG_LOG("sbrdOMgr - CleanUp!");

	//sbrdDataMgr::RemoveDataChangedCallback(&l_DataChangedObj);
	envSTLHelpers::DeleteContainer(sm_Objects);
}


//--------------------------------------------------------------------
//  clear
//--------------------------------------------------------------------
//static 
void sbrdObjectMgr::Clear()
{
	//DBG_LOG("sbrdOMgr - Clear!");

	sel3dMgr::ClearSelection();
	envSTLHelpers::DeleteContainer(sm_Objects);

	l_StoryboardData.m_Filenames.clear();
	l_StoryboardData.m_Items.clear();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
sbrdListData& sbrdObjectMgr::GetListData()
{
	//DBG_LOG("sbrdOMgr - Get ListData!");
	return l_StoryboardData;
}

//--------------------------------------------------------------------
//  Access to whole data as one structure for easy display and
// parsing
//--------------------------------------------------------------------
//static 
sbrdListData sbrdObjectMgr::GetData()
{
	sbrdListData data;
	data.m_Filenames = l_StoryboardData.m_Filenames;

	const int num_objects = sm_Objects.size();
	for (int i=0; i<num_objects; i++)
	{
		data.m_Items.push_back( sm_Objects[i]->GetScriptData() );
	}
	return data;
}

//--------------------------------------------------------------------
// Set data as a whole 
//--------------------------------------------------------------------
//static 
void sbrdObjectMgr::SetData(const sbrdListData &i_Data)
{
	//DBG_LOG("sbrdOMgr - Set Data!");

	//	remove the objects
	envSTLHelpers::DeleteContainer( sm_Objects );

	int itemsize = i_Data.m_Items.size();
	int filenames = i_Data.m_Filenames.size();
	if ((filenames > 0) && (itemsize > 0))
	{
		DBG_LOG("Storyboard object mgr has " << i_Data.m_Items[0].m_Drivers.size() << " drivers" );
		DBG_LOG("Storyboard object mgr has " << i_Data.m_Items[0].m_ChannelInfo.size() << " channels" );
	}
	else
	{
		// if there aren't any objects don't create one automatically.
		return;
	}

	//	Only need ONE storyboard object
	//
	sbrdScriptData data;
	if (itemsize > 0)
	{
		data = i_Data.m_Items[0];
	}
	data.m_BaseData.m_Name.SetValue("Storyboards");
	DBG_LOG("SetData name = " << data.m_BaseData.m_Name.GetUID());

	// BUG FIX:  This is only needed for a little while.
	//	If a scene was created BEFORE storyboards existed, there is a high likely hood
	//	of a nameString conflict since storyboards get created first, so it has a really
	//	low name ID.  If objects were already created in the scene it is likely there
	//	would be a conflict.
	//
	//	NOTE: changed this to just do this everytime.  if I didn't do it this way then
	//	the user would have to load, save, and reload the problem scenes.
	//
	//	The fix is to search for the name ID and if it is a conflict, set a
	//	new one.
	//
	//nameList name_list;
	//nameMgr::GetNameList(name_list);
	//for (int i = 0; i < name_list.size(); ++i)
	//{
	//	DBG_LOG4("%02d %s  vs %02d %s", name_list[i]->GetUID(), name_list[i]->GetString().c_str(), data.m_BaseData.m_Name.GetUID(), data.m_BaseData.m_Name.GetString().c_str() );
	//
	//	if (name_list[i]->GetUID() == data.m_BaseData.m_Name.GetUID())
	//	{
	//		if (name_list[i]->GetString() != data.m_BaseData.m_Name.GetString())
	//		{
				//	get a new (and valid) UID
				nameString new_name( data.m_BaseData.m_Name.GetValue() );
				new_name.SetUID(-1);
				nameMgr::RegisterName(new_name);
				data.m_BaseData.m_Name.SetValue(new_name);
	//			break;
	//		}
	//	}
	//}

	//
	create_and_add_object( data );

	l_StoryboardData.m_Filenames = i_Data.m_Filenames;

	//if (i_Data.m_Items.size() > 0)
	//	sbrdObjectCreator::Create( i_Data.m_Items[0] );
	//
	//const int num_objects = i_Data.m_Items.size();
	//for (int i=0; i<num_objects; i++)
	//{
	//	create_and_add_object( i_Data.m_Items[i] );
	//}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int add_visible_driver(sbrdScriptObject* i_pScriptObject)
{
	//	visible driver - if there isn't one add one.
	//
	if (i_pScriptObject->ChannelVisible().GetNumDrivers() == 0)
	{
		// get the drivers and find if the driver exists
		//
		tmlnDriverNameList driver_names;
		tmlnCreator::GatherPossibleDrivers(i_pScriptObject, driver_names);

		if (!driver_names.Empty())
		{
			//	find the index
			//
			int i;
			for (i=0; i< driver_names.m_DriverNames.size(); i++)
			{
				if (_stricmp(driver_names.m_DriverNames[i].m_Name.c_str(),"Visible") == 0)
				{
					break;
				}
			}

			if ( i >= driver_names.m_DriverNames.size() )
			{
				return -1;
			}

			tmlnDriverNameList::DriverName driver_info = driver_names.m_DriverNames[i];

			// Create Driver
			//
			tmlnDriver* pDriver = driver_info.m_Creator->CreateDriverByName( "Visible", i_pScriptObject );

			// Give driver to script object to own
			if (pDriver)
			{
				dynamic_cast<tmlnDriverEnable*>(pDriver)->SetEnabled(true);
				i_pScriptObject->AddDriver( pDriver );

				//	driver changed notification
				i_pScriptObject->NotifyDriverChanged();
			}
		}
	}
	
	return 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int add_texture_driver(sbrdScriptObject* i_pScriptObject, const itString& i_Filename)
{
	//	texture drivers
	//
	float lastdrivertime = 0.0f;
	int j;
	for (j=0; j < i_pScriptObject->ChannelTexture().GetNumDrivers(); ++j)
	{
		const tmlnDriver& driver = i_pScriptObject->ChannelTexture().GetDriver(j);
		float dtime = driver.GetBeginTime();

		//DBG_LOG4("%02d - texture driver (%s) endtime (%6.3f - %6.3f)", j, driver.GetName().c_str(), driver.GetBeginTime(), driver.GetEndTime());

		if (dtime >= lastdrivertime)
			lastdrivertime = dtime + 0.5f;	// add 1/2 second
	}

	// get the drivers and find if the driver exists
	//
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(i_pScriptObject, driver_names);

	//	animated texture drivers
	//
	if (!driver_names.Empty())
	{
		//	find the index
		//
		int i;
		for (i=0; i< driver_names.m_DriverNames.size(); i++)
		{
			if (_stricmp(driver_names.m_DriverNames[i].m_Name.c_str(),"Animated Texture") == 0)
			{
				break;
			}
		}

		if ( i >= driver_names.m_DriverNames.size() )
		{
			return -1;
		}

		tmlnDriverNameList::DriverName driver_info = driver_names.m_DriverNames[i];

		// Create Driver
		//
		tmlnDriverAnimatedFileName* pDriver = dynamic_cast<tmlnDriverAnimatedFileName*>(driver_info.m_Creator->CreateDriverByName( "Animated Texture", i_pScriptObject ));

		// Give driver to script object to own
		if (pDriver)
		{
			DBG_LOG1("Setting Driver Time (%6.3f)", lastdrivertime);
			pDriver->SetBeginTime(lastdrivertime);
			pDriver->SetEndTime(lastdrivertime);
			pDriver->SetTextures(i_Filename,1);
			i_pScriptObject->AddDriver( pDriver );

			//	driver changed notification
			i_pScriptObject->NotifyDriverChanged();
		}

		chnlDialogUtil::UpdateChannels();
	}

	return 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void delete_texture_driver(const itString& i_Filename, sbrdScriptObject* i_pScriptObject)
{
	// find the driver with the storyboard file and remove it.
	//	Delete by index or filename? what if a storyboard name
	//	is used more than once?  what if they add another f'name driver?
	//
	sbrdScriptObject* pSO = i_pScriptObject;
	tmlnChannelFileName& tx_chnl = pSO->ChannelTexture();

	int j; // = i_Index;
	for (j=0; j < tx_chnl.GetNumDrivers(); ++j)
	{
		const tmlnDriverAnimatedFileName& driver = dynamic_cast<const tmlnDriverAnimatedFileName&>(tx_chnl.GetDriver(j));
		if (driver.GetFirstFileName() == i_Filename)
		{
			tx_chnl.RemoveDriver( &(tx_chnl.Driver(j)) );
			chnlDialogUtil::UpdateChannels();
			break;
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void add_storyboard_camera()
{
	//	if there isn't a storyboard camera, then create one
	//
	const char* c_STORYBOARD_CAMERA_NAME = "Storyboard Camera";
	cmraScriptObject* pSO = cmraObjectMgr::GetObject( nameString(c_STORYBOARD_CAMERA_NAME) );
	if (pSO == 0)
	{
		cmraOperations::AddObject();
		pick3dPickObject* pPO = sel3dMgr::GetSelected();
		cmraCameraObject* pCO = dynamic_cast<cmraCameraObject*>(pPO);
		if (pCO != 0)
		{
			int cindex = cmraObjectMgr::GetIndexForObject(pCO->GetName());
			cmraCameraData& cdata = cmraObjectMgr::GetBaseData(cindex);
			cdata.m_Name.SetString(c_STORYBOARD_CAMERA_NAME);
			cdata.m_Description.SetValue("Camera for Storyboards");
			cmraOperations::ChangeBaseData(cdata);

			pSO = dynamic_cast<cmraScriptObject*>(pCO->GetParentObject());
			DBG_ASSERT0(pSO != 0, "Need a valid cmraScriptObject");

			// Create Static Position Driver
			//
			tmlnDriverNameList driver_names;
			tmlnCreator::GatherPossibleDrivers(pSO, driver_names);

			int i;
			for (i=0; i< driver_names.m_DriverNames.size(); i++)
			{
				std::string dname = driver_names.m_DriverNames[i].m_Name;
				if (_stricmp(dname.c_str(),"Static Position") == 0)
					break;
			}

			if ( i >= driver_names.m_DriverNames.size() )
				return;

			tmlnDriverNameList::DriverName driver_info = driver_names.m_DriverNames[i];
			tmlnDriver* pDriver = driver_info.m_Creator->CreateDriverByName( "Static Position", pSO );
			if (pDriver)
			{
				//	TODO - base position of size of billboard
				dynamic_cast<tmlnDriverPosition*>(pDriver)->SetValue(maPoint3d(0.755f,0.166f,6.823f));
				pSO->AddDriver( pDriver );

				pSO->NotifyDriverChanged();
			}

			// Create Static Target Driver
			//
			for (int i=0; i< driver_names.m_DriverNames.size(); i++)
			{
				std::string dname = driver_names.m_DriverNames[i].m_Name;
				if (_stricmp(dname.c_str(),"Static Target") == 0)
					break;
			}

			if ( i >= driver_names.m_DriverNames.size() )
				return;

			driver_info = driver_names.m_DriverNames[i];
			pDriver = driver_info.m_Creator->CreateDriverByName( "Static Target", pSO );
			if (pDriver)
			{
				//	TODO - base Target of size of billboard
				dynamic_cast<tmlnDriverPosition*>(pDriver)->SetValue(maPoint3d(1.043f,-0.03f,-5.433f));
				pSO->AddDriver( pDriver );

				pSO->NotifyDriverChanged();
			}
		}
	}
}

//--------------------------------------------------------------------
//  Add new object to world
//--------------------------------------------------------------------
int sbrdObjectMgr::AddObject(const sbrdScriptData& i_Data)
{
	try
	{
		create_and_add_object( i_Data );
	}
	catch( fsFileDoesntExistX& i_Ex )
	{
		char msg[256];
		std::string txt;
		txt = itStringUtil::GetStdString(i_Ex.GetLocator().GetLastName());
		sprintf( msg, "Cannot add the object -- missing %s", txt.c_str() );
		DBG_ERROR( msg );
		guiMessageBox::Show(msg,"Error Loading Object", guiMessageBox::e_OKOnly);

		return -1;
		//assert(false);
	}

	int index = sm_Objects.size() - 1;
	SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete object with given index
//--------------------------------------------------------------------
void sbrdObjectMgr::DeleteObject(int i_Index)
{
	l_StoryboardData.Clear();

	// Note: could use a function here in xxxObjectCreator in order to 
	// do anything before deletion.
	sel3dMgr::ClearSelection();

	sm_Objects.clear();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int sbrdObjectMgr::AddStoryboard(const itString& i_Filename, bool i_bAddTo3DWorld)
{
	//	add the filename
	l_StoryboardData.Add(i_Filename);

	if (i_bAddTo3DWorld)
	{
		//	add the appropriate drivers for the object.
		//
		const int num_objects = sm_Objects.size();
		DBG_ASSERT0( num_objects == 1, "Only one storyboard object is allowed" );
		sbrdScriptObject* pSO = dynamic_cast<sbrdScriptObject*>(sm_Objects[0]);

		pSO->GetBillboardObject()->SetRenderable(true);

		add_storyboard_camera();

		int index;
		index = add_visible_driver( pSO );
		if (index < 0) return index;
		index = add_texture_driver( pSO, i_Filename );
		if (index < 0) return index;
	}

	//	return the index
	return (l_StoryboardData.m_Filenames.size() - 1);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
void sbrdObjectMgr::DeleteStoryboard(int i_Index)
{
	itString filename = l_StoryboardData.m_Filenames[i_Index].GetValue();

	//fsResourceTracker::Remove(l_StoryboardData.m_Filenames[i_Index].GetValue());

	//	remove the appropriate driver
	sbrdScriptObject* pSO = dynamic_cast<sbrdScriptObject*>(sm_Objects[0]);
	delete_texture_driver(filename, pSO);

	// remove
	l_StoryboardData.Remove(i_Index);

	//	driver changed notification
	pSO->NotifyDriverChanged();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
void sbrdObjectMgr::SwapStoryboards(int i_Index1, int i_Index2)
{
	//bga - needs to be re-implemented with new texture animation driver.
	
	//itString filename1 = l_StoryboardData.m_Filenames[i_Index1].GetValue();
	//itString filename2 = l_StoryboardData.m_Filenames[i_Index2].GetValue();

	////	swap filename order
	//prtyFileName fname = l_StoryboardData.m_Filenames[i_Index1];
	//l_StoryboardData.m_Filenames[i_Index1] = l_StoryboardData.m_Filenames[i_Index2];
	//l_StoryboardData.m_Filenames[i_Index2] = fname;

	////	swap driver times
	//sbrdScriptObject* pSO = dynamic_cast<sbrdScriptObject*>(sm_Objects[0]);
	//tmlnChannelFileName& tx_chnl = pSO->ChannelTexture();

	//int actual1 = 0;
	//int actual2 = 0;
	//int j;
	//for (j=0; j < tx_chnl.GetNumDrivers(); ++j)
	//{
	//	const tmlnDriverAnimatedFileName& driver = dynamic_cast<const tmlnDriverAnimatedFileName&>(tx_chnl.GetDriver(j));
	//	if (driver.GetFirstFileName() == filename1)
	//		actual1 = j;
	//	if (driver.GetFirstFileName() == filename2)
	//		actual2 = j;
	//}

	//tx_chnl.SwapDriverTimes(actual1, actual2);
	//chnlDialogUtil::UpdateChannels();

	////	driver changed notification
	//pSO->NotifyDriverChanged();
}


