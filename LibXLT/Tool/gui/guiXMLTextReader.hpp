/*****************************************************************************
**	guiXMLTextReader.hpp
**
**		non-managed interface for XML text file Reader
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_XMLTEXTREADER_HPP
#error guiXMLTextReader multiply included
#endif
#define GUI_XMLTEXTREADER_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif

#include <string>


//============================================================================
// forward declaration
//============================================================================
class fsLocator;
class guiXMLTextReaderImpl;


//============================================================================
// static functions define API
//============================================================================
class guiXMLTextReader : public envAbstraction<guiXMLTextReaderImpl>
{
public:
	//------------------------------------------------------------------------
	//	matches tma type list
	//------------------------------------------------------------------------
	enum gui_Node_Type
	{
		e_Element = 0,
		e_Text,
		e_EndElement,
		e_None,
		e_EOF
	};

	//------------------------------------------------------------------------
	//	open the XML file, returns false if file does not exist
	//------------------------------------------------------------------------
	static bool Open(const char* i_Filename);
	static bool Open(const fsLocator& i_Filename);

	//------------------------------------------------------------------------
	//	finalize + close the XML file
	//------------------------------------------------------------------------
	static void Close();

	//------------------------------------------------------------------------
	//	Read the actual values
	//------------------------------------------------------------------------
	static gui_Node_Type ReadNode(std::string& o_KeyName, std::string& o_Text);

	//------------------------------------------------------------------------
	//	Convert values 
	//
	//	other possible elements to implement are:
	//		maVector3d
	//		maRotation
	//		maPoint3d
	//		maMatrix4x4
	//		itString
	//		fsLocator
	//		nameString
	//------------------------------------------------------------------------
	static void Convert(const std::string& i_Text, int& o_Value);
	static void Convert(const std::string& i_Text, bool& o_Value);
	static void Convert(const std::string& i_Text, short& o_Value);
	static void Convert(const std::string& i_Text, float& o_Value);
	static void Convert(const std::string& i_Text, envType::Int8& o_Value);
	static void Convert(const std::string& i_Text, envType::UInt8& o_Value);
	//static void Convert(const std::string& i_Text, envType::Int16& o_Value);
	static void Convert(const std::string& i_Text, envType::UInt16& o_Value);
	//static void Convert(const std::string& i_Text, envType::Int32& o_Value);
	static void Convert(const std::string& i_Text, envType::UInt32& o_Value);
	static void Convert(const std::string& i_Text, envType::Int64& o_Value);
	static void Convert(const std::string& i_Text, envType::UInt64& o_Value);
	//static void Convert(const std::string& i_Text, envType::Float32& o_Value);
	static void Convert(const std::string& i_Text, envType::Float64& o_Value);
	static void Convert(const std::string& i_Text, char * o_Value);
	static void Convert(const std::string& i_Text, std::string& o_Value);
	static void Convert(const std::string& i_Text, fsLocator& o_Value);
	static void Convert(const std::string& i_Text, itString& o_Value);
	static void Convert(const std::string& i_Text, maFloatRGBA& o_Value);

};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiXMLTextReaderImpl
{
public:
	
	//------------------------------------------------------------------------
	//	open the XML file, returns false if file does not exist
	//------------------------------------------------------------------------
	virtual bool Open(const char* i_Filename) = 0;
	virtual bool Open(const fsLocator& i_Filename) = 0;

	//------------------------------------------------------------------------
	//	finalize + close the XML file
	//------------------------------------------------------------------------
	virtual void Close() = 0;

	//------------------------------------------------------------------------
	//	Read the actual values
	//------------------------------------------------------------------------
	virtual guiXMLTextReader::gui_Node_Type ReadNode(std::string& o_KeyName, std::string& o_Text) = 0;

};
