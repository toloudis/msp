/*****************************************************************************
**	guiXMLTextReaderStd.hpp
**
**		Non-managed interface for XML text file Reader
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_XMLTEXTREADERSTD_HPP
#error guiXMLTextReaderStd multiply included
#endif
#define GUI_XMLTEXTREADERSTD_HPP

#ifndef GUI_XMLTEXTREADER_HPP
#include "Tool/gui/guiXMLTextReader.hpp"
#endif


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
class guiXMLTextReaderStd : public guiXMLTextReaderImpl
{
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
	virtual bool Open(const char* i_Filename);
	virtual bool Open(const fsLocator& i_Filename);

	//------------------------------------------------------------------------
	//	finalize + close the XML file
	//------------------------------------------------------------------------
	virtual void Close();

	//------------------------------------------------------------------------
	//	Read the actual values
	//------------------------------------------------------------------------
	virtual guiXMLTextReader::gui_Node_Type ReadNode(std::string& o_KeyName, std::string& o_Text);
};
