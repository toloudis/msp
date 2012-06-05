/*****************************************************************************
**	actnTitleCardMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/actn/actnTitleCardMgr.hpp"

#include "FCSupport/fcmd/fcmdConstants.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiTimelineMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/qtGUI/fcuiTextEditOperations.hpp"
#include "FCSupport/fcui/qtGUI/qtfc3d.h"
#include "FCSupport/path/pathDirectoryParser.hpp"
#include "Features/Import/ImportData.hpp"
#include "Features/Import/ImportUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Drivers/Attach/tmlnDriverAttach.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/RenderPanels/rpnOperations.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/Sc/scBillboard.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Systems/Billboard/Data/billScriptData.hpp"
#include "Systems/Billboard/Object/billBillboardObject.hpp"
#include "Systems/Billboard/Object/billObjectMgr.hpp"
#include "Systems/Billboard/Undo/billOperations.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#include "Systems/Cameras/Undo/cmraOperations.hpp"
#include "Systems/Environments/Undo/envtOperations.hpp"
#include "Systems/Environments/Object/envtScriptObject.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/api3d/api3dBillboard.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"

#include <boost/lexical_cast.hpp>


//============================================================================
//============================================================================
namespace
{
	itString l_DDSExt("dds");
	itString l_JPGExt("jpg");
	itString l_PNGExt("png");
	itString l_XMLExt("xml");
	itString l_SystemName("Objects");
	const maTime c_TitlecardDuration = maTime::FromSeconds(3.0f);
	maTime init_time, duration;
	maTime final_time(c_TitlecardDuration);
	
	int l_Filenumber = 0;

	// The following consts are to map the text from the image of unknown size
	// to the viewport which is 782 x 440 in size.
	const int l_ViewportWidth = 782;
	const int l_ViewportHeight = 440;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
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
	}
}

///---------------------------------------------------------------------------
///	the current instance of the mode mgr singleton
///---------------------------------------------------------------------------
actnTitleCardMgr* actnTitleCardMgr::Instance = NULL;


///-----------------------------------------------------------------------
/// constructors
///-----------------------------------------------------------------------
actnTitleCardMgr::actnTitleCardMgr()
{
}

///-----------------------------------------------------------------------
/// destructors
///-----------------------------------------------------------------------
actnTitleCardMgr::~actnTitleCardMgr()
{
}

//--------------------------------------------------------------------
///	Deinitialize/Initialize
//--------------------------------------------------------------------
void actnTitleCardMgr::Initialize()
{
}

void actnTitleCardMgr::DeInitialize()
{
}

///-----------------------------------------------------------------------
/// Create the TitleCard
///-----------------------------------------------------------------------
void actnTitleCardMgr::CreateTitleCard()
{
	// Create the environment light
	envtScriptData envtScript;
	maFloatRGBA white(1.0f,1.0f,1.0f,1.0f);
	envtScript.m_BaseData.m_DiffuseColor.SetValue(white);
	envtScript.m_BaseData.m_DiffuseFactor.SetValue(1.0);
	std::string env_billboard = "Titlecards";
	envtScript.m_BaseData.m_Name.SetString(env_billboard.c_str());
	int index_bill = envtObjectMgr::GetIndexForObject(nameString(env_billboard.c_str()));
	if(index_bill <0)
		envtOperations::AddObject(envtScript);
	
	 //Manually set the positions of the billboard and the camera that faces them
	maPoint3d billBoardPosition(1484.252f , 2362.205f, 334.646f);
	maPoint3d cameraPosition(1484.252f , 2362.205f, 697.165f);
	maPoint3d cameraTPosition(1484.252f ,2362.205f, 334.646f);
	float billboard_scale = 380.0f;
	maPoint3d billboardscale(228.330f,128.590f,0.0f);
	
	itString icon( fcmdConstants::c_Icon );
	itString extension, icon_check;
	nameString camName("Cam_Billboard");

	// Check if "cam_billboard" exists. If not, create one.
	cmraScriptObject* pSO = cmraObjectMgr::GetObject(camName);
	if (pSO == NULL)
	{
		cmraOperations::AddObject();
		sel3dObject* pPO = sel3dMgr::GetSelected();
		cmraCameraObject* pCO = dynamic_cast<cmraCameraObject*>(pPO);
		if (pCO != NULL)
		{
			int cindex = cmraObjectMgr::GetIndexForObject(pCO->GetName());
			cmraCameraData& cdata = cmraObjectMgr::GetBaseData(cindex);
			cdata.m_Name.SetString("Cam_Billboard");
			cdata.m_Position.SetValue(cameraPosition);
			cdata.m_Target.SetValue(cameraTPosition);
			cdata.m_FOV.SetValue(35.0);
			cmraOperations::ChangeBaseData(cdata);
		}
	}
	else
	{
		//if the camera does exist, lets be sure that it's properties are correct
		cmraCameraData& cdata = pSO->GetBaseData();
		cdata.m_Position.SetValueWithoutNotify(cameraPosition);
		cdata.m_Target.SetValueWithoutNotify(cameraTPosition);
		cdata.m_FOV.SetValue(35.0);
		pSO->SetBaseData(cdata);
	}

	// Change the camera to cam_billboard
	init_time = fcuiTimelineMgr::Instance->GetEndTime() ;
	final_time  = init_time + c_TitlecardDuration;
	//float endTime = -1.0f;
	
	int index = camsCameraMgr::GetIndexForName(camName);
	/*if (index >= 0 && index < camsCameraMgr::GetNumCameras())
	{
		rpnOperations::ChangeCamera(camsCameraMgr::GetCameraProxy(index), camName.GetString());
		rpnOperations::SelectCamera(index);
	}
	*/
	//itString TexturePath;
	
	// Create a billboard and load the JPG in the folder.
	//fsFileUtil::LocatorToUnicodeString(i_ElementPath, TexturePath);
	billScriptData billScript;
	nameString objName;
	billOperations::CreateNewObjectName( nameString(), objName);
	//io_ItemData.m_CategoryData = itString(objName.GetString().c_str());
	billScript.m_BaseData.m_Name = objName;
	m_BillboardName = itString(objName.GetString().c_str());
	billScript.m_BaseData.m_bOrientToCamera = true;
	//billScript.m_BaseData.m_Filename.SetString(itStringUtil::GetStdString(TexturePath));
	billScript.m_BaseData.m_Position.SetValue(billBoardPosition, true);
	billScript.m_BaseData.m_bCKActive = false;
	//billScript.m_BaseData.m_bSnapToCamera = true;
	//billScript.m_BaseData.m_DistToCamera.SetValue(10.0);
	//billScript.m_BaseData.m_CameraIndex.SetValue(index + 1);

	//billScript.m_BaseData.m_Scale.SetValue(1.0);
	billScript.m_BaseData.m_NonUniformScale.SetValue(billboardscale, true);
	index_bill = envtObjectMgr::GetIndexForObject(nameString(env_billboard.c_str()));
	
	//scBillboard::SetCameraPosition(cam3dMgr::GetCamera());

	billOperations::AddObject(billScript);
	if(index_bill >= 0)
		envtOperations::AddObjectToEnvironment(nameString(env_billboard.c_str()),objName);
}

///-----------------------------------------------------------------------
/// Add the texture to the titlecard
///-----------------------------------------------------------------------
void actnTitleCardMgr::AddTitleCardTexture(const fsLocator& i_ElementPath)
{
	// create the capture driver for the billboard camera at this point in time
	init_time = fcuiTimelineMgr::Instance->GetEndTime();
	final_time  = init_time + c_TitlecardDuration;
	std::string camera = "Cam_Billboard";
	nameString camName(camera);
	std::string capturedrivername("Capture");
	tmlnDriver* pCaptureDriver = fcuiUtils::CreateDriver(capturedrivername, camName, init_time,final_time );

	itString uniqueDriverName;
	fcuiUtils::CreateTitlecardTextureDriver(nameString(itStringUtil::GetStdString(m_BillboardName)), i_ElementPath, init_time, final_time, uniqueDriverName);
	m_MostRecentDriverName = uniqueDriverName;

	fcuiUtils::CopyCamProperties(fcuiConstants::c_CAMERA_DIRECTOR, camera);
	SwitchToCamera(fcuiConstants::c_CAMERA_DIRECTOR);

	// check if the time has gone over the scene's time limit
	PrefsData& data = PrefsMgr::Data();
	maTime curMaxTime = maTime::FromSeconds(data.m_ChannelEditor_DefaultTime.GetValue());
	if ( final_time > curMaxTime )
	{
		curMaxTime = final_time + maTime::FromSeconds(5);
		data.m_ChannelEditor_DefaultTime.SetValue((int) curMaxTime.AsSeconds());
	}

	tmlnTimeLine::SetTimeRange( maTime::c_ZeroTime, curMaxTime );
	//update the end time of the timeline.
	tmlnTimeLine::SetValue(final_time);
	if( fcuiTimelineMgr::Instance != NULL )
		fcuiTimelineMgr::Instance->SetEndTime(tmlnTimeLine::GetValue());
	
	tmlnTimeLine::SetValue(init_time);
}

///---------------------------------------------------------------------------
/// Perform the mode specific operations when a TitleCard element needs to 
/// execute an action.
///---------------------------------------------------------------------------
void actnTitleCardMgr::ProcessTitleCard(const fsLocator& i_ElementDirectory)
{
	DBG_TRACE("TitleCard folder: " << i_ElementDirectory);

	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(i_ElementDirectory, files);

	itString icon( fcmdConstants::c_Icon );
	itString extension, icon_check;
	for ( int i = 0; i < files.size(); ++i )
	{
		icon_check = files[i].GetLastName();
		//don't process the icon file of the asset
		if ( icon_check == icon )
			continue;
		if ( icon_check == itString("temp.jpg"))
			continue;

		files[i].GetLastName().GetExtension(extension);
		if (extension == l_PNGExt)
		{
			AddTitleCardTexture(files[i]);
		}
	}
}

///-----------------------------------------------------------------------
/// Action to perform when titlecards are clicked on the timeline
///-----------------------------------------------------------------------
void actnTitleCardMgr::ProcessTitleCardTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	tmlnTimeLine::SetValue(i_ItemData.m_StartTime);
	std::string camString("Director");
	SwitchToCamera(camString);
}

///-----------------------------------------------------------------------
/// Write on the image
///-----------------------------------------------------------------------
void actnTitleCardMgr::WriteOnTitleCard(const fsLocator& i_Filename, const itString& i_BodyText, const itString& i_TitleText, const maFloatRGBA& i_docColor)
{
	fsLocator fs_file = gfPaths::GetPath(mnmPaths::e_Cache);
	fs_file.Push("TitleCards");
	fsLocator XML_path = i_Filename;
	fioTitleCardItemData io_Data;
	XML_path.Pop();

	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(XML_path, files);
	itString icon( fcmdConstants::c_Icon );
	itString extension, icon_check;
	int i;
	for ( i = 0; i < files.size(); ++i )
	{
		icon_check = files[i].GetLastName();
		//don't process the icon file of the asset
		if ( icon_check == icon )
			continue;
		files[i].GetLastName().GetExtension(extension);

		if(extension == l_XMLExt)
		{
			//WriteXML(files[i], i_docColor);
			ReadXML(files[i], io_Data);
			
		}
	}

	std::string cachefilename, filename_str;
	itString filename_itstr = i_Filename.GetLastName();
	std::string tempFileName = "temp";
	tempFileName += boost::lexical_cast<std::string>(l_Filenumber);
	tempFileName += ".png";
	fsFileUtil::LocatorToANSIFilename(i_Filename, filename_str);
	QImage tempImage(filename_str.c_str());

	//Remap the position of the texts based on the image size
	float image_width = tempImage.width();
	float image_height = tempImage.height();
	float width_ratio = (float)(image_width/ l_ViewportWidth);
	float height_ratio = (float)(image_height/ l_ViewportHeight);
	
	float start_x = io_Data.TitleX * width_ratio;
	float start_y = io_Data.TitleY * height_ratio;
	float body_x = io_Data.BodyX * width_ratio;
	float body_y = io_Data.BodyY * height_ratio;

	// Set the text color
	QImage image(filename_str.c_str());
	QPainter Painter(&image);
	QColor doc_color;
	doc_color.setRgb(i_docColor.GetRed(),i_docColor.GetGreen(), i_docColor.GetBlue());
	QBrush doc_brush(doc_color);
	QPen doc_pen(doc_color);
	Painter.setBrush(doc_brush);
	Painter.setPen(doc_pen);

	// Set the text
	std::string strBodyString = itStringUtil::GetStdString(i_BodyText);
	std::string strTitleString = itStringUtil::GetStdString(i_TitleText);
	QString bodyString = (strBodyString.c_str());
	QString titleString = (strTitleString.c_str());

	//Set the font size
	QFont title_font;
	title_font.setPointSize((io_Data.TitleSize + 7) * 1.77f);
	//title_font.setBold(io_Data.isBold);
	title_font.setFamily(QString(itStringUtil::GetStdString(io_Data.FontFamily).c_str()));
	Painter.setFont(title_font);

	// The rectangle around the title
	Painter.drawText(start_x, start_y, io_Data.TitleWidth * width_ratio, io_Data.TitleHeight * height_ratio, Qt::TextWordWrap|Qt::AlignHCenter, titleString);
	Painter.setBrush(doc_brush);
	QFont body_font;
	body_font.setPointSize((io_Data.BodySize + 7) * 1.77f);
	body_font.setFamily(QString(itStringUtil::GetStdString(io_Data.FontFamily).c_str()));
	Painter.setFont(body_font);
	
	// The rectangle around the body
	Painter.drawText(body_x,body_y,io_Data.BodyWidth * width_ratio, io_Data.BodyHeight * height_ratio,Qt::TextWordWrap|Qt::AlignHCenter,bodyString);
	filename_itstr.StripExtension();
	fs_file.Push(filename_itstr);
	fsFileUtil::CreateDirectory(fs_file);
	fs_file.Push(tempFileName.c_str());
		
	// Save the image to the cache
	fsFileUtil::LocatorToANSIFilename(fs_file, cachefilename);
	int result = image.save(QString(cachefilename.c_str()),"PNG");
}

///-----------------------------------------------------------------------
/// reload texture on a billboard
///-----------------------------------------------------------------------
void actnTitleCardMgr::ReloadTexture(const itString& ObjName, const fsLocator& i_TexturePath)
{
	fsLocator fs_file = gfPaths::GetPath(mnmPaths::e_Cache);
	fs_file.Push("TitleCards");
	std::string cachefilename, filename_str;
	itString filename_itstr = i_TexturePath.GetLastName();
	fsFileUtil::LocatorToANSIFilename(i_TexturePath, filename_str);
	std::string tempFileName = "temp";
	tempFileName += boost::lexical_cast<std::string>(l_Filenumber);
	tempFileName += ".png";
	l_Filenumber++;
	filename_itstr.StripExtension();
	fs_file.Push(filename_itstr);
	fs_file.Push(tempFileName.c_str());
	fsFileUtil::LocatorToANSIFilename(fs_file, cachefilename);

	itString tempItName(cachefilename.c_str()); 
	fcuiUtils::ReloadTitlecardTextureValue(nameString(itStringUtil::GetStdString(m_BillboardName)), m_EditDriver, fs_file);
}

///-----------------------------------------------------------------------
/// Action to perform when titlecards are moved in timeline
///-----------------------------------------------------------------------
void actnTitleCardMgr::ShiftTitleCardTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration, bool i_bManualShift)
{
	maTime old_start = io_ItemData.m_StartTime;

	io_ItemData.m_StartTime += i_Duration;
	io_ItemData.m_EndTime += i_Duration;
	
	nameString cam_name("Cam_Billboard");
	//std::string positionattach_driver_name = "Attach";
	//std::string targetattach_driver_name = "Attach";
	std::string capture_driver_name = "Capture";
	std::string texture_driver_name = itStringUtil::GetStdString(io_ItemData.m_CategoryData);

	//fcuiUtils::ShiftDriverToTimeAtPosX(cam_name, positionattach_driver_name, old_start, io_ItemData.m_StartTime, io_ItemData.m_BillboardPosX);
	//fcuiUtils::ShiftDriverToTimeAtPosX(cam_name, targetattach_driver_name, old_start, io_ItemData.m_StartTime, io_ItemData.m_BillboardPosX);
	fcuiUtils::ShiftDriverToTime(cam_name, capture_driver_name, old_start, io_ItemData.m_StartTime);
	fcuiUtils::ShiftDriverToTime(nameString(itStringUtil::GetStdString(m_BillboardName)), texture_driver_name, old_start, io_ItemData.m_StartTime);
}

///-----------------------------------------------------------------------
/// Action to be performed when titlecards are removed
///-----------------------------------------------------------------------
void actnTitleCardMgr::RemoveTitleCardTimelineItem(const fcuiTimelineItemData& i_ItemData)
{
	maTime start_time = i_ItemData.m_StartTime;

	nameString cam_name;
	cam_name = nameString("Cam_Billboard");
	std::string texture_driver_name = itStringUtil::GetStdString(i_ItemData.m_CategoryData);

	//fcuiUtils::RemoveDriverAtTime(cam_name, "Attach", start_time);
	//fcuiUtils::RemoveDriverAtTime(cam_name, "Attach", start_time);
	fcuiUtils::RemoveDriverAtTime(cam_name, "Capture", start_time);
	fcuiUtils::RemoveDriverAtTime(nameString(itStringUtil::GetStdString(m_BillboardName)), texture_driver_name, start_time);
}

///-----------------------------------------------------------------------
/// Action to be performed when we click on the edit on the timeline item
///-----------------------------------------------------------------------
void actnTitleCardMgr::EditTitleCardTimelineItem(fcuiTimelineItemData& io_ItemData)
{
	//save our title card driver name for a possible texture reload 
	m_EditDriver = io_ItemData.m_CategoryData;
	io_ItemData.m_BillboardName = m_BillboardName;

	// Show the TextEdit options and load the corresponding image in the background
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	fcuiTextEditOperations* TextEdit = fcuiFormMgr::GetTextEdits();

	i_ui->ui.TitleCard_LabelWidget->setVisible(false);
	i_ui->ui.widget_3->setVisible(false);
	i_ui->ui.Edit_TitleCardWidget->setVisible(true);
	fsLocator item = io_ItemData.m_ItemLocator;

	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(item, files);
	
	itString icon( fcmdConstants::c_Icon );
	itString extension, icon_check;
	for ( int i = 0; i < files.size(); ++i )
	{
		icon_check = files[i].GetLastName();
		//don't process the icon file of the asset
		if ( icon_check == icon )
			continue;
		
		files[i].GetLastName().GetExtension(extension);
		if (extension == l_PNGExt)
		{
			item.Push(files[i].GetLastName());
			std::string itemString;
			fsFileUtil::LocatorToANSIFilename(item, itemString);
			QPixmap pixmap(itemString.c_str());
			TextEdit->setlabelBackground(pixmap);
		}

		else if(extension == l_XMLExt)
		{
			fioTitleCardItemData io_Data;
			ReadXML(files[i], io_Data);
			TextEdit->SetTextEdits(io_Data);
			if(!io_ItemData.m_colorChanged)
				io_ItemData.m_TitlecardColor.Set(io_Data.DocumentColor[0],io_Data.DocumentColor[1],io_Data.DocumentColor[2],255.0);
		}

	}
	TextEdit->SetItemData(io_ItemData);
}


///-----------------------------------------------------------------------
/// Check if there are any files missing for the TitleCards
///-----------------------------------------------------------------------
void actnTitleCardMgr::CheckTitleCardData(fcuiTimelineItemData& io_ItemData)
{
	// Go through all the drivers and check if texture is missing
	itString m_TextureDriver = io_ItemData.m_CategoryData;
	itString m_BillboardName = io_ItemData.m_BillboardName;
	bool isFileMissing = fcuiUtils::CheckforMissingFile(nameString(itStringUtil::GetStdString(m_BillboardName)),m_TextureDriver);
	// If a texture is missing, create a new texture using io_ItemData
	if(isFileMissing)
	{
		//call write on titlecard
		std::vector<fsLocator> files;
		fsLocator item = io_ItemData.m_ItemLocator;
		pathDirectoryParser parser;
		parser.GetDirectoryFiles(item, files);
	
		itString icon( fcmdConstants::c_Icon );
		itString extension, icon_check;
		for ( int i = 0; i < files.size(); ++i )
		{
			icon_check = files[i].GetLastName();
			//don't process the icon file of the asset
			if ( icon_check == icon )
				continue;
			files[i].GetLastName().GetExtension(extension);
			if (extension == l_PNGExt)
			{
				WriteOnTitleCard(files[i], io_ItemData.m_BodyText, io_ItemData.m_TitleText, io_ItemData.m_TitlecardColor);	
				ReloadTexture(m_BillboardName, files[i]);
				std::string camString("Director");
				SwitchToCamera(camString);
			}
		}
	}
}


///-----------------------------------------------------------------------
/// Read the XML
///-----------------------------------------------------------------------
void actnTitleCardMgr::ReadXML(fsLocator& i_XMLFile, fioTitleCardItemData& io_Data)
{
	guiXMLTextReader::Open(i_XMLFile);

	//	read in the preferences
	//
	guiXMLTextReader::gui_Node_Type node_type;
	std::string keyname, strvalue;

	while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
	{
		if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
		{
			if (keyname == "TitleX")
				{ guiXMLTextReader::Convert(strvalue, io_Data.TitleX); }
			else if (keyname == "TitleY")
				{ guiXMLTextReader::Convert(strvalue, io_Data.TitleY); }
			else if (keyname == "TitleWidth")
				{ guiXMLTextReader::Convert(strvalue, io_Data.TitleWidth); }
			else if (keyname == "TitleHeight")
				{ guiXMLTextReader::Convert(strvalue, io_Data.TitleHeight); }
			if (keyname == "BodyX")
				{ guiXMLTextReader::Convert(strvalue, io_Data.BodyX); }
			else if (keyname == "BodyY")
				{ guiXMLTextReader::Convert(strvalue, io_Data.BodyY); }
			else if (keyname == "BodyWidth")
				{ guiXMLTextReader::Convert(strvalue, io_Data.BodyWidth); }
			else if (keyname == "BodyHeight")
				{ guiXMLTextReader::Convert(strvalue, io_Data.BodyHeight); }
			else if (keyname == "Family")
				{ guiXMLTextReader::Convert(strvalue, io_Data.FontFamily); }
			else if (keyname == "TitleColorR")
			{float value; guiXMLTextReader::Convert(strvalue, value); io_Data.DocumentColor[0] = value;}
			else if (keyname == "TitleColorG")
			{float value; guiXMLTextReader::Convert(strvalue, value); io_Data.DocumentColor[1] = value;}
			else if (keyname == "TitleColorB")
			{float value; guiXMLTextReader::Convert(strvalue, value); io_Data.DocumentColor[2] = value;}
			else if(keyname == "Bold")
			{guiXMLTextReader::Convert(strvalue, io_Data.isBold);}
			else if(keyname == "TitleMaxCols")
			{guiXMLTextReader::Convert(strvalue, io_Data.TitleMaxCols);}
			else if(keyname == "BodyMaxCols")
			{guiXMLTextReader::Convert(strvalue, io_Data.BodyMaxCols);}
			else if(keyname == "TitleMaxRows")
			{guiXMLTextReader::Convert(strvalue, io_Data.TitleMaxRows);}
			else if(keyname == "BodyMaxRows")
			{guiXMLTextReader::Convert(strvalue, io_Data.BodyMaxRows);}
			else if(keyname == "TitleSize")
			{guiXMLTextReader::Convert(strvalue, io_Data.TitleSize);}
			else if(keyname == "BodySize")
			{guiXMLTextReader::Convert(strvalue, io_Data.BodySize);}
		}
	}

	guiXMLTextReader::Close();
}

///-----------------------------------------------------------------------
/// Return the x position for the newest billboard
///-----------------------------------------------------------------------
float actnTitleCardMgr::GetMostRecentXPosition()
{
	return m_MostRecentCardPosX;
}

///-----------------------------------------------------------------------
/// Return the most recent name given to a new texture driver for the billboard
///-----------------------------------------------------------------------
itString actnTitleCardMgr::GetMostRecentDriverName()
{
	return m_MostRecentDriverName;
}

///-----------------------------------------------------------------------
/// Return the name of the billboard
///-----------------------------------------------------------------------
itString actnTitleCardMgr::GetBillboardName()
{
	return m_BillboardName;
}