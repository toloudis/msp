/*****************************************************************************
**	guiXMLTextReader.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Core/env/envNumberText.hpp"

#include "Core/fs/fsFileUtil.hpp"

#include <string>
#include <sstream>
#include <iostream>


//============================================================================
//============================================================================
namespace
{
	//
	bool boolfrom_string(bool& t, 
						const std::string& s)
	{
		std::istringstream iss(s);
		return !(iss >> std::boolalpha >> t).fail();
	};
}

//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
bool guiXMLTextReader::Open(const char* i_Filename)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextReader: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->Open(i_Filename);
	}
	return false;
}

//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
bool guiXMLTextReader::Open(const fsLocator& i_Filename)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextReader: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->Open(i_Filename);
	}
	return false;
}

//------------------------------------------------------------------------
//	finalize + close the XML file
//------------------------------------------------------------------------
void guiXMLTextReader::Close()
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextReader: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->Close();
	}
}

//------------------------------------------------------------------------
//	Read the actual values
//------------------------------------------------------------------------
guiXMLTextReader::gui_Node_Type guiXMLTextReader::ReadNode(std::string& o_KeyName, std::string& o_Text)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextReader: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->ReadNode(o_KeyName, o_Text);
	}
	return e_EOF;
}

//------------------------------------------------------------------------
//	Convert values 
//
//	other possible elements to implement are:
//		maVector3d
//		maRotation
//		maPoint3d
//		maMatrix4x4
//		maFloatRGBA
//		itString
//		fsLocator
//		nameString
//------------------------------------------------------------------------
void guiXMLTextReader::Convert(const std::string& i_Text, int& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
void guiXMLTextReader::Convert(const std::string& i_Text, bool& o_Value)
{
	if (i_Text.length() > 0) boolfrom_string(o_Value, i_Text);
}
void guiXMLTextReader::Convert(const std::string& i_Text, short& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
void guiXMLTextReader::Convert(const std::string& i_Text, float& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
void guiXMLTextReader::Convert(const std::string& i_Text, int8_t& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
void guiXMLTextReader::Convert(const std::string& i_Text, uint8_t& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
//void guiXMLTextReader::Convert(const std::string& i_Text, int16_t& o_Value)
//{
//	if (i_Text.length() > 0) from_string<int16_t>(o_Value, i_Text);
//}
void guiXMLTextReader::Convert(const std::string& i_Text, uint16_t& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
//void guiXMLTextReader::Convert(const std::string& i_Text, int32_t& o_Value)
//{
//	if (i_Text.length() > 0) from_string<int32_t>(o_Value, i_Text);
//}
void guiXMLTextReader::Convert(const std::string& i_Text, uint32_t& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
void guiXMLTextReader::Convert(const std::string& i_Text, int64_t& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
void guiXMLTextReader::Convert(const std::string& i_Text, uint64_t& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
//void guiXMLTextReader::Convert(const std::string& i_Text, float& o_Value)
//{
//	if (i_Text.length() > 0) from_string<float>(o_Value, i_Text);
//}
void guiXMLTextReader::Convert(const std::string& i_Text, double& o_Value)
{
	envNumberText::FromText(i_Text, o_Value);
}
void guiXMLTextReader::Convert(const std::string& i_Text, char * o_Value)
{
	if (i_Text.length() > 0) strcpy(o_Value, i_Text.c_str());
}
void guiXMLTextReader::Convert(const std::string& i_Text, std::string& o_Value)
{
	if (i_Text.length() > 0) o_Value = i_Text;
}
void guiXMLTextReader::Convert(const std::string& i_Text, fsLocator& o_Value)
{
	if (i_Text.length() > 0) fsFileUtil::ANSIFilenameToLocator( i_Text, o_Value );
}
void guiXMLTextReader::Convert(const std::string& i_Text, itString& o_Value)
{
	if (i_Text.length() > 0) o_Value = i_Text.c_str();
}
void guiXMLTextReader::Convert(const std::string& i_Text, maFloatRGBA& o_Value)
{
	if (i_Text.length() > 0)
	{
		std::istringstream iss(i_Text);
		float val;
		char c;
		iss >> val;
		o_Value.SetRed( val );
		iss >> c;
		iss >> val;
		o_Value.SetGreen( val );
		iss >> c;
		iss >> val;
		o_Value.SetBlue( val );
		iss >> c;
		iss >> val;
		o_Value.SetAlpha( val );
	}
}