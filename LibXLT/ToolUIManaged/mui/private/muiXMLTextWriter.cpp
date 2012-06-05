/*****************************************************************************
**  muiXMLTextWriter.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/mui/muiXMLTextWriter.hpp"

#include "Core/fs/fsXMLWriter.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
namespace
{
	namespace XMLWriter
	{
		static fsXMLWriter* m_pWriter;
	};
}

//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
void muiXMLTextWriter::Open(const char* i_Filename)
{
	XMLWriter::m_pWriter = new fsXMLWriter(i_Filename);
	XMLWriter::m_pWriter->Open();
}

//------------------------------------------------------------------------
//	finalize + close the XML file
//------------------------------------------------------------------------
void muiXMLTextWriter::Close()
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->Close();
	delete XMLWriter::m_pWriter;
}

//------------------------------------------------------------------------
//	Every XML file needs one start element.  This happens BEFORE any
//	elements are written.  At the end of the elements call the matching
//	WriteEndElement.
//------------------------------------------------------------------------
void muiXMLTextWriter::WriteStartElement(const char* i_Element)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");

	//	write the preferences
	XMLWriter::m_pWriter->WriteStartElement( std::string(i_Element) );
}

//------------------------------------------------------------------------
//	Every Start element needs a matching end element.
//------------------------------------------------------------------------
void muiXMLTextWriter::WriteEndElement()
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteEndElement();
}

//------------------------------------------------------------------------
//	write the actual values
//
//	other possible elements to implement are:
//		maVector3d
//		maRotation
//		maRect
//		maPoint3d
//		maMatrix4x4
//		maFloatRGBA
//		maAngle
//		itString
//		nameString
//------------------------------------------------------------------------
void muiXMLTextWriter::WriteElement(const char* i_KeyName, int i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement( std::string(i_KeyName), i_Value );
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, bool i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, short i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, float i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Int8 i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::UInt8 i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

//void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Int16 i_Value)
//{

//}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::UInt16 i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

//void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Int32 i_Value)
//{
//}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::UInt32 i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Int64 i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::UInt64 i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

//void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Float32 i_Value)
//{
//}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Float64 i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, const char * i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), (i_Value));
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, const std::string& i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), (i_Value.c_str()));
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, const fsLocator& i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, const itString& i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}

void muiXMLTextWriter::WriteElement(const char* i_KeyName, const maFloatRGBA& i_Value)
{
	DBG_ASSERT0(XMLWriter::m_pWriter != NULL, "Must Open() the Writer");
	XMLWriter::m_pWriter->WriteElement(std::string(i_KeyName), i_Value);
}
