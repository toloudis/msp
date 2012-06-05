/*****************************************************************************
**  muiPackage.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "ToolUIManaged/mui/muiPackage.hpp"

#include "ToolUIManaged/mui/muiDialogTabbedMgr.hpp"
#include "ToolUIManaged/mui/muiFileDialogUtils.hpp"
#include "ToolUIManaged/mui/muiMainWindow.hpp"
#include "ToolUIManaged/mui/muiMenuMgr.hpp"
#include "ToolUIManaged/mui/muiMessageBox.hpp"
#include "ToolUIManaged/mui/muiProgressDialog.hpp"
#include "ToolUIManaged/mui/muiPropertyDialog.hpp"
#include "ToolUIManaged/mui/muiSplashScreen.hpp"
#include "ToolUIManaged/mui/muiStatusBarMgr.hpp"
#include "ToolUIManaged/mui/muiToolbarMgr.hpp"
#include "ToolUIManaged/mui/muiXMLTextReader.hpp"
#include "ToolUIManaged/mui/muiXMLTextWriter.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"

namespace
{
	int l_RefCount = 0;

	muiToolbarMgr* l_pToolbarMgrImpl = NULL;
	muiDialogTabbedMgr* l_pDialogTabbedMgrImpl = NULL;
	muiFileDialogUtils* l_pFileDialogUtilsImpl = NULL;
	muiMainWindow* l_pMainWindowImpl = NULL;
	muiMenuMgr* l_pMenuMgrImpl = NULL;
	muiMessageBox* l_pMessageBoxImpl = NULL;
	muiProgressDialog* l_pProgressDialogImpl = NULL;
	muiPropertyDialog* l_pPropertyDialogImpl = NULL;
	muiSplashScreen* l_pSplashScreenImpl = NULL;
	muiStatusBarMgr* l_pStatusBarMgrImpl = NULL;
	muiXMLTextReader* l_pXMLTextReaderImpl = NULL;
	muiXMLTextWriter* l_pXMLTextWriterImpl = NULL;
}

//===========================================================================
//	muiPackage functions
//===========================================================================


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void muiPackage::Initialize()
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages we depend on

		// initialize our internal stuff
		//
		l_pToolbarMgrImpl = new muiToolbarMgr;
		guiToolbarMgr::SetImplementation(l_pToolbarMgrImpl);

		l_pDialogTabbedMgrImpl = new muiDialogTabbedMgr;
		guiDialogTabbedMgr::SetImplementation(l_pDialogTabbedMgrImpl);

		l_pFileDialogUtilsImpl = new muiFileDialogUtils;
		guiFileDialogUtils::SetImplementation(l_pFileDialogUtilsImpl);

		l_pMainWindowImpl = new muiMainWindow;
		guiMainWindow::SetImplementation(l_pMainWindowImpl);

		l_pMenuMgrImpl = new muiMenuMgr;
		guiMenuMgr::SetImplementation(l_pMenuMgrImpl);

		l_pMessageBoxImpl = new muiMessageBox;
		guiMessageBox::SetImplementation(l_pMessageBoxImpl);

		l_pProgressDialogImpl = new muiProgressDialog;
		guiProgressDialog::SetImplementation(l_pProgressDialogImpl);

		l_pPropertyDialogImpl = new muiPropertyDialog;
		guiPropertyDialog::SetImplementation(l_pPropertyDialogImpl);

		l_pSplashScreenImpl = new muiSplashScreen;
		guiSplashScreen::SetImplementation(l_pSplashScreenImpl);

		l_pStatusBarMgrImpl = new muiStatusBarMgr;
		guiStatusBarMgr::SetImplementation(l_pStatusBarMgrImpl);

		l_pXMLTextReaderImpl = new muiXMLTextReader;
		guiXMLTextReader::SetImplementation(l_pXMLTextReaderImpl);

		l_pXMLTextWriterImpl = new muiXMLTextWriter;
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
void muiPackage::DeInitialize()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
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

