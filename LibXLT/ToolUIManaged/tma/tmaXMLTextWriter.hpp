#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaXMLTextWriter.hpp
//**
//**      XML Text file writer
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_XMLTEXTWRITER_HPP
//#error tmaXMLTextWriter multiply included
//#endif
//#define TMA_XMLTEXTWRITER_HPP
//
//#ifndef TMA_XMLWRITER_HPP
//#include "ToolUIManaged/tma/tmaXMLWriter.hpp"
//#endif
//
//#ifndef FS_FILEX_HPP
//#include "Core/fs/fsFileX.hpp"
//#endif
//#ifndef FS_LOCATOR_HPP
//#include "Core/fs/fsLocator.hpp"
//#endif
//#ifndef IT_STRING_HPP
//#include "Core/it/itString.hpp"
//#endif
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
//public ref class tmaXMLTextWriter: public tmaXMLWriter
//{
//public:
//	//------------------------------------------------------------------------
//	//	constructor
//	//------------------------------------------------------------------------
//	tmaXMLTextWriter(System::String^ i_Filename)
//	:	tmaXMLWriter(i_Filename)
//	{
//	}
//	tmaXMLTextWriter(const char* i_Filename)
//	:	tmaXMLWriter(i_Filename)
//	{
//	}
//
//	//------------------------------------------------------------------------
//	//	destructor
//	//------------------------------------------------------------------------
//	~tmaXMLTextWriter() {};
//
//	//------------------------------------------------------------------------
//	//	open + create the XML Text file
//	//------------------------------------------------------------------------
//	virtual void Open() override
//	{
//		//	if empty or missing filename, throw an error
//		//
//		if (   (m_Filename == nullptr)
//			|| (m_Filename->Length == 0))
//		{
//			fsLocator file(itString("[No XML filename set]"));
//			throw fsFileDoesntExistX(file);
//		}
//
//		//	Write out the XML file
//		//
//		m_pWriterSet = gcnew XmlWriterSettings();
//		m_pWriterSet->Indent = true; //Formatting::Indented;
//
//		m_pWriter = XmlTextWriter::Create(gcnew System::String(m_Filename), m_pWriterSet);
//	}
//};
//
//#endif // _MANAGED
