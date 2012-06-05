/****************************************************************************\
**	mainInitLibrary.hpp
**
**		Object that initializes libraries for our application on
**	constructor and then closes them down on destructor.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_INITLIBRARY_HPP
#error mainInitLibrary.hpp multiply included
#endif
#define MAIN_INITLIBRARY_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#include <windows.h>


//============================================================================
//============================================================================
class mainInitLibrary
{
public:
	//--------------------------------------------------------------------
	// Initializes library packages
	//--------------------------------------------------------------------
	mainInitLibrary(HINSTANCE hInstance,
					int nCmdShow);

	//--------------------------------------------------------------------
	// DeInitializes library packages
	//--------------------------------------------------------------------
	~mainInitLibrary();

	//--------------------------------------------------------------------
	// Returns true if Init succeeded
	//--------------------------------------------------------------------
	bool InitSuccessful();

private:
	bool m_bSuccess;
	int m_Argc;
    TCHAR *m_Argv[2];
};
