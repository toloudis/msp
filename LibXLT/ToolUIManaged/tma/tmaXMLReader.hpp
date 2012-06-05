#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaXMLReader.hpp
//**
//**      XML file Reader
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_XMLREADER_HPP
//#error tmaXMLReader multiply included
//#endif
//#define TMA_XMLREADER_HPP
//
//#ifndef IT_STRING_HPP
//#include "Core/it/itString.hpp"
//#endif
//
//#ifndef TMA_MANAGEDSTRINGUTILS_HPP
//#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
//#endif
//
//#ifndef FS_FILEX_HPP
//#include "Core/fs/fsFileX.hpp"
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
////============================================================================
//namespace tmaXMLData
//{
//	enum tma_Node_Type
//	{
//		e_Element = 0,
//		e_Text,
//		e_EndElement,
//		e_None,
//		e_EOF
//	};
//}
//
////============================================================================
////	Managed XML Reader
////============================================================================
//public ref class tmaXMLReader abstract
//{
//public:
//	//------------------------------------------------------------------------
//	//	constructor
//	//------------------------------------------------------------------------
//	tmaXMLReader(System::String^ i_Filename)
//	:	m_pReaderSet(nullptr),
//		m_pReader(nullptr)
//	{
//		m_Filename = gcnew System::String(i_Filename);
//	}
//	tmaXMLReader(const char* i_Filename)
//	:	m_pReaderSet(nullptr),
//		m_pReader(nullptr)
//	{
//		m_Filename = gcnew System::String(i_Filename);
//	}
//
//	//------------------------------------------------------------------------
//	//	destructor
//	//------------------------------------------------------------------------
//	~tmaXMLReader()
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
//		DBG_ASSERT0(m_pReader != nullptr, "Must Open() the Reader");
//		m_pReader->Close();
//
//		free_resources();
//	}
//
//	//------------------------------------------------------------------------
//	//	Read the actual values
//	//------------------------------------------------------------------------
//	tmaXMLData::tma_Node_Type ReadNode(std::string& o_KeyName, std::string& o_Text)
//	{
//		DBG_ASSERT0(m_pReader != nullptr, "Must Open() the Reader");
//
//		try
//		{
//			bool data_read = m_pReader->Read();
//			if (!data_read)
//				return tmaXMLData::e_EOF;
//
//			//
//			if (m_pReader->NodeType == System::Xml::XmlNodeType::Element)
//			{
//				System::String^ namestr = m_pReader->Name;
//				namestr = namestr->Replace('_',' ');	// elements cannot have spaces, so remove the underscores
//				tmaManagedStringUtils::ManagedStringToStdString(namestr, o_KeyName);
//				return tmaXMLData::e_Element;
//			}
//			else if (m_pReader->NodeType == System::Xml::XmlNodeType::Text)
//			{
//				System::String^ valuestr = m_pReader->Value;
//				tmaManagedStringUtils::ManagedStringToStdString(valuestr, o_Text);
//				return tmaXMLData::e_Text;
//			}
//			else if (m_pReader->NodeType == System::Xml::XmlNodeType::EndElement)
//			{
//				return tmaXMLData::e_EndElement;
//			}
//		}
//		catch(XmlException^)   // Handle the XML exceptions here.   
//		{
//			fsLocator noname;
//			throw fsXMLErrorX( noname );
//		}
//		return tmaXMLData::e_None;
//	}
//
//	//------------------------------------------------------------------------
//	//	Convert values 
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
//	//		fsLocator
//	//		nameString
//	//------------------------------------------------------------------------
//	void Convert(const std::string& i_Text, int& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToInt32(gcnew System::String(i_Text.c_str()));
//	}
//	void Convert(const std::string& i_Text, bool& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToBoolean(gcnew System::String(i_Text.c_str()));
//	}
//	void Convert(const std::string& i_Text, short& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToInt16(gcnew System::String(i_Text.c_str()));
//	}
//	void Convert(const std::string& i_Text, float& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToSingle(gcnew System::String(i_Text.c_str()));
//	}
//	void Convert(const std::string& i_Text, envType::Int8& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToByte(gcnew System::String(i_Text.c_str()));
//	}
//	void Convert(const std::string& i_Text, envType::UInt8& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToChar(gcnew System::String(i_Text.c_str()));
//	}
//	//void Convert(const std::string& i_Text, envType::Int16& o_Value)
//	//{
//	//	if (i_Text.length() > 0) o_Value = XmlConvert::ToInt16(gcnew System::String(i_Text.c_str()));
//	//}
//	void Convert(const std::string& i_Text, envType::UInt16& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToUInt16(gcnew System::String(i_Text.c_str()));
//	}
//	//void Convert(const std::string& i_Text, envType::Int32& o_Value)
//	//{
//	//	if (i_Text.length() > 0) o_Value = XmlConvert::ToInt32(gcnew System::String(i_Text.c_str()));
//	//}
//	void Convert(const std::string& i_Text, envType::UInt32& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToUInt32(gcnew System::String(i_Text.c_str()));
//	}
//	void Convert(const std::string& i_Text, envType::Int64& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToInt64(gcnew System::String(i_Text.c_str()));
//	}
//	void Convert(const std::string& i_Text, envType::UInt64& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToUInt64(gcnew System::String(i_Text.c_str()));
//	}
//	//void Convert(const std::string& i_Text, envType::Float32& o_Value)
//	//{
//	//	if (i_Text.length() > 0) o_Value = XmlConvert::ToDecimal(gcnew System::String(i_Text.c_str()));
//	//}
//	void Convert(const std::string& i_Text, envType::Float64& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = XmlConvert::ToDouble(gcnew System::String(i_Text.c_str()));
//	}
//	void Convert(const std::string& i_Text, char * o_Value)
//	{
//		if (i_Text.length() > 0) strcpy(o_Value, i_Text.c_str());
//	}
//	void Convert(const std::string& i_Text, std::string& o_Value)
//	{
//		if (i_Text.length() > 0) o_Value = i_Text;
//	}
//
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
//		if (m_pReaderSet != nullptr)
//		{
//			delete m_pReaderSet;
//			m_pReaderSet = nullptr;
//		}
//		if (m_pReader != nullptr)
//		{
//			delete m_pReader;
//			m_pReader = nullptr;
//		}
//	}
//
//protected:
//	//
//	//	member variables
//	//
//	System::String^ m_Filename;
//	XmlReaderSettings^ m_pReaderSet;
//	XmlReader^ m_pReader;
//};
//
//#endif // _MANAGED
