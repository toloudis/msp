/*****************************************************************************
**  gsupDialogUtil.hpp
**
**	Helper functions to easily add tree nodes based on a fsLocator path.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef GSUP_DIALOGUTIL_HPP
#error gsupDialogUtil.hpp multiply included
#endif
#define GSUP_DIALOGUTIL_HPP

#include "Support/gsup/gsupFindFile.h"

#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::Collections;		// iEnumerator
using namespace System::Windows::Forms;


//============================================================================
//	forward references
//============================================================================


//============================================================================
//============================================================================
public ref class gsupDialogUtil
{
	public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static String^  ShowFindFile()
	{
		Support::gsupFindFile^ pDialog = gcnew Support::gsupFindFile();
		System::Windows::Forms::DialogResult result = pDialog->ShowDialog();
		if ( result == ::DialogResult::OK )
		{
			String^ pString;
			pString = pDialog->GetFullpath();
			delete pDialog;
			return pString;
		}
		delete pDialog;
		return nullptr;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static String^ ShowFindFile(	const fsLocator& i_StartFolder, 
									const char* i_FileFilter )
	{
		Support::gsupFindFile^ pDialog = gcnew Support::gsupFindFile();
		pDialog->Configure( i_StartFolder, i_FileFilter );
		System::Windows::Forms::DialogResult result = pDialog->ShowDialog();
		if ( result == ::DialogResult::OK )
		{
			String^ pString;
			pString = pDialog->GetFullpath();
			delete pDialog;
			return pString;
		}
		delete pDialog;
		return nullptr;
	}

	//--------------------------------------------------------------------
	// gsupInfoBox is used to display info while a task is working 
	//--------------------------------------------------------------------
	static void ShowInfoBox( System::String^ i_Message )
	{
		//m_Dialog = gcnew Support::gsupInfoBox(i_Message);
		//m_Dialog->Show();
		//m_Dialog->Refresh();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void HideInfoBox()
	{
		//if (m_Dialog)
		//{
		//	m_Dialog->Hide();
		//	m_Dialog = nullptr;
		//}
	}

private:
//	static Support::gsupInfoBox ^m_Dialog = nullptr;

};
#endif // _MANAGED
