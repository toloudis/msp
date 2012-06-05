/*****************************************************************************
**  quiPackage.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiPackage.hpp"
#include "ToolUIQt/qui/quiCursor.hpp"
#include "ToolUIQt/qui/quiDialogTabbedMgr.hpp"
#include "ToolUIQt/qui/quiFileDialogUtils.hpp"
#include "ToolUIQt/qui/quiMainWindow.hpp"
#include "ToolUIQt/qui/quiMenuMgr.hpp"
#include "ToolUIQt/qui/quiMessageBox.hpp"
#include "ToolUIQt/qui/quiProgressDialog.hpp"
#include "ToolUIQt/qui/quiPropertyDialog.hpp"
#include "ToolUIQt/qui/quiPropertyGrid.hpp"
#include "ToolUIQt/qui/quiSplashScreen.hpp"
#include "ToolUIQt/qui/quiStatusBarMgr.hpp"
#include "ToolUIQt/qui/quiToolbarMgr.hpp"
#include "ToolUIQt/qui/quiXMLTextReader.hpp"
#include "ToolUIQt/qui/quiXMLTextWriter.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"


namespace
{
	int l_RefCount = 0;
	quiCursor* l_pCursorImpl = NULL;
	quiToolbarMgr* l_pToolbarMgrImpl = NULL;
	quiDialogTabbedMgr* l_pDialogTabbedMgrImpl = NULL;
	quiFileDialogUtils* l_pFileDialogUtilsImpl = NULL;
	quiMainWindow* l_pMainWindowImpl = NULL;
	quiMenuMgr* l_pMenuMgrImpl = NULL;
	quiMessageBox* l_pMessageBoxImpl = NULL;
	quiProgressDialog* l_pProgressDialogImpl = NULL;
	quiPropertyDialog* l_pPropertyDialogImpl = NULL;
	quiPropertyGrid* l_pPropertyGridImpl = NULL;
	quiSplashScreen* l_pSplashScreenImpl = NULL;
	quiStatusBarMgr* l_pStatusBarMgrImpl = NULL;
	quiXMLTextReader* l_pXMLTextReaderImpl = NULL;
	quiXMLTextWriter* l_pXMLTextWriterImpl = NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void quiPackage::Initialize()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		l_pCursorImpl = new quiCursor;
		guiCursor::SetImplementation(l_pCursorImpl);

		l_pToolbarMgrImpl = new quiToolbarMgr;
		guiToolbarMgr::SetImplementation(l_pToolbarMgrImpl);

		l_pDialogTabbedMgrImpl = new quiDialogTabbedMgr;
		guiDialogTabbedMgr::SetImplementation(l_pDialogTabbedMgrImpl);

		l_pFileDialogUtilsImpl = new quiFileDialogUtils;
		guiFileDialogUtils::SetImplementation(l_pFileDialogUtilsImpl);

		l_pMainWindowImpl = new quiMainWindow;
		guiMainWindow::SetImplementation(l_pMainWindowImpl);

		l_pMenuMgrImpl = new quiMenuMgr;
		guiMenuMgr::SetImplementation(l_pMenuMgrImpl);

		l_pMessageBoxImpl = new quiMessageBox;
		guiMessageBox::SetImplementation(l_pMessageBoxImpl);

		l_pProgressDialogImpl = new quiProgressDialog;
		guiProgressDialog::SetImplementation(l_pProgressDialogImpl);

		l_pPropertyDialogImpl = new quiPropertyDialog;
		guiPropertyDialog::SetImplementation(l_pPropertyDialogImpl);

		l_pPropertyGridImpl = new quiPropertyGrid;
		guiPropertyGrid::SetImplementation(l_pPropertyGridImpl);

		l_pSplashScreenImpl = new quiSplashScreen;
		guiSplashScreen::SetImplementation(l_pSplashScreenImpl);

		l_pStatusBarMgrImpl = new quiStatusBarMgr;
		guiStatusBarMgr::SetImplementation(l_pStatusBarMgrImpl);

		l_pXMLTextReaderImpl = new quiXMLTextReader;
		guiXMLTextReader::SetImplementation(l_pXMLTextReaderImpl);

		l_pXMLTextWriterImpl = new quiXMLTextWriter;
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
void quiPackage::DeInitialize()
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

