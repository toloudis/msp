/*****************************************************************************
**	cmmSceneDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmSceneDialog.hpp"
#include "Systems/Common/GUI/wxGUI/cmmPlacedPane.hpp"
#include "Systems/Common/GUI/wxGUI/cmmPlacedImages.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/gsup/gsupTreeCtrlUtil.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/vis/visMgr.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"

#include "Core/App/appTime.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define ID_DEFAULT wxID_ANY // Default

//============================================================================
//============================================================================
namespace
{
}	// end of namespace

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
cmmSceneDialog* cmmSceneDialog::FormInstance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmSceneDialog::cmmSceneDialog( wxWindow* parent, 
								const std::wstring& i_Title,
								const std::wstring& i_Caption ) 
:	cmmSceneDialogBase( parent )
{
	m_pCategoryPane = new cmmPlacedPane(this, cmmPlacedPane::e_CategoryView);
	m_notebook1->AddPage( m_pCategoryPane, wxT("Category"), true, wxNullBitmap );
	m_pGroupPane = new cmmPlacedPane(this, cmmPlacedPane::e_GroupView);
	m_notebook1->AddPage( m_pGroupPane, wxT("Hierarchy"), true, wxNullBitmap );
	m_pLightsPane = new cmmPlacedPane(this, cmmPlacedPane::e_LightsView);
	m_notebook1->AddPage( m_pLightsPane, wxT("Light Sets"), true, wxNullBitmap );

	// Load images for the different system 
	itString icon_dir;
	fsFileUtil::LocatorToUnicodeString( guiMenuMgr::GetIconDirectory(), icon_dir );
	std::wstring icondir = icon_dir.GetString();
	wxImageList *pIconImages = new wxImageList(13, 13, false, cmmPlacedImages::e_NumSystemImageIcons);
    {
        wxLogNull nullLog;
	    pIconImages->Add(wxBitmap(icondir + L"\\tree-parent.png", wxBITMAP_TYPE_PNG)); 
	    pIconImages->Add(wxBitmap(icondir + L"\\tree-geometry.png", wxBITMAP_TYPE_PNG));
	    pIconImages->Add(wxBitmap(icondir + L"\\tree-light.png", wxBITMAP_TYPE_PNG));
	    pIconImages->Add(wxBitmap(icondir + L"\\tree-camera.png", wxBITMAP_TYPE_PNG));
    }
	m_pGroupPane->AssignImageList(pIconImages); // tree ctrl takes ownership

	// Icons for the lighting tree
	pIconImages = new wxImageList(13, 13, false, 2);
    {
        wxLogNull nullLog;
	    pIconImages->Add(wxBitmap(icondir + L"\\tree-light.png", wxBITMAP_TYPE_PNG));
	    pIconImages->Add(wxBitmap(icondir + L"\\tree-set.png", wxBITMAP_TYPE_PNG));
    }
    m_pLightsPane->AssignImageList(pIconImages); // tree ctrl takes ownership

	// Select Category pane to display
	m_notebook1->SetSelection(0);

	// Add this panel to the AUI manager
	wxAuiPaneInfo api = wxAuiPaneInfo().Name(i_Title).Caption(i_Caption).Show().Layer(1).Left();
	twxPaneMgr::AddPane(this, api);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmSceneDialog::~cmmSceneDialog()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing cmmSceneDialog()");
	if (cmmSceneDialog::FormInstance == this)
		cmmSceneDialog::FormInstance = NULL;
}

	
//--------------------------------------------------------------------
// Clear scene data from form
//--------------------------------------------------------------------	
void cmmSceneDialog::Clear()
{
	//	clear the pane
	m_pCategoryPane->Clear();
	m_pGroupPane->Clear();
	m_pLightsPane->Clear();
}

//--------------------------------------------------------------------
// Update scene data in form
//--------------------------------------------------------------------	
void cmmSceneDialog::UpdateDialog()
{
	m_pCategoryPane->UpdateDialog();
	m_pGroupPane->UpdateDialog();
	m_pLightsPane->UpdateDialog();
}

//--------------------------------------------------------------------
// Update scene data in form
//--------------------------------------------------------------------	
void cmmSceneDialog::UpdatePlacedList(const std::string& i_SystemName, 
									  cmmDialogDataList& i_DataList)
{
	m_pCategoryPane->UpdatePlacedList(i_SystemName, i_DataList);
	m_pGroupPane->UpdatePlacedList(i_SystemName, i_DataList);
	m_pLightsPane->UpdatePlacedList(i_SystemName, i_DataList);
}

//------------------------------------------------------------------------
// Update the scene hierarchy tree view
//------------------------------------------------------------------------
void cmmSceneDialog::UpdateSceneHierarchy()
{
	// Update just the hierarchy pane
	m_pGroupPane->UpdateDialog();
}

//--------------------------------------------------------------------
//  Update tree view of light sets
//--------------------------------------------------------------------
void  cmmSceneDialog::UpdateSetRelationships()
{
	// Update just the light sets pane
	m_pLightsPane->UpdateDialog();
}

//--------------------------------------------------------------------
// Update scene data in form
//--------------------------------------------------------------------	
void cmmSceneDialog::UpdateAvailableList()
{
}

//------------------------------------------------------------------------
//	Find the item on the placed tree an highlight it. 
//	This is a notfication of what is already selected,
//	so the tree control should not cause an event to trigger.
//------------------------------------------------------------------------
void cmmSceneDialog::SelectObjectOnPlacedList(sel3dObject* i_pPickObject)
{
	m_pCategoryPane->SelectObjectOnPlacedList(i_pPickObject);
	m_pGroupPane->SelectObjectOnPlacedList(i_pPickObject);
	m_pLightsPane->SelectObjectOnPlacedList(i_pPickObject);
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cmmSceneDialog::AddToSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject)
{
	m_pCategoryPane->AddToSelectedObjectsOnPlacedList(i_pPickObject);
	m_pGroupPane->AddToSelectedObjectsOnPlacedList(i_pPickObject);
	m_pLightsPane->AddToSelectedObjectsOnPlacedList(i_pPickObject);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cmmSceneDialog::RemoveFromSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject)
{
	m_pCategoryPane->RemoveFromSelectedObjectsOnPlacedList(i_pPickObject);
	m_pGroupPane->RemoveFromSelectedObjectsOnPlacedList(i_pPickObject);
	m_pLightsPane->RemoveFromSelectedObjectsOnPlacedList(i_pPickObject);
}


#endif // USE_WXWIDGETS