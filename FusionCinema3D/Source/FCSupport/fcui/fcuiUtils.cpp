/*****************************************************************************
**	fcuiUtils.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/fcuiUtils.hpp"

#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcdc/fcdcDataMgr.hpp"
#include "FCSupport/fcdc/data/fcdcTimelineItemData.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/path/pathDirectoryParser.hpp"

#include "MainApp/mnmApp.hpp"

#include "Drivers/Animation/tmlnDriverAnimationFull.hpp"
#include "Drivers/Animation/tmlnDriverAnimationFullInfo.hpp"
#include "Drivers/FilePath/tmlnDriverFilePath.hpp"
#include "Drivers/FilePath/tmlnDriverFilePathInfo.hpp"
#include "Drivers/Attach/tmlnDriverAttach.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Drivers/Video/tmlnDriverVideo.hpp"
#include "Drivers/Video/tmlnDriverVideoInfo.hpp"
#include "Features/Capture/cptrPackage.hpp"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/ObjectManip/mnpPackage.hpp"
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrRenderStateUtil.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Systems/Billboard/Object/billBillboardObject.hpp"
#include "Systems/Billboard/Object/billObjectMgr.hpp"
#include "Systems/Cameras/Drivers/cmraDriverKeyPositionAndTargetInfo.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"
#include "Systems/Character/Data/chtrScriptData.hpp"
#include "Systems/Character/Object/chtrScriptObject.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlSurfaceShader.hpp"
//#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/pfx/pfxPostEffectObject.hpp"
#include "Support/pfx/pfxPostEffectMgr.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
//#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnCreator.hpp"
//#include "Support/tmln/tmlnScriptObject.hpp"

//#include "Systems/Character/Object/chtrObject.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Character/GUI/chtrDialogInterest.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"

#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g2d/g2dImageCreate.hpp"
#include "Graphics/g2d/g2dImageSave.hpp"
#include "Graphics/g2d/g2dScreenCaptureUtil.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
//#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxShaderParams.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

#include <QtGui/qfiledialog.h>
#include <QtGui/QWidget>



//============================================================================
//============================================================================
namespace
{
	itString l_SystemName("Objects");
	fsLocator l_MovieName;
	fsLocator l_MovieTitle(itString("My Movie"));
	itString l_Objectname;
	fsLocator l_countdownmoviePath;
	bool l_AppendMovie = false;
}


//------------------------------------------------------------------------
///	Exit the application
//------------------------------------------------------------------------
void fcuiUtils::ExitApplication()
{
	appApplicationPAC::Instance()->Exit();
}

//------------------------------------------------------------------------
///	Ask the user to save the project (.mab)
//------------------------------------------------------------------------
void fcuiUtils::UserSaveProject(QWidget* i_pParent)
{
	//QFileDialog::Options options;
	//options |= QFileDialog::DontUseNativeDialog;
#ifdef WRITECHUNK
	fcuiFormMgr::WriteTimelineItemsToChunk(); // Write timeline items into chunk
#endif
	QString caption("Save Project");
	QString selectedFilter("Projects (*.mab)");
	fsLocator fs_file;
	fs_file = gfPaths::GetPath(mnmPaths::e_UserProjects);
	if (!fsFileUtil::DirectoryExists(fs_file))
	{
		fsFileUtil::CreateDirectory(fs_file);
	}
	itString file_name = docSingleDocumentMgr::GetFilename().GetLastName();
	file_name = "Scene.mab";
	fs_file.Push(GetMovieTitle());
	DBG_TRACE("Default directory + file = " << fs_file);
	itString str_file;
	fsFileUtil::LocatorToUnicodeString(fs_file, str_file);
	DBG_TRACE("Also Default = " << str_file);
	QString qs_file( (const QChar *)(str_file.GetString()), str_file.GetLength() );
	QString fileName = QFileDialog::getSaveFileName(i_pParent,
													caption,
													qs_file,
													selectedFilter
													);
	if (!fileName.isEmpty())
	{
		itString itstr;
		itstr = ((const itString::CharType*)fileName.data());
		fsFileUtil::UnicodeStringToLocator(itstr, fs_file);
		DBG_TRACE("Saving ... " << fs_file);

		docSingleDocumentMgr::SetFilename( fs_file );
		guiSingleDocHandler::Save();
		
		//guiSingleDocHandler::SaveAs();
	}
}
//------------------------------------------------------------------------
///	Ask the user to load the project (.mab)
//------------------------------------------------------------------------
void fcuiUtils::LaunchMovie(fsLocator& i_projectpath)
{
	DBG_TRACE("Opening ... " << i_projectpath);

	docSingleDocumentMgr::LoadDocument( i_projectpath );
}

//------------------------------------------------------------------------
///	Ask the user to load the project (.mab)
//------------------------------------------------------------------------
void fcuiUtils::UserLoadProject()
{
	pathDirectoryParser path;
	fsLocator file_dir;
	file_dir = gfPaths::GetPath( mnmPaths::e_AppGUI );
	file_dir.Push(fcuiConstants::c_MODES);

	std::vector<fsLocator> files;
	path.GetSubDirectories(file_dir, files);
	fcuiFormMgr::UpdatePanelIcons(fcmdModeMgr::e_Mode, files);
	fcuiFormMgr::HighlightPanelItemAtIndex(fcmdModeMgr::e_Mode, 0, false); //highlight cast by default
	//fcuiFormMgr::ClearTimeline();

	//	go to the cast screen
	api3dSubdiv::SetSubdivLevel(1);
	std::string camname = fcuiConstants::c_CAMERA_CAST;
	nameString camName(camname);
	int index = camsCameraMgr::GetIndexForName(camName);
	if (index >= 0 && index < camsCameraMgr::GetNumCameras())
	{
		rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
		camsCameraMgr::SelectCamera(index);
	}

	//	after loading the scene make sure all the projected lights icons are hidden
	ProjectedLight_HideIcons();
}


//------------------------------------------------------------------------
///	make (or "capture") the project to a movie file
//------------------------------------------------------------------------
void fcuiUtils::MakeProject()
{

	ExtendDirectorDriver(fcuiTimelineMgr::Instance->GetEndTime());

	fsLocator make_movie = docSingleDocumentMgr::GetFilename();
	//fsFileUtil::ANSIFilenameToLocator( std::string("C:\\Projects\\Test-Data\\BallAndSphere\\BallAndSphere.mab"), make_movie );

	try
	{
		bool c_CLEAR_MODE_STACK = false;
		cptrPackage::LaunchRender();
	}
	catch(...)
	{
		DBG_ERROR("File is in use");
	}
	captRenderOutputData& capData = captRenderOutputDataUtil::Data();

	capData.m_bCaptureAllCameras.SetValue(false);
	//capData.m_bUseCameraNameInFilename.SetValue(false);
	capData.m_bUseSceneFilenameInFilename.SetValue(false);
	capData.m_bUseRenderPassInFilename.SetValue(false);
	capData.m_bUseCompressionInFilename.SetValue(false);
	capData.m_bOutputFileCustomize.SetValue(true);
	
	itString itFilename;

	for(int i =0; i < capData.m_CameraList.size() ; i++)
	{
		if (capData.m_CameraList[i].m_CameraName == "Director")

			capData.m_CameraList[i].m_bCapture = true;
		else
			capData.m_CameraList[i].m_bCapture = false;
	}
	capData.m_OutputDirectoryRoot.SetValue( gfPaths::GetPath( mnmPaths::e_UserMovies ) );
	captRenderOutputDataUtil::UpdateData( capData );

	DBG_TRACE("Making movie from scene: " << make_movie);
	DBG_TRACE("Output Dir: " << capData.m_OutputDirectoryRoot.GetValue());
	fsLocator i_MovieName =  capData.m_OutputDirectoryRoot.GetValue();

	fsLocator movie_name;
	itString itstr;
	std::string str_movietitle; 
	fsFileUtil::LocatorToANSIFilename(GetMovieTitle(), str_movietitle);
	if (str_movietitle == "")
		str_movietitle = "CustomMovie.avi";
	else
		str_movietitle +=".avi";

	i_MovieName.Push(str_movietitle.c_str());
	SetMovieName(i_MovieName);
	capData.m_OutputFileName.SetValue(i_MovieName.GetLastName());
	fsFileUtil::LocatorToUnicodeString(i_MovieName, itFilename);
	int num_renderlayers = rlyrRenderLayerMgr::GetNumRenderLayers();
	pfxPostEffectObject* obj = dynamic_cast<pfxPostEffectObject*>(pfxPostEffectMgr::GetDataObject(pfxPostEffectMgr::e_ViewportPfx));
	pfxData& data= obj->GetData();

	for (size_t i = 0; i < num_renderlayers ; i++)
	{
		nameString renderlayer_name = rlyrRenderLayerMgr::GetLayerName(i);
		rlyrRenderLayerMgr::SetLayerPfxData(renderlayer_name,data);
	}
}

//------------------------------------------------------------------------
/// Abort a render
//------------------------------------------------------------------------
void fcuiUtils::AbortMake()
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(pMode);
	if ( pModeRender != 0 )
	{
		pModeRender->AbortRender();
		pModeRender->PauseRender( false );
		mnmApp::EnableRender(true);
	}
}

//------------------------------------------------------------------------
/// given a path to a gxb file, load that file to the scene
//------------------------------------------------------------------------
void fcuiUtils::LoadObject(const fsLocator& i_Path)
{
	fsLocator full_path;
	full_path.Push(l_SystemName);
	full_path.Push(i_Path);
	cmmDialogInterestMgr::AddObject(full_path.GetLastName(), full_path);
}

//------------------------------------------------------------------------
/// load a video to a billboard
//------------------------------------------------------------------------
void fcuiUtils::LoadVideo(const itString& i_CurrentCategoryName, const fsLocator& i_VideoFilePath)
{
	cmmDialogDataList data_list;
	cmmDialogInterestMgr::GetPlacedObjects(data_list);
	tmlnDriverVideo* pDriverVideo;
	std::string video_path;
	fsLocator empty_filename = i_VideoFilePath;
	empty_filename.Clear();
	tmlnDriver* pDriver = NULL;
	fsFileUtil::LocatorToANSIFilename(i_VideoFilePath,video_path);

	cmmDialogDataList::iterator it;
	for (it = data_list.begin(); it != data_list.end(); ++it)
	{
		cmmDialogData &data = (*it);
		if (data.m_Name == nameString(itStringUtil::GetStdString(i_CurrentCategoryName)))
		{
			// You have the name of the billboard
			std::string video_driver("Video");
			tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(data.m_Name);
			if (!pScriptObj)
			{
				DBG_ERROR("Script object with name, " << data.m_Name.GetString() << " does not exist.");
			}

			// Create Driver only if there isn't a video driver already there
			//
			pDriverVideo = NULL;
			int num_drivers = pScriptObj->GetNumDrivers();
			for (int dnum = 0; dnum < num_drivers; ++dnum)
			{
				pDriver = pScriptObj->GetDriverDirect(dnum);
				if ((pDriverVideo = dynamic_cast<tmlnDriverVideo*>(pDriver)) != NULL)
				{
					continue;
				}
			}

			if (pDriverVideo == NULL)
			{
				pDriver = tmlnCreator::CreateDriverByName( video_driver.c_str(), pScriptObj );
				pScriptObj->AddDriver( pDriver );
				pDriverVideo = dynamic_cast<tmlnDriverVideo*>(pDriver);
			}

			if (pDriver != NULL)
			{
				pDriver->SetBeginTime(maTime::c_ZeroTime);
				pScriptObj->NotifyDriverChanged();

				//update the channel dialog
				chnlDialogUtil::ObjectSelected(pScriptObj);
				billBillboardObject *new_obj = billObjectMgr::GetObject(billObjectMgr::GetIndexForObject(data.m_Name))->GetPickObject();
				if (pDriverVideo != NULL)
				{
					tmlnDriverVideoInfo *pVideoInfo = dynamic_cast<tmlnDriverVideoInfo*>(pDriverVideo->GetDriverInfo());
					if (pVideoInfo != NULL)
					{
						if (i_VideoFilePath.GetLastName() != itString(fcuiConstants::c_FILE_NONE_VIDEO))
						{
							billData bData = new_obj->GetData();
							bData.m_bVisible = true;
							new_obj->SetData(bData);
							pVideoInfo->m_FileName = i_VideoFilePath;
						}
						else
						{
							billData bData = new_obj->GetData();
							bData.m_bVisible = false;
							new_obj->SetData(bData);
							
							pVideoInfo->m_FileName = empty_filename ;
						}
						pDriverVideo->SetDriverInfo(*pVideoInfo);
					}
				}
			}
			else
			{
				DBG_ERROR("Could not create billboard video driver.");
			}
		}
	}
}

//------------------------------------------------------------------------
/// load a texture to an object
//------------------------------------------------------------------------
void fcuiUtils::LoadTexture(const itString& i_CurrentCategoryName, const fsLocator& i_TextureFilePath)
{
	fcuiUtils::LoadTexture( i_CurrentCategoryName, i_TextureFilePath, fsLocator() );
}

void fcuiUtils::LoadTexture(const itString& i_CurrentCategoryName, const fsLocator& i_TextureFilePath, const fsLocator& i_Thumbnail)
{
	gpxRenderControl::ConfirmSingleThread();
	cmmDialogDataList data_list;
	cmmDialogInterestMgr::GetPlacedObjects(data_list);
	cmmDialogDataList::iterator it;
	prtyTextureFileData TextureFileName(i_TextureFilePath);
	
	fsLocator empty_filename = i_TextureFilePath; 
	empty_filename.Clear();
	prtyTextureFileData EmptyTextureFileName(empty_filename);
	for (it = data_list.begin(); it != data_list.end(); ++it)
	{
		cmmDialogData &data = (*it);

		//	match the category
		if (data.m_Name == nameString(itStringUtil::GetStdString(i_CurrentCategoryName)))
		{
			mtrlScriptObject *pMaterialObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(data.m_pPickObject);
			if (pMaterialObj != NULL)
			{
				int index = pMaterialObj->GetIndexForName(fcuiConstants::c_IMAGE_USER_MATERIAL);
				if (index >=0)
				{
					mtrlSurfaceShader* pSurfaceShader = dynamic_cast<mtrlSurfaceShader*>(pMaterialObj->GetShaderDataObject(index));
					if (pSurfaceShader != NULL)
					{
						if ((pSurfaceShader->GetShaderData())->GetUIShaderParams().m_pDiffuseMap != NULL)
						{
							if (i_TextureFilePath.GetLastName() != itString(fcuiConstants::c_FILE_NONE_IMAGE))
							{
								(pSurfaceShader->GetShaderData())->GetUIShaderParams().m_pDiffuseMap->Property().SetValue(TextureFileName);

								if (i_Thumbnail.GetNumNames() > 0)
								{
									//	generate a thumbnail
									fcuiUtils::CreateUserThumbnail( (pSurfaceShader->GetShaderData())->GetUIShaderParams().m_pDiffuseMap->GetTexture(), i_Thumbnail);
								}
							}
							else
							{
								(pSurfaceShader->GetShaderData())->GetUIShaderParams().m_pDiffuseMap->Property().SetValue(EmptyTextureFileName);
							}
							gpxRenderControl::SetNeedsNewRender();
						}
						else
						{
							DBG_ERROR("No diffuse map on this object");
						}
					}
					else
					{
						DBG_ERROR( "Cannot find surface shader UserContent at index " << index << ".  " << i_CurrentCategoryName << " " << i_TextureFilePath);
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------
/// load a gmb to an object
//------------------------------------------------------------------------
void fcuiUtils::LoadMaterials(const itString& i_ObjectName, const fsLocator& i_MaterialFilepath)
{
	gpxRenderControl::ConfirmSingleThread();

	cmmDialogDataList data_list;
	cmmDialogInterestMgr::GetPlacedObjects(data_list);
	prtyTextureFileData TextureFileName(i_MaterialFilepath);
	
	mdlMaterialInfo mat_info;
	mtrMaterialSaver::ReadSingleMaterial(i_MaterialFilepath, mat_info);
	nameString ObjectName(i_ObjectName);

	bool bFound = false;
	cmmDialogDataList::iterator it;
	for (it = data_list.begin(); it != data_list.end(); ++it)
	{
		cmmDialogData &data = (*it);
		if (data.m_Name == ObjectName || data.m_Filename == i_ObjectName)
		{
			bFound = true;
			mtrlScriptObject *pMaterialObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(data.m_pPickObject);
			if (pMaterialObj != NULL)
			{
				pMaterialObj->ImportMaterials(i_MaterialFilepath);
			}
			else
			{
				DBG_ERROR("Found object " << ObjectName << " but it isn't a material object");
			}
		}
	}

	//	if it wasn't found by object name, search all objects and try to set the material
	//
	if (!bFound)
	{
		itString object_name;
		i_MaterialFilepath.GetLastName().GetBase(object_name);

		for (it = data_list.begin(); it != data_list.end(); ++it)
		{
			cmmDialogData &data = (*it);
			if (data.m_Name == object_name)
			{
				mtrlScriptObject *pMaterialObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(data.m_pPickObject);
				if (pMaterialObj != NULL)
				{
					pMaterialObj->ImportMaterials(i_MaterialFilepath);
				}
				else
				{
					DBG_ERROR("Found second object " << ObjectName << " but it isn't a material object");
				}
			}
		}
	}
}


//------------------------------------------------------------------------
/// load a material to an object
//------------------------------------------------------------------------
void fcuiUtils::LoadMaterial(const itString& i_ObjectName, const itString& i_MaterialName, const fsLocator& i_MaterialFilePath)
{
	gpxRenderControl::ConfirmSingleThread();

	//	get all the objects in the scene
	cmmDialogDataList data_list;
	cmmDialogInterestMgr::GetPlacedObjects(data_list);

	nameString object_name(itStringUtil::GetStdString(i_ObjectName));
	//nameString current_subject(itStringUtil::GetStdString(i_CurrentSubjectName));

	//	go through the objects looking for a match
	//
	bool bFound = false;
	int index = -1;
	cmmDialogDataList::iterator it;
	for (it = data_list.begin(); it != data_list.end(); ++it)
	{
		cmmDialogData &data = (*it);
		//DBG_TRACE(" loadmaterial: " << data.m_Name << " vs " << object_name);

		if (data.m_Name == object_name)
		{
			bFound = true;
			mtrlScriptObject *pMaterialObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(data.m_pPickObject);

			index = pMaterialObj->GetIndexForName(itStringUtil::GetStdString(i_MaterialName));
			if (index >=0)
			{
				//	read in the new material
				mdlMaterialInfo mat_info;
				mtrMaterialSaver::ReadSingleMaterial(i_MaterialFilePath, mat_info);

				pMaterialObj->ChangeMaterialData(index, mat_info, true);
			}
		}
	}

	//	if it wasn't found by object name, search all objects and try to set the material
	//
	if (!bFound)
	{
		//itString object_name;
		//i_MaterialFilePath.GetLastName().GetBase(object_name);

		for (it = data_list.begin(); it != data_list.end(); ++it)
		{
			cmmDialogData &data = (*it);
			mtrlScriptObject *pMaterialObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(data.m_pPickObject);

			if (pMaterialObj != NULL)
			{
				index = pMaterialObj->GetIndexForName(itStringUtil::GetStdString(i_MaterialName));
				if (index >=0)
				{
					//	read in the new material
					mdlMaterialInfo mat_info;
					mtrMaterialSaver::ReadSingleMaterial(i_MaterialFilePath, mat_info);

					pMaterialObj->ChangeMaterialData(index, mat_info, true);
					break; // or change all?
				}
			}
		}
	}
}


//------------------------------------------------------------------------
/// reload object based on the category name
//------------------------------------------------------------------------
void fcuiUtils::ReloadObject(const itString& i_CurrentCategoryName, fsLocator& i_Path)
{
	cmmDialogDataList data_list;
	cmmDialogInterestMgr::GetPlacedObjects(data_list);
	fsLocator full_path;
	full_path.Push(l_SystemName);
	full_path.Push(i_Path);
	cmmDialogDataList::iterator it;
	chtrObject* temp;
	for (it = data_list.begin(); it != data_list.end(); ++it)
	{
		cmmDialogData &data = (*it);
		if (data.m_Name == i_CurrentCategoryName)
		{
		
			cmmDialogInterestMgr::SelectObject(data.m_Name, 
										itStringUtil::GetStdString(l_SystemName), 
										false);
			
			temp = sel3dCastUtil::CastSelectedObject<chtrObject>();
			
			if (temp)
			{
				chtrData new_data = temp->GetData();
				new_data.m_Filename.SetValue( i_Path);
				temp->SetData(new_data);
			}
			data.m_Desc = itStringUtil::GetStdString(i_Path.GetLastName());
			cmmDialogInterestMgr::ReloadObject(data.m_Name,itStringUtil::GetStdString(l_SystemName));
	
		}
	}
}

//----------------------------------------------------------------------------
/// Get script object by name
//----------------------------------------------------------------------------
tmlnScriptObject* fcuiUtils::GetTimelineScriptObject(const nameString& i_ObjectName)
{
	nameObject *pNameObj = nameMgr::GetObjectByName(i_ObjectName);
	if (!pNameObj)
		return NULL;

	tmlnScriptObject* script_obj = dynamic_cast<tmlnScriptObject*>(pNameObj);
	sel3dObject* pPickObject = dynamic_cast<sel3dObject*>(pNameObj);
	if (pPickObject && !script_obj)
	{
		// If this is not a scripted object, it may be associated 
		// with a script object through the "parent object" relationship
		relObject* cur_obj = pPickObject;
		while (!script_obj)
		{
			cur_obj = cur_obj->GetParentObject();
			if (!cur_obj) 
				break;
			script_obj = dynamic_cast<tmlnScriptObject*>(cur_obj);
		}
	}

	return script_obj;
}

//----------------------------------------------------------------------------
/// create a capture driver for the given camera for the specified time
//----------------------------------------------------------------------------
void fcuiUtils::CreateCameraCaptureDriver( const nameString& i_CameraName,
										   const maTime& i_StartTime, const maTime& i_EndTime )
{
	const char* driverName = "Capture";
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_CameraName);
	if (!pScriptObj)
	{
		DBG_ERROR("Script object with name, " << i_CameraName.GetString() << " does not exist.");
		return;
	}

	// get the drivers and find if the driver exists
	//
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	//	Find the index within this driver list
	for (int i=0; i< driver_names.m_DriverNames.size(); i++)
	{
		// Create Driver
		tmlnDriver* pDriver = tmlnCreator::CreateDriverByName( 
									driverName, pScriptObj );

		// Give driver to script object to own
		if (pDriver)
		{	
			//set the start time, get the end time
			pDriver->SetBeginTime(i_StartTime);
			pDriver->SetEndTime(i_EndTime);

			pScriptObj->AddDriver( pDriver );
			pScriptObj->NotifyDriverChanged();
			
			//update the channel dialog
			chnlDialogUtil::ObjectSelected(pScriptObj);
			break;
		}
		else
		{
			DBG_ERROR("Could not create animation driver.");
			break;
		}
	}
}

//----------------------------------------------------------------------------
/// Extend the capture drivers for the director cam to the new end time
//----------------------------------------------------------------------------
void fcuiUtils::ExtendDirectorDriver(const maTime& i_EndTime)
{
	nameString directorCam(fcuiConstants::c_CAMERA_DIRECTOR);
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(directorCam);
	if (!pScriptObj)
	{
		DBG_ERROR("Script object with name, " << directorCam.GetString() << " does not exist.");
		return;
	}

	// get the drivers and find if the driver exists
	//
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	int num_names = pScriptObj->GetNumDrivers();
	tmlnDriver* curDriver = NULL;
	itString dName;
	itString directorDriver("Director");

	// find the correct driver to manipulate
	for (int i=0; i<num_names; ++i)
	{
		curDriver = pScriptObj->GetDriverDirect(i);
		dName = itString(curDriver->GetName().c_str());
		dName.StripExtension();
		//if the name and old start time match, we should change the driver's start time
		if ( dName == directorDriver )
		{
			curDriver->SetEndTime( i_EndTime );
			break;
		}
	}
}

//----------------------------------------------------------------------------
/// create a driver for the given Object for the specified time
//----------------------------------------------------------------------------
tmlnDriver* fcuiUtils::CreateDriver( const std::string& i_driverName,  
							  const nameString& i_ObjectName,
							  const maTime& i_StartTime, const maTime& i_EndTime )
{
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
	{
		DBG_ERROR("Script object with name, " << i_ObjectName.GetString() << " does not exist.");
		return NULL;
	}

	// get the drivers and find if the driver exists
	//
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return NULL;
	}

	//	Find the index within this driver list
	for (int i=0; i< driver_names.m_DriverNames.size(); i++)
	{
		// Create Driver
		tmlnDriver* pDriver = tmlnCreator::CreateDriverByName( 
							  i_driverName.c_str(), pScriptObj );

		// Give driver to script object to own
		if (pDriver)
		{	
			//set the start time, get the end time
			pDriver->SetBeginTime(i_StartTime);
			pDriver->SetEndTime(i_EndTime);

			pScriptObj->AddDriver( pDriver );
			pScriptObj->NotifyDriverChanged();
			
			//update the channel dialog
			chnlDialogUtil::ObjectSelected(pScriptObj);
			return pDriver;
			
		}
		else
		{
			DBG_ERROR("Could not create animation driver.");
			return NULL;
		}
	}

	return NULL;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
const maTime fcuiUtils::SetObjectAnims( const itString& i_AnimationName, 
										const fsLocator& i_CurrentAnimFile, 
										const maTime& i_StartTime )
{
	maTime endTime = maTime::FromSeconds(-1.0f);
	
	//itString anim_directory(L"Animation");

	//get all objects in the scene
	cmmDialogDataList data_list;
	cmmDialogInterestMgr::GetPlacedObjects(data_list);
	cmmDialogDataList::iterator it;

	fsLocator cur_anim;
	itString item_name;
	//std::vector<fsLocator> anim_files;
	fsLocator anim_file = i_CurrentAnimFile;
	pathDirectoryParser parser;
	
	std::string str_animname;
	fsFileUtil::LocatorToANSIFilename(i_AnimationName, str_animname);

	//	verify the file is of the format "subject_category_subcategory"
	int count = 0, k = 0;
	while (str_animname[k] != '\0')
	{
		if (str_animname[k] == '_')
			count++;
		k++;
	}
	if (count < 2)
	{
		DBG_ERROR("Need subject, category and subcategory in animation name: " << i_AnimationName );
		return maTime::c_ZeroTime;
	}

	std::string subject_name, category_name, subcategory_name;
	subject_name = strtok((char*)str_animname.c_str(),"_");
	category_name = strtok(NULL, "_");
	subcategory_name = strtok(NULL, "_"); 
	std::string object_name = subject_name + "_" + category_name + "_" + subcategory_name + ".gxb";
	
	for (it = data_list.begin(); it != data_list.end(); ++it)
	{
		cmmDialogData &data = (*it);
		if (data.m_Filename == itString(object_name.c_str())) 
		{
			cur_anim = i_CurrentAnimFile;

			if (!fsFileUtil::FileExists(cur_anim) )
			{
				DBG_ERROR( "No animation folder found for object " << data.m_Name.GetString() );
				break;
			}

			fcuiUtils::CreateObjectAnimationDriver(data.m_Name, anim_file, i_StartTime, endTime);
		}
	}
	
	return endTime;
}


//----------------------------------------------------------------------------
/// Return the number of drivers for the object
//----------------------------------------------------------------------------
int fcuiUtils::GetNumDrivers( const nameString& i_ObjectName )
{
	tmlnScriptObject* pScriptObj = GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return -1;

	// get the drivers and find if the driver exists
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return -1;
	}

	return pScriptObj->GetNumDrivers();
}

//----------------------------------------------------------------------------
/// Get a unique texture driver name for the title card
//----------------------------------------------------------------------------
void fcuiUtils::GetUniqueDriverName( const nameString& i_ObjectName, 
								     const itString& i_ProposedName,
								     itString& o_UniqueName)
{

	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;

	// get the drivers and find if the driver exists
	//
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	int nameCount = 0;
	itString dName;
	tmlnDriver* curDriver = NULL;
	int num_names = pScriptObj->GetNumDrivers();
	//	Find the index within this driver list
	for (int i=0; i< num_names; i++)
	{
		curDriver = pScriptObj->GetDriverDirect(i);
		dName = itString(curDriver->GetName().c_str());
		if ( dName.HasSubString(i_ProposedName) )
			nameCount++;
	}

	// Create the new unique name for the texture
	std::string newName = itStringUtil::GetStdString(i_ProposedName);
	std::stringstream o_ss;
	o_ss << newName << nameCount + 1;
	newName = o_ss.str();
	o_UniqueName = itString(newName.c_str());
}

//----------------------------------------------------------------------------
/// Create the texture driver and set its property values
//----------------------------------------------------------------------------
void fcuiUtils::CreateTitlecardTextureDriver( const nameString& i_ObjectName, 
											  const fsLocator& i_TextureFile, 
										      const maTime& i_StartTime, const maTime& i_EndTime,
											  itString& o_TextureName)
{
	const char* driverName = "Texture";
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;

	// get the drivers and find if the driver exists
	//
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	//	Find the index within this driver list
	for (int i=0; i< driver_names.m_DriverNames.size(); i++)
	{
		// Create Driver
		tmlnDriver* pDriver = tmlnCreator::CreateDriverByName( 
									driverName, pScriptObj );
		tmlnDriverFilePath* pTexDriver;
		if (pDriver)
			pTexDriver = dynamic_cast<tmlnDriverFilePath*>(pDriver);

		// Give driver to script object to own
		if (pTexDriver)
		{
			tmlnDriverFilePathInfo* texInfo;
			
			//set the start time, get the end time
			pTexDriver->SetBeginTime(i_StartTime);
			pTexDriver->SetEndTime(i_EndTime);
			//now get the animation driver
			texInfo = dynamic_cast<tmlnDriverFilePathInfo*>(pTexDriver->GetDriverInfo());
			
			itString unique_name;
			itString driver_name = i_TextureFile.GetLastName();
			driver_name.StripExtension();
			GetUniqueDriverName(i_ObjectName, driver_name, unique_name);
			std::string s_name = itStringUtil::GetStdString(unique_name);

			pTexDriver->SetName(s_name.c_str());
			o_TextureName = unique_name;

			//set file name and driver name
			if (texInfo)
			{
				texInfo->m_Value = i_TextureFile;
				texInfo->m_Name = s_name;	
			}
			
			pTexDriver->SetDriverInfo(*texInfo);
			pScriptObj->AddDriver( pTexDriver );
			break;
		}
		else
		{
			DBG_ERROR("Could not create texture driver.");
			break;
		}
	}
}

///---------------------------------------------------------------------------
/// Assign a new texture file to the title card driver
///---------------------------------------------------------------------------
void fcuiUtils::ReloadTitlecardTextureValue( const nameString& i_ObjectName,
											 const itString& i_DriverName,
											 const fsLocator& i_NewTextureFile )
{
	const char* driverName = "Texture";
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;

	// get the drivers and find if the driver exists
	//
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	itString dName;
	tmlnDriver* curDriver = NULL;
	int num_names = pScriptObj->GetNumDrivers();
	for (int i=0; i< num_names; i++)
	{
		curDriver = pScriptObj->GetDriverDirect(i);
		dName = itString(curDriver->GetName().c_str());
		dName.StripExtension();
		if ( dName == i_DriverName )
		{
			tmlnDriverFilePath* pTexDriver;
			pTexDriver = dynamic_cast<tmlnDriverFilePath*>(curDriver);

			// Give driver to script object to own
			if (pTexDriver)
			{
				tmlnDriverFilePathInfo* texInfo;
				//now get the animation driver and set the new texture file
				texInfo = dynamic_cast<tmlnDriverFilePathInfo*>(pTexDriver->GetDriverInfo());
				if ( texInfo != NULL )
				{
					texInfo->m_Value = i_NewTextureFile;
					pTexDriver->SetDriverInfo(*texInfo);
				}
			}
			break;
		}
	}
}

///---------------------------------------------------------------------------
/// check if the billboard is missing the texture
///---------------------------------------------------------------------------
bool fcuiUtils::CheckforMissingFile( const nameString& i_ObjectName,
									 const itString& i_DriverName)
{
	const char* driverName = "Texture";
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return true;

	// get the drivers and find if the driver exists
	//
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return true;
	}

	itString dName;
	tmlnDriver* curDriver = NULL;
	int num_names = pScriptObj->GetNumDrivers();
	for (int i=0; i< num_names; i++)
	{
		curDriver = pScriptObj->GetDriverDirect(i);
		dName = itString(curDriver->GetName().c_str());
		dName.StripExtension();
		if ( dName == i_DriverName )
		{
			tmlnDriverFilePath* pTexDriver;
			pTexDriver = dynamic_cast<tmlnDriverFilePath*>(curDriver);

			// Give driver to script object to own
			if (pTexDriver)
			{
				tmlnDriverFilePathInfo* texInfo;
				texInfo = dynamic_cast<tmlnDriverFilePathInfo*>(pTexDriver->GetDriverInfo());
				if ( texInfo != NULL )
				{
					if(!fsFileUtil::FileExists(texInfo->m_Value))
						return true;
					else 
						return false;
					
				}
			}
		}
	}
	return true;
}

//----------------------------------------------------------------------------
/// Create the animation driver and set its property values
//----------------------------------------------------------------------------
void fcuiUtils::CreateObjectAnimationDriver( const nameString& i_ObjectName, 
											 const fsLocator& i_AnimationFile, 
										     const maTime& i_StartTime, maTime& o_EndTime, 
										  	 bool i_bLooping)
{
	const char* driverName = "Animation";
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;

	// get the drivers and find if the driver exists
	//
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	//	Find the index within this driver list
	for (int i=0; i< driver_names.m_DriverNames.size(); i++)
	{
		// Create Driver
		tmlnDriver* pDriver = tmlnCreator::CreateDriverByName( 
									driverName, pScriptObj );
		tmlnDriverAnimationFull* pAnimDriver;
		if (pDriver)
			pAnimDriver = dynamic_cast<tmlnDriverAnimationFull*>(pDriver);

		// Give driver to script object to own
		if (pAnimDriver)
		{
			tmlnDriverAnimationFullInfo* animInfo;
			
			//set the start time, get the end time
			pAnimDriver->SetBeginTime(i_StartTime);

			//now get the animation driver
			animInfo = dynamic_cast<tmlnDriverAnimationFullInfo*>(pAnimDriver->GetDriverInfo());
			
			itString driver_name = i_AnimationFile.GetLastName();
			driver_name.StripExtension();
			std::string s_name = itStringUtil::GetStdString(driver_name);
			pAnimDriver->SetName(s_name.c_str());

			//set file name and driver name
			if (animInfo)
			{
				animInfo->m_Info.m_AnimFilename = i_AnimationFile;
				animInfo->m_Info.m_bLooping = i_bLooping;
				animInfo->m_Name = s_name;
			}
			
			pAnimDriver->SetDriverInfo(*animInfo);
			pScriptObj->AddDriver( pAnimDriver );

			//get the end time of the animation
			o_EndTime = pAnimDriver->GetEndTime();
			break;
		}
		else
		{
			DBG_ERROR("Could not create animation driver.");
			break;
		}
	}
}

//----------------------------------------------------------------------------
/// When a cast member's item is changed, we need to change the animation 
/// drivers to point to the proper item animation files
//----------------------------------------------------------------------------
void fcuiUtils::UpdateAnimationDrivers( const nameString& i_ObjectName,
										const itString& i_NewObjectName )
{
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;

	// get the drivers and find if the driver exists
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	int num_names = pScriptObj->GetNumDrivers();
	tmlnDriver* curDriver = NULL;
	tmlnDriverAnimationFull* pAnimDriver = NULL;
	tmlnDriverAnimationFullInfo* animInfo = NULL;
	fsLocator animFile;
	itString dName;
	// find the correct driver to manipulate
	for (int i=0; i<num_names; ++i)
	{
		dName.Clear();
		animFile.Clear();
		std::string driver_name;
		itString subject_name, category_name, subcategory_name, anim_name, full_file;
		itString full_name;

		curDriver = pScriptObj->GetDriverDirect(i);
		pAnimDriver = dynamic_cast<tmlnDriverAnimationFull*>(curDriver);
		if ( pAnimDriver != NULL )
		{
			dName = itString(pAnimDriver->GetName().c_str());
			dName.StripExtension();
			//now get the animation driver
			animInfo = dynamic_cast<tmlnDriverAnimationFullInfo*>(pAnimDriver->GetDriverInfo());
			if ( animInfo != NULL )
			{
				driver_name = itStringUtil::GetStdString(dName);
				subject_name = itString(strtok((char*)driver_name.c_str(), "_"));
				category_name = itString(strtok(NULL, "_"));
				subcategory_name = itString(strtok(NULL, "_"));
				anim_name = itString(strtok(NULL, "_"));

				full_name = subject_name + itString(L"_") + category_name + itString(L"_") + subcategory_name;
				if ( i_NewObjectName == full_name )
					continue;

				animFile = animInfo->m_Info.m_AnimFilename;
				animFile.Pop();
				full_name = i_NewObjectName;
				full_name += itString(L"_") + anim_name;
				full_file = full_name + itString(L".") + fcuiConstants::l_EXT_GAB;
				animFile.Push(full_file);
				if (fsFileUtil::FileExists(animFile))
				{
					animInfo->m_Name = itStringUtil::GetStdString(full_name);
					animInfo->m_Info.m_AnimFilename = animFile;
					pAnimDriver->SetName(animInfo->m_Name.c_str());
					pAnimDriver->SetDriverInfo(*animInfo);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
/// Shift a driver from its old start time to its new start time
//----------------------------------------------------------------------------
void fcuiUtils::ShiftDriverToTime( const nameString& i_ObjectName,
								   const std::string& i_DriverName,
								   const maTime& i_OldStartTime, 
								   const maTime& i_NewStartTime)
{
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;

	// get the drivers and find if the driver exists
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	int num_names = pScriptObj->GetNumDrivers();
	tmlnDriver* curDriver = NULL;
	itString dName;
	// find the correct driver to manipulate
	for (int i=0; i<num_names; ++i)
	{
		curDriver = pScriptObj->GetDriverDirect(i);
		dName = itString(curDriver->GetName().c_str());
		dName.StripExtension();
		//if the name and old start time match, we should change the driver's start time
		if ( dName == itString(i_DriverName.c_str()) )
		{
			//TIME - why a floor comparison on seconds here?
			//if ( floor(curDriver->GetBeginTime()) == floor(i_OldStartTime) )
			if ( floor(curDriver->GetBeginTime().AsSeconds()) == floor(i_OldStartTime.AsSeconds()) )
			{
				curDriver->SetBeginTime( i_NewStartTime );
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------
/// Shift a driver from its old start time to its new start time
//----------------------------------------------------------------------------
void fcuiUtils::ShiftDriverToTimeAtPosX( const nameString& i_ObjectName,
									     const std::string& i_DriverName,
										 const maTime& i_OldStartTime, 
										 const maTime& i_NewStartTime,
										 float i_PositionX)
{
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;

	// get the drivers and find if the driver exists
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	int num_names = pScriptObj->GetNumDrivers();
	tmlnDriver* curDriver = NULL;
	itString dName;
	// find the correct driver to manipulate
	for (int i=0; i<num_names; ++i)
	{
		curDriver = pScriptObj->GetDriverDirect(i);
		dName = itString(curDriver->GetName().c_str());
		dName.StripExtension();
		//if the name and old start time match, we should change the driver's start time
		if ( dName == itString(i_DriverName.c_str()) )
		{
			//TIME - why a floor comparison on seconds here?
			//if ( floor(curDriver->GetBeginTime()) == floor(i_OldStartTime))
			if ( floor(curDriver->GetBeginTime().AsSeconds()) == floor(i_OldStartTime.AsSeconds()))
			{
				tmlnDriverAttachInfo* pDriverAttachInfo = dynamic_cast<tmlnDriverAttachInfo*>(curDriver->GetDriverInfo());
				if (pDriverAttachInfo != NULL && pDriverAttachInfo->m_WorldSpaceOffset.GetX() == i_PositionX)
				{
					curDriver->SetBeginTime( i_NewStartTime );
					break;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnDriver* fcuiUtils::GetDriverAtTime( const nameString& i_ObjectName,
											  const std::string& i_DriverName,
											  const maTime& i_StartTime )
{
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return NULL;

	// get the drivers and find if the driver exists
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return NULL;
	}

	int num_names = pScriptObj->GetNumDrivers();
	tmlnDriver* curDriver = NULL;
	itString dName;
	// find the correct driver to manipulate
	for (int i=0; i<num_names; ++i)
	{
		curDriver = pScriptObj->GetDriverDirect(i);
		dName = itString(curDriver->GetName().c_str());
		dName.StripExtension();
		//if the name and old start time match, we should change the driver's start time
		if ( dName == itString(i_DriverName.c_str()) )
		{
			//TIME - why a floor comparison on seconds here?
			//if ( floor(curDriver->GetBeginTime()) == floor(i_StartTime) )
			if ( floor(curDriver->GetBeginTime().AsSeconds()) == floor(i_StartTime.AsSeconds()) )
			{
				return curDriver;
			}
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
/// Get all drivers for the object within the given time
//----------------------------------------------------------------------------
void fcuiUtils::GetDrivers( const nameString& i_ObjectName,
						    std::vector<tmlnDriver*>& o_Drivers )
{
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;

	// get the drivers and find if the driver exists
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	int num_names = pScriptObj->GetNumDrivers();
	tmlnDriver* curDriver = NULL;
	itString dName;
	// find the correct driver to manipulate
	for (int i=0; i<num_names; ++i)
	{
		curDriver = pScriptObj->GetDriverDirect(i);
		dName = itString(curDriver->GetName().c_str());
		if ( dName == itString("Capture") ) // don't add capture drivers
			continue;

		o_Drivers.push_back(curDriver);
	}
}

//----------------------------------------------------------------------------
/// Shift all drivers in a list to the new time
//----------------------------------------------------------------------------
void fcuiUtils::ShiftDriversToTime( const nameString& i_ObjectName,
									const maTime& i_StartOffset,
								    const std::vector<tmlnDriver*>& i_Drivers )
{
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;

	// get the drivers and find if the driver exists
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	for( int i = 0; i < i_Drivers.size(); ++i )
	{
		i_Drivers[i]->SetBeginTime(i_Drivers[i]->GetBeginTime() + i_StartOffset);
	}
}

//----------------------------------------------------------------------------
/// Remove a driver at the start time
//----------------------------------------------------------------------------
void fcuiUtils::RemoveDriverAtTime( const nameString& i_ObjectName,
									const std::string& i_DriverName,
									const maTime& i_StartTime )
{
	tmlnScriptObject* pScriptObj =  GetTimelineScriptObject(i_ObjectName);
	if (!pScriptObj)
		return;
 
	// get the drivers and find if the driver exists
	tmlnDriverNameList driver_names;
	tmlnCreator::GatherPossibleDrivers(pScriptObj, driver_names);
	if (driver_names.Empty())
	{
		DBG_ERROR("No drivers available for an object of this type.");
		return;
	}

	int num_names = pScriptObj->GetNumDrivers();
	tmlnDriver* curDriver = NULL;
	itString dName;
	// find the correct driver to manipulate
	for (int i=0; i<num_names; ++i)
	{
		curDriver = pScriptObj->GetDriverDirect(i);
		dName = itString(curDriver->GetName().c_str());
		dName.StripExtension();
		//if the name and old start time match, we should change the driver's start time
		if ( dName == itString(i_DriverName.c_str()) )
		{
			//TIME - why a floor comparison on seconds here?
			//if ( floor(curDriver->GetBeginTime()) == floor(i_StartTime) )
			if ( floor(curDriver->GetBeginTime().AsSeconds()) == floor(i_StartTime.AsSeconds()) )
			{
				if ( pScriptObj->RemoveDriver(curDriver) )
					break;
			}
		}
	}
}

//------------------------------------------------------------------------
///	Play a movie file()
//------------------------------------------------------------------------
void fcuiUtils::PlayMovie(QWidget* i_pParent)
{
//	Phonon::VideoPlayer *player = new Phonon::VideoPlayer(Phonon::VideoCategory, i_pParent);
////	QString name("Video Player");
////	connect(player, name, name);
////	player->connect(player, SIGNAL(finished()), SIGNAL(finished()));
//	player->connect(player, SIGNAL(finished()), player, SLOT(deleteLater()));
//	Phonon::MediaSource src("C:\\Users\\robert.knaack\\Documents\\Studio GPU Footage\\BallAndSphere\\BallAndSphere.mov");
//	player->play( src );
}

//------------------------------------------------------------------------
///Set the name of the movie to be played in theater
//------------------------------------------------------------------------
void fcuiUtils::SetMovieName(const fsLocator& i_MovieName)
{
	l_MovieName = i_MovieName;
}

//------------------------------------------------------------------------
///	Get the name of the movie to be played in theater
//------------------------------------------------------------------------
fsLocator fcuiUtils::GetMovieName()
{
	return l_MovieName;
}


//------------------------------------------------------------------------
///Set the title of the movie to be played in theater
//------------------------------------------------------------------------
void fcuiUtils::SetMovieTitle(const fsLocator& i_MovieTitle)
{
	std::string str_title;
	fsFileUtil::LocatorToANSIFilename(i_MovieTitle,str_title);

	fcdcTimelineItemData temp = fcdcDataMgr::GetData();
	temp.m_ProjectData.m_Title.SetValue(str_title);
	fcdcDataMgr::SetData(temp);

	l_MovieTitle = i_MovieTitle;
}

//------------------------------------------------------------------------
///	Get the title of the movie to be played in theater
//------------------------------------------------------------------------
fsLocator fcuiUtils::GetMovieTitle()
{
	return l_MovieTitle;
}

//------------------------------------------------------------------------
///	Get/Set the checked state for the append countdown CB
//------------------------------------------------------------------------
void fcuiUtils::SetChecked(bool i_Checkedstate)
{
	l_AppendMovie = i_Checkedstate;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool fcuiUtils::GetChecked()
{
	return l_AppendMovie;
}

//------------------------------------------------------------------------
//	Strip the beginning number from a text string.
//
//	e.g. 01.Wardrobe -> Wardrobe
//------------------------------------------------------------------------
void fcuiUtils::StripNumber(itString &io_FileList)
{
	//	if there is a dot in the text, strip the front assuming
	//	it is in the format listed in the comments. 
	//	Otherwise just return the string
	//
	if (io_FileList.HasSubString(itString(".")))
	{
		itString base_text;
		io_FileList.GetExtension(base_text);
		io_FileList = base_text;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool SortFiles_GXB(const fsLocator& i_File1, const fsLocator& i_File2)
{
	return (i_File1 < i_File2);
}

//------------------------------------------------------------------------
//	Sort file list with preferences to GXBs
//------------------------------------------------------------------------
void fcuiUtils::SortFileList_GXB(std::vector<fsLocator> &io_FileList)
{
	// sort elements first
	sort( io_FileList.begin(), io_FileList.end(), SortFiles_GXB );

	//	go through list and put all GXB files to the top
	//
	std::vector<fsLocator>::iterator it, it_gxb;
	it_gxb = io_FileList.begin();
	for (it = io_FileList.begin(); it != io_FileList.end(); ++it)
	{
		if ((*it).GetLastName().EndsWith(fcuiConstants::l_EXT_GXB))
		{
			if (it != it_gxb)
			{
				iter_swap(it_gxb, it);
			}
			++it_gxb;
		}
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//bool SortData_GXB(const cmmDialogData& d1, const cmmDialogData& d2)
//{
//	return (d1.m_Name < d2.m_Name);
//}

//------------------------------------------------------------------------
//	Sort file list with preferences to GXBs
//------------------------------------------------------------------------
//void fcuiUtils::SortDataList(cmmDialogDataList &io_DataList)
//{
//	// sort elements first
//	sort( io_DataList.begin(), io_DataList.end(), SortData_GXB );
//
//	//	go through list and put all GXB files to the top
//	//
//	cmmDialogDataList::iterator it, it_gxb;
//	it_gxb = io_DataList.begin();
//	for (it = io_DataList.begin(); it != io_DataList.end(); ++it)
//	{
//		cmmDialogData &data = (*it);
//		if ((*it).m_Filename.EndsWith(fcuiConstants::l_EXT_GXB))
//		{
//			if (it != it_gxb)
//			{
//				it_gxb.swap(it);
//			}
//			++it_gxb;
//		}
//	}
//}

//------------------------------------------------------------------------
//	Force the projected lights to not show their icons (on by default)
//------------------------------------------------------------------------
void fcuiUtils::ProjectedLight_HideIcons()
{
	int num_objs = prjltObjectMgr::GetNumObjects();

	for (int i=0; i < num_objs; ++i)
	{
		prjltScriptObject* pSObj = prjltObjectMgr::GetObject(i);
		if (pSObj != NULL)
		{
			prjltProjectedLightObject* pObj = pSObj->GetPickObject();
			if (pObj != NULL)
			{
				pObj->PropertyShowTexture().SetValue(false);
			}
		}
	}
}

//------------------------------------------------------------------------
//	Generate a thumbnail from a matTexture
//------------------------------------------------------------------------
void fcuiUtils::CreateUserThumbnail( matTexture* i_pTexture, const fsLocator& i_DestinationFile )
{
	// convert to surface representation
	//
	g2dImage* pImg;
	g2dScreenCaptureUtil::Capture2DTextureToImage(i_pTexture, pImg);
	g2dImage* pDownSampledImg = NULL;
	const int thumbW = 130;
	const int thumbH = 73;
	pDownSampledImg = g2dImageCreate::Make( thumbW, thumbH, 
											pImg->GetPixelFormat(),
											g2dImage::e_SystemMemory);
	pDownSampledImg->CopyImage(*pImg, NULL, 
								pImg->GetHeight(), pImg->GetHeight(), 1, 1);

	//	if the directory doesn't exist, create it!
	//	if the file exists, delete it!
	fsLocator dest_folder = i_DestinationFile;
	dest_folder.Pop();
	if (!fsFileUtil::DirectoryExists( dest_folder ))
		fsFileUtil::CreateDirectory( dest_folder );
	if (fsFileUtil::FileExists(i_DestinationFile))
	{
		//	don't overwrite it for now
		//fsFileUtil::DeleteFile(i_DestinationFile);
	}
	else
	{
		//	save the thumbnail
		g2dImageSave::Save(i_DestinationFile, pDownSampledImg);
	}

	// Clean-up
	delete pDownSampledImg;
	delete pImg;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int fcuiUtils::GetWidescreenHeight( int i_Width )
{
	// calculate the height for a 16x9 aspect given the width
	int val = i_Width / 16;
	val *= 9;
	return val;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int fcuiUtils::GetWidescreenWidth( int i_Height )
{
	// calculate the width for a 16x9 aspect given the height
	int val = i_Height / 9;
	val *= 16;
	return val;
}

//------------------------------------------------------------------------
/// Copy the data from the base camera to the copy camera
///-----------------------------------------------------------------------
void fcuiUtils::CopyCamProperties(const std::string& i_CopyCamString, const std::string& i_BaseCamString)
{
	if( i_CopyCamString.length() == 0 || i_BaseCamString.length() == 0 )
		return;

	//get the camera names and index
	nameString copyCam(i_CopyCamString);
	nameString baseCam(i_BaseCamString);

	int copyIndex = camsCameraMgr::GetIndexForName(copyCam);
	int baseIndex = camsCameraMgr::GetIndexForName(baseCam);

	//get the camera data
	cmraCameraData& copyData = cmraObjectMgr::GetBaseData(copyIndex);
	cmraCameraData& baseData = cmraObjectMgr::GetBaseData(baseIndex);

	//now copy the position, target, and fov values of the base camera to the copy camer
	copyData.m_Position.SetValue(baseData.m_Position.GetValue());
	copyData.m_Target.SetValue(baseData.m_Target.GetValue());
	copyData.m_FOV.SetValue(baseData.m_FOV.GetValue());

	//set the data
	cmraOperations::SetSelectedIndex(copyIndex);
	cmraOperations::ChangeBaseData(copyData);
}
//-----------------------------------------------------------------------
/// Get/Set Object name to be used to set audio driver
//-----------------------------------------------------------------------
void fcuiUtils::SetObjectName(const itString& i_CastmemberName)
{
	l_Objectname = i_CastmemberName;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
itString fcuiUtils::GetObjectName()
{
	return l_Objectname;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void fcuiUtils::SetCountdownMoviepath(const fsLocator& i_CountdownMoviePath)
{
	l_countdownmoviePath = i_CountdownMoviePath;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
fsLocator fcuiUtils::GetCountdownMoviePath()
{
	return l_countdownmoviePath;
}