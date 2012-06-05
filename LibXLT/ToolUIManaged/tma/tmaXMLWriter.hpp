#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaXMLWriter.hpp
//**
//**      XML file writer
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_XMLWRITER_HPP
//#error tmaXMLWriter multiply included
//#endif
//#define TMA_XMLWRITER_HPP
//
//#ifndef FS_LOCATOR_HPP
//#include "Core/fs/fsLocator.hpp"
//#endif
//#ifndef IT_STRING_HPP
//#include "Core/it/itString.hpp"
//#endif
//#ifndef TMA_MANAGEDSTRINGUTILS_HPP
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//#endif
//
//#include <string>
//
//
//#ifdef _MANAGED
////============================================================================
////	forward references
////============================================================================
//using namespace System::Xml;
//
//
////============================================================================
////	Managed XML writer
////============================================================================
//public ref class tmaXMLWriter abstract
//{
//public:
//	//------------------------------------------------------------------------
//	//	constructor
//	//------------------------------------------------------------------------
//	tmaXMLWriter(System::String^ i_Filename)
//	:	m_pWriterSet(nullptr),
//		m_pWriter(nullptr)
//	{
//		m_Filename = gcnew System::String(i_Filename);
//	}
//	tmaXMLWriter(const char* i_Filename)
//	:	m_pWriterSet(nullptr),
//		m_pWriter(nullptr)
//	{
//		m_Filename = gcnew System::String(i_Filename);
//	}
//
//	//------------------------------------------------------------------------
//	//	destructor
//	//------------------------------------------------------------------------
//	~tmaXMLWriter()
//	{
//		free_resources();
//
//		delete m_Filename;
//	}
//
//	//------------------------------------------------------------------------
//	//	open + create the XML file
//	//------------------------------------------------------------------------
//	virtual void Open() = 0;
//
//	//------------------------------------------------------------------------
//	//	finalize + close the XML file
//	//------------------------------------------------------------------------
//	void Close()
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->Close();
//
//		free_resources();
//	}
//
//	//------------------------------------------------------------------------
//	//	Every XML file needs one start element.  This happens BEFORE any
//	//	elements are written.  At the end of the elements call the matching
//	//	WriteEndElement.
//	//------------------------------------------------------------------------
//	void WriteStartElement(System::String^ i_Element)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		//	write the preferences
//		m_pWriter->WriteStartElement(i_Element);
//	}
//
//	//------------------------------------------------------------------------
//	//	Every Start element needs a matching end element.
//	//------------------------------------------------------------------------
//	void WriteEndElement()
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteEndElement();
//	}
//
//	//------------------------------------------------------------------------
//	//	write the actual values
//	//
//	//	other possible elements to implement are:
//	//		maVector3d
//	//		maRotation
//	//		maRect
//	//		maPoint3d
//	//		maMatrix4x4
//	//		maFloatRGBA
//	//		maAngle
//	//		itString
//	//		nameString
//	//------------------------------------------------------------------------
//	void WriteElement(System::String^ i_KeyName, int i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	void WriteElement(System::String^ i_KeyName, bool i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	void WriteElement(System::String^ i_KeyName, short i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	void WriteElement(System::String^ i_KeyName, float i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	void WriteElement(System::String^ i_KeyName, envType::Int8 i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString( (envType::Int8)i_Value));
//	}
//	void WriteElement(System::String^ i_KeyName, envType::UInt8 i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	//void WriteElement(System::String^ i_KeyName, envType::Int16 i_Value)
//	//{
//	//	DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//	//	m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	//}
//	void WriteElement(System::String^ i_KeyName, envType::UInt16 i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	//void WriteElement(System::String^ i_KeyName, envType::Int32 i_Value)
//	//{
//	//	DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//	//	m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	//}
//	void WriteElement(System::String^ i_KeyName, envType::UInt32 i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	void WriteElement(System::String^ i_KeyName, envType::Int64 i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	void WriteElement(System::String^ i_KeyName, envType::UInt64 i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	//void WriteElement(System::String^ i_KeyName, envType::Float32 i_Value)
//	//{
//	//	DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//	//	m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	//}
//	void WriteElement(System::String^ i_KeyName, envType::Float64 i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), XmlConvert::ToString(i_Value));
//	}
//	void WriteElement(System::String^ i_KeyName, const char * i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), gcnew System::String(i_Value));
//	}
//	void WriteElement(System::String^ i_KeyName, const std::string& i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), gcnew System::String(i_Value.c_str()));
//	}
//	void WriteElement(System::String^ i_KeyName, System::String^ i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(convert_keyname(i_KeyName), i_Value);
//	}
//	void WriteElement(const char* i_KeyName, const fsLocator& i_Value)
//	{
//		DBG_ASSERT0(m_pWriter != nullptr, "Must Open() the Writer");
//		m_pWriter->WriteElementString(gcnew System::String(i_KeyName), tmaManagedStringUtils::LocatorToManagedString(i_Value));
//	}
//
//private:
//	//------------------------------------------------------------------------
//	//------------------------------------------------------------------------
//	System::String^ convert_keyname(System::String^ i_KeyName)
//	{
//		//	take out spaces in the element name because it isn't allowed.
//		//	the read will reverse this.
//		//
//		System::String^ keystr = gcnew System::String(i_KeyName);
//		keystr = keystr->Replace(' ','_');				// elements cannot have spaces in them
//		return keystr;
//	}
//
//	//------------------------------------------------------------------------
//	//------------------------------------------------------------------------
//	void free_resources()
//	{
//		if (m_pWriterSet != nullptr)
//		{
//			delete m_pWriterSet;
//			m_pWriterSet = nullptr;
//		}
//		if (m_pWriter != nullptr)
//		{
//			delete m_pWriter;
//			m_pWriter = nullptr;
//		}
//	}
//
//protected:
//	//
//	//	member variables
//	//
//	System::String^ m_Filename;
//	XmlWriterSettings^ m_pWriterSet;
//	XmlWriter^ m_pWriter;
//};
//
//#endif // _MANAGED
