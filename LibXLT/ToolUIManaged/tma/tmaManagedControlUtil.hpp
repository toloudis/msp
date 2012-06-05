#error THIS_FILE_IS_OBSOLETE

///********************************************************************************************\
//**  tmaManagedControlUtil.hpp
//**
//**      Utilities for managed control objects
//**
//**	StudioGPU
//**	Copyright(C) 2004 - All Rights Reserved
//\********************************************************************************************/
//
//#ifdef	TMA_MANAGEDCONTROLUTIL_HPP
//#error	tmaManagedControlUtil.hpp included recursively.
//#endif
//#define	TMA_MANAGEDCONTROLUTIL_HPP
//
//
////============================================================================
////============================================================================
//#ifdef _MANAGED
//
//using namespace System;
//using namespace System::Collections;
//using namespace System::Drawing;
//using namespace System::Windows::Forms;
//
//
////--------------------------------------------------------------------------------------------
////	tmaManagedControlUtil Functions
////--------------------------------------------------------------------------------------------
//namespace tmaManagedControlUtil
//{
//	//---------------------------------------------------------------------------
//	//	Create + Assign an image to a button.  This function will also resize
//	//	the image so it FITS inside the button.  
//	//
//	//	the function returns the image index.
//	//---------------------------------------------------------------------------
//	int Create_Button_Image( Button^ io_pButton, System::String^ i_pFileName );
//
//	//---------------------------------------------------------------------------
//	//	Create + Assign an image to a ToolBarButton.  This function will also resize
//	//	the image so it FITS inside the button. 
//	//
//	//	the function returns the image index.
//	//---------------------------------------------------------------------------
//	int Create_ToolStripButton_Image( ToolStripButton^ io_pButton, System::String^ i_pFileName, ToolStrip^ io_pToolBar );
//
//	//---------------------------------------------------------------------------
//	//	Dispose of all the controls in a controlCollection
//	//---------------------------------------------------------------------------
//	void DisposeOfControls(System::Windows::Forms::Control::ControlCollection^ i_pCollection);
//
//	//---------------------------------------------------------------------------
//	//	Function to have only ONE item checked in a listbox.
//	//---------------------------------------------------------------------------
//	void CheckedListBox_CheckOnlySelected( CheckedListBox^ i_pCLB );
//
//	//---------------------------------------------------------------------------
//	//	Find the text in the listbox and return the index
//	//---------------------------------------------------------------------------
//	int GetMatchingListBoxIndex(ListBox^ i_pLB, System::String^ i_pText);
//
//	//---------------------------------------------------------------------------
//	//	Convert a keycombo from a string
//	//---------------------------------------------------------------------------
//	void ConvertKeyComboString(String^ i_KeyCombo, Keys& o_HotKey, Keys& o_Modifiers);
//
//	//---------------------------------------------------------------------------
//	//	Convert a keycombo to a string
//	//---------------------------------------------------------------------------
//	String^ ConvertKeyCombo(Keys i_HotKey, Keys i_Modifiers);
//};
//
//
//#endif // _MANAGED
