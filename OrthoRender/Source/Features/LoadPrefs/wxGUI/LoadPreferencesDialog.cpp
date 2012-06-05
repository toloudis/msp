/*****************************************************************************
**	LoadPreferencesDialog.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/LoadPrefs/wxGUI/LoadPreferencesDialog.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS

namespace
{
}	// end of namespace


//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
LoadPreferencesDialog* LoadPreferencesDialog::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
LoadPreferencesDialog::LoadPreferencesDialog( wxWindow* parent )
: LoadPreferencesDialogBase( parent )
{

	// Add this panel to the AUI manager
	//twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(1).Left());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
LoadPreferencesDialog::~LoadPreferencesDialog()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG0("Closing LoadPreferencesDialog()");
	if (LoadPreferencesDialog::Instance == this)
		LoadPreferencesDialog::Instance = NULL;

}
//--------------------------------------------------------------------
// Return access to tab page that will contain load prefs properties
//--------------------------------------------------------------------
wxPanel* LoadPreferencesDialog::GetTabPageMain()
{
	return m_panel_LoadPrefs;
}

//--------------------------------------------------------------------
// Fill in list box with filenames from the textures 
// that were skipped
//--------------------------------------------------------------------
void LoadPreferencesDialog::SetMissingTextureList(const std::set<fsLocator>& i_Set)
{
	m_listBox1->Clear();
	std::set<fsLocator>::const_iterator it;
	for (it = i_Set.begin(); it != i_Set.end(); ++it)
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(*it, filename);
		m_listBox1->Append( filename );
	}
}


#endif // USE_WXWIDGETS
