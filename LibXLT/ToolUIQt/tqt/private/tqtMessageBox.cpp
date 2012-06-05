/*****************************************************************************
**	tqtMessageBox.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtMessageBox.hpp"

#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/Env/envString.hpp"
#include "Core/It/itString.hpp"

#include <QtGui/QMessageBox>

#include <string>


//===========================================================================
//	tqtMessageBox functions
//===========================================================================


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int tqtMessageBox::Show( const char* i_Message, const char* i_Title, int i_Type )
{
	itString message(i_Message);
	itString title(i_Title);

#ifdef USE_QT
	QMessageBox::StandardButtons style = QMessageBox::Ok;
	switch (i_Type)
	{
		case e_OKOnly:
			style = QMessageBox::Ok;
			break;
		case e_OKCancel:
			style = QMessageBox::Ok | QMessageBox::Cancel;
			break;
		case e_YesNo:
			style = QMessageBox::Yes | QMessageBox::No;
			break;
		case e_YesNoCancel:
			style = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
			break;
		case e_RetryCancel:
			style = QMessageBox::Ok | QMessageBox::Cancel; // no retry?
			message += itString("\nRetry?");
			break;
		case e_AbortRetryIgnore:
			style = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel; // no abort, retry, ignore?
			message += itString("\nRetry/Ignore/Abort?");
			break;
	}

	QWidget *pParent = tqtSystem::g_pMainForm;
	int result = QMessageBox::information(pParent, i_Title, i_Message, style);
#endif

	int safeResult = e_Cancel;

#ifdef USE_QT
	switch (result)
	{
		case QMessageBox::Ok:
			if (i_Type == e_RetryCancel)
				safeResult = e_Retry;
			else
				safeResult = e_OK;
			break;
		case QMessageBox::Cancel:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Abort;
			else
				safeResult = e_Cancel;
			break;
		case QMessageBox::Yes:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Retry;
			else
				safeResult = e_Yes;
			break;
		case QMessageBox::No:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Ignore;
			else
				safeResult = e_No;
			break;
	}
#endif

	return safeResult;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int tqtMessageBox::Show( const itString& i_Message, const itString& i_Title, int i_Type )
{
	itString message(i_Message);
	itString title(i_Title);

#ifdef USE_QT
	QMessageBox::StandardButtons style = QMessageBox::Ok;
	switch (i_Type)
	{
		case e_OKOnly:
			style = QMessageBox::Ok;
			break;
		case e_OKCancel:
			style = QMessageBox::Ok | QMessageBox::Cancel;
			break;
		case e_YesNo:
			style = QMessageBox::Yes | QMessageBox::No;
			break;
		case e_YesNoCancel:
			style = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
			break;
		case e_RetryCancel:
			style = QMessageBox::Ok | QMessageBox::Cancel; // no retry?
			message += itString(L"\nRetry?");
			break;
		case e_AbortRetryIgnore:
			style = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel; // no abort, retry, ignore?
			message += itString(L"\nRetry/Ignore/Abort?");
			break;
	}

	QWidget *pParent = tqtSystem::g_pMainForm;
	std::string title_utf8 = envString::WideCharToUTF8( title.GetString(), title.GetLength() );
	std::string message_utf8 = envString::WideCharToUTF8( message.GetString(), message.GetLength() );
	int result = QMessageBox::information(pParent, title_utf8.c_str(), message_utf8.c_str(), style);
#endif

	int safeResult = e_Cancel;
#ifdef USE_QT
	switch (result)
	{
		case QMessageBox::Ok:
			if (i_Type == e_RetryCancel)
				safeResult = e_Retry;
			else
				safeResult = e_OK;
			break;
		case QMessageBox::Cancel:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Abort;
			else
				safeResult = e_Cancel;
			break;
		case QMessageBox::Yes:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Retry;
			else
				safeResult = e_Yes;
			break;
		case QMessageBox::No:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Ignore;
			else
				safeResult = e_No;
			break;
	}
#endif
	return safeResult;
}

