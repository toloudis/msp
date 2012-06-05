/*****************************************************************************
**	guiXMLTextWriterStd.hpp
**
**		non-managed interface for XML text file writer
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_XMLTEXTWRITERSTD_HPP
#error guiXMLTextWriterStd multiply included
#endif
#define GUI_XMLTEXTWRITERSTD_HPP

#ifndef GUI_XMLTEXTWRITER_HPP
#include "Tool/gui/guiXMLTextWriter.hpp"
#endif


//============================================================================
//============================================================================
class guiXMLTextWriterStd : public guiXMLTextWriterImpl
{
	//------------------------------------------------------------------------
	//	open + create the XML file
	//------------------------------------------------------------------------
	virtual void Open(const char* i_Filename);
	virtual void Open(const fsLocator& i_Filename);

	//------------------------------------------------------------------------
	//	finalize + close the XML file
	//------------------------------------------------------------------------
	virtual void Close();

	//------------------------------------------------------------------------
	//	Every XML file needs one start element.  This happens BEFORE any
	//	elements are written.  At the end of the elements call the matching
	//	WriteEndElement.
	//------------------------------------------------------------------------
	virtual void WriteStartElement(const char* i_Element);

	//------------------------------------------------------------------------
	//	Every Start element needs a matching end element.
	//------------------------------------------------------------------------
	virtual void WriteEndElement();

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
	virtual void WriteElement(const char* i_KeyName, int i_Value);
	virtual void WriteElement(const char* i_KeyName, bool i_Value);
	virtual void WriteElement(const char* i_KeyName, short i_Value);
	virtual void WriteElement(const char* i_KeyName, float i_Value);
	virtual void WriteElement(const char* i_KeyName, envType::Int8 i_Value);
	virtual void WriteElement(const char* i_KeyName, envType::UInt8 i_Value);
	//virtual void WriteElement(const char* i_KeyName, envType::Int16 i_Value);
	virtual void WriteElement(const char* i_KeyName, envType::UInt16 i_Value);
	//virtual void WriteElement(const char* i_KeyName, envType::Int32 i_Value);
	virtual void WriteElement(const char* i_KeyName, envType::UInt32 i_Value);
	virtual void WriteElement(const char* i_KeyName, envType::Int64 i_Value);
	virtual void WriteElement(const char* i_KeyName, envType::UInt64 i_Value);
	//virtual void WriteElement(const char* i_KeyName, envType::Float32 i_Value);
	virtual void WriteElement(const char* i_KeyName, envType::Float64 i_Value);
	virtual void WriteElement(const char* i_KeyName, const char * i_Value);
	virtual void WriteElement(const char* i_KeyName, const std::string& i_Value);
	virtual void WriteElement(const char* i_KeyName, const fsLocator& i_Value);
	virtual void WriteElement(const char* i_KeyName, const itString& i_Value);
	virtual void WriteElement(const char* i_KeyName, const maFloatRGBA& i_Value);
};
