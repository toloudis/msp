/*****************************************************************************
**	tqtMessageBox.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtMessageBox.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include <string>


//===========================================================================
//	tqtMessageBox functions
//===========================================================================

#ifdef USE_QT

namespace
{
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	int show_message(const QString& i_Message, const QString& i_Title, int i_Type)
	{
		QMessageBox::StandardButtons buttons = QMessageBox::Ok;
		QMessageBox::Icon icon = QMessageBox::Information;
		switch (i_Type)
		{
			case tqtMessageBox::e_OKOnly:
				buttons = QMessageBox::Ok;
				break;
			case tqtMessageBox::e_OKCancel:
				buttons = QMessageBox::Ok | QMessageBox::Cancel;
				break;
			case tqtMessageBox::e_YesNo:
				buttons = QMessageBox::Yes | QMessageBox::No;
				icon = QMessageBox::Question;
				break;
			case tqtMessageBox::e_YesNoCancel:
				buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
				icon = QMessageBox::Question;
				break;
			case tqtMessageBox::e_RetryCancel:
				buttons = QMessageBox::Retry | QMessageBox::Cancel;
				icon = QMessageBox::Warning;
				break;
			case tqtMessageBox::e_AbortRetryIgnore:
				buttons = QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore;
				icon = QMessageBox::Warning;
				break;
		}

		QMessageBox dialog(icon, i_Title, i_Message, buttons, tqtSystem::g_pMainForm);
		int result = dialog.exec();

		switch (result)
		{
			case QMessageBox::Ok:		return tqtMessageBox::e_OK;
			case QMessageBox::Yes:		return tqtMessageBox::e_Yes;
			case QMessageBox::No:		return tqtMessageBox::e_No;
			case QMessageBox::Retry:	return tqtMessageBox::e_Retry;
			case QMessageBox::Abort:	return tqtMessageBox::e_Abort;
			case QMessageBox::Ignore:	return tqtMessageBox::e_Ignore;
			default:					return tqtMessageBox::e_Cancel;
		}
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int tqtMessageBox::Show( const char* i_Message, const char* i_Title, int i_Type )
{
	return show_message(tqtQString(i_Message), tqtQString(i_Title), i_Type);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int tqtMessageBox::Show( const std::string& i_Message, const std::string& i_Title, int i_Type )
{
	return show_message(QString::fromStdString(i_Message),
						QString::fromStdString(i_Title),
						i_Type);
}

#endif // USE_QT
