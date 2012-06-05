/*****************************************************************************
**  twxDialogTabbedMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxDialogTabbedMgr.hpp"

#include "ToolUIWx/twx/twxDialogTabbed.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include <map>

#ifdef USE_WXWIDGETS


//============================================================================
//============================================================================
namespace
{
	std::map<std::string, twxDialogTabbed*> m_DialogMap;

	twxDialogTabbed* get_or_create_dialog(const std::string& i_Name,
										  const char* i_Title = NULL)
	{
		std::map<std::string, twxDialogTabbed*>::iterator it = m_DialogMap.find(i_Name);
		if (it == m_DialogMap.end())
		{
			DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			wxString name(i_Name.c_str(), wxConvUTF8);
			wxString title(((i_Title) ? i_Title : i_Name.c_str()), wxConvUTF8);
			twxDialogTabbed* pDialog = new twxDialogTabbed(twxSystem::g_pMainForm, wxID_ANY, name, title);
			m_DialogMap[i_Name] = pDialog;
			return pDialog;
		}
		return it->second;
	}
} // end of namespace



//---------------------------------------------------------------------------
// Create the dialog with the given name, but will not show it
//---------------------------------------------------------------------------
void twxDialogTabbedMgr::Create(const char* i_DialogName, const char* i_Title)
{
	twxDialogTabbed* pDialog = get_or_create_dialog(i_DialogName, i_Title);
}

//---------------------------------------------------------------------------
// Show the dialog with the given name
//---------------------------------------------------------------------------
void twxDialogTabbedMgr::Show(const char* i_DialogName)
{
	twxDialogTabbed* pDialog = get_or_create_dialog(i_DialogName);
//	pDialog->Show();
	twxPaneMgr::Show(pDialog);
}

//---------------------------------------------------------------------------
// Hide the dialog with the given name
//---------------------------------------------------------------------------
void twxDialogTabbedMgr::Hide(const char* i_DialogName)
{
	if (twxSystem::g_pMainForm)
	{
		twxDialogTabbed* pDialog = get_or_create_dialog(i_DialogName);
		pDialog->Hide();
		twxPaneMgr::Show(pDialog, false);
	}
}

//---------------------------------------------------------------------------
// Returns true if the dialog is currently visible
//---------------------------------------------------------------------------
bool twxDialogTabbedMgr::IsVisible(const char* i_DialogName) 
{
	std::map<std::string, twxDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		twxDialogTabbed* pDialog = it->second;
		return twxPaneMgr::IsVisible(pDialog);
	}
	return false;
}

//---------------------------------------------------------------------------
// Returns true if the dialog is existed
//---------------------------------------------------------------------------
bool twxDialogTabbedMgr::IsExisted(const char* i_DialogName)
{
	std::map<std::string, twxDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
// Add new tab page to dialog with given name
//---------------------------------------------------------------------------
void twxDialogTabbedMgr::AddTabPage(const char* i_DialogName, const char* i_TabName)
{
	twxDialogTabbed* pDialog = get_or_create_dialog(i_DialogName);
	pDialog->AddTabPage(i_TabName);
}

//---------------------------------------------------------------------------
// Remove tab page from dialog with given name
//---------------------------------------------------------------------------
void twxDialogTabbedMgr::RemoveTabPage(const char* i_DialogName, const char* i_TabName)
{
	std::map<std::string, twxDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		twxDialogTabbed* pDialog = it->second;
		pDialog->RemoveTabPage(i_TabName);
	}
}

//--------------------------------------------------------------------
// Delete tab page from dialog with given name
//--------------------------------------------------------------------
void twxDialogTabbedMgr::DeleteTabPage(const char* i_DialogName, const char* i_TabName)
{
	std::map<std::string, twxDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		twxDialogTabbed* pDialog = it->second;
		pDialog->DeleteTabPage(i_TabName);
	}
}

//---------------------------------------------------------------------------
// Remove all tab pages for the dialog with the given name
//---------------------------------------------------------------------------
void twxDialogTabbedMgr::RemoveTabPages(const char* i_DialogName)
{
	std::map<std::string, twxDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		twxDialogTabbed* pDialog = it->second;
		pDialog->RemoveTabPages();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
wxPanel* twxDialogTabbedMgr::GetTabPage(const char* i_DialogName, const std::string& i_TabName)
{
	std::map<std::string, twxDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		twxDialogTabbed* pDialog = it->second;
		return pDialog->GetTabPage(i_TabName);
	}
	return NULL;
}

//---------------------------------------------------------------------------
// Called from destructor of twxDialogTabbed in order to make sure we free
// the entry in our map.
//---------------------------------------------------------------------------
void twxDialogTabbedMgr::NotifyDialogDeleted(twxDialogTabbed* i_pDialog)
{
	std::map<std::string, twxDialogTabbed*>::iterator it;
	for (it = m_DialogMap.begin(); it != m_DialogMap.end(); ++it)
	{
		if (it->second == i_pDialog)
		{
			m_DialogMap.erase(it);
			break;
		}
	}
	
}

//---------------------------------------------------------------------------
// Stop the dialog from updating
//---------------------------------------------------------------------------
void twxDialogTabbedMgr::Freeze(const char* i_DialogName) 
{
	std::map<std::string, twxDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		twxDialogTabbed* pDialog = it->second;
		pDialog->Freeze();
	}
}

//---------------------------------------------------------------------------
// Allow dialog to update
//---------------------------------------------------------------------------
void twxDialogTabbedMgr::Thaw(const char* i_DialogName) 
{
	std::map<std::string, twxDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		twxDialogTabbed* pDialog = it->second;
		pDialog->Thaw();
	}
}

#endif // USE_WXWIDGETS
