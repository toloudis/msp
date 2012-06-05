/****************************************************************************\
**	wxMRUManager.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "wxMRUManager.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"

#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxMRUManager, wxEvtHandler)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, wxMRUManager::OnMenuClick)
    EVT_MENU_OPEN(wxMRUManager::OnMenuOpen)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
// Makes MRU sub menu for the given item,
//	usually a "Recent Files" item.  Adds i_NumItems number of MRU
//	items to the list.
//----------------------------------------------------------------------------
wxMRUManager::wxMRUManager(wxMenu* i_pMRUMenu, int i_NumItems, wxMenu* i_pTopLevelMenu)
: m_pMRUMenu(i_pMRUMenu), 
	m_pTopLevelMenu(i_pTopLevelMenu),
	m_MaxNumItems(i_NumItems)
{
	m_MaxNumItems = maFunctions::Lowest(i_NumItems, 9); // only have 9 ids
	//for (int i=0; i<num_items; i++)
	//{
	//	m_pMRUMenu->Append(wxID_FILE1 + i, _T("Filename"), _T("Open file"));
	//}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void wxMRUManager::OnMenuClick(wxCommandEvent& i_Event)
{
	int index = i_Event.GetId() - wxID_FILE1;

	std::vector<fsLocator> locators;
	docSingleTypeMgr::GetMRUList(locators);
	if (index>=0 && index < locators.size())
		guiCustomDocHandler::Open(locators[index]);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void wxMRUManager::OnMenuOpen(wxMenuEvent& i_Event)
{
	if (i_Event.GetMenu() == m_pTopLevelMenu)
	{
		// Update MRU list
		std::vector<fsLocator> locators;
		docSingleTypeMgr::GetMRUList(locators);

		int num_items = maFunctions::Lowest(m_MaxNumItems, (int)locators.size()); 
		// Expand out enough menu items
		for (int i=m_pMRUMenu->GetMenuItemCount(); i<num_items; i++)
		{
			m_pMRUMenu->Append(wxID_FILE1 + i, _T("Filename"), _T("Open file"));
		}

		num_items = m_pMRUMenu->GetMenuItemCount();
		for (int i=0; i<num_items; i++)
		{
			bool have_MRU = (locators.size() > i);
			if (have_MRU)
			{
				itString filename;
				fsFileUtil::LocatorToUnicodeString(locators[i], filename);
				m_pMRUMenu->SetLabel(wxID_FILE1 + i, filename.GetString());
			}
			else
			{
				//m_pMRUMenu->SetLabel(wxID_FILE1 + i, "Filename");
				m_pMRUMenu->Remove(wxID_FILE1 + i);
			}

			//m_pMRUMenu->Enable(wxID_FILE1 + i, have_MRU);

			// Can't figure out how to make the menu items without
			// filenames invisible. Not worth it anymore to try right now.
			//this->m_pMRUMenu->MenuItems[i]->Visible = have_MRU;
		}
	}
	i_Event.Skip();
}

#endif // USE_WXWIDGETS

