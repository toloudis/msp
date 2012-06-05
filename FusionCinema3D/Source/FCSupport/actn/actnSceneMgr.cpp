/*****************************************************************************
**	actnSceneMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/actn/actnSceneMgr.hpp"

#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcui/qtGUI/fcuiTimelineItem.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Systems/Character/GUI/chtrDialogInterest.hpp"
#include "Systems/Character/Object/chtrObject.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	///---------------------------------------------------------------------------
	/// Given the directory of the animation, return the name of the camera
	/// in the scene that belongs to this animation
	///---------------------------------------------------------------------------
	std::string GetAnimationCameraName(const fsLocator& i_AnimationDirectory, const itString& i_CamName)
	{
		std::string camString("");
		if (i_AnimationDirectory.GetNumNames() == 0)
		{
			DBG_ERROR("Empty animation directory");
			return camString;
		}

		std::string animation_name;

		//for each object in the scene apply the anim driver of the same name.
		animation_name = itStringUtil::GetStdString(i_AnimationDirectory.GetLastName());
		
		// need to construct the string "Cam_Animation_Cam1 (or Cam2 or Cam3) ( Eg. Cam_Longdress_Clap_Cam1)"
		camString += fcuiConstants::c_CAMERA_BASENAME;
		camString += animation_name;
		camString += "_";
		camString += itStringUtil::GetStdString(i_CamName);

		return camString;
	}

	///-----------------------------------------------------------------------
	/// Switch the viewport to the camera with the given name
	///-----------------------------------------------------------------------
	void SwitchToCamera(const std::string& i_CameraString)
	{
		if ( i_CameraString.length() == 0 )
			return;

		//now select the appropriate camera
		nameString camName(i_CameraString);
		int index = camsCameraMgr::GetIndexForName(camName);
		if (index >= 0 && index < camsCameraMgr::GetNumCameras())
		{
			rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
			camsCameraMgr::SelectCamera(index);
		}
		else
		{
			DBG_WARNING("Camera: " << i_CameraString << " was not found.");
		}
	}

	///-----------------------------------------------------------------------
	/// Assign a new camera to the sequence item
	///-----------------------------------------------------------------------
	void SetSceneItemCamera(fcuiTimelineItemData& io_ItemData, const fsLocator& i_CamDirectory)
	{
		fcuiTimelineItemData originalData = io_ItemData;
		itString camName = i_CamDirectory.GetLastName();
		fsLocator animDirectory = io_ItemData.m_ItemLocator;
		std::string old_cam = GetAnimationCameraName(animDirectory, io_ItemData.m_CategoryData);
		std::string new_cam = GetAnimationCameraName(animDirectory, camName);
		io_ItemData.m_CategoryData = camName;

		//using old_cam and new_cam, swap the capture drivers for those cameras
		fcuiUtils::RemoveDriverAtTime(nameString(old_cam), "Capture", io_ItemData.m_StartTime);
		fcuiUtils::CreateDriver("Capture", nameString(new_cam), io_ItemData.m_StartTime, io_ItemData.m_EndTime);

		fcuiFormMgr::UpdateTimelineItemData(originalData, io_ItemData);
		fcuiFormMgr::HideCameraPicker();
		tmlnTimeLine::SetValue(io_ItemData.m_StartTime);
		SwitchToCamera(fcuiConstants::c_CAMERA_DIRECTOR);
	}
}

///-----------------------------------------------------------------------
/// constructors
///-----------------------------------------------------------------------
actnSceneMgr::actnSceneMgr()
{
}

///-----------------------------------------------------------------------
/// destructors
///-----------------------------------------------------------------------
actnSceneMgr::~actnSceneMgr()
{
}

///---------------------------------------------------------------------------
///	the current instance of the mode mgr singleton
///---------------------------------------------------------------------------
actnSceneMgr* actnSceneMgr::Instance = NULL;

//--------------------------------------------------------------------
//	Deinitialize/Initialize
//--------------------------------------------------------------------
void actnSceneMgr::Initialize()
{
}
void actnSceneMgr::DeInitialize()
{
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a scene element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void actnSceneMgr::ProcessScene(const fsLocator& i_SceneItem)
{
	DBG_ASSERT( fcuiTimelineMgr::Instance != NULL, "Timeline Manager has not been initialized." );

	//get the current end time of the timeline, this will be the start time for
	//this action.
	maTime startTime = fcuiTimelineMgr::Instance->GetEndTime();
	maTime endTime = maTime::FromSeconds(-1.0f);	
	fsLocator subject_dir;

	//assign animation drivers to each component of the subject
	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(i_SceneItem, files);
	itString anim_name, anim_name_noext;
	maTime diff_EndTime;
	//for each object in the scene apply the anim driver of the same name.
	itString extension, icon_check;
	bool bFoundTime = false;
	for (int i = 0; i < files.size(); ++i)
	{
		files[i].GetLastName().GetExtension(extension);
		if(extension == itString(fcuiConstants::l_EXT_GAB))
		{
			anim_name = files[i].GetLastName();
			anim_name_noext = anim_name;
			anim_name_noext.StripExtension();
			
			diff_EndTime = fcuiUtils::SetObjectAnims(anim_name_noext, files[i], startTime);
		
			//make sure capture drivers are set to the end of the longest animation
			if (diff_EndTime > endTime)
			{
				endTime = diff_EndTime;
				bFoundTime = true;
			}
			
		}
	}
	if (!bFoundTime)
	{
		//no animations found, don't increment time or create capture drivers
		return;
	}
	
	//create capture driver for the first camera in the folder for the duration
	//of the anim.
	std::vector<fsLocator> cam_directories;
	parser.GetSubDirectories(i_SceneItem, cam_directories);
	itString camName;
	if ( cam_directories.size() >= 1 )
	{
		camName = cam_directories[0].GetLastName();
	}

	std::string camera = GetAnimationCameraName(i_SceneItem, camName);
	fcuiUtils::CreateDriver("Capture", nameString(camera), startTime, endTime);
	
	fcuiUtils::CopyCamProperties(fcuiConstants::c_CAMERA_DIRECTOR, camera);
	SwitchToCamera(fcuiConstants::c_CAMERA_DIRECTOR);

	// check if the time has gone over the scene's time limit
	PrefsData& data = PrefsMgr::Data();
	maTime curMaxTime = maTime::FromSeconds(data.m_ChannelEditor_DefaultTime.GetValue());
	if ( endTime > curMaxTime )
	{
		curMaxTime = endTime + maTime::FromSeconds(5);
		data.m_ChannelEditor_DefaultTime.SetValue((int) curMaxTime.AsSeconds());
	}

	tmlnTimeLine::SetTimeRange( maTime::c_ZeroTime, curMaxTime );
	//update the end time of the timeline.
	tmlnTimeLine::SetValue(endTime);
	fcuiTimelineMgr::Instance->SetEndTime(tmlnTimeLine::GetValue());
	
	tmlnTimeLine::SetValue(startTime);
	fcuiTimelineMgr::Instance->IncrementTime(2.0f);
	fcuiTimelineMgr::Instance->DecrementTime(2.0f);
}

///-----------------------------------------------------------------------
/// Perfrom the appropriate operations for a timeline object belonging
/// to the scene category
///-----------------------------------------------------------------------
void actnSceneMgr::ProcessSceneTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.TitleCard_LabelWidget->setVisible(false);
	tmlnTimeLine::SetValue(i_ItemData.m_StartTime);
	//std::string camString = GetAnimationCameraName(i_ItemData.m_ItemLocator, i_ItemData.m_CategoryData);
	std::string camString = fcuiConstants::c_CAMERA_DIRECTOR;
	ShiftCameraKeys(i_ItemData);
	SwitchToCamera(camString);
}

///---------------------------------------------------------------------------
/// shift the timeline item belonging to the scene category
///---------------------------------------------------------------------------
void actnSceneMgr::ShiftSceneTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration)
{
	fsLocator subject_dir;
	//grab the main subject in the scene
	/*if ( fcmdModeMgr::Instance )
		subject_dir = fcmdModeMgr::Instance->GetMainSubjectDirectory();
	else
		return;*/

	maTime old_start = io_ItemData.m_StartTime;
	io_ItemData.m_StartTime += i_Duration;
	io_ItemData.m_EndTime += i_Duration;

	//assign animation drivers to each component of the subject
	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(io_ItemData.m_ItemLocator, files);
	itString extension, icon_check, animation_name;
	std::string subject_name, category_name, animation_name_str;
	std::string subcategory_name;
	std::string object_name;
	//for each object in the scene, shift the anim driver of the same name.
	for (int i = 0; i < files.size(); ++i)
	{
		files[i].GetLastName().GetExtension(extension);
		object_name = "";
		if(extension == itString(fcuiConstants::l_EXT_GAB))
		{
			std::string str_animname;
			itString anim_name = files[i].GetLastName();
			anim_name.StripExtension();
			fsFileUtil::LocatorToANSIFilename(anim_name, str_animname);
			
			
			subject_name = strtok((char*)str_animname.c_str(),"_");
			category_name = strtok(NULL, "_");
			subcategory_name = strtok(NULL, "_"); 
			animation_name_str = itStringUtil::GetStdString(anim_name);
			object_name = subject_name + "_" + category_name + "_" + subcategory_name + ".gxb";
		}
		else
		{
			continue;
		}
		
		cmmDialogDataList data_list;
		cmmDialogInterestMgr::GetPlacedObjects(data_list);
		cmmDialogDataList::iterator it;
		for (it = data_list.begin(); it != data_list.end(); ++it)
		{
			cmmDialogData &data = (*it);
			if (data.m_Filename == itString(object_name.c_str())) 
			{
				fcuiUtils::ShiftDriverToTime(data.m_Name, animation_name_str, old_start, io_ItemData.m_StartTime);
			}
		}

		/*
		animation_name_str = itStringUtil::GetStdString(animation_name);
		std::string object_name = subject_name ;
		fcuiUtils::ShiftDriverToTime(object_name, animation_name_str, old_start, io_ItemData.m_StartTime);
		*/
	}

	std::string camera = GetAnimationCameraName(io_ItemData.m_ItemLocator, io_ItemData.m_CategoryData);
	//shift the capture driver for the camera to the new time
	fcuiUtils::ShiftDriverToTime(nameString(camera), "Capture", old_start, io_ItemData.m_StartTime);
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
void actnSceneMgr::ShiftCameraKeys(const fcuiTimelineItemData& i_ItemData)
{
	std::vector<tmlnDriver*> drivers;
	fsLocator animDirectory = i_ItemData.m_ItemLocator;
	std::string camera = GetAnimationCameraName(animDirectory, i_ItemData.m_CategoryData);
	
	maTime prevStart;
	if( m_CamStartMap.find(camera) != m_CamStartMap.end() )
		prevStart = m_CamStartMap[camera];
	
	fcuiUtils::GetDrivers(nameString(camera), drivers);
	fcuiUtils::ShiftDriversToTime(nameString(camera), i_ItemData.m_StartTime - prevStart, drivers);
	m_CamStartMap[camera] = i_ItemData.m_StartTime;
}

///---------------------------------------------------------------------------
/// shift the timeline item belonging to the scene category
///---------------------------------------------------------------------------
void actnSceneMgr::RemoveSceneTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	fsLocator subject_dir;
	//grab the main subject in the scene
	
	maTime start_time = i_ItemData.m_StartTime;

	//assign animation drivers to each component of the subject
	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(i_ItemData.m_ItemLocator, files);
	itString extension, icon_check, animation_name;
	std::string subject_name, category_name, animation_name_str;
	std::string subcategory_name;
	std::string object_name;
	//for each object in the scene, shift the anim driver of the same name.
	for (int i = 0; i < files.size(); ++i)
	{
		files[i].GetLastName().GetExtension(extension);
		object_name = "";
		if(extension == itString(fcuiConstants::l_EXT_GAB))
		{
			std::string str_animname;
			itString anim_name = files[i].GetLastName();
			anim_name.StripExtension();
			fsFileUtil::LocatorToANSIFilename(anim_name, str_animname);
			
			
			subject_name = strtok((char*)str_animname.c_str(),"_");
			category_name = strtok(NULL, "_");
			subcategory_name = strtok(NULL, "_"); 
			animation_name_str = itStringUtil::GetStdString(anim_name);
			object_name = subject_name + "_" + category_name + "_" + subcategory_name + ".gxb";
		}
		else
		{
			continue;
		}
		
		
		cmmDialogDataList data_list;
		cmmDialogInterestMgr::GetPlacedObjects(data_list);
		cmmDialogDataList::iterator it;
		for (it = data_list.begin(); it != data_list.end(); ++it)
		{
			cmmDialogData &data = (*it);
			if (data.m_Filename == itString(object_name.c_str())) 
			{
				fcuiUtils::RemoveDriverAtTime(data.m_Name, animation_name_str, start_time);
			}
		}
		
	}
	
	std::string camera = GetAnimationCameraName(i_ItemData.m_ItemLocator, i_ItemData.m_CategoryData);
	fcuiUtils::RemoveDriverAtTime(camera, "Capture", start_time);
}

///---------------------------------------------------------------------------
/// Edit the timeline item belonging to the scene category
///---------------------------------------------------------------------------
void actnSceneMgr::EditSceneTimelineItem(fcuiTimelineItemData& io_ItemData)
{
	std::vector<fsLocator> cam_directories;
	pathDirectoryParser parser;
	parser.GetSubDirectories(io_ItemData.m_ItemLocator, cam_directories);

	//show the camera dialog
	//populate the camera dialog with images from the camera folders
	fcuiFormMgr::ShowCameraPicker(io_ItemData, cam_directories, &SetSceneItemCamera);
}
