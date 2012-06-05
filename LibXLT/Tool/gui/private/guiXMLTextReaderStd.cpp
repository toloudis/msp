/*****************************************************************************
**	guiXMLTextReaderStd.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiXMLTextReaderStd.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsXMLReader.hpp"

#include <string>
#include <sstream>
#include <iostream>


//============================================================================
//============================================================================
namespace XMLReader
{
	static fsXMLReader* m_pReader = NULL;
};


//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
bool  guiXMLTextReaderStd::Open(const char* i_Filename)
{
	try
	{
		DBG_ASSERT(XMLReader::m_pReader == NULL, "Must Close() the previous XML Reader");
		XMLReader::m_pReader = new fsXMLReader(i_Filename);
		XMLReader::m_pReader->Open();
	}
	catch (fsInvalidLocatorX& /*i_Ex*/)
	{
		return false;
	}
	catch (fsFileDoesntExistX& /*i_Ex*/)
	{
		return false;
	}
	return true;
}


//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
bool  guiXMLTextReaderStd::Open(const fsLocator& i_Filename)
{
	try
	{
		DBG_ASSERT(XMLReader::m_pReader == NULL, "Must Close() the previous XML Reader");
		XMLReader::m_pReader = new fsXMLReader(i_Filename);
		XMLReader::m_pReader->Open();
	}
	catch (fsInvalidLocatorX& /*i_Ex*/)
	{
		return false;
	}
	catch (fsFileDoesntExistX& /*i_Ex*/)
	{
		return false;
	}
	return true;
}

//------------------------------------------------------------------------
//	finalize + close the XML file
//------------------------------------------------------------------------
void guiXMLTextReaderStd::Close()
{
	DBG_ASSERT(XMLReader::m_pReader != NULL, "Must Open() the Reader");

	XMLReader::m_pReader->Close();
	delete XMLReader::m_pReader;
	XMLReader::m_pReader = NULL;
}

//------------------------------------------------------------------------
//	Read the actual values
//------------------------------------------------------------------------
guiXMLTextReader::gui_Node_Type guiXMLTextReaderStd::ReadNode(std::string& o_KeyName, std::string& o_Text)
{
	//	TODO - this requires fs_Node_Type and gui_Node_Type to match
	return (guiXMLTextReader::gui_Node_Type)XMLReader::m_pReader->ReadNode(o_KeyName, o_Text);
}
