///********************************************************************************************\
//**  tmaManagedControlUtil.cpp
//**
//**      see .hpp
//**
//**	StudioGPU
//**	Copyright(C) 2004 - All Rights Reserved
//\********************************************************************************************/
//#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
//
//#include "Core/dbg/dbgAssert.hpp"
//
//#ifdef _MANAGED
//
////--------------------------------------------------------------------------------------------
////	tmaManagedControlUtil Functions
////--------------------------------------------------------------------------------------------
//namespace tmaManagedControlUtil
//{
//	const int BUTTON_BORDER = 0;
//
//	//---------------------------------------------------------------------------
//	//	Create + Assign an image to a button.  This function will also resize
//	//	the image so it FITS inside the button.  The ImageList parameters should
//	//	be a pointer the a variable that can hold a returned ImageList.
//	//
//	//	the function returns the image index.
//	//---------------------------------------------------------------------------
//	int Create_Button_Image( Button^ io_pButton, System::String^ i_pFileName )
//	{
//		//	attach the image to an image list first
//		int w,h;
//		System::Drawing::Size bsize = io_pButton->Size;
//		w = (int)(bsize.Width - BUTTON_BORDER);
//		h = (int)(bsize.Height - BUTTON_BORDER);
//
//		if ( io_pButton->ImageList == nullptr )
//		{
//			io_pButton->ImageList = gcnew ImageList();
//		}
//		(io_pButton->ImageList)->ImageSize = System::Drawing::Size(w,h);
//
//		(io_pButton->ImageList)->Images->Add( System::Drawing::Image::FromFile( i_pFileName ) );
//		io_pButton->ImageIndex = (io_pButton->ImageList)->Images->Count - 1;
//
//		return io_pButton->ImageIndex;
//	}
//
//	//---------------------------------------------------------------------------
//	//	Create + Assign an image to a ToolStripButton.  This function will also resize
//	//	the image so it FITS inside the button.  The ImageList parameters should
//	//	be a pointer the a variable that can hold a returned ImageList.
//	//
//	//	the function returns the image index.
//	//---------------------------------------------------------------------------
//	int Create_ToolStripButton_Image( ToolStripButton^ io_pButton, System::String^ i_pFileName, ToolStrip^ io_pToolStrip )
//	{
//		//	attach the image to an image list first
//		int w,h;
////		System::Drawing::Size bsize = io_pToolStrip->ButtonSize;
////		w = (int)(bsize.Width - BUTTON_BORDER);
////		h = (int)(bsize.Height - BUTTON_BORDER);
//		w = 16;
//		h = 16;
//
//		if ( io_pToolStrip->ImageList == nullptr )
//		{
//			io_pToolStrip->ImageList = gcnew ImageList();
//		}
//		io_pToolStrip->ImageList->ImageSize = System::Drawing::Size(w,h);
//
//		System::Drawing::Image^ pImage = System::Drawing::Image::FromFile( i_pFileName );
//		io_pToolStrip->ImageList->Images->Add( pImage );
//		io_pButton->ImageIndex = io_pToolStrip->ImageList->Images->Count - 1;
//
//		return io_pButton->ImageIndex;
//	}
//
//
//	//---------------------------------------------------------------------------
//	//	Dispose of all the controls in a controlCollection
//	//---------------------------------------------------------------------------
//	void DisposeOfControls(System::Windows::Forms::Control::ControlCollection^ i_pCollection)
//	{
//		if (i_pCollection != nullptr)
//		{
//			//int disposed_of = 0;
//			//System::Collections::IEnumerator^ controlenumerator = i_pCollection->GetEnumerator();
//			//while (controlenumerator->MoveNext())
//			//{
//			//	Control^ pControl = dynamic_cast<Control^>(controlenumerator->Current);
//			//	if (pControl != nullptr)
//			//	{
//			//		//i_pCollection->Remove(pControl);
//			//		pControl->Dispose();
//			//		++disposed_of;
//			//	}
//			//	else
//			//	{
//			//		// do we get here?
//			//		int x = 0;
//			//	}
//			//}
//			//int cnt = i_pCollection->Count;  // cnt vs disposed_of
//			
//			i_pCollection->Clear();
//		}
//	}
//
//	//---------------------------------------------------------------------------
//	//	Function to have only ONE item checked in a listbox.
//	//---------------------------------------------------------------------------
//	void CheckedListBox_CheckOnlySelected( CheckedListBox^ i_pCLB )
//	{
//		DBG_ASSERT0( (i_pCLB != nullptr), "NULL CheckedListBox");
//
//		//	First unselect all and then select only the current one
//		//
//		IEnumerator^ myEnum1 = i_pCLB->CheckedIndices->GetEnumerator();
//		while (myEnum1->MoveNext()) 
//		{
//			//int indexChecked =  ^safe_cast<__box int^>(myEnum1->Current);
//			int indexChecked =  (int)myEnum1->Current;
//			i_pCLB->SetItemCheckState(indexChecked,CheckState::Unchecked);
//		}
//
//		//	check just the one
//		i_pCLB->SetItemCheckState( i_pCLB->SelectedIndex, CheckState::Checked );
//	}
//
//	//---------------------------------------------------------------------------
//	//	Find the text in the listbox and return the index
//	//---------------------------------------------------------------------------
//	int GetMatchingListBoxIndex(ListBox^ i_pLB, System::String^ i_pText)
//	{
//		DBG_ASSERT0( (i_pLB != nullptr), "NULL ListBox");
//
//		for (int i = 0; i < i_pLB->Items->Count; ++i)
//		{
//			if (i_pLB->Items[i]->ToString()->Equals(i_pText))
//				return i;
//		}
//		return -1;
//	}
//
//
//	//---------------------------------------------------------------------------
//	//	Convert a keycombo from a string
//	//---------------------------------------------------------------------------
//	void ConvertKeyComboString(System::String^ i_KeyCombo, Keys& o_HotKey, Keys& o_Modifiers)
//	{
//		DBG_ASSERT0(false, "Need to implement ConvertKeyComboString");
//	}
//
//	//---------------------------------------------------------------------------
//	//	Convert a keycombo to a string
//	//---------------------------------------------------------------------------
//	System::String^ ConvertKeyCombo(Keys i_HotKey, Keys i_Modifiers)
//	{
//		Keys^ newkey = gcnew Keys((Keys)(i_HotKey | i_Modifiers));
//		return newkey->ToString();
//	}
//
//};
//
//#endif // _MANAGED
