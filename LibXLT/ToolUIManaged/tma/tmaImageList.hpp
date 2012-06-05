#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaImageList.hpp
//**
//**      List class for .Net Images
//**
//**	StudioGPU
//**	Copyright(C) 2007 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_IMAGELIST_HPP
//#error tmaImageList.hpp multiply included
//#endif
//#define TMA_IMAGELIST_HPP
//
//#ifndef DBG_ASSERT_HPP
//#include "Core/dbg/dbgAssert.hpp"
//#endif
//#ifndef FS_LOCATOR_HPP
//#include "Core/fs/fsLocator.hpp"
//#endif
//
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//using namespace System::Collections;
//using namespace System::Drawing;
//
//
////============================================================================
////============================================================================
//public ref class tmaImageItem
//{
//public:
//	System::Drawing::Image^	m_pImage;
//	fsLocator*				m_pImageFilename;
//};
//
//
////============================================================================
////============================================================================
//public ref class tmaImageList
//{
//public:
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	tmaImageList()
//	{
//		m_Images = gcnew ArrayList();
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	int Add(System::Drawing::Image^ i_pImage)
//	{
//		DBG_ASSERT0( i_pImage != nullptr, "Cannot add a NULL image" );
//
//		tmaImageItem^ pIT = gcnew tmaImageItem();
//		pIT->m_pImage = i_pImage;
//		pIT->m_pImageFilename = nullptr;
//
//		return m_Images->Add(pIT);
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	int Add(System::Drawing::Image^ i_pImage, const fsLocator& i_Filename)
//	{
//		DBG_ASSERT0( i_pImage != nullptr, "Cannot add a NULL image" );
//
//		tmaImageItem^ pIT = gcnew tmaImageItem();
//		pIT->m_pImage = i_pImage;
//		pIT->m_pImageFilename = new fsLocator(i_Filename);
//
//		return m_Images->Add(pIT);
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void RemoveAll()
//	{
//		m_Images->Clear();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void RemoveAt(int i_Index)
//	{
//		DBG_ASSERT0( ((i_Index >= 0) && (i_Index < m_Images->Count)), "Index out of range" );
//
//		m_Images->RemoveAt(i_Index);
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	int Count()
//	{
//		return m_Images->Count;
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	System::Drawing::Image^ Get(int i_Index)
//	{
//		DBG_ASSERT0( ((i_Index >= 0) && (i_Index < m_Images->Count)), "Index out of range" );
//
//		return static_cast<tmaImageItem^>( m_Images[i_Index] )->m_pImage;
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	tmaImageItem^ GetImageData(int i_Index)
//	{
//		DBG_ASSERT0( ((i_Index >= 0) && (i_Index < m_Images->Count)), "Index out of range" );
//
//		return static_cast<tmaImageItem^>( m_Images[i_Index] );
//	};
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	void Set(int i_Index, System::Drawing::Image^ i_pImage)
//	{
//		DBG_ASSERT0( ((i_Index >= 0) && (i_Index < m_Images->Count)), "Index out of range" );
//		DBG_ASSERT0( i_pImage != nullptr, "Cannot add a NULL image" );
//		
//		static_cast<tmaImageItem^>( m_Images[i_Index] )->m_pImage = i_pImage;
//		//m_Images->set_Item(i_Index, i_pImage);
//	};
//
//private:
//	ArrayList^ m_Images;
//};
//
////{
////   IEnumerator^ myEnum = myList->GetEnumerator();
////   while ( myEnum->MoveNext() )
////   {
////      Object^ obj = safe_cast<Object^>(myEnum->Current);
////   }
////}
//#endif // _MANAGED
