/********************************************************************************************\
**  cptrRenderStateDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Features/Capture/cptrRenderStateDataParser.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "Support/tmln/tmlnParser.hpp"


//============================================================================
//============================================================================
namespace cptrRenderStateDataParser
{
namespace
{
const char* lc_Key_RenderState_Scene = "Scene";
const char* lc_Key_RenderState_Camera = "Camera";
const char* lc_Key_RenderState_Time = "Time";
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReadFromConfigFile(fsLocator& i_ConfigFile,
						captRenderOutputData& o_Data )
{
	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

	guiXMLTextReader::Open(cfgpath.c_str());

	//	read in the preferences
	//
	guiXMLTextReader::gui_Node_Type node_type;
	std::string keyname, strvalue;
	while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
	{
		if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
		{
			if (keyname == lc_Key_RenderState_Scene)
				{itString value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_RenderPosScene.SetValue(value);};
			if (keyname == lc_Key_RenderState_Camera)
				{itString value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_RenderPosCamera.SetValue(value);};
			if (keyname == lc_Key_RenderState_Time)
				{float value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_RenderPosTime.SetValue(maTime::FromSeconds(value));};
		}
	}

	guiXMLTextReader::Close();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReadData(	fsLocator& i_ConfigFile,
				captRenderOutputData& o_Data )
{
	//l_bReadingData = true;

	o_Data.m_RenderPosTime.SetValue(captRenderOutputData::c_InitRenderPosTime);

	if (fsFileUtil::FileExists(i_ConfigFile))
	{
		ReadFromConfigFile(i_ConfigFile, o_Data);
	}

	//l_bReadingData = false;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void WriteDataToConfigFile( fsLocator& i_DataFile,
							const captRenderOutputData& i_Data )
{
	//	if the code is in the middle of reading the file, don't allow writing.
	//if (l_bReadingData)
	//	return;

	fsLocator cfgdir = i_DataFile;
	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);


	//	quick and dirty XML Writer
	guiXMLTextWriter::Open(cfgpath.c_str());
	guiXMLTextWriter::WriteStartElement("RenderState");

	//	write the actual values
	guiXMLTextWriter::WriteElement(lc_Key_RenderState_Scene, i_Data.m_RenderPosScene.GetValue());
	guiXMLTextWriter::WriteElement(lc_Key_RenderState_Camera, i_Data.m_RenderPosCamera.GetValue());
	guiXMLTextWriter::WriteElement(lc_Key_RenderState_Time, i_Data.m_RenderPosTime.GetValue().AsSeconds());

	//	finish it up
	guiXMLTextWriter::WriteEndElement();
	guiXMLTextWriter::Close();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void WriteData( fsLocator& i_ConfigFile,
				const captRenderOutputData& i_Data )
{
	//	if the code is in the middle of reading the file, don't allow writing.
	//if (l_bReadingData)
	//	return;

	WriteDataToConfigFile( i_ConfigFile, i_Data );
}

}	// end of namespace

