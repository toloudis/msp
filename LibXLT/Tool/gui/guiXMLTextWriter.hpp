/*****************************************************************************
**	guiXMLTextWriter.hpp
**
**		Non-managed interface for XML text file writer
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_XMLTEXTWRITER_HPP
#error guiXMLTextWriter multiply included
#endif
#define GUI_XMLTEXTWRITER_HPP

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
#include "Core/ma/maFloatRGBA.hpp"
#endif

#include <string>


//============================================================================
// forward declaration
//============================================================================
class guiXMLTextWriterImpl;


//============================================================================
// static functions define API
//============================================================================
class guiXMLTextWriter : public envAbstraction<guiXMLTextWriterImpl>
{
public:
	//------------------------------------------------------------------------
	//	open + create the XML file
	//------------------------------------------------------------------------
	static void Open(const char* i_Filename);
	static void Open(const fsLocator& i_Filename);

	//------------------------------------------------------------------------
	//	finalize + close the XML file
	//------------------------------------------------------------------------
	static void Close();

	//------------------------------------------------------------------------
	//	Every XML file needs one start element.  This happens BEFORE any
	//	elements are written.  At the end of the elements call the matching
	//	WriteEndElement.
	//------------------------------------------------------------------------
	static void WriteStartElement(const char* i_Element);

	//------------------------------------------------------------------------
	//	Every Start element needs a matching end element.
	//------------------------------------------------------------------------
	static void WriteEndElement();

	//------------------------------------------------------------------------
	//	write the actual values
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
	static void WriteElement(const char* i_KeyName, int i_Value);
	static void WriteElement(const char* i_KeyName, bool i_Value);
	static void WriteElement(const char* i_KeyName, short i_Value);
	static void WriteElement(const char* i_KeyName, float i_Value);
	static void WriteElement(const char* i_KeyName, envType::Int8 i_Value);
	static void WriteElement(const char* i_KeyName, envType::UInt8 i_Value);
	//static void WriteElement(const char* i_KeyName, envType::Int16 i_Value);
	static void WriteElement(const char* i_KeyName, envType::UInt16 i_Value);
	//static void WriteElement(const char* i_KeyName, envType::Int32 i_Value);
	static void WriteElement(const char* i_KeyName, envType::UInt32 i_Value);
	static void WriteElement(const char* i_KeyName, envType::Int64 i_Value);
	static void WriteElement(const char* i_KeyName, envType::UInt64 i_Value);
	//static void WriteElement(const char* i_KeyName, envType::Float32 i_Value);
	static void WriteElement(const char* i_KeyName, envType::Float64 i_Value);
	static void WriteElement(const char* i_KeyName, const char * i_Value);
	static void WriteElement(const char* i_KeyName, const std::string& i_Value);
	static void WriteElement(const char* i_KeyName, const fsLocator& i_Value);
	static void WriteElement(const char* i_KeyName, const itString& i_Value);
	static void WriteElement(const char* i_KeyName, const maFloatRGBA& i_Value);
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiXMLTextWriterImpl
{
public:	
	//------------------------------------------------------------------------
	//	open + create the XML file
	//------------------------------------------------------------------------
	virtual void Open(const char* i_Filename) = 0;
	virtual void Open(const fsLocator& i_Filename) = 0;

	//------------------------------------------------------------------------
	//	finalize + close the XML file
	//------------------------------------------------------------------------
	virtual void Close() = 0;

	//------------------------------------------------------------------------
	//	Every XML file needs one start element.  This happens BEFORE any
	//	elements are written.  At the end of the elements call the matching
	//	WriteEndElement.
	//------------------------------------------------------------------------
	virtual void WriteStartElement(const char* i_Element) = 0;

	//------------------------------------------------------------------------
	//	Every Start element needs a matching end element.
	//------------------------------------------------------------------------
	virtual void WriteEndElement() = 0;

	//------------------------------------------------------------------------
	//	write the actual values
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
	virtual void WriteElement(const char* i_KeyName, int i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, bool i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, short i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, float i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, envType::Int8 i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, envType::UInt8 i_Value) = 0;
	//virtual void WriteElement(const char* i_KeyName, envType::Int16 i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, envType::UInt16 i_Value) = 0;
	//virtual void WriteElement(const char* i_KeyName, envType::Int32 i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, envType::UInt32 i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, envType::Int64 i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, envType::UInt64 i_Value) = 0;
	//virtual void WriteElement(const char* i_KeyName, envType::Float32 i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, envType::Float64 i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, const char * i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, const std::string& i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, const fsLocator& i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, const itString& i_Value) = 0;
	virtual void WriteElement(const char* i_KeyName, const maFloatRGBA& i_Value) = 0;
};