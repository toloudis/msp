/*****************************************************************************
**	cmaPackage.hpp
**
**		Command Management Package
**
**	This package handles the connecting of application controls (buttons,etc)
**	to the appropriate callbacks.
**
**----------------------------------------------------------------------------
**Dictionary
**----------------------------------------------------------------------------
**CommandManager          -   Global service that manages a collection of
**                            commands
**
**Command                 -   A conceptual representation for an application
**                            operation (ie.  Save, Edit, Load, etc...)
**
**Command Instance        -   A UI element associated with a command (ie, Menu
**                            item, Toolbar Item, etc...).  A command can have
**                            multiple instances
**
**Command Type            -   A UI class that can house a command Instance (ie
**                            "System.Windows.Forms.MenuItem",
**                            "System.Windows.Forms.ToolbarItem" )
**
**CommandExecutor         -   An object that can handle all communication between
**                            the command manager and a particular command instance
**                            for a particular command type.
**
**UpdateHandler           -   Event handler for the Commands Update event.
**----------------------------------------------------------------------------
**
**	NOTE: You must Init before use to register with gfErrorHandler and the
**	appropriate engine packages
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMA_PACKAGE_HPP
#error cmaPackage.hpp multiply included
#endif
#define CMD_PACKAGE_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"	// defines NULL
#endif


//============================================================================
//============================================================================
class cmaPackage
{
	public:
		//--------------------------------------------------------------------
		// Init -- i_Parser, the user defined level parser.  NULL indicates
		//			to the package that the default parsing system be used.
		//--------------------------------------------------------------------
		//static void Init(cmaPackageParser* i_Parser=NULL);
		static void Init();

		//--------------------------------------------------------------------
		// CleanUp -- cleans up all relevent package data.  It also destroys
		// all waypoint lists that may have been filled when the data was read
		// from the level file and/or added by the user.
		// Something the user may do manually if desired.
		//--------------------------------------------------------------------
		static void CleanUp();
};
