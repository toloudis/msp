/*****************************************************************************
**	fcmdModeMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcmd/fcmdModeMgr.hpp"

#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/qtGUI/fcuiTimelineItem.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Support/cams/camsCameraMgr.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	fsLocator l_MoviePackDirectory;
	fsLocator l_CastDirectory;
	fsLocator l_LocationDirectory;
	fsLocator l_ActionDirectory;
	fsLocator l_MakeDirectory;
	fsLocator l_TheaterDirectory;
	itString l_ActionCategoryName;
	itString l_CustomIcons_Images("Images");
	itString l_CustomIcons_Videos("Videos");
	itString l_CustomIcons_Audio("Audio");
	itString l_CustomIcons_PostFX("Post FX");
	itString l_CustomIcons_Props("Props");
}


///---------------------------------------------------------------------------
/// constructors
///---------------------------------------------------------------------------
fcmdModeMgr::fcmdModeMgr()
:	m_pCurrentMode(NULL),
	m_pNextMode(NULL),
	m_bCanDrag(false),
	m_bCanCheck(false),
	m_bTimelineLocked(false), 
	m_CastAnimStart(maTime::FromSeconds(9000)) //start cast anims at 2.5 hours
{
	l_MoviePackDirectory.Push(gfPaths::GetPath(mnmPaths::e_MoviePacks));
	l_ActionCategoryName = itString( fcuiConstants::c_ACTION_SCENE_CATEGORY );
}

///---------------------------------------------------------------------------
/// destructors
///---------------------------------------------------------------------------
fcmdModeMgr::~fcmdModeMgr()
{
	if (m_pCurrentMode != NULL)
		delete m_pCurrentMode;
	m_pCurrentMode = NULL;
	m_pNextMode = NULL;
}

///---------------------------------------------------------------------------
/// Initialize the event animations when in cast mode
///---------------------------------------------------------------------------
void fcmdModeMgr::InitCastAnims()
{
	m_CastAnimList.clear();
	m_CastAnimList.resize((int)e_NumStates);

	std::vector<fsLocator> subjects;
	pathDirectoryParser parser;
	parser.GetSubDirectories(l_CastDirectory, subjects);
	m_CastAnimEnd = m_CastAnimStart;

	//for each subject in the scene, we need to go through their folders and grab the anims
	for( int i = 0; i < subjects.size(); ++i )
	{
		m_CastAnimEnd = SetSubjectAnims(subjects[i]);
	}
	
	maTime timeline_end = tmlnTimeLine::GetMaximum();
	PrefsData& data = PrefsMgr::Data();
	if ( timeline_end < m_CastAnimEnd )
	{
		timeline_end = m_CastAnimEnd;
		data.m_ChannelEditor_DefaultTime.SetValue((int)timeline_end.AsSeconds());
	}
	tmlnTimeLine::SetTimeRange( maTime::c_ZeroTime, timeline_end );
	//put the time back at the beginning
	tmlnTimeLine::SetValue(maTime::c_ZeroTime);

	//extend the director driver to match our last anim time
	fcuiUtils::ExtendDirectorDriver( timeline_end ); //TIME - was data.m_ChannelEditor_DefaultTime, was that correct?
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
maTime fcmdModeMgr::SetSubjectAnims(const fsLocator& i_SubjectDirectory)
{
	std::vector<fsLocator> parts;
	std::vector<fsLocator> animFiles;
	pathDirectoryParser parser;
	parser.GetSubDirectories(i_SubjectDirectory, parts);
	itString extension;
	itString anim_name, anim_name_noext;
	maTime anim_end, diff_end;
	anim_end = maTime::FromSeconds(-1);
	maTime start_time = m_CastAnimStart;

	// go through each part of the subject (wardrobe, hair, etc.) and apply any 
	// animations within their directories
	for( int i = 0; i < parts.size(); ++i )
	{
		parts[i].Push("99.Animations");
		if( !fsFileUtil::DirectoryExists(parts[i]) )
			continue;
		pathDirectoryParser animParser;
		animFiles.clear();
		animParser.GetDirectoryFiles(parts[i], animFiles);
		for( int j = 0; j < animFiles.size(); ++j )
		{
			animFiles[j].GetLastName().GetExtension(extension);
			if(extension == itString(fcuiConstants::l_EXT_GAB))
			{
				anim_name = animFiles[j].GetLastName();
				anim_name_noext = anim_name;
				anim_name_noext.StripExtension();
				
				diff_end = fcuiUtils::SetObjectAnims(anim_name_noext, animFiles[j], start_time);
				if( diff_end > anim_end )
					anim_end = diff_end;
			}
		}
	}
	return anim_end;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
const maTime& fcmdModeMgr::GetCastAnimStart()
{
	return m_CastAnimStart;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
const maTime& fcmdModeMgr::GetCastAnimEnd()
{
	return m_CastAnimEnd;
}

//static
///---------------------------------------------------------------------------
/// Return the directory of a mode
///---------------------------------------------------------------------------
const fsLocator fcmdModeMgr::GetModeLocator(const Mode& i_ModeID)
{
	fsLocator mode_directory;
	switch( i_ModeID )
	{
	case e_Cast:
		mode_directory = l_CastDirectory;
		break;
	case e_Location:
		mode_directory = l_LocationDirectory;
		break;
	case e_Action:
		mode_directory = l_ActionDirectory;
		break;
	case e_Make:
		mode_directory = l_MakeDirectory;
		break;
	case e_Theater:
		mode_directory = l_TheaterDirectory;
		break;
	case e_NewMovie:
		mode_directory = l_MoviePackDirectory;
		break;
	default:
		break;
	}

	return mode_directory;
}

///---------------------------------------------------------------------------
/// Link the mode to the given QT button
///---------------------------------------------------------------------------
fcmdModeTemplate* fcmdModeMgr::CreateNewMode(const Mode& i_ModeID)
{
	fcmdModeTemplate* new_mode = NULL;
	switch( i_ModeID )
	{
	case e_Cast:
		new_mode = new fcmdModeCast(l_CastDirectory);
		break;
	case e_Location:
		new_mode = new fcmdModeLocation(l_LocationDirectory);
		break;
	case e_Action:
		{
			QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
			i_ui->ui.widget_2->setVisible(true);
			i_ui->ui.Edit_ActionScrollLeftButton->show();
			i_ui->ui.Edit_ActionScrollRightButton->show();
			new_mode = new fcmdModeAction(l_ActionDirectory);
		}
		break;

	case e_Make:
		new_mode = new fcmdModeMake(l_MakeDirectory);
		break;
	case e_Theater:
		new_mode = new fcmdModeTheater(l_TheaterDirectory);
		break;
	case e_NewMovie:
		new_mode = new fcmdModeNewMovie(l_MoviePackDirectory);
		break;
	default:
		DBG_ERROR("Mode ID: " << (int)i_ModeID << " not recognized");
		break;
	}

	//return the new mode pointer
	return new_mode;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
void fcmdModeMgr::SetCurrentMode()
{
}

///---------------------------------------------------------------------------
/// Add an event to the current mode to process
///---------------------------------------------------------------------------
void fcmdModeMgr::AddEventToCurrentMode()
{
}

///---------------------------------------------------------------------------
/// Process an event based on the initiators mode level and their data
///---------------------------------------------------------------------------
void fcmdModeMgr::ProcessModeEvent(const ModeLevel& i_ModeLevel)
{}
void fcmdModeMgr::ProcessModeEvent(const ModeLevel& i_ModeLevel, const fsLocator& i_Locator, const int& i_ItemID)
{
	//depending on the mode level of the event, we may have to perform different tasks
	// i.e. If the mode level is "e_Category" we will have to populate the files of within i_Locator
	// as images in the element panel
	switch(i_ModeLevel)
	{
	case e_Mode:
		ActivateMode((Mode)i_ItemID);
		if( m_pCurrentMode != NULL )
		{
			ProcessModeOperation(i_Locator);
			SwitchModeCamera(i_Locator);
		}
		break;
	case e_Subject:
		if( m_pCurrentMode != NULL )
		{
			ProcessSubjectOperation(i_Locator);
			SwitchModeCamera(i_Locator);
		}
		break;
	case e_Category:
		if( m_pCurrentMode != NULL )
		{
			ProcessCategoryOperation(i_Locator);
			SwitchModeCamera(i_Locator);
		}
		break;
	case e_Element:
		if( m_pCurrentMode != NULL ) 
			if( m_pCurrentMode->GetAllowElementClick() )
			{
				ProcessElementOperation(i_Locator);
				fsLocator category_dir = i_Locator;
				category_dir.Pop(); // we want to snap to the category camera
				SwitchModeCamera(category_dir);
			}
			else
				SelectElement(i_Locator);  //we still want to update the UI with the click
		break;
	case e_CustomIcons:
		if( m_pCurrentMode != NULL ) 
			if( m_pCurrentMode->GetAllowElementClick() )
			{
				ProcessCustomIconsOperation(i_Locator);
			}
			else
				SelectElement(i_Locator);  //we still want to update the UI with the click
		break;
	case e_HomeScreen:
		if( m_pCurrentMode != NULL )
			LoadNewMovie(i_Locator);
		break;
	default:
		break;
	}
}

///---------------------------------------------------------------------------
/// Do any mode operations and process events for the current mode
///---------------------------------------------------------------------------
void fcmdModeMgr::Think()
{
	if (m_pCurrentMode != NULL)
	{
		m_pCurrentMode->Think();
	}
}

///---------------------------------------------------------------------------
/// the current instance of the mode mgr singleton
///---------------------------------------------------------------------------
fcmdModeMgr* fcmdModeMgr::Instance = NULL;

///---------------------------------------------------------------------------
/// Activate the given mode
///---------------------------------------------------------------------------
void fcmdModeMgr::ActivateMode(const Mode& i_ModeID)
{
	if(m_pCurrentMode != NULL)
	{
		m_pCurrentMode->DeActivate();
		QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
		i_ui->ui.TitleCard_LabelWidget->setVisible(false);
		i_ui->ui.widget_2->setVisible(false);
		delete m_pCurrentMode;
		m_pCurrentMode = NULL;
		m_pNextMode = NULL;
	}
	
	//create the next mode
	m_pNextMode = CreateNewMode(i_ModeID);

	if(m_pNextMode != NULL)
		m_pNextMode->Activate();
	
	m_pCurrentMode = m_pNextMode;
	m_pCurrentMode->SetModeID((int)i_ModeID);
}

//----------------------------------------------------------------------------
/// Process the panel when a new movie request is made
//----------------------------------------------------------------------------
void fcmdModeMgr::ProcessNewMovieRequest()
{
	std::vector<fsLocator> directories;
	pathDirectoryParser parser;
	if(l_MoviePackDirectory.GetNumNames() > 0)
		parser.GetSubDirectories(l_MoviePackDirectory, directories);

	//update the panel with the set of movie packs
	fcuiFormMgr::UpdatePanelIcons(e_HomeScreen, directories);
}

//----------------------------------------------------------------------------
/// Load the new selected movie
//----------------------------------------------------------------------------
void fcmdModeMgr::LoadNewMovie(const fsLocator& i_MovieDirectory)
{
	if( i_MovieDirectory.GetNumNames() > 0 )
	{
		try
		{
			fcuiFormMgr::LoadMovie(i_MovieDirectory.GetLastName());
		}
		catch (const fsFileDoesntExistX& i_Ex)
		{
			DBG_ERROR("Cannot find file " <<  i_Ex.GetLocator() ); //<< ". " << i_Ex.GetErrorMessage() );
		}
	}
}

///---------------------------------------------------------------------------
/// Switch to the proper mode camera when an item is clicked
///---------------------------------------------------------------------------
void fcmdModeMgr::SwitchModeCamera(const fsLocator& i_Directory)
{
	std::string camname = fcuiConstants::c_CAMERA_BASENAME;
	itString Modename = i_Directory.GetLastName();
	// Restrict the camera change if the Mode is action 
	fsLocator Directory = i_Directory;
	Directory.Pop();
	itString tempModeName = Directory.GetLastName();
	if(itStringUtil::GetStdString(Modename) == fcuiConstants::c_MODE_ACTION)
	{
		if((fcuiTimelineMgr::Instance != NULL) && (fcuiTimelineMgr::Instance->GetEndTime() > maTime::c_ZeroTime))
			camname = fcuiConstants::c_CAMERA_DIRECTOR;
		else
			camname += itStringUtil::GetStdString(Modename);
	}
	else if((itStringUtil::GetStdString(tempModeName) == fcuiConstants::c_MODE_ACTION )||
		    (itStringUtil::GetStdString(Modename) == fcuiConstants::c_ACTION_SCENE_CATEGORY ) ||
			(itStringUtil::GetStdString(Modename) == fcuiConstants::c_ACTION_LIGHT_CATEGORY ) ||
			(itStringUtil::GetStdString(Modename) == fcuiConstants::c_ACTION_TITLECARD_CATEGORY ) ||
			(itStringUtil::GetStdString(Modename) == fcuiConstants::c_ACTION_AUDIO_CATEGORY ) || 
			(itStringUtil::GetStdString(Modename) == fcuiConstants::c_ACTION_POSTFX_CATEGORY ) 
		   )
	{
		if((fcuiTimelineMgr::Instance != NULL) && (fcuiTimelineMgr::Instance->IsPlaying()))
			camname = fcuiConstants::c_CAMERA_DIRECTOR;
		else 
			camname += itStringUtil::GetStdString(Modename);
	}
	else
		camname += itStringUtil::GetStdString(Modename);
	nameString camName(camname);
	int index = camsCameraMgr::GetIndexForName(camName);
	if(index >= 0 && index < camsCameraMgr::GetNumCameras())
	{
		rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
		camsCameraMgr::SelectCamera(index);
	}
	else
	{
		DBG_WARNING("Camera: " << camName << " was not found.");
	}
}

///---------------------------------------------------------------------------
/// When a mode operation is initiated, we should handle the appropriate
/// events here.
///---------------------------------------------------------------------------
void fcmdModeMgr::ProcessModeOperation(const fsLocator& i_ModeDirectory)
{
	std::vector<fsLocator> directories;
	pathDirectoryParser parser;
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	if(i_ModeDirectory.GetNumNames() > 0)
		parser.GetSubDirectories(i_ModeDirectory, directories);

	//update the panel for the next level - subject
	fcuiFormMgr::UpdatePanelIcons(e_Subject, directories);
	fcuiFormMgr::CreateLabels(i_ui->ui.Subject_LabelWidget, i_ModeDirectory,e_Subject);

	/*if( m_pCurrentMode->GetModeID() == (int)e_Action )
		fcuiFormMgr::ReOrderActionPanel(directories);*/

	if( m_pCurrentMode->GetAutoPopulatePanels() )
	{
		fsLocator child_dir; 
		// check if there is at least 1 sub-directory
		if( directories.size() >= 1 )
		{
			child_dir = directories[0];
			ProcessSubjectOperation(child_dir);
			fcuiFormMgr::HighlightPanelItemAtIndex(e_Subject, 0, true);
		}
		else
		{
			//update the other panels with an empty directory list
			fcuiFormMgr::UpdatePanelIcons(e_Category, directories);
			fcuiFormMgr::UpdatePanelIcons(e_Element, directories);
		}
	}
	else
	{
		directories.clear();
		fsLocator empty_dir;
		//update the other panels with an empty directory list
		fcuiFormMgr::UpdatePanelIcons(e_Category, directories);
		//fcuiFormMgr::CreateLabels(i_ui->ui.Edit_CategoryTextWidget, empty_dir, e_Category);
		fcuiFormMgr::UpdatePanelIcons(e_Element, directories);
		itString name("");
		fcuiFormMgr::UpdateElementName(name);
	}
}

///---------------------------------------------------------------------------
/// When a subject operation is initiated, we should handle the appropriate
/// events here.
///---------------------------------------------------------------------------
void fcmdModeMgr::ProcessSubjectOperation(const fsLocator& i_SubjectDirectory)
{
	//QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();

	m_pCurrentMode->ProcessSubject(i_SubjectDirectory);
	std::vector<fsLocator> directories;
	pathDirectoryParser parser;
	if(i_SubjectDirectory.GetNumNames() > 0)
		parser.GetSubDirectories(i_SubjectDirectory, directories);

	//update the panel for the next level - category
	fcuiFormMgr::UpdatePanelIcons(e_Category, directories);

	//fcuiFormMgr::CreateLabels(i_ui->ui.Edit_CategoryTextWidget, i_SubjectDirectory,e_Category);

	
	// Based on the subject name, fill in the custom icon panel
	fsLocator CustomIconsDirectory = gfPaths::GetPath(mnmPaths::e_Data);
	fsLocator subjectDirectory = i_SubjectDirectory;
	itString sub_name = subjectDirectory.GetLastName();
	fcuiUtils::StripNumber(sub_name);
	CustomIconsDirectory.Push(sub_name);
	if((sub_name == l_CustomIcons_Images) || (sub_name == l_CustomIcons_Videos) || 
	   (sub_name == l_CustomIcons_Audio)  || (sub_name == l_CustomIcons_PostFX) || 
       (sub_name == l_CustomIcons_Props))
	{
		std::vector<fsLocator> directory;
		pathDirectoryParser parser;
		if(i_SubjectDirectory.GetNumNames() > 0)
			parser.GetSubDirectories(CustomIconsDirectory, directory);
		fcuiFormMgr::UpdatePanelIcons(e_CustomIcons, directory);
	//	ProcessCustomIconsOperation(directory);
	}
	else
	{
		std::vector<fsLocator> directory;
		directory.clear();
		//update the customicon panel with an empty directory list
		fcuiFormMgr::UpdatePanelIcons(e_CustomIcons, directory);
	}

	if( m_pCurrentMode->GetAutoPopulatePanels() )
	{
		fsLocator child_dir; 
		// check if there is at least 1 sub-directory
		if( directories.size() >= 1 )
		{
			child_dir = directories[0];
			ProcessCategoryOperation(child_dir);
			fcuiFormMgr::HighlightPanelItemAtIndex(e_Category, 0, true);
		}
		else
		{
			//update the other panels with an empty directory list
			fcuiFormMgr::UpdatePanelIcons(e_Element, directories);
		}
	}
	else
	{
		directories.clear();
		//update the other panels with an empty directory list
		fcuiFormMgr::UpdatePanelIcons(e_Element, directories);
		itString name("");
		fcuiFormMgr::UpdateElementName(name);
	}
}

///---------------------------------------------------------------------------
/// When a category operation is initiated, we should handle the appropriate
/// events here.
///---------------------------------------------------------------------------
void fcmdModeMgr::ProcessCategoryOperation(const fsLocator& i_CategoryDirectory)
{
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.TitleCard_LabelWidget->setVisible(false);
	fsLocator category_dir = i_CategoryDirectory;
	itString category_name;
	itString subject_name;
	itString mode_name;
	if(category_dir.GetNumNames() > 0)
		category_name = i_CategoryDirectory.GetLastName();
	
	//if this is the action mode, we need to push the current character onto the
	////category directory
	//if( m_pCurrentMode->GetModeID() == (int)e_Action &&
	//	l_ActionCategoryName == category_name)
	//{
	//	if( m_MainSubject.GetLength() != 0 )	
	//		category_dir.Push(m_MainSubject);
	//	//DBG_ASSERT(fsFileUtil::DirectoryExists(category_dir), category_dir << " does not exist.");
	//}

	//get the elements belonging to the movie pack category
	m_CurrentCategoryDir = category_dir;
	m_pCurrentMode->ProcessCategory(category_dir);
	std::vector<fsLocator> directories;
	pathDirectoryParser parser;
	parser.GetSubDirectories(category_dir, directories);

	//find any elements in our cache directory
	fsLocator cacheDirectory = gfPaths::GetPath(mnmPaths::e_Cache);
	fsLocator catPath = i_CategoryDirectory;
	catPath.Pop();
	subject_name = catPath.GetLastName();
	fcuiUtils::StripNumber(subject_name);
	catPath.Pop();
	mode_name = catPath.GetLastName();

	cacheDirectory.Push(mode_name);
	cacheDirectory.Push(subject_name);
	
	
	

	//add the cache icons to our vector
	parser.GetSubDirectories(cacheDirectory, directories);

	//update the panel for the next level - Element
	fcuiFormMgr::UpdatePanelIcons(e_Element, directories);
	fcuiFormMgr::SelectElementPair(m_CurrentCategoryDir);

}

///---------------------------------------------------------------------------
/// When an element operation is initiated, we should handle the appropriate
/// events here.
///---------------------------------------------------------------------------
void fcmdModeMgr::ProcessElementOperation(const fsLocator& i_ElementDirectory)
{
	SelectElement(i_ElementDirectory);
	m_pCurrentMode->ProcessElement(i_ElementDirectory);
	if(m_CurrentCategoryDir.GetNumNames() > 0)
		fcuiFormMgr::UpdateSelectedElementPair(m_CurrentCategoryDir, i_ElementDirectory);
}

///---------------------------------------------------------------------------
/// When custom/none icons are clicked , we should handle the appropriate
/// events here.
///---------------------------------------------------------------------------
void fcmdModeMgr::ProcessCustomIconsOperation(const fsLocator& i_CustomIconDirectory)
{
	SelectElement(i_CustomIconDirectory);
	m_pCurrentMode->ProcessCustomIcon(i_CustomIconDirectory);
	if(m_CurrentCategoryDir.GetNumNames() > 0)
		fcuiFormMgr::UpdateSelectedElementPair(m_CurrentCategoryDir, i_CustomIconDirectory);
}
///---------------------------------------------------------------------------
/// Select the element in the given directory
///---------------------------------------------------------------------------
void fcmdModeMgr::SelectElement(const fsLocator& i_ElementDirectory)
{
	itString name("");
	if(i_ElementDirectory.GetNumNames() > 0)
		name = i_ElementDirectory.GetLastName();

	fcuiFormMgr::UpdateElementName(name);
}

///---------------------------------------------------------------------------
/// When an element is checked, we need to execute it's operation
///---------------------------------------------------------------------------
void fcmdModeMgr::ProcessCheckedElement(const fsLocator& i_ElementDirectory)
{
	SelectElement(i_ElementDirectory);
	m_pCurrentMode->ProcessElement(i_ElementDirectory);
	fcmdModeAction* actionMode = dynamic_cast<fcmdModeAction*>(m_pCurrentMode);
	if(actionMode != NULL)
	{
		actionMode->ProcessCheckedItem(i_ElementDirectory);
	}
}

///---------------------------------------------------------------------------
/// Return the checked item from the current mode
///---------------------------------------------------------------------------
const fsLocator fcmdModeMgr::GetCheckedElement()
{
	fsLocator checkedLocator;
	fcmdModeAction* actionMode = dynamic_cast<fcmdModeAction*>(m_pCurrentMode);
	if(actionMode != NULL)
	{
		checkedLocator = actionMode->GetCheckedLocator();
	}
	return checkedLocator;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
void fcmdModeMgr::SetTimelineLocked( bool i_bLocked )
{
	m_bTimelineLocked = i_bLocked;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
bool fcmdModeMgr::GetTimelineLocked()
{
	 return m_bTimelineLocked;
}


///---------------------------------------------------------------------------
/// When a timeline object is clicked, we need to execute it's operation
///---------------------------------------------------------------------------
void fcmdModeMgr::ProcessTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	fcmdModeAction* actionMode = dynamic_cast<fcmdModeAction*>(m_pCurrentMode);
	if(actionMode != NULL)
	{
		actionMode->ProcessTimelineItem(i_ItemData);
	}
}

///---------------------------------------------------------------------------
/// Shift a timeline object's drivers and data in our main timeline by the
/// duration amount
///---------------------------------------------------------------------------
void fcmdModeMgr::ShiftTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration, bool i_bManualShift)
{
	fcmdModeAction* actionMode = dynamic_cast<fcmdModeAction*>(m_pCurrentMode);
	if(actionMode != NULL)
	{
		actionMode->ShiftTimelineItem(io_ItemData, i_Duration, i_bManualShift);
	}
}

///---------------------------------------------------------------------------
/// Remove a timeline object's drivers
///---------------------------------------------------------------------------
void fcmdModeMgr::RemoveTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	fcmdModeAction* actionMode = dynamic_cast<fcmdModeAction*>(m_pCurrentMode);
	if(actionMode != NULL)
	{
		actionMode->RemoveTimelineItem(i_ItemData);
	}
}

///---------------------------------------------------------------------------
/// Edit a timeline object's drivers or properties
///---------------------------------------------------------------------------
void fcmdModeMgr::EditTimelineItem(fcuiTimelineItemData& io_ItemData)
{
	fcmdModeAction* actionMode = dynamic_cast<fcmdModeAction*>(m_pCurrentMode);
	if(actionMode != NULL)
	{
		actionMode->EditTimelineItem(io_ItemData);
	}
}

///---------------------------------------------------------------------------
/// Set the main subject of the scene
///---------------------------------------------------------------------------
void fcmdModeMgr::SetMainSubject(const itString& i_CurrentSubject)
{
	m_MainSubject = i_CurrentSubject;
}

///---------------------------------------------------------------------------
/// Set the main subject directory of the scene
///---------------------------------------------------------------------------
void fcmdModeMgr::SetMainSubjectDirectory(const fsLocator& i_CurrentSubjectDirectory)
{
	m_MainSubjectDirectory = i_CurrentSubjectDirectory;
}

///---------------------------------------------------------------------------
/// Set the main subject directory of the scene
///---------------------------------------------------------------------------
const fsLocator& fcmdModeMgr::GetMainSubjectDirectory()
{
	return m_MainSubjectDirectory;
}

///---------------------------------------------------------------------------
/// Set the movie pack directory
///---------------------------------------------------------------------------
void fcmdModeMgr::SetMoviePackDirectory(const itString& i_CurrentMoviePack)
{
	mnmPaths::SetupMoviePackPaths( i_CurrentMoviePack );

	l_CastDirectory = gfPaths::GetPath( mnmPaths::e_CurrentMoviePack );
	l_CastDirectory.Push( fcuiConstants::c_MEDIA );
	l_CastDirectory.Push( fcuiConstants::c_MODE_CAST );

	l_LocationDirectory = gfPaths::GetPath( mnmPaths::e_CurrentMoviePack );
	l_LocationDirectory.Push( fcuiConstants::c_MEDIA );
	l_LocationDirectory.Push( fcuiConstants::c_MODE_LOCATION );

	l_ActionDirectory = gfPaths::GetPath( mnmPaths::e_CurrentMoviePack );
	l_ActionDirectory.Push( fcuiConstants::c_MEDIA );
	l_ActionDirectory.Push( fcuiConstants::c_MODE_ACTION );

	l_MakeDirectory = gfPaths::GetPath( mnmPaths::e_CurrentMoviePack );
	l_MakeDirectory.Push( fcuiConstants::c_MEDIA );
	l_MakeDirectory.Push( fcuiConstants::c_MODE_MAKE );

	l_TheaterDirectory = gfPaths::GetPath( mnmPaths::e_CurrentMoviePack );
	l_TheaterDirectory.Push( fcuiConstants::c_MEDIA );
	l_TheaterDirectory.Push( fcuiConstants::c_MODE_THEATER );

	l_ActionCategoryName = itString( fcuiConstants::c_ACTION_SCENE_CATEGORY );

	DBG_TRACE("Movie Pack CAST     - " << l_CastDirectory);
	DBG_TRACE("Movie Pack LOCATION - " << l_LocationDirectory);
	DBG_TRACE("Movie Pack ACTION   - " << l_ActionDirectory);
	DBG_TRACE("Movie Pack MAKE     - " << l_MakeDirectory);
	DBG_TRACE("Movie Pack THEATER  - " << l_TheaterDirectory);
}

///---------------------------------------------------------------------------
/// Set the timeline subject of the scene
///---------------------------------------------------------------------------
void fcmdModeMgr::SetTimelineSubject(const itString& i_CurrentSubject)
{
	m_TimelineSubject = i_CurrentSubject;
}

///---------------------------------------------------------------------------
/// Get the timeline subject of the scene
///---------------------------------------------------------------------------
const itString& fcmdModeMgr::GetTimelineSubject()
{
	return m_TimelineSubject;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
bool fcmdModeMgr::GetCanDrag()
{
	return m_bCanDrag;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
void fcmdModeMgr::SetCanDrag(bool i_bCanDrag)
{
	m_bCanDrag = i_bCanDrag;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
bool fcmdModeMgr::GetCanCheck()
{
	return m_bCanCheck;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
void fcmdModeMgr::SetCanCheck(bool i_bCanCheck)
{
	m_bCanCheck = i_bCanCheck;
}
