/*****************************************************************************
**	guiProgressDialog.hpp
**
**		A non-managed interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_PROGRESSDIALOG_HPP
#error guiProgressDialog.hpp multiply included
#endif
#define GUI_PROGRESSDIALOG_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif


//============================================================================
// forward declaration
//============================================================================
class guiProgressDialogImpl;


//============================================================================
// static functions define API
//============================================================================
class guiProgressDialog : public envAbstraction<guiProgressDialogImpl>
{
public:
	//--------------------------------------------------------------------
	//	Show a progress dialog with the given properties. 
	//--------------------------------------------------------------------
	static void Show(const char* i_DialogTitle, 
						const char* i_Message = NULL);

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away
	//--------------------------------------------------------------------
	static void Hide();

	//--------------------------------------------------------------------
	//	Set the percentage and poll for cancellation. Return false if cancelled.
	//--------------------------------------------------------------------
	static bool SetPercentage( float i_Percentage );
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiProgressDialogImpl
{
public:
	//--------------------------------------------------------------------
	//	Show a progress dialog with the given properties. 
	//--------------------------------------------------------------------
	virtual void Show(const char* i_DialogTitle, 
						const char* i_Message) = 0;

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away
	//--------------------------------------------------------------------
	virtual void Hide() = 0;

	//--------------------------------------------------------------------
	//	Set the percentage and poll for cancellation. Return false if cancelled.
	//--------------------------------------------------------------------
	virtual bool SetPercentage( float i_Percentage ) = 0;
};


