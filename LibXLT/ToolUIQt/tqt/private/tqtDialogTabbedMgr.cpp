/*****************************************************************************
**	tqtDialogTabbedMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtDialogTabbedMgr.hpp"

#include "ToolUIQt/tqt/tqtDialogTabbed.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include <map>


#ifdef USE_QT

//============================================================================
//============================================================================
namespace
{
	std::map<std::string, tqtDialogTabbed*> m_DialogMap;

	tqtDialogTabbed* get_or_create_dialog(const std::string& i_Name,
										  const char* i_Title = NULL)
	{
		std::map<std::string, tqtDialogTabbed*>::iterator it = m_DialogMap.find(i_Name);
		if (it == m_DialogMap.end())
		{
			DBG_ASSERT(tqtSystem::g_pMainForm != NULL, "MainForm not yet initialized.");

#ifdef QT_FINISH_PORT
			wxString name(i_Name.c_str(), wxConvUTF8);
			wxString title(((i_Title) ? i_Title : i_Name.c_str()), wxConvUTF8);
			tqtDialogTabbed* pDialog = new tqtDialogTabbed(tqtSystem::g_pMainForm, wxID_ANY, name, title);
			m_DialogMap[i_Name] = pDialog;
			return pDialog;
#endif
		}
		return it->second;
	}
} // end of namespace



//---------------------------------------------------------------------------
// Create the dialog with the given name, but will not show it
//---------------------------------------------------------------------------
void tqtDialogTabbedMgr::Create(const char* i_DialogName, const char* i_Title)
{
	tqtDialogTabbed* pDialog = get_or_create_dialog(i_DialogName, i_Title);
}

//---------------------------------------------------------------------------
// Show the dialog with the given name
//---------------------------------------------------------------------------
void tqtDialogTabbedMgr::Show(const char* i_DialogName)
{
	tqtDialogTabbed* pDialog = get_or_create_dialog(i_DialogName);
//	pDialog->Show();
	tqtPaneMgr::Show(pDialog);
}

//---------------------------------------------------------------------------
// Hide the dialog with the given name
//---------------------------------------------------------------------------
void tqtDialogTabbedMgr::Hide(const char* i_DialogName)
{
	if (tqtSystem::g_pMainForm != NULL)
	{
		tqtDialogTabbed* pDialog = get_or_create_dialog(i_DialogName);
#ifdef QT_FINISH_PORT
		pDialog->Hide();
#endif
		tqtPaneMgr::Show(pDialog, false);
	}
}

//---------------------------------------------------------------------------
// Returns true if the dialog is currently visible
//---------------------------------------------------------------------------
bool tqtDialogTabbedMgr::IsVisible(const char* i_DialogName) 
{
	std::map<std::string, tqtDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		tqtDialogTabbed* pDialog = it->second;
		return tqtPaneMgr::IsVisible(pDialog);
	}
	return false;
}

//---------------------------------------------------------------------------
// Returns true if the dialog is existed
//---------------------------------------------------------------------------
bool tqtDialogTabbedMgr::IsExisted(const char* i_DialogName)
{
	std::map<std::string, tqtDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
// Add new tab page to dialog with given name
//---------------------------------------------------------------------------
void tqtDialogTabbedMgr::AddTabPage(const char* i_DialogName, const char* i_TabName)
{
	tqtDialogTabbed* pDialog = get_or_create_dialog(i_DialogName);
	pDialog->AddTabPage(i_TabName);
}

//---------------------------------------------------------------------------
// Remove tab page from dialog with given name
//---------------------------------------------------------------------------
void tqtDialogTabbedMgr::RemoveTabPage(const char* i_DialogName, const char* i_TabName)
{
	std::map<std::string, tqtDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		tqtDialogTabbed* pDialog = it->second;
		pDialog->RemoveTabPage(i_TabName);
	}
}

//--------------------------------------------------------------------
// Delete tab page from dialog with given name
//--------------------------------------------------------------------
void tqtDialogTabbedMgr::DeleteTabPage(const char* i_DialogName, const char* i_TabName)
{
	std::map<std::string, tqtDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		tqtDialogTabbed* pDialog = it->second;
		pDialog->DeleteTabPage(i_TabName);
	}
}

//---------------------------------------------------------------------------
// Remove all tab pages for the dialog with the given name
//---------------------------------------------------------------------------
void tqtDialogTabbedMgr::RemoveTabPages(const char* i_DialogName)
{
	std::map<std::string, tqtDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		tqtDialogTabbed* pDialog = it->second;
		pDialog->RemoveTabPages();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
#ifdef QT_FINISH_PORT
wxPanel* tqtDialogTabbedMgr::GetTabPage(const char* i_DialogName, const std::string& i_TabName)
{
	std::map<std::string, tqtDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		tqtDialogTabbed* pDialog = it->second;
		return pDialog->GetTabPage(i_TabName);
	}
	return NULL;
}
#endif

//---------------------------------------------------------------------------
// Called from destructor of tqtDialogTabbed in order to make sure we free
// the entry in our map.
//---------------------------------------------------------------------------
void tqtDialogTabbedMgr::NotifyDialogDeleted(tqtDialogTabbed* i_pDialog)
{
	std::map<std::string, tqtDialogTabbed*>::iterator it;
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
void tqtDialogTabbedMgr::Freeze(const char* i_DialogName) 
{
	std::map<std::string, tqtDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		tqtDialogTabbed* pDialog = it->second;
#ifdef QT_FINISH_PORT
		pDialog->Freeze();
#endif
	}
}

//---------------------------------------------------------------------------
// Allow dialog to update
//---------------------------------------------------------------------------
void tqtDialogTabbedMgr::Thaw(const char* i_DialogName) 
{
	std::map<std::string, tqtDialogTabbed*>::iterator it = m_DialogMap.find(i_DialogName);
	if (it != m_DialogMap.end())
	{
		tqtDialogTabbed* pDialog = it->second;
#ifdef QT_FINISH_PORT
		pDialog->Thaw();
#endif
	}
}

#endif	// USE_QT
