/*****************************************************************************
**	twxMessageBox.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxMessageBox.hpp"

#include "ToolUIWx/twx/twxSystem.hpp"

#include "Core/It/itString.hpp"


//===========================================================================
//	twxMessageBox functions
//===========================================================================

#ifdef USE_WXWIDGETS

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int twxMessageBox::Show( const char* i_Message, const char* i_Title, int i_Type )
{
	itString message(i_Message);
	itString title(i_Title);

	long style = wxOK;
	switch (i_Type)
	{
		case e_OKOnly:
			style = wxOK;
			break;
		case e_OKCancel:
			style = wxOK | wxCANCEL;
			break;
		case e_YesNo:
			style = wxYES_NO;
			break;
		case e_YesNoCancel:
			style = wxYES_NO | wxCANCEL;
			break;
		case e_RetryCancel:
			style = wxOK | wxCANCEL; // no retry?
			message += itString("\nRetry?");
			break;
		case e_AbortRetryIgnore:
			style = wxYES_NO | wxCANCEL; // no abort, retry, ignore?
			message += itString("\nRetry/Ignore/Abort?");
			break;
	}

	wxWindow *pParent = twxSystem::g_pMainForm;
	wxMessageDialog dialog(pParent, message.GetString(), title.GetString(), style);

	int result = dialog.ShowModal();
	
	int safeResult = e_Cancel;
	switch (result)
	{
		case wxID_OK:
			if (i_Type == e_RetryCancel)
				safeResult = e_Retry;
			else
				safeResult = e_OK;
			break;
		case wxID_CANCEL:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Abort;
			else
				safeResult = e_Cancel;
			break;
		case wxID_YES:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Retry;
			else
				safeResult = e_Yes;
			break;
		case wxID_NO:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Ignore;
			else
				safeResult = e_No;
			break;
	}

	return safeResult;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int twxMessageBox::Show( const itString& i_Message, const itString& i_Title, int i_Type )
{
	itString message(i_Message);
	itString title(i_Title);

	long style = wxOK;
	switch (i_Type)
	{
		case e_OKOnly:
			style = wxOK;
			break;
		case e_OKCancel:
			style = wxOK | wxCANCEL;
			break;
		case e_YesNo:
			style = wxYES_NO;
			break;
		case e_YesNoCancel:
			style = wxYES_NO | wxCANCEL;
			break;
		case e_RetryCancel:
			style = wxOK | wxCANCEL; // no retry?
			message += itString(L"\nRetry?");
			break;
		case e_AbortRetryIgnore:
			style = wxYES_NO | wxCANCEL; // no abort, retry, ignore?
			message += itString(L"\nRetry/Ignore/Abort?");
			break;
	}

	wxWindow *pParent = twxSystem::g_pMainForm;
	wxMessageDialog dialog(pParent, 
		message.GetString(), 
		title.GetString(), 
		style);

	int result = dialog.ShowModal();
	
	int safeResult = e_Cancel;
	switch (result)
	{
		case wxID_OK:
			if (i_Type == e_RetryCancel)
				safeResult = e_Retry;
			else
				safeResult = e_OK;
			break;
		case wxID_CANCEL:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Abort;
			else
				safeResult = e_Cancel;
			break;
		case wxID_YES:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Retry;
			else
				safeResult = e_Yes;
			break;
		case wxID_NO:
			if (i_Type == e_AbortRetryIgnore)
				safeResult = e_Ignore;
			else
				safeResult = e_No;
			break;
	}

	return safeResult;
}

#endif // USE_WXWIDGETS