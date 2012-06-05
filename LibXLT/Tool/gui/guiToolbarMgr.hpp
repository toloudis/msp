/*****************************************************************************
**	guiToolbarMgr.hpp
**
**		A generic interface to the toolbar.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_TOOLBARMGR_HPP
#error guiToolbarMgr.hpp multiply included
#endif
#define GUI_TOOLBARMGR_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif


//============================================================================
// forward declaration
//============================================================================
class guiToolbarMgrImpl;


//============================================================================
// static functions define API
//============================================================================
class guiToolbarMgr : public envAbstraction<guiToolbarMgrImpl>
{
public:
	//---------------------------------------------------------------------------
	// Create tool strip grouping with given name
	//---------------------------------------------------------------------------
	static void Create(const char* i_ToolbarName);

	//---------------------------------------------------------------------------
	// Show the toolbar with the given name
	//---------------------------------------------------------------------------
	static void Show(const char* i_ToolbarName, bool i_bVisible);

	//---------------------------------------------------------------------------
	// Returns true if the toolbar is currently visible
	//---------------------------------------------------------------------------
	static bool IsVisible(const char* i_ToolbarName);

	//---------------------------------------------------------------------------
	// Refreshes a toolbar, use when toggle states change
	//---------------------------------------------------------------------------
	static void Refresh(const char* i_ToolbarName);
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiToolbarMgrImpl
{
public:
	//---------------------------------------------------------------------------
	// Create tool strip grouping with given name
	//---------------------------------------------------------------------------
	virtual void Create(const char* i_ToolbarName) = 0;

	//---------------------------------------------------------------------------
	// Show the toolbar with the given name
	//---------------------------------------------------------------------------
	virtual void Show(const char* i_ToolbarName, bool i_bVisible) = 0;

	//---------------------------------------------------------------------------
	// Returns true if the toolbar is currently visible
	//---------------------------------------------------------------------------
	virtual bool IsVisible(const char* i_ToolbarName) = 0;

	//---------------------------------------------------------------------------
	// Refreshes a toolbar, use when toggle states change
	//---------------------------------------------------------------------------
	virtual void Refresh(const char* i_ToolbarName) = 0;
};


