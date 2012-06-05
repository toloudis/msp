/*****************************************************************************
**	fcmdModeLocation.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcmd/fcmdModeLocation.hpp"

#include "FCSupport/fcmd/fcmdConstants.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fio/fioUtils.hpp"
#include "FCSupport/ovrly/ovrlyMgr.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/Ma/maFunctions.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
	itString l_State_Props(fcuiConstants::c_LOCATION_STATE_PROPS);
	itString l_State_Themes(fcuiConstants::c_LOCATION_STATE_THEMES);
	itString l_State_Sets(fcuiConstants::c_LOCATION_STATE_SETS);
	maPoint3d	l_HotspotLoc;
	maRotation	l_HotspotOrient;
	maPoint3d	l_HotspotScale;

	const itString l_Asset_Folder("99.Assets");
}


//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------
fcmdModeLocation::fcmdModeLocation()
:	m_ModeDirectory(fsLocator())
{
	DBG_TRACE("Created Location Mode");
}

fcmdModeLocation::fcmdModeLocation(const fsLocator& i_Directory)
:	m_ModeDirectory(i_Directory)
{
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
fcmdModeLocation::~fcmdModeLocation()
{}

//------------------------------------------------------------------------
// Activate the Location mode and any other systems associated with it
//------------------------------------------------------------------------
void fcmdModeLocation::Activate()
{
	DBG_TRACE("Location Mode Activated");

	QtFC3D *ui_win = fcuiFormMgr::GetMainWindow();
	ui_win->ui.zMake_Widget->setVisible(false);
	ui_win->ui.Action_TimelineWidgetOverlay->setVisible(false);
	
	if( (fcmdModeMgr::Instance != NULL) && (fcuiTimelineMgr::Instance != NULL) )
	{
		tmlnTimeLine::SetValue(fcmdModeMgr::Instance->GetCastAnimStart());
		fcuiTimelineMgr::Instance->Pause();  //make sure playback is stopped
	}
}

//------------------------------------------------------------------------
// Deactivate the Location mode and clean up any systems
//------------------------------------------------------------------------
void fcmdModeLocation::DeActivate()
{
	DBG_TRACE("Location Mode DeActivated");
	if ((fcuiTimelineMgr::Instance != NULL) && fcuiTimelineMgr::Instance->IsPlaying())
	{
		tmlnTimeLine::SetValue(maTime::c_ZeroTime);
		fcuiTimelineMgr::Instance->Pause();
	}
}

///-----------------------------------------------------------------------
/// Do any mode thinking
///-----------------------------------------------------------------------
void fcmdModeLocation::Think()
{
	if (ovrlyMgr::Instance != NULL)
	{
		ovrlyMgr::Instance->CheckPicks();
	}
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a subject needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeLocation::ProcessSubject(const fsLocator& i_SubjectDirectory)
{
	// should load the main subject
	m_CurrentSubject = i_SubjectDirectory.GetLastName();
	m_CurrentCategory.Clear();
	m_CurrentElement.Clear();

	if (m_CurrentSubject == itString(fcuiConstants::c_LOCATION_STATE_VFX))
	{
		if (ovrlyMgr::Instance != NULL)
		{
			ovrlyMgr::Instance->Something();
		}
	}
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a category needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeLocation::ProcessCategory(const fsLocator& i_CategoryDirectory)
{
	// set any states here depending on the category that needs to process
	//rpnOperations::NextCamera();
	itString cat_name = i_CategoryDirectory.GetLastName();
	fcuiUtils::StripNumber(cat_name);
	itString cam_name(fcuiConstants::c_CAMERA_BASENAME);
	cam_name += cat_name;
	nameString camName(cam_name);

	int index = camsCameraMgr::GetIndexForName(camName);
	if (index >= 0 && index < camsCameraMgr::GetNumCameras())
	{
		rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
		camsCameraMgr::SelectCamera(index);
	}
	else
	{
		rpnOperations::ChangeCameraToEditor();
	}
	m_CurrentCategory = cat_name;
	m_CurrentElement.Clear();
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeLocation::ProcessElement(const fsLocator& i_ElementDirectory)
{
	m_CurrentElement = i_ElementDirectory.GetLastName();

	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(i_ElementDirectory, files);
	DBG_TRACE("Loc Dir = " << i_ElementDirectory);

	//	sort the file list with preference towards GXBs
	fcuiUtils::SortFileList_GXB( files );

	ProcessFiles( i_ElementDirectory, files );
}
///---------------------------------------------------------------------------
/// Perform the mode specific operations when an element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeLocation::ProcessCustomIcon(const fsLocator& i_ElementDirectory)
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
/// Perform the mode specific operations when dealing with props
///-----------------------------------------------------------------------
void fcmdModeLocation::ProcessProps(const fsLocator& i_Element, const fsLocator& i_ElementDirectory)
{
	//	Find the hotspot object (hidden) and grab its postiion
	//DBG_TRACE("HOTSPOT: finding hotspot marker");
	
	const int num_objects = chtrObjectMgr::GetNumObjects();
	for (int i=0; i<num_objects; ++i)
	{
		//DBG_TRACE("    looking: " << chtrObjectMgr::GetObject(i)->GetName().GetString().c_str() << " vs " << itStringUtil::GetStdString(m_CurrentCategory).c_str() << " (" << m_CurrentCategory << ")");

		if (chtrObjectMgr::GetObject(i)->GetName().GetString() == itStringUtil::GetStdString(m_CurrentCategory))
		{
			l_HotspotLoc = chtrObjectMgr::GetObject(i)->GetPickObject()->GetPosition();
			l_HotspotOrient = chtrObjectMgr::GetObject(i)->GetPickObject()->GetOrientation();
			l_HotspotScale = chtrObjectMgr::GetObject(i)->GetPickObject()->GetScale();
			//DBG_TRACE("          Found! " << chtrObjectMgr::GetObject(i)->GetName().GetString().c_str() << " at " << l_HotspotLoc);
		}
	}

	if (i_Element.GetLastName() != itString(fcuiConstants::c_FILE_NONE_MODEL))
	{
		//	Is there an object already there?  If so, remove it
		//DBG_TRACE("HOTSPOT: checking for existing object");
		for (int i=0; i<num_objects; ++i)
		{
			//DBG_TRACE("    looking: " << chtrObjectMgr::GetObject(i)->GetBaseData().m_Position.GetValue() << " vs " << l_HotspotLoc );

			const float c_TOLERANCE = 0.00001f;
			if ((chtrObjectMgr::GetObject(i)->GetName().GetString() == itStringUtil::GetStdString(m_CurrentCategory)))
			{
				//DBG_TRACE("          Found! " << chtrObjectMgr::GetObject(i)->GetName().GetString().c_str());
				chtrObjectMgr::DeleteObject(i);
				break;
			}
		}

		//	Load the new object and place it in the hotspot's location
		//DBG_TRACE("HOTSPOT: loading new object");

		fcuiUtils::LoadObject(i_Element);

		chtrObject* pCSO = dynamic_cast<chtrObject*>(sel3dMgr::GetSelected());
		itString object_name;
		if (pCSO != NULL)
		{
			//DBG_TRACE("HOTSPOT: setting new position " << l_HotspotLoc);
			const bool c_CREATEUNDO = false;
			pCSO->UpdatePosition(l_HotspotLoc, c_CREATEUNDO);
			pCSO->UpdateOrientation(l_HotspotOrient, c_CREATEUNDO);
			pCSO->UpdateScale(l_HotspotScale, c_CREATEUNDO);
			pCSO->SetName(nameString(m_CurrentCategory));
			object_name = itString(pCSO->GetDisplayName().c_str());
		}

		lsetOperations::AddObjectToLightSet(nameString("Set"), nameString(object_name));
		envtOperations::AddObjectToEnvironment(nameString("MAIN"), nameString(object_name));
		std::vector<fsLocator> files;
		pathDirectoryParser parser;
		parser.GetDirectoryFiles(i_ElementDirectory, files);
	
		itString icon( fcmdConstants::c_Icon );
		itString extension, icon_check;

		//decide what to do with each file in the directory
		for ( int i = 0; i < files.size(); ++i )
		{
			icon_check = files[i].GetLastName();
			//don't process the icon file of the asset
			if ( icon_check == icon )
				continue;

			files[i].GetLastName().GetExtension(extension);
			if ( extension == fcuiConstants::l_EXT_GMB )
			{
				fcuiUtils::LoadMaterials(object_name, files[i]);
			}
			else if ( extension == fcuiConstants::l_EXT_GAB )
			{
				//this is the idle animation for the prop, so we need to set its driver and make it loop through the timeline
				bool bLooping = true;
				maTime start_time = maTime::c_ZeroTime;
				maTime end_time = maTime::FromSeconds(-1.0f);
				fcuiUtils::CreateObjectAnimationDriver(nameString(object_name), files[i], start_time, end_time, bLooping);
			}
		}
	}

	else 
	{
		for (int i=0; i<num_objects; ++i)
		{
			if ((chtrObjectMgr::GetObject(i)->GetName().GetString() == itStringUtil::GetStdString(m_CurrentCategory)))
			{
				//DBG_TRACE("          Found! " << chtrObjectMgr::GetObject(i)->GetName().GetString().c_str());
				chtrObjectMgr::GetObject(i)->SetEditorVisible(false);
				break;
			}
		}
	}
}

///-----------------------------------------------------------------------
/// ProcessFiles
///-----------------------------------------------------------------------
void fcmdModeLocation::ProcessFiles(const fsLocator& i_ElementDirectory, const std::vector<fsLocator>& i_Files)
{
	
	itString icon_file( fcmdConstants::c_Icon );
	itString element_file;

	//decide what to do with each file in the directory
	bool found_local = false;

	//	build the assets folder
	//
	//	it will be the root of the assets folder plus a sub-folder per prop
	//	that MUST match the folder name in the props directory
	fsLocator assets = i_ElementDirectory;
	assets.Pop();
	assets.Pop();
	assets.Push(l_Asset_Folder);
	assets.Push(i_ElementDirectory.GetLastName());
	
	//	if the directory is there search it.
	//
	if ( fsFileUtil::DirectoryExists(assets) )
	{
		//	Search through all the files in the directory and decide what
		//	to do with each one.
		//
		std::vector<fsLocator> files;
		pathDirectoryParser parser;
		itString asset_name = i_ElementDirectory.GetLastName();
		parser.GetDirectoryFiles(assets, files);

		fcuiUtils::SortFileList_GXB( files );	//	sort the file list with preference towards GXBs
		for ( int i = 0; i < files.size(); ++i )
		{
			found_local = ProcessFile( i_ElementDirectory, files[i] );
		}
	}

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
bool fcmdModeLocation::ProcessFile(const fsLocator& i_ElementDirectory, const fsLocator& i_File)
{
	QtFC3D *ui_win = fcuiFormMgr::GetMainWindow();
	itString extension, element_file;
	element_file = i_File.GetLastName();
	element_file.GetExtension(extension);

	// Test for opening custom images
	if (element_file == itString(fcuiConstants::c_FILE_BROWSE_IMAGE))
	{
		QString caption("Open Image");
		QString selectedFilter("Images (*.jpg)");
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

			//	build the output filename for the thumbnail
			fsLocator thumbnail_file = gfPaths::GetPath(mnmPaths::e_Cache);
			thumbnail_file.Push( fcuiConstants::c_MODE_LOCATION );
			itString subj(m_CurrentSubject);
			fcuiUtils::StripNumber( subj );
			thumbnail_file.Push( subj );

			//	create folder for each user image
			itString thumbnail_name = fs_file.GetLastName();
			thumbnail_name.StripExtension();
			thumbnail_file.Push( thumbnail_name );

			//	create the icon name
			thumbnail_file.Push(fcuiConstants::c_FILE_ICON);

			//	load the texture (and create a thumbnail)
			fcuiUtils::LoadTexture(m_CurrentCategory, fs_file, thumbnail_file);

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

			//update the panel icons
			fsLocator category_dir = i_ElementDirectory;
			category_dir.Pop();
			if( fcmdModeMgr::Instance != NULL )
			{
				fcmdModeMgr::Instance->ProcessCategoryOperation(category_dir);
			}
		}
		return true;
	}
	else if (element_file == itString(fcuiConstants::c_FILE_NONE_IMAGE))
	{
			//	load the texture (and create a thumbnail)
			fcuiUtils::LoadTexture(m_CurrentCategory, i_File);
			return true;
	}
	else if (element_file == itString(fcuiConstants::c_FILE_BROWSE_VIDEO))
	{
		QString caption("Open Video");
		QString selectedFilter("Videos (*.avi)");
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

			//	build the output filename for the thumbnail
			fsLocator thumbnail_file = gfPaths::GetPath(mnmPaths::e_Cache);
			thumbnail_file.Push( fcuiConstants::c_MODE_LOCATION );
			itString subj(m_CurrentSubject);
			fcuiUtils::StripNumber( subj );
			thumbnail_file.Push( subj );

			//	create folder for each user image
			itString thumbnail_name = fs_file.GetLastName();
			thumbnail_name.StripExtension();
			thumbnail_file.Push( thumbnail_name );

			//	create the icon name
			thumbnail_file.Push(fcuiConstants::c_FILE_ICON);

			//	path the original icon
			fsLocator generic_icon = gfPaths::GetPath( gfPaths::e_ExePath );
			generic_icon.Push(fcuiConstants::c_MEDIA);
			generic_icon.Push(fcuiConstants::c_MODE_ICONS);
			generic_icon.Push(fcuiConstants::c_MODE_LOCATION);
			generic_icon.Push(fcuiConstants::c_FILE_VIDEO_CUSTOM_ICON);

			//	copy the icon
			fioUtils::CopyFile( generic_icon, thumbnail_file );

			fcuiUtils::LoadVideo(m_CurrentCategory, fs_file);

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
	else if (element_file == itString(fcuiConstants::c_FILE_NONE_VIDEO))
	{
			fcuiUtils::LoadVideo(m_CurrentCategory, i_File);
			return true;
	}
	else if(element_file == itString(fcuiConstants::c_FILE_NONE_PROPS))
	{
		ProcessProps(element_file, i_ElementDirectory);
	}
	else if ( extension == fcuiConstants::l_EXT_XML )
	{
		fsLocator image_file;
		fioUtils::ReadLocationXML( i_File, image_file );

		//	load the texture
		fcuiUtils::LoadTexture(m_CurrentCategory, image_file);
	}
	else if ( extension == fcuiConstants::l_EXT_GXB )
	{
		if ( m_CurrentSubject == l_State_Props )
		{
			ProcessProps(i_File, i_ElementDirectory);
		}
		else if ( m_CurrentSubject == l_State_Themes )
		{
			//fcuiUtils::ReloadObject(m_CurrentCategory,files[i]);
			itString ObjectName = i_File.GetLastName();
			ObjectName.StripExtension();
			fsLocator ObjectPath = i_File;
			fcuiUtils::ReloadObject(ObjectName,ObjectPath);
		}
		else if ( m_CurrentSubject == l_State_Sets )
		{
			itString ObjectName = i_File.GetLastName();
			ObjectName.StripExtension();
			fsLocator ObjectPath = i_File;
			fcuiUtils::ReloadObject(ObjectName,ObjectPath);
		}
		else 
		{
			//fcuiUtils::ReloadObject(m_CurrentCategory,files[i]);
			fcuiUtils::LoadObject(i_File);
		}
		return true;
	}
	else if ( extension == fcuiConstants::l_EXT_DDS || extension == fcuiConstants::l_EXT_JPG)
	{
		fcuiUtils::LoadTexture(m_CurrentCategory, i_File);
		return true;
	}
	else if ( extension == fcuiConstants::l_EXT_MTL)
	{
		itString object_name;
		object_name = m_CurrentCategory;
		itString material_name;
		element_file.GetBase(material_name);
		fcuiUtils::LoadMaterial(object_name, material_name, i_File);
		return true;
	}
	else if ( extension == fcuiConstants::l_EXT_GMB)
	{
		itString object_name = m_CurrentCategory;
		fcuiUtils::LoadMaterials(object_name, i_File);
		return true;
	}
	else if ( extension == fcuiConstants::l_EXT_AVI)
	{
		fcuiUtils::LoadVideo(m_CurrentCategory, i_File);
		return true;
	}
	else if ( extension == fcuiConstants::l_EXT_PY)
	{
		fsLocator script_file = i_File;
		pythUtil::ScriptFile(script_file);
	}
	else
	{
		DBG_ERROR("Don't know what to do with this extension: " << extension);
	}
	return false;
}


