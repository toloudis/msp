/*****************************************************************************
**  quiProgressDialog.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiProgressDialog.hpp"

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"


//===========================================================================
//===========================================================================
namespace
{
#ifdef USE_QT
	QProgressDialog* l_ProgressDialog = NULL;
#endif // USE_QT
}; // anonymous namespace


//===========================================================================
//	quiProgressDialog functions
//===========================================================================

//---------------------------------------------------------------------------
//	Show a progress dialog with the given properties.
//---------------------------------------------------------------------------
void quiProgressDialog::Show(const char* i_DialogTitle,
							 const char* i_Message)
{
#ifdef USE_QT
	delete l_ProgressDialog;
	l_ProgressDialog = new QProgressDialog(tqtQString(i_Message),
										   "Cancel",
										   0, 100,
										   tqtSystem::g_pMainForm);
	l_ProgressDialog->setWindowTitle(tqtQString(i_DialogTitle));
	l_ProgressDialog->setWindowModality(Qt::ApplicationModal);
	l_ProgressDialog->setAutoClose(true);
	l_ProgressDialog->setMinimumDuration(0);
	l_ProgressDialog->setValue(0);
#else // USE_QT
	DBG_ERROR("MessageBox: "<<i_Message);
#endif // USE_QT
}

//---------------------------------------------------------------------------
//  Hide - the dialog is going away
//---------------------------------------------------------------------------
void quiProgressDialog::Hide()
{
#ifdef USE_QT
	delete l_ProgressDialog;
	l_ProgressDialog = NULL;
#endif // USE_QT
}

//---------------------------------------------------------------------------
//	Set the percentage and poll for cancellation. Return false if cancelled.
//---------------------------------------------------------------------------
bool quiProgressDialog::SetPercentage( float i_Percentage )
{
#ifdef USE_QT
	if (l_ProgressDialog != NULL)
	{
		// setValue processes events for a modal progress dialog, which
		// lets the cancel button work
		l_ProgressDialog->setValue((int)(i_Percentage*100.0f));
		return !l_ProgressDialog->wasCanceled();
	}
#endif // USE_QT
	return true;
}
