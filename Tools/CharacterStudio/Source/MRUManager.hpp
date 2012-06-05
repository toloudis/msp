/*****************************************************************************
**  MRUManager.hpp
**
**     Managed class for adding most recently used files to File menu
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MRU_MANAGER_HPP
#error MRUManager.hpp multiply included
#endif
#define MRU_MANAGER_HPP

//============================================================================
//============================================================================
public ref class MRUManager
{
	public:
		//--------------------------------------------------------------------\
		// Makes MRU sub menu for the given item,
		//	usually a "Recent Files" item.  Adds i_NumItems number of MRU
		//	items to the list.
		//--------------------------------------------------------------------
		MRUManager(System::Windows::Forms::MenuItem^ i_pMenu, int i_NumItems)
			: m_pMenu(i_pMenu)
		{
			i_pMenu->Popup += gcnew System::EventHandler(this, &MRUManager::menu_popup);

			for (int i=0; i<i_NumItems; i++)
				AddMenuItem(i_pMenu, i);
		}


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~MRUManager()
		{
		}


		//--------------------------------------------------------------------
		// Add MRU meni item to "MenuItems" of given menu
		//--------------------------------------------------------------------
		void AddMenuItem(System::Windows::Forms::MenuItem^ i_pMenu, int i_Index)
		{
			System::Windows::Forms::MenuItem ^  menu_item =
					gcnew System::Windows::Forms::MenuItem();
			i_pMenu->MenuItems->Add(menu_item);
			menu_item->Index = i_Index;
			menu_item->Text = "";
			menu_item->Click += gcnew System::EventHandler(this, &MRUManager::menuItem_MRU_Click);
		}

	private:
		System::Windows::Forms::MenuItem^ m_pMenu;

		System::Void menuItem_MRU_Click(System::Object ^  sender, System::EventArgs ^  e);
		System::Void menu_popup(System::Object ^  sender, System::EventArgs ^  e);
};
