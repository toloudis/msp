///*****************************************************************************
//**  MRUManager.hpp
//**
//**     Managed class for adding most recently used files to File menu
//**
//**	StudioGPU
//**	Copyright(C) 2004 - All Rights Reserved
//\****************************************************************************/
//#ifdef MRU_MANAGER_HPP
//#error MRUManager.hpp multiply included
//#endif
//#define MRU_MANAGER_HPP
//
//
////============================================================================
////============================================================================
//public ref class MRUManager
//{
//	public:
//		//--------------------------------------------------------------------
//		// Makes MRU sub menu for the given item,
//		//	usually a "Recent Files" item.  Adds i_NumItems number of MRU
//		//	items to the list.
//		//--------------------------------------------------------------------
//		MRUManager(System::Windows::Forms::ToolStripMenuItem^ i_pMenu, int i_NumItems)
//			: m_pMenu(i_pMenu)
//		{
//			i_pMenu->DropDownOpening += gcnew System::EventHandler(this, &MRUManager::menu_popup);
//
//			SetMRULimit( i_NumItems );
//		}
//
//		//--------------------------------------------------------------------
//		//--------------------------------------------------------------------
//		virtual ~MRUManager()
//		{
//		}
//
//		//--------------------------------------------------------------------
//		// Add MRU meni item to "ToolStripMenuItems" of given menu
//		//--------------------------------------------------------------------
//		void AddMenuItem(System::Windows::Forms::ToolStripMenuItem^ i_pMenu, int i_Index)
//		{
//			System::Windows::Forms::ToolStripMenuItem ^  menu_item =
//					gcnew System::Windows::Forms::ToolStripMenuItem();
//			i_pMenu->DropDownItems->Insert(i_Index, menu_item);
//			menu_item->MergeIndex = i_Index;
//			menu_item->Text = "";
//			menu_item->Click += gcnew System::EventHandler(this, &MRUManager::menuItem_MRU_Click);
//		}
//
//		//--------------------------------------------------------------------
//		//--------------------------------------------------------------------
//		void SetMRULimit( int i_NumItems )
//		{
//			int count = (m_pMenu->DropDownItems->Count-1);
//
//			//	check if the menu needs to be adjusted up or down.
//			//
//			if ( i_NumItems > count )
//			{
//				//	if number is larger, add more items
//				for (int i=count; i<i_NumItems; ++i)
//				{
//					AddMenuItem(m_pMenu, i);
//				}
//			}
//			else if ( i_NumItems < count )
//			{
//				//	number is smaller, remove items
//				for (int i=count; i >= i_NumItems; --i)
//				{
//					m_pMenu->DropDownItems->Remove( m_pMenu->DropDownItems[i] );
//				}
//			}
//		}
//
//	private:
//		System::Windows::Forms::ToolStripMenuItem^ m_pMenu;
//
//		System::Void menuItem_MRU_Click(System::Object ^  sender, System::EventArgs ^  e);
//		System::Void menu_popup(System::Object ^  sender, System::EventArgs ^  e);
//};
//
