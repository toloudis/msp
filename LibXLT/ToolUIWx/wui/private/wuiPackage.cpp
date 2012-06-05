/*****************************************************************************
**  wuiPackage.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiPackage.hpp"
#include "ToolUIWx/wui/wuiCursor.hpp"
#include "ToolUIWx/wui/wuiDialogTabbedMgr.hpp"
#include "ToolUIWx/wui/wuiFileDialogUtils.hpp"
#include "ToolUIWx/wui/wuiMainWindow.hpp"
#include "ToolUIWx/wui/wuiMenuMgr.hpp"
#include "ToolUIWx/wui/wuiMessageBox.hpp"
#include "ToolUIWx/wui/wuiProgressDialog.hpp"
#include "ToolUIWx/wui/wuiPropertyDialog.hpp"
#include "ToolUIWx/wui/wuiPropertyGrid.hpp"
#include "ToolUIWx/wui/wuiSplashScreen.hpp"
#include "ToolUIWx/wui/wuiStatusBarMgr.hpp"
#include "ToolUIWx/wui/wuiToolbarMgr.hpp"
#include "ToolUIWx/wui/wuiXMLTextReader.hpp"
#include "ToolUIWx/wui/wuiXMLTextWriter.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"


namespace
{
	int l_RefCount = 0;
	wuiCursor* l_pCursorImpl = NULL;
	wuiToolbarMgr* l_pToolbarMgrImpl = NULL;
	wuiDialogTabbedMgr* l_pDialogTabbedMgrImpl = NULL;
	wuiFileDialogUtils* l_pFileDialogUtilsImpl = NULL;
	wuiMainWindow* l_pMainWindowImpl = NULL;
	wuiMenuMgr* l_pMenuMgrImpl = NULL;
	wuiMessageBox* l_pMessageBoxImpl = NULL;
	wuiProgressDialog* l_pProgressDialogImpl = NULL;
	wuiPropertyDialog* l_pPropertyDialogImpl = NULL;
	wuiPropertyGrid* l_pPropertyGridImpl = NULL;
	wuiSplashScreen* l_pSplashScreenImpl = NULL;
	wuiStatusBarMgr* l_pStatusBarMgrImpl = NULL;
	wuiXMLTextReader* l_pXMLTextReaderImpl = NULL;
	wuiXMLTextWriter* l_pXMLTextWriterImpl = NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void wuiPackage::Initialize()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		l_pCursorImpl = new wuiCursor;
		guiCursor::SetImplementation(l_pCursorImpl);

		l_pToolbarMgrImpl = new wuiToolbarMgr;
		guiToolbarMgr::SetImplementation(l_pToolbarMgrImpl);

		l_pDialogTabbedMgrImpl = new wuiDialogTabbedMgr;
		guiDialogTabbedMgr::SetImplementation(l_pDialogTabbedMgrImpl);

		l_pFileDialogUtilsImpl = new wuiFileDialogUtils;
		guiFileDialogUtils::SetImplementation(l_pFileDialogUtilsImpl);

		l_pMainWindowImpl = new wuiMainWindow;
		guiMainWindow::SetImplementation(l_pMainWindowImpl);

		l_pMenuMgrImpl = new wuiMenuMgr;
		guiMenuMgr::SetImplementation(l_pMenuMgrImpl);

		l_pMessageBoxImpl = new wuiMessageBox;
		guiMessageBox::SetImplementation(l_pMessageBoxImpl);

		l_pProgressDialogImpl = new wuiProgressDialog;
		guiProgressDialog::SetImplementation(l_pProgressDialogImpl);

		l_pPropertyDialogImpl = new wuiPropertyDialog;
		guiPropertyDialog::SetImplementation(l_pPropertyDialogImpl);

		l_pPropertyGridImpl = new wuiPropertyGrid;
		guiPropertyGrid::SetImplementation(l_pPropertyGridImpl);

		l_pSplashScreenImpl = new wuiSplashScreen;
		guiSplashScreen::SetImplementation(l_pSplashScreenImpl);

		l_pStatusBarMgrImpl = new wuiStatusBarMgr;
		guiStatusBarMgr::SetImplementation(l_pStatusBarMgrImpl);

		l_pXMLTextReaderImpl = new wuiXMLTextReader;
		guiXMLTextReader::SetImplementation(l_pXMLTextReaderImpl);

		l_pXMLTextWriterImpl = new wuiXMLTextWriter;
		guiXMLTextWriter::SetImplementation(l_pXMLTextWriterImpl);

		// Set up defaults for these functions
		cmaCommandMgr::SetMenuCheckedFunction(guiMenuMgr::MenuObjectsCheck);
		cmaCommandMgr::SetValidHotKeyFunction(guiMenuMgr::IsValidShortcut);
	}

	//	increment the ref count
	l_RefCount++;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void wuiPackage::DeInitialize()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		delete l_pCursorImpl ;
		l_pCursorImpl = NULL;
		guiCursor::SetImplementation(NULL);

		delete l_pToolbarMgrImpl;
		l_pToolbarMgrImpl = NULL;
		guiToolbarMgr::SetImplementation(NULL);

		delete l_pDialogTabbedMgrImpl;
		l_pDialogTabbedMgrImpl = NULL;
		guiDialogTabbedMgr::SetImplementation(NULL);

		delete l_pFileDialogUtilsImpl;
		l_pFileDialogUtilsImpl = NULL;
		guiFileDialogUtils::SetImplementation(NULL);

		delete l_pMainWindowImpl;
		l_pMainWindowImpl = NULL;
		guiMainWindow::SetImplementation(NULL);

		delete l_pMenuMgrImpl;
		l_pMenuMgrImpl = NULL;
		guiMenuMgr::SetImplementation(NULL);

		delete l_pMessageBoxImpl;
		l_pMessageBoxImpl = NULL;
		guiMessageBox::SetImplementation(NULL);

		delete l_pProgressDialogImpl;
		l_pProgressDialogImpl = NULL;
		guiProgressDialog::SetImplementation(NULL);

		delete l_pPropertyDialogImpl;
		l_pPropertyDialogImpl = NULL;
		guiPropertyDialog::SetImplementation(NULL);

		delete l_pPropertyGridImpl;
		l_pPropertyGridImpl = NULL;
		guiPropertyGrid::SetImplementation(NULL);

		delete l_pSplashScreenImpl;
		l_pSplashScreenImpl = NULL;
		guiSplashScreen::SetImplementation(NULL);

		delete l_pStatusBarMgrImpl;
		l_pStatusBarMgrImpl = NULL;
		guiStatusBarMgr::SetImplementation(NULL);

		delete l_pXMLTextReaderImpl;
		l_pXMLTextReaderImpl = NULL;
		guiXMLTextReader::SetImplementation(NULL);

		delete l_pXMLTextWriterImpl;
		l_pXMLTextWriterImpl = NULL;
		guiXMLTextWriter::SetImplementation(NULL);

		// clean up packages we depend on
	}
}

