/*****************************************************************************
**	cmmSceneDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/qtGUI/cmmSceneDialog.hpp"
#include "Systems/Common/GUI/qtGUI/cmmPlacedPane.hpp"
#include "Systems/Common/GUI/qtGUI/cmmPlacedImages.hpp"
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
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"


#ifdef USE_QT
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#include <QTabWidget>
#include <QVBoxLayout>

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
cmmSceneDialog::cmmSceneDialog( QWidget* parent,
								const std::wstring& i_Title,
								const std::wstring& i_Caption )
:	QWidget( parent )
{
	QVBoxLayout* sizer_Dialog = new QVBoxLayout( this );
	m_notebook1 = new QTabWidget( this );
	m_notebook1->setUsesScrollButtons( true );
	m_notebook1->setMovable( true );
	sizer_Dialog->addWidget( m_notebook1, 1 );

	m_pCategoryPane = new cmmPlacedPane(m_notebook1, cmmPlacedPane::e_CategoryView);
	m_notebook1->addTab( m_pCategoryPane, "Category" );

	m_pGroupPane = new cmmPlacedPane(m_notebook1, cmmPlacedPane::e_GroupView);
	m_notebook1->addTab( m_pGroupPane, "Hierarchy" );

	m_pLightsPane = new cmmPlacedPane(m_notebook1, cmmPlacedPane::e_LightsView);
	m_notebook1->addTab( m_pLightsPane, "Light Sets" );

	// Load images for the different system
	std::string icon_dir;
	fsFileUtil::LocatorToUnicodeString( guiMenuMgr::GetIconDirectory(), icon_dir );
	const QString icondir = QString::fromStdString(icon_dir);

	std::vector<QPixmap> icon_images;
	icon_images.push_back(QPixmap(icondir + "\\tree-parent.png"));
	icon_images.push_back(QPixmap(icondir + "\\tree-geometry.png"));
	icon_images.push_back(QPixmap(icondir + "\\tree-light.png"));
	icon_images.push_back(QPixmap(icondir + "\\tree-camera.png"));
	m_pGroupPane->AssignImageList(icon_images);

	// Icons for the lighting tree
	icon_images.clear();
	icon_images.push_back(QPixmap(icondir + "\\tree-light.png"));
	icon_images.push_back(QPixmap(icondir + "\\tree-set.png"));
	m_pLightsPane->AssignImageList(icon_images);

	// Select Category pane to display
	m_notebook1->setCurrentIndex(0);

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(i_Title.c_str()).Caption(i_Caption.c_str()).Show().Layer(1).Left().BestSize(300, 400));
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


#endif // USE_QT