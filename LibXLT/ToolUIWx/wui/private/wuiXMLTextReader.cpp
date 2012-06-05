/*****************************************************************************
**  wuiXMLTextReader.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiXMLTextReader.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsXMLReader.hpp"

#include "Tool/gui/guiMessageBox.hpp"

#include <string>
#include <sstream>
#include <iostream>

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//	namespace
//	{
	//
	namespace XMLReader
	{
		static fsXMLReader* m_pReader = NULL;
	};

//	}

//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
bool  wuiXMLTextReader::Open(const char* i_Filename)
{
	std::string error_msg = "Missing file from MSPro. The program cannot continue. Please reinstall the application.";
	try
	{
		DBG_ASSERT(XMLReader::m_pReader == NULL, "Must Close() the previous XML Reader");
		XMLReader::m_pReader = new fsXMLReader(i_Filename);
		XMLReader::m_pReader->Open();
	}
	catch (fsInvalidLocatorX& /*i_Ex*/)
	{
		guiMessageBox::Show(error_msg.c_str(), "Error");
		exit(1);
		return false;
	}
	catch (fsFileDoesntExistX& /*i_Ex*/)
	{
		guiMessageBox::Show(error_msg.c_str(), "Error");
		exit(1);
		return false;
	}
	return true;
}

//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
bool wuiXMLTextReader::Open(const fsLocator& i_Filename)
{
	std::string error_msg = "Missing file from MSPro. The program cannot continue. Please reinstall the application.";
	try
	{
		DBG_ASSERT(XMLReader::m_pReader == NULL, "Must Close() the previous XML Reader");
		XMLReader::m_pReader = new fsXMLReader(i_Filename);
		XMLReader::m_pReader->Open();
	}
	catch (fsInvalidLocatorX& /*i_Ex*/)
	{
		guiMessageBox::Show(error_msg.c_str(), "Error");
		exit(1);
		return false;
	}
	catch (fsFileDoesntExistX& /*i_Ex*/)
	{
		guiMessageBox::Show(error_msg.c_str(), "Error");
		exit(1);
		return false;
	}
	return true;
}

//------------------------------------------------------------------------
//	finalize + close the XML file
//------------------------------------------------------------------------
void wuiXMLTextReader::Close()
{
	DBG_ASSERT(XMLReader::m_pReader != NULL, "Must Open() the Reader");

	XMLReader::m_pReader->Close();
	delete XMLReader::m_pReader;
	XMLReader::m_pReader = NULL;
}

//------------------------------------------------------------------------
//	Read the actual values
//------------------------------------------------------------------------
guiXMLTextReader::gui_Node_Type wuiXMLTextReader::ReadNode(std::string& o_KeyName, std::string& o_Text)
{
	//	TODO - this requires fs_Node_Type and gui_Node_Type to match
	return (guiXMLTextReader::gui_Node_Type)XMLReader::m_pReader->ReadNode(o_KeyName, o_Text);
}
