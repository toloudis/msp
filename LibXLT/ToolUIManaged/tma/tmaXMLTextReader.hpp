#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaXMLTextReader.hpp
//**
//**      XML Text file Reader
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_XMLTEXTREADER_HPP
//#error tmaXMLTextReader multiply included
//#endif
//#define TMA_XMLTEXTREADER_HPP
//
//#ifndef TMA_XMLReader_HPP
//#include "ToolUIManaged/tma/tmaXMLReader.hpp"
//#endif
//
//#ifndef IT_STRING_HPP
//#include "Core/it/itString.hpp"
//#endif
//
//#include "Tool/gui/guiMessageBox.hpp"
//#ifdef _MANAGED
////============================================================================
////	forward references
////============================================================================
//using namespace System::Xml;
//
//
////============================================================================
////	Managed XML Reader
////============================================================================
//public ref class tmaXMLTextReader: public tmaXMLReader
//{
//public:
//	//------------------------------------------------------------------------
//	//	constructor
//	//------------------------------------------------------------------------
//	tmaXMLTextReader(System::String^ i_Filename)
//	:	tmaXMLReader(i_Filename)
//	{
//	}
//	tmaXMLTextReader(const char* i_Filename)
//	:	tmaXMLReader(i_Filename)
//	{
//	}
//
//	//------------------------------------------------------------------------
//	//	destructor
//	//------------------------------------------------------------------------
//	~tmaXMLTextReader() {};
//
//	//------------------------------------------------------------------------
//	//	open + create the XML Text file
//	//------------------------------------------------------------------------
//	virtual void Open() override
//	{
//		//	Write out the XML file
//		//
//		m_pReaderSet = gcnew XmlReaderSettings();
//		m_pReaderSet->IgnoreWhitespace = true;
//		m_pReaderSet->IgnoreComments = true;
//		m_pReaderSet->IgnoreProcessingInstructions = true;
//		m_pReaderSet->ProhibitDtd = true;
//
//		try
//		{
//			try
//			{
//				m_pReader = XmlTextReader::Create(gcnew System::String(m_Filename), m_pReaderSet);
//			}
//			catch (...)
//			{
//				fsLocator temp_locator;
//				throw fsFileDoesntExistX(temp_locator);
//			}
//
//		}
//		catch (fsFileDoesntExistX)
//		{
//			guiMessageBox::Show("Could not read Resolution Configuration File.", "Critical Error", guiMessageBox::e_OKOnly);
//		}
//		//	skip the XML header
//		//DBUG_ERROR("Missing Data folder for Resolution Config. File");
//		m_pReader->MoveToContent();
//		
//	}
//};
//
//#endif // _MANAGED
