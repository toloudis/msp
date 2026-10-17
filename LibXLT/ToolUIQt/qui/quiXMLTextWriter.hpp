/*****************************************************************************
**	quiXMLTextWriter.hpp
**
**		non-managed interface for XML text file writer
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_XMLTEXTWRITER_HPP
#error quiXMLTextWriter multiply included
#endif
#define QUI_XMLTEXTWRITER_HPP

#ifndef GUI_XMLTEXTWRITER_HPP
#include "Tool/gui/guiXMLTextWriter.hpp"
#endif


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
class quiXMLTextWriter : public guiXMLTextWriterImpl
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
	virtual void WriteElement(const char* i_KeyName, int8_t i_Value);
	virtual void WriteElement(const char* i_KeyName, uint8_t i_Value);
	//virtual void WriteElement(const char* i_KeyName, int16_t i_Value);
	virtual void WriteElement(const char* i_KeyName, uint16_t i_Value);
	//virtual void WriteElement(const char* i_KeyName, int32_t i_Value);
	virtual void WriteElement(const char* i_KeyName, uint32_t i_Value);
	virtual void WriteElement(const char* i_KeyName, int64_t i_Value);
	virtual void WriteElement(const char* i_KeyName, uint64_t i_Value);
	//virtual void WriteElement(const char* i_KeyName, float i_Value);
	virtual void WriteElement(const char* i_KeyName, double i_Value);
	virtual void WriteElement(const char* i_KeyName, const char * i_Value);
	virtual void WriteElement(const char* i_KeyName, const std::string& i_Value);
	virtual void WriteElement(const char* i_KeyName, const fsLocator& i_Value);
	virtual void WriteElement(const char* i_KeyName, const itString& i_Value);
	virtual void WriteElement(const char* i_KeyName, const maFloatRGBA& i_Value);
};
