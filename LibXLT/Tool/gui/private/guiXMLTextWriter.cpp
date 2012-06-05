/*****************************************************************************
**	guiXMLTextWriter.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiXMLTextWriter.hpp"


//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
void guiXMLTextWriter::Open(const char* i_Filename)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Open(i_Filename);
	}
}

//------------------------------------------------------------------------
//	open + create the XML file
//------------------------------------------------------------------------
void guiXMLTextWriter::Open(const fsLocator& i_Filename)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Open(i_Filename);
	}
}

//------------------------------------------------------------------------
//	finalize + close the XML file
//------------------------------------------------------------------------
void guiXMLTextWriter::Close()
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->Close();
	}
}

//------------------------------------------------------------------------
//	Every XML file needs one start element.  This happens BEFORE any
//	elements are written.  At the end of the elements call the matching
//	WriteEndElement.
//------------------------------------------------------------------------
void guiXMLTextWriter::WriteStartElement(const char* i_Element)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteStartElement(i_Element);
	}
}

//------------------------------------------------------------------------
//	Every Start element needs a matching end element.
//------------------------------------------------------------------------
void guiXMLTextWriter::WriteEndElement()
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteEndElement();
	}
}

//------------------------------------------------------------------------
//	write the actual values
//
//	other possible elements to implement are:
//		maVector3d
//		maRotation
//		maPoint3d
//		maMatrix4x4
//		maFloatRGBA
//		itString
//		nameString
//------------------------------------------------------------------------
void guiXMLTextWriter::WriteElement(const char* i_KeyName, int i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, bool i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, short i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, float i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Int8 i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::UInt8 i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
//void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Int16 i_Value)
//{
//}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::UInt16 i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
//void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Int32 i_Value)
//{
//}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::UInt32 i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Int64 i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::UInt64 i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
//void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Float32 i_Value)
//{
//}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, envType::Float64 i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, const char * i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, const std::string& i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, const fsLocator& i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, const itString& i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}
void guiXMLTextWriter::WriteElement(const char* i_KeyName, const maFloatRGBA& i_Value)
{
	DBG_ASSERT(sm_pImplementation, "guiXMLTextWriter: No implementation");
	if (sm_pImplementation)
	{
		sm_pImplementation->WriteElement(i_KeyName, i_Value);
	}
}

