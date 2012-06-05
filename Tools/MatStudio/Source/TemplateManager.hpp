/*****************************************************************************
**  TemplateManager.hpp
**
**     Managed class for adding material templates to menu
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef TEMPLATE_MANAGER_HPP
#error TemplateManager.hpp multiply included
#endif
#define TEMPLATE_MANAGER_HPP

#ifndef MTR_TEMPLATEMGR_HPP
#include "mtrTemplateMgr.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "tmaManagedStringUtils.hpp"
#endif

//============================================================================
//============================================================================
public __gc class TemplateManager
{
	public:
		//--------------------------------------------------------------------\
		// Makes menu items for the template loaded from directory
		//--------------------------------------------------------------------
		TemplateManager(System::Windows::Forms::MenuItem* i_pMenu)
			: m_pMenu(i_pMenu)
		{
			i_pMenu->Popup += new System::EventHandler(this, menu_popup);

			const int num_items = mtrTemplateMgr::GetNumTemplates();
			for (int i=0; i<num_items; i++)
			{
				System::String* name = tmaManagedStringUtils::ItStringToManagedString(mtrTemplateMgr::GetTemplateName(i));
				AddMenuItem(i_pMenu, i, name);
			}
		}


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~TemplateManager()
		{
		}


		//--------------------------------------------------------------------
		// Add menu item for given template
		//--------------------------------------------------------------------
		void AddMenuItem(System::Windows::Forms::MenuItem* i_pMenu, int i_Index, System::String* i_pName)
		{
			System::Windows::Forms::MenuItem *  menu_item =
					new System::Windows::Forms::MenuItem();
			i_pMenu->MenuItems->Insert(i_Index, menu_item);
			menu_item->Index = i_Index;
			menu_item->Text = i_pName;
			menu_item->Click += new System::EventHandler(this, menuItem_Click);
		}

	private:
		System::Windows::Forms::MenuItem* m_pMenu;

		System::Void menuItem_Click(System::Object *  sender, System::EventArgs *  e);
		System::Void menu_popup(System::Object *  sender, System::EventArgs *  e);
};
