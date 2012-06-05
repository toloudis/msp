/*****************************************************************************
**	fcmdModeAction.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcmd/fcmdModeAction.hpp"

#include "Drivers/Attach/tmlnDriverAttach.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "FCSupport/actn/actnLightMgr.hpp"
#include "FCSupport/actn/actnAudioMgr.hpp"
#include "FCSupport/actn/actnSceneMgr.hpp"
#include "FCSupport/actn/actnPostFXMgr.hpp"
#include "FCSupport/actn/actnTitleCardMgr.hpp"
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#include "FCSupport/fcmd/fcmdConstants.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/qtGUI/fcuiLabel.hpp"
#include "FCSupport/fcui/qtGUI/fcuiTextEditOperations.hpp"
#include "FCSupport/fio/fioUtils.hpp"
#include "FCSupport/ovrly/ovrlyMgr.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Systems/Character/GUI/chtrDialogInterest.hpp"
#include "Systems/Character/Object/chtrObject.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"

#include "AudioDS/sn/snSoundManager.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"

#include <boost/bind.hpp>
#include <QtGui/QtGui>
#include <vector>


//============================================================================
//============================================================================
namespace
{
	itString l_SceneState(fcuiConstants::c_ACTION_STATE_SCENE);
	itString l_AudioState(fcuiConstants::c_ACTION_STATE_AUDIO);
	itString l_LightsState(fcuiConstants::c_ACTION_STATE_LIGHTS);
	itString l_TitleCardState(fcuiConstants::c_ACTION_STATE_TITLECARD);
	itString l_PostFXState(fcuiConstants::c_ACTION_STATE_POSTFX);

	itString l_SystemName("Objects");
	int billboard_width = 0;
	float init_time = 0.0, final_time = 3.0, duration = 0.0;
}

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------
fcmdModeAction::fcmdModeAction()
:	fcmdModeTemplate(fsLocator())
{
	DBG_TRACE("Created Action Mode");
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.zMake_Widget->setVisible(false);
	i_ui->ui.Action_TimelineWidgetOverlay->setVisible(true);
	
}
fcmdModeAction::fcmdModeAction(const fsLocator& i_Directory)
:	fcmdModeTemplate(i_Directory)
{
	m_ModeDirectory = i_Directory;
	SetAllowElementClick(false);
}

//----------------------------------------------------------------------------
// Destructor
//----------------------------------------------------------------------------
fcmdModeAction::~fcmdModeAction()
{}

//----------------------------------------------------------------------------
// Activate the Action mode and any other systems associated with it
//----------------------------------------------------------------------------
void fcmdModeAction::Activate()
{
	DBG_TRACE("Action Mode Activated");
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.Action_TimelineWidgetOverlay->setVisible(true);

	if (fcmdModeMgr::Instance != NULL)
	{
		//fcmdModeMgr::Instance->SetCanCheck(false);
		fcmdModeMgr::Instance->SetCanDrag(true);
		fcmdModeMgr::Instance->SetTimelineLocked(false);
	}

	//enable timeline
	fcuiFormMgr::SetActionTimelineEnabled(true);
	SetAutoPopulatePanels(true);
	tmlnTimeLine::SetValue(maTime::c_ZeroTime);
}

//----------------------------------------------------------------------------
// Deactivate the Action mode and clean up any systems
//----------------------------------------------------------------------------
void fcmdModeAction::DeActivate()
{
	DBG_TRACE("Action Mode DeActivated");

	if (fcmdModeMgr::Instance != NULL)
	{
		//fcmdModeMgr::Instance->SetCanCheck(false);
		fcmdModeMgr::Instance->SetCanDrag(false);
		fcmdModeMgr::Instance->SetTimelineLocked(false);
	}
	//disable timeline
	fcuiFormMgr::SetActionTimelineEnabled(false);
	fcuiFormMgr::HideCameraPicker();
	fcuiFormMgr::HideCheckboxes();
	if ((fcuiTimelineMgr::Instance != NULL) && fcuiTimelineMgr::Instance->IsPlaying())
	{
		fcuiTimelineMgr::Instance->Pause();
	}
	tmlnTimeLine::SetValue(maTime::c_ZeroTime);
}

///-----------------------------------------------------------------------
/// Do any mode thinking
///-----------------------------------------------------------------------
void fcmdModeAction::Think()
{
	if (ovrlyMgr::Instance != NULL)
	{
		ovrlyMgr::Instance->CheckPicks();
	}
	if ((fcuiTimelineMgr::Instance != NULL) && fcuiTimelineMgr::Instance->IsPlaying())
	{
		if(tmlnTimeLine::GetValue() >= fcuiTimelineMgr::Instance->GetEndTime())
		{
			tmlnTimeLine::SetValue( fcuiTimelineMgr::Instance->GetEndTime() );
			fcuiTimelineMgr::Instance->Pause();
			snSoundManager::Pause();
			if( fcmdModeMgr::Instance != NULL)
				fcmdModeMgr::Instance->SetTimelineLocked(false);
		}
		fcuiFormMgr::UpdateTimelineSelection(tmlnTimeLine::GetValue());
		fcuiFormMgr::UpdateCurrentMovieTime();
	}
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessSubject(const fsLocator& i_SubjectDirectory)
{
	m_ActionState = i_SubjectDirectory.GetLastName();
	m_CurrentSubject = m_ActionState;
	if (fcmdModeMgr::Instance != NULL)
		fcmdModeMgr::Instance->SetTimelineSubject(m_ActionState);
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessCategory(const fsLocator& i_CategoryDirectory)
{
	fcuiFormMgr::HideCheckboxes();
	m_CurrentCategory = i_CategoryDirectory.GetLastName();
	//show the item checkboxes for Audio, lights, and postfx
	if (( m_ActionState == l_AudioState ) || 
		( m_ActionState == l_LightsState ) ||
		( m_ActionState == l_PostFXState ))
	{
		//fcuiFormMgr::ShowCheckboxes();
		//fsLocator curChecked = GetCheckedLocator();
		//fcuiFormMgr::ResetCheckboxes(curChecked);
		if (fcmdModeMgr::Instance != NULL)
		{
			//fcmdModeMgr::Instance->SetCanCheck(true);
			fcmdModeMgr::Instance->SetCanDrag(false);
			SetAllowElementClick(true);
		}
	}
	else
	{
		if (fcmdModeMgr::Instance != NULL)
		{
			//fcmdModeMgr::Instance->SetCanCheck(false);
			fcmdModeMgr::Instance->SetCanDrag(true);
			SetAllowElementClick(false);
		}
	}
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessElement(const fsLocator& i_ElementDirectory)
{
	m_CurrentElement = i_ElementDirectory.GetLastName();
	// Determine which functon needs to process the current element
	// based on the current state of the action mode
	//
	if ( m_ActionState == l_SceneState )
	{
		ProcessScene(i_ElementDirectory);
	}
	else if ( m_ActionState == l_AudioState )
	{
		ProcessAudio(i_ElementDirectory);
	}
	else if ( m_ActionState == l_LightsState )
	{
		ProcessLights(i_ElementDirectory);
	}
	else if ( m_ActionState == l_TitleCardState )
	{
		ProcessTitleCard(i_ElementDirectory);
	}
	else if ( m_ActionState == l_PostFXState )
	{
		ProcessPostFX(i_ElementDirectory);
	}
	else
	{
		DBG_ERROR("Unknown Action State");
	}

	fcuiFormMgr::UpdateCurrentMovieTime();
	fcuiFormMgr::UpdateMovieEndTime();
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessCustomIcon(const fsLocator& i_ElementDirectory)
{
	m_CurrentElement = i_ElementDirectory.GetLastName();
	fsLocator l_elementDirectory;
	l_elementDirectory = m_ModeDirectory;
	l_elementDirectory.Push(m_CurrentSubject);
	l_elementDirectory.Push(m_CurrentCategory);
	l_elementDirectory.Push(m_CurrentElement);


	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(i_ElementDirectory, files);
	DBG_TRACE("Loc Dir = " << i_ElementDirectory);

	ProcessFiles( l_elementDirectory, files );
}

///-----------------------------------------------------------------------
/// ProcessFiles
///-----------------------------------------------------------------------
void fcmdModeAction::ProcessFiles(const fsLocator& i_ElementDirectory, const std::vector<fsLocator>& i_Files)
{
	itString icon_file( fcmdConstants::c_Icon );
	itString element_file;

	//decide what to do with each file in the directory
	bool found_local = false;
	for ( int i = 0; i < i_Files.size(); ++i )
	{
		element_file = i_Files[i].GetLastName();
		//DBG_TRACE( i << ". " << element_file << " " );

		//don't process the icon file of the asset
		if ( element_file == icon_file )
			continue;

		found_local = ProcessFile( i_ElementDirectory, i_Files[i] );
	}
}


///-----------------------------------------------------------------------
/// ProcessFile
/// Returns true if the file was processed.
///-----------------------------------------------------------------------
bool fcmdModeAction::ProcessFile(const fsLocator& i_ElementDirectory, const fsLocator& i_File)
{
	fsLocator audio_file(i_File);
	QtFC3D *ui_win = fcuiFormMgr::GetMainWindow();
	itString extension, element_file;
	element_file = audio_file.GetLastName();
	element_file.GetExtension(extension);

	// Test for opening custom items
	//
	if (element_file == itString(fcuiConstants::c_FILE_BROWSE_AUDIO))
	{
		QString caption("Open Audio File");
		QString selectedFilter("Sound (*.wav)");
		fsLocator fs_file;
		QString qs_file;
		QString fileName = QFileDialog::getOpenFileName(ui_win->ui.page,
														caption,
														qs_file,
														selectedFilter
														);
		if (!fileName.isEmpty())
		{
			itString itstr;
			itstr = ((const itString::CharType*)fileName.data());
			fsFileUtil::UnicodeStringToLocator(itstr, fs_file);

			audio_file = fs_file;

			//	build the output filename for the thumbnail
			fsLocator thumbnail_file = gfPaths::GetPath(mnmPaths::e_Cache);
			thumbnail_file.Push( fcuiConstants::c_MODE_ACTION );
			itString subj(fcuiConstants::c_ACTION_STATE_AUDIO);
			fcuiUtils::StripNumber( subj );
			thumbnail_file.Push( subj );

			//	create folder for each custom item
			itString thumbnail_name = fs_file.GetLastName();
			thumbnail_name.StripExtension();
			thumbnail_file.Push( thumbnail_name );

			//	create the save icon name
			thumbnail_file.Push(fcuiConstants::c_FILE_ICON);

			//	path the original icon
			fsLocator generic_icon = gfPaths::GetPath( gfPaths::e_ExePath );
			generic_icon.Push(fcuiConstants::c_MEDIA);
			generic_icon.Push(fcuiConstants::c_MODE_ICONS);
			generic_icon.Push(fcuiConstants::c_MODE_ACTION);
			generic_icon.Push(fcuiConstants::c_FILE_AUDIO_CUSTOM_ICON);

			//	copy the icon
			fioUtils::CopyFile( generic_icon, thumbnail_file );

			if (actnAudioMgr::Instance != NULL)
			{
				actnAudioMgr::Instance->ProcessAudio( audio_file );
			}

			//	save the XML reference file
			//
			thumbnail_file.Pop();
			itString xml_name = fs_file.GetLastName();
			xml_name.StripExtension();
			xml_name += itString(L".");
			xml_name += itString(fcuiConstants::l_EXT_XML);
			thumbnail_file.Push( xml_name );

			// write out the file
			fioUtils::WriteLocationXML( thumbnail_file, fs_file );

			fsLocator category_dir = i_ElementDirectory;
			category_dir.Pop();
			if( fcmdModeMgr::Instance != NULL )
			{
				fcmdModeMgr::Instance->ProcessCategoryOperation(category_dir);
			}
		}
		return true;
	}
	else if (element_file == itString(fcuiConstants::c_FILE_NONE_AUDIO))
	{
		if (actnAudioMgr::Instance != NULL)
			{
				actnAudioMgr::Instance->ProcessAudio( i_File );
			}
		return true;
	}
	else if (element_file == itString(fcuiConstants::c_FILE_NONE_POSTFX))
	{
		if (actnPostFXMgr::Instance != NULL)
		{
			fsLocator element_dir = i_File;
			element_dir.Pop();
			actnPostFXMgr::Instance->ProcessPostFX( element_dir );
		}
	}
	else
	{
		DBG_ERROR("Don't know what to do with this extension: " << extension);
	}
	return false;
}

///---------------------------------------------------------------------------
/// When an element is checked, we need to execute it's operation
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessCheckedItem(const fsLocator& i_ElementDirectory)
{
	//show the item checkboxes for Audio, lights, and postfx
	if ( m_ActionState == l_AudioState )
	{
		if (actnAudioMgr::Instance != NULL)
		{
			actnAudioMgr::Instance->SetCheckedItem( i_ElementDirectory );
		}
	}
	else if ( m_ActionState == l_LightsState )
	{
		if (actnLightMgr::Instance != NULL)
		{
			actnLightMgr::Instance->SetCheckedItem(i_ElementDirectory);
		}
	}
	else if ( m_ActionState == l_PostFXState )
	{
		if (actnPostFXMgr::Instance != NULL)
		{
			actnPostFXMgr::Instance->SetCheckedItem(i_ElementDirectory);
		}
	}
}

///---------------------------------------------------------------------------
/// When the panel is repopulated, we need to return the checkbox data if necessary
///---------------------------------------------------------------------------
const fsLocator fcmdModeAction::GetCheckedLocator()
{
	fsLocator checkedLocator;
	if ( m_ActionState == l_AudioState )
	{
		if (actnAudioMgr::Instance != NULL)
		{
			checkedLocator = actnAudioMgr::Instance->GetCheckedItem();
		}
	}
	else if ( m_ActionState == l_LightsState )
	{
		if (actnLightMgr::Instance != NULL)
		{
			checkedLocator = actnLightMgr::Instance->GetCheckedItem();
		}
	}
	else if ( m_ActionState == l_PostFXState )
	{
		if (actnPostFXMgr::Instance != NULL)
		{
			checkedLocator = actnPostFXMgr::Instance->GetCheckedItem();
		}
		
	}

	return checkedLocator;
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a timeline item needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	itString subject = i_ItemData.m_TimelineCategory;
	if ( subject == l_SceneState )
	{
		ProcessSceneTimelineItem(i_ItemData);
	}
	else if ( subject == l_AudioState )
	{
	}
	else if ( subject == l_LightsState )
	{
	}
	else if ( subject == l_TitleCardState )
	{
		ProcessTitlecardTimelineItem(i_ItemData);
	}
	else if ( subject == l_PostFXState )
	{
	}
	else
	{
		DBG_ERROR("Unknown Action State");
	}
	fcuiFormMgr::UpdateCurrentMovieTime();
	fcuiFormMgr::UpdateMovieEndTime();
}

///---------------------------------------------------------------------------
/// Shift a timeline object's drivers and data in our main timeline by the
/// duration amount
///---------------------------------------------------------------------------
void fcmdModeAction::ShiftTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration, bool i_bManualShift)
{
	itString subject = io_ItemData.m_TimelineCategory;
	if ( subject == l_SceneState )
	{
		ShiftSceneTimelineItem(io_ItemData, i_Duration);
	}
	else if ( subject == l_AudioState )
	{
	}
	else if ( subject == l_LightsState )
	{
	}
	else if ( subject == l_TitleCardState )
	{
		ShiftTitleCardTimelineItem(io_ItemData, i_Duration, i_bManualShift);
	}
	else if ( subject == l_PostFXState )
	{
	}
	else
	{
		DBG_ERROR("Unknown Action State");
	}
	fcuiFormMgr::UpdateCurrentMovieTime();
	fcuiFormMgr::UpdateMovieEndTime();
}

///---------------------------------------------------------------------------
/// Remove a timeline object's drivers
///---------------------------------------------------------------------------
void fcmdModeAction::RemoveTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	itString subject = i_ItemData.m_TimelineCategory;
	if ( subject == l_SceneState )
	{
		RemoveSceneTimelineItem(i_ItemData);
		if(fcuiTimelineMgr::Instance != NULL)
			fcuiTimelineMgr::Instance->SetEndTime( fcuiTimelineMgr::Instance->GetEndTime() - 
													(i_ItemData.m_EndTime - i_ItemData.m_StartTime));
	}
	else if ( subject == l_AudioState )
	{
	}
	else if ( subject == l_LightsState )
	{
	}
	else if ( subject == l_TitleCardState )
	{
		RemoveTitleCardTimelineItem(i_ItemData);
		if(fcuiTimelineMgr::Instance != NULL)
			fcuiTimelineMgr::Instance->SetEndTime( fcuiTimelineMgr::Instance->GetEndTime() - 
												   (i_ItemData.m_EndTime - i_ItemData.m_StartTime));
	}
	else if ( subject == l_PostFXState )
	{
	}
	else
	{
		DBG_ERROR("Unknown Action State");
	}

	fcuiFormMgr::UpdateCurrentMovieTime();
	fcuiFormMgr::UpdateMovieEndTime();
}

///---------------------------------------------------------------------------
/// Edit a timeline object's drivers or properties
///---------------------------------------------------------------------------
void fcmdModeAction::EditTimelineItem(fcuiTimelineItemData& io_ItemData)
{
	itString subject = io_ItemData.m_TimelineCategory;
	if ( subject == l_SceneState )
	{
		fcuiFormMgr::SetTimelineEditMode(true);
		EditSceneTimelineItem(io_ItemData);
	}
	else if ( subject == l_AudioState )
	{
	}
	else if ( subject == l_LightsState )
	{
	}
	else if ( subject == l_TitleCardState )
	{
		fcuiFormMgr::SetTimelineEditMode(true);
		EditTitleCardTimelineItem(io_ItemData);
	}
	else if ( subject == l_PostFXState )
	{

	}
	else
	{
		DBG_ERROR("Unknown Action State");
	}
	fcuiFormMgr::UpdateCurrentMovieTime();
	fcuiFormMgr::UpdateMovieEndTime();
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a scene element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessScene(const fsLocator& i_ElementDirectory)
{
	if (actnSceneMgr::Instance != NULL)
	{
		actnSceneMgr::Instance->ProcessScene( i_ElementDirectory );
	}
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an audio element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessAudio(const fsLocator& i_ElementDirectory)
{
	//	search through and find the audio file(s)
	//
	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(i_ElementDirectory, files);
	itString icon( fcmdConstants::c_Icon );
	itString extension, icon_check;
	fsLocator Audio_file;
	for ( int i = 0; i < files.size(); ++i )
	{
		//	don't process the icon file of the asset
		icon_check = files[i].GetLastName();
		if ( icon_check == icon )
			continue;

		//	check for valid types
		files[i].GetLastName().GetExtension(extension);
		if (extension == itString("wav"))
		{
			Audio_file = files[i];
		}
		else if (extension == itString(fcuiConstants::l_EXT_XML))
		{
			//	read in the actual location of the file
			fioUtils::ReadLocationXML( files[i], Audio_file );
		}
	}
	fsLocator audio_file(Audio_file);
	if (actnAudioMgr::Instance != NULL)
	{
		actnAudioMgr::Instance->ProcessAudio( audio_file );
	}

	
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a lighting element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessLights(const fsLocator& i_ElementDirectory)
{
	if (actnLightMgr::Instance != NULL)
	{
		actnLightMgr::Instance->ProcessLights( i_ElementDirectory );
	}
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a title card element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessTitleCard(const fsLocator& i_ElementDirectory)
{
	if (actnTitleCardMgr::Instance != NULL)
	{
		actnTitleCardMgr::Instance->ProcessTitleCard( i_ElementDirectory );
	}
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a post fx element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeAction::ProcessPostFX(const fsLocator& i_ElementDirectory)
{
	if (actnPostFXMgr::Instance != NULL)
	{
		actnPostFXMgr::Instance->ProcessPostFX( i_ElementDirectory );
	}
}

///-----------------------------------------------------------------------
/// Perfrom the appropriate operations for a timeline object belonging
/// to the scene category
///-----------------------------------------------------------------------
void fcmdModeAction::ProcessSceneTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	if (actnSceneMgr::Instance != NULL)
	{
		actnSceneMgr::Instance->ProcessSceneTimelineItem( i_ItemData );
	}
}

///-----------------------------------------------------------------------
/// Perfrom the appropriate operations for a timeline object belonging
/// to the TitleCard category
///-----------------------------------------------------------------------
void fcmdModeAction::ProcessTitlecardTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	if (actnTitleCardMgr::Instance != NULL)
	{
		actnTitleCardMgr::Instance->ProcessTitleCardTimelineItem( i_ItemData );
	}
}



///---------------------------------------------------------------------------
/// shift the timeline item belonging to the scene category
///---------------------------------------------------------------------------
void fcmdModeAction::ShiftSceneTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration)
{
	if (actnSceneMgr::Instance != NULL)
	{
		actnSceneMgr::Instance->ShiftSceneTimelineItem( io_ItemData, i_Duration );
	}
}

///---------------------------------------------------------------------------
/// shift the timeline item belonging to the Titlecard category
///---------------------------------------------------------------------------
void fcmdModeAction::ShiftTitleCardTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration, bool i_bManualShift)
{
	if (actnTitleCardMgr::Instance != NULL)
	{
		actnTitleCardMgr::Instance->ShiftTitleCardTimelineItem(io_ItemData, i_Duration, i_bManualShift);
	}
	
}

///---------------------------------------------------------------------------
/// shift the timeline item belonging to the scene category
///---------------------------------------------------------------------------
void fcmdModeAction::RemoveSceneTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	if (actnSceneMgr::Instance != NULL)
	{
		actnSceneMgr::Instance->RemoveSceneTimelineItem( i_ItemData );
	}
}

///---------------------------------------------------------------------------
/// Remove the timeline item belonging to the Titlecard category
///---------------------------------------------------------------------------
void fcmdModeAction::RemoveTitleCardTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	if (actnTitleCardMgr::Instance != NULL)
	{
		actnTitleCardMgr::Instance->RemoveTitleCardTimelineItem(i_ItemData);
	}
}
///---------------------------------------------------------------------------
/// Edit the timeline item belonging to the scene category
///---------------------------------------------------------------------------
void fcmdModeAction::EditSceneTimelineItem(fcuiTimelineItemData& io_ItemData)
{
	if (actnSceneMgr::Instance != NULL)
	{
		actnSceneMgr::Instance->EditSceneTimelineItem( io_ItemData );
	}
}

void fcmdModeAction::EditTitleCardTimelineItem(fcuiTimelineItemData& io_ItemData)
{
	if (actnTitleCardMgr::Instance != NULL)
	{
		actnTitleCardMgr::Instance->EditTitleCardTimelineItem(io_ItemData);
	}
}
