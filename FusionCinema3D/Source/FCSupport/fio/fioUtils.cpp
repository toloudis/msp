/*****************************************************************************
**	fioUtils.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fio/fioUtils.hpp"

#include "FCSupport/fcui/fcuiConstants.hpp"

#include "Core/ch/chDefs.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fioUtils::CopyFile( const fsLocator& i_From, const fsLocator& i_To )
{
	fsLocator dest_folder(i_To);
	dest_folder.Pop();	// pop the file

	if (!fsFileUtil::DirectoryExists(dest_folder))
	{
		fsFileUtil::CreateDirectory(dest_folder);
	}

	DBG_TRACE( "Copy [" << i_From << "] -> [" << i_To << "]");
	fsFileUtil::CopyFile( i_From, i_To );
}

//------------------------------------------------------------------------
///	Write an XML file that stores an absolute location
//------------------------------------------------------------------------
void fioUtils::WriteLocationXML(const fsLocator& i_XMLFile, const fsLocator& i_DataPathToSave)
{
	//	quick and dirty XML Writer
	guiXMLTextWriter::Open(i_XMLFile);
	guiXMLTextWriter::WriteStartElement( fcuiConstants::c_XML_MAIN_TAG );

	//	write the actual values
	guiXMLTextWriter::WriteElement(fcuiConstants::c_XML_FILE_PATH_TAG, i_DataPathToSave);

	//	finish it up
	guiXMLTextWriter::WriteEndElement();
	guiXMLTextWriter::Close();
}

//------------------------------------------------------------------------
///	Read an XML file that stores an absolute location
//------------------------------------------------------------------------
void fioUtils::ReadLocationXML(const fsLocator& i_XMLFile, fsLocator& o_DataPathSaved)
{
	guiXMLTextReader::Open(i_XMLFile);

	//	read in the preferences
	//
	guiXMLTextReader::gui_Node_Type node_type;
	std::string keyname, strvalue;
	while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
	{
		if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
		{
			if (fcuiConstants::c_XML_FILE_PATH_TAG)
				{ guiXMLTextReader::Convert(strvalue, o_DataPathSaved); };
		}
	}

	guiXMLTextReader::Close();
}

