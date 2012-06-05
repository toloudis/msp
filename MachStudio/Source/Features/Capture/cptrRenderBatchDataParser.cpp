/********************************************************************************************\
**  cptrRenderBatchDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Features/Capture/cptrRenderBatchDataParser.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"


//============================================================================
//============================================================================
namespace cptrRenderBatchDataParser
{
namespace
{
const char* lc_Key_NumScenes = "NumScenes";
const char* lc_Key_SceneFileName = "SceneFName";
const char* lc_Key_SceneChecked = "Checked";
const char* lc_Key_SkipSceneWithErrors = "SkipSceneWithErrors";
}	// local namespace


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReadFromConfigFile(const fsLocator& i_ConfigFile,
						cptrRenderBatchData& o_Data )
{
	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

	guiXMLTextReader::Open(cfgpath.c_str());

	//	read in the preferences
	//
	guiXMLTextReader::gui_Node_Type node_type;
	std::string keyname, strvalue;
	int i=0;
	while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
	{
		if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
		{
			if (keyname == lc_Key_NumScenes)
				{int value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_Scenes.resize(value);};
			if (keyname == lc_Key_SceneFileName)
				{fsLocator value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_Scenes[i].m_Filename.SetValue(value);};
			if (keyname == lc_Key_SceneChecked)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_Scenes[i].m_bChecked.SetValue(value);++i;};
			if (keyname == lc_Key_SkipSceneWithErrors)
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_SkipDialog.SetValue(value);};
		}
	}

	guiXMLTextReader::Close();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReadData(	const fsLocator& i_ConfigFile,
				cptrRenderBatchData& o_Data )
{
	//l_bReadingData = true;

	std::string cfgpath;
	fsLocator cfgdir;

	cfgdir = i_ConfigFile;
	//cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	//cfgdir.Push( i_ConfigFile );

	//std::string filename;
	//fsFileUtil::LocatorToANSIFilename( cfgdir, filename );
	//DBG_LOG( "reading batch file (" << filename.c_str() << ")" );

	if (fsFileUtil::FileExists(cfgdir))
	{
		ReadFromConfigFile(cfgdir, o_Data);
	}

	//l_bReadingData = false;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void WriteDataToConfigFile( const fsLocator& i_DataFile,
							const cptrRenderBatchData& i_Data )
{
	//	if the code is in the middle of reading the file, don't allow writing.
	//if (l_bReadingData)
	//	return;

	fsLocator cfgdir;
	cfgdir = i_DataFile;
	//cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	//cfgdir.Push(i_DataFile);
	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);

	//	quick and dirty XML Writer
	guiXMLTextWriter::Open(cfgpath.c_str());
	guiXMLTextWriter::WriteStartElement("RenderBatch");

	//	write the actual values
	guiXMLTextWriter::WriteElement(lc_Key_SkipSceneWithErrors, i_Data.m_SkipDialog.GetValue());
	guiXMLTextWriter::WriteElement(lc_Key_NumScenes, i_Data.m_Scenes.size());
	for (int i=0; i<i_Data.m_Scenes.size(); i++)
	{
		guiXMLTextWriter::WriteElement(lc_Key_SceneFileName, i_Data.m_Scenes[i].m_Filename.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_SceneChecked, i_Data.m_Scenes[i].m_bChecked.GetValue());
	}

	//	finish it up
	guiXMLTextWriter::WriteEndElement();
	guiXMLTextWriter::Close();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void WriteData( const fsLocator& i_DataFile,
				const cptrRenderBatchData& i_Data )
{
	//	if the code is in the middle of reading the file, don't allow writing.
	//if (l_bReadingData)
	//	return;

	WriteDataToConfigFile( i_DataFile, i_Data );
}


}	// end of namespace

