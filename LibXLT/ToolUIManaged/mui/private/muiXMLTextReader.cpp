/*****************************************************************************
**  muiXMLTextReader.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/mui/muiXMLTextReader.hpp"

#include "Core/fs/fsFileUtil.hpp"

#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsXMLReader.hpp"

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
		static fsXMLReader* m_pReader;
	};

	//
	template <class T>
	bool from_string(T& t, 
			         const std::string& s) //, std::ios_base& (*f)(std::ios_base&))
	{
	  std::istringstream iss(s);
	  return !(iss >> std::dec >> t).fail();
	};

	bool boolfrom_string(bool& t, 
						const std::string& s)
	{
	  std::istringstream iss(s);
	  return !(iss >> std::boolalpha >> t).fail();
	};

//	}

//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
bool  muiXMLTextReader::Open(const char* i_Filename)
{
	try
	{
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
void muiXMLTextReader::Close()
{
	DBG_ASSERT0(XMLReader::m_pReader != NULL, "Must Open() the Reader");

	XMLReader::m_pReader->Close();
	delete XMLReader::m_pReader;
}

//------------------------------------------------------------------------
//	Read the actual values
//------------------------------------------------------------------------
guiXMLTextReader::gui_Node_Type muiXMLTextReader::ReadNode(std::string& o_KeyName, std::string& o_Text)
{
	//	TODO - this requires fs_Node_Type and gui_Node_Type to match
	return (guiXMLTextReader::gui_Node_Type)XMLReader::m_pReader->ReadNode(o_KeyName, o_Text);
}
