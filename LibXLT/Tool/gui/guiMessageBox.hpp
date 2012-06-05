/*****************************************************************************
**	guiMessageBox.hpp
**
**		A non-managed interface to a message box.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_MESSAGEBOX_HPP
#error guiMessageBox.hpp multiply included
#endif
#define GUI_MESSAGEBOX_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif


//============================================================================
// forward declaration
//============================================================================
class guiMessageBoxImpl;
class itString;


//============================================================================
// static functions define API
//============================================================================
class guiMessageBox : public envAbstraction<guiMessageBoxImpl>
{
public:
	enum MBReturn
	{
		e_No = 0,
		e_OK = 1,
		e_Yes = 1,
		e_Retry,
		e_Abort,
		e_Ignore,
		e_Cancel
	};

	enum MBType
	{
		e_OKOnly = 0,
		e_OKCancel,
		e_YesNo,
		e_YesNoCancel,
		e_RetryCancel,
		e_AbortRetryIgnore,
	};

	//---------------------------------------------------------------------------'
	//	Show() - display a dialog with text
	//
	//	return: one of the enumerations MBReturn
	//---------------------------------------------------------------------------
	static int Show( const char* i_Message, const char* i_Title, int i_Type = e_OKOnly );
	static int Show( const itString& i_Message, const itString& i_Title, int i_Type = e_OKOnly );
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiMessageBoxImpl
{
public:
	//---------------------------------------------------------------------------'
	//	Show() - display a dialog with text
	//
	//	return: one of the enumerations MBReturn
	//---------------------------------------------------------------------------
	virtual int Show( const char* i_Message, const char* i_Title, int i_Type ) = 0;
	virtual int Show( const itString& i_Message, const itString& i_Title, int i_Type ) = 0;

};
