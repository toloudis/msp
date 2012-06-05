/*****************************************************************************
**	fcmdModeCast.cpp
**
**		see .hpp
**
**
**	The model
**	---------
**
**	Media > Cast > Dancer > Hair > Bun   > Dancer_Hair.gxb
**	                                     > Black > Black.gmb
**	                                     > Brown > Brown.gmb
**	                             > Plait > Dancer_Hair.gxb
**	                                     > Black > Black.mtl
**	                                     > Brown > Brown.mtl
**
**	Media > Cast > Dancer > Wardrobe > Longdress > Dancer_Wardrobe.gxb
**	                                             > Black > Black.mtl
**	                                             > White > White.mtl
**	                                 > Skirt     > Dancer_Wardrobe.gxb
**	                                             > Black > Black.mtl
**	                                             > White > White.mtl
**
**	The animations
**	--------------
**
**	Media > Action > 01.Scene > Sequences > Clap >
**	{
**		Dancer_Wardrobe_Longdress_Clap.gab, Dancer_Hair_Bun_Clap.gab,
**		Dancer_Hair_Plait_Clap.gab 
**	}
**	                                      > Kiss >
**	{
**		Dancer_Wardrobe_Skirt_Kiss.gab, Dancer_Wardrobe_Longdress_Kiss.gab,
**		Dancer_Hair_Bun_Kiss.gab 
**	}
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcmd/fcmdModeCast.hpp"

#include "Features/RenderPanels/rpnOperations.hpp"
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#include "FCSupport/fcui/qtGUI/fcuiLabel.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcmd/fcmdConstants.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/ovrly/ovrlyMgr.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Core/fs/fsFileUtil.hpp"

#include <vector>


///----------------------------------------------------------------------------
/// Constructors
///----------------------------------------------------------------------------
fcmdModeCast::fcmdModeCast()
:	fcmdModeTemplate(fsLocator())
{
	DBG_TRACE("Created Cast Mode");
}
fcmdModeCast::fcmdModeCast(const fsLocator& i_Directory)
:	fcmdModeTemplate(i_Directory)
{
}

///----------------------------------------------------------------------------
/// Destructor
///----------------------------------------------------------------------------
fcmdModeCast::~fcmdModeCast()
{}

///----------------------------------------------------------------------------
/// Activate the cast mode and any other systems associated with it
///----------------------------------------------------------------------------
void fcmdModeCast::Activate()
{
	DBG_TRACE("Cast Mode Activated");
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.zMake_Widget->setVisible(false);
	i_ui->ui.Action_TimelineWidgetOverlay->setVisible(false);

	if( (fcmdModeMgr::Instance != NULL) && (fcuiTimelineMgr::Instance != NULL) )
	{
		tmlnTimeLine::SetValue(fcmdModeMgr::Instance->GetCastAnimStart());
		fcuiTimelineMgr::Instance->Play();
	}
}

///----------------------------------------------------------------------------
/// Deactivate the cast mode and clean up any systems
///----------------------------------------------------------------------------
void fcmdModeCast::DeActivate()
{
	DBG_TRACE("Cast Mode DeActivated");
	if ((fcuiTimelineMgr::Instance != NULL))
	{
		//stop idle playback
		fcuiTimelineMgr::Instance->Pause(); 
	}
}

///-----------------------------------------------------------------------
/// Do any mode thinking
///-----------------------------------------------------------------------
void fcmdModeCast::Think()
{
	if (ovrlyMgr::Instance != NULL)
	{
		ovrlyMgr::Instance->CheckPicks();
	}
	if ((fcmdModeMgr::Instance != NULL) && (fcuiTimelineMgr::Instance != NULL))
	{	
		//TIME why use floor with seconds?
		//if(floor(tmlnTimeLine::GetValue()) >= floor(fcmdModeMgr::Instance->GetCastAnimEnd()))
		if(floor(tmlnTimeLine::GetValue().AsSeconds()) >= floor(fcmdModeMgr::Instance->GetCastAnimEnd().AsSeconds()))
		{
			tmlnTimeLine::SetValue(fcmdModeMgr::Instance->GetCastAnimStart());
			//restart idle playback
			fcuiTimelineMgr::Instance->Play(); 			
		}
	}
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a subject needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeCast::ProcessSubject(const fsLocator& i_SubjectDirectory)
{
	// should load the main subject
	m_CurrentSubject = i_SubjectDirectory.GetLastName();
	m_CurrentCategory.Clear();
	m_CurrentElement.Clear();

	fcuiUtils::SetObjectName(m_CurrentSubject);

	if( fcmdModeMgr::Instance != NULL )
	{
		fcmdModeMgr::Instance->SetMainSubject(m_CurrentSubject);
		fcmdModeMgr::Instance->SetMainSubjectDirectory(i_SubjectDirectory);
	}

	/*std::string camname = "Cam_Location";
	nameString camName(camname);
	int index = camsCameraMgr::GetIndexForName(camName);
	if(index >= 0 && index < camsCameraMgr::GetNumCameras())
	{
		rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
		camsCameraMgr::SelectCamera(index);
	}*/
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a category needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeCast::ProcessCategory(const fsLocator& i_CategoryDirectory)
{
	// set any states here depending on the category that needs to process
	itString cat_name = i_CategoryDirectory.GetLastName();
	fcuiUtils::StripNumber(cat_name);
	itString cam_name(fcuiConstants::c_CAMERA_BASENAME);
	cam_name += cat_name;
	nameString camName(cam_name);

	//	change the camera
	int index = camsCameraMgr::GetIndexForName(camName);
	if(index >= 0 && index < camsCameraMgr::GetNumCameras())
	{
		rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
		camsCameraMgr::SelectCamera(index);
	}
	else 
	{
		rpnOperations::ChangeCameraToEditor();
	}

	//	set the current variables
	m_CurrentCategory = cat_name;
	m_CurrentElement.Clear();
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when an element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void fcmdModeCast::ProcessElement(const fsLocator& i_ElementDirectory)
{
	DBG_TRACE("Element needs to process in cast mode");
	
	m_CurrentElement = i_ElementDirectory.GetLastName();
	fcuiUtils::StripNumber(m_CurrentElement);

	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(i_ElementDirectory, files);
	fcuiUtils::SortFileList_GXB( files );
	
	itString icon_file( fcmdConstants::c_Icon );
	itString extension, element_file;

	// For gxb reloading, first we need to delete Subject_Category_SubCategory.gxb
	// For eg. To reload hair, we need to do the following
	// 1. Search for Dancer_Hair_X.gxb
	// 2. Delete the gxb
	// 3. Load the new gxb.

	std::string subject_name = itStringUtil::GetStdString(m_CurrentSubject);
	std::string category_name = itStringUtil::GetStdString(m_CurrentCategory);

	
	std::string object_to_delete = subject_name + "_" + category_name ;
	
	itString reload_obj(object_to_delete.c_str());
	//fsFileUtil::ANSIFilenameToLocator(object_to_delete,delObj);


	//decide what to do with each file in the directory
	for ( int i = 0; i < files.size(); ++i )
	{
		element_file = files[i].GetLastName();
		//don't process the icon file of the asset
		if ( element_file == icon_file )
			continue;

		element_file.GetExtension(extension);

		if ( extension == fcuiConstants::l_EXT_GXB )
		{
			fcuiUtils::ReloadObject(reload_obj,files[i]);
			chtrObject* pCSO = dynamic_cast<chtrObject*>(sel3dMgr::GetSelected());
			itString object_name;
			if (pCSO != NULL)
			{
				object_name = itString(pCSO->GetDisplayName().c_str());
		
				lsetOperations::AddObjectToLightSet(nameString("DancerLightSet"), nameString(object_name));

				itString new_item_name = files[i].GetLastName();
				new_item_name.StripExtension();  //will now have "subject_category_sub-category" convention
				fcuiUtils::UpdateAnimationDrivers(nameString(object_name), new_item_name);

			}
			//fcuiUtils::LoadObject(files[i]);
		}
		else if ( extension == fcuiConstants::l_EXT_DDS || extension == fcuiConstants::l_EXT_JPG) 
		{
			fcuiUtils::LoadTexture(m_CurrentCategory, files[i]);
		}
		else if ( extension == fcuiConstants::l_EXT_MTL)
		{
			// TODO - GetExtension will return the wrong string if there is another period in the name.
			//	There should be better itString functions to handle stripping off the front of the string.
			//
			itString object_name;
			m_CurrentSubject.GetExtension(object_name);
			itString material_name;
			element_file.GetBase(material_name);
			try
			{
				fcuiUtils::LoadMaterial(m_CurrentSubject, material_name, files[i]);
			}
			catch (fsFileDoesntExistX& i_Ex)
			{
				DBG_ERROR("Cannot find file " << files[i]);
			}
		}
		else if (extension == fcuiConstants::l_EXT_GMB)
		{
			itString object_name = reload_obj;
			fcuiUtils::LoadMaterials(object_name, files[i]);
		}
	}
}
