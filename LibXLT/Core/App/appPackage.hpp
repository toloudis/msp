/*****************************************************************************
**  appPackage.hpp
**
**      appPackage contains the initialization functions
**	for the app package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_PACKAGE_HPP
#error appPackage.hpp multiply included
#endif
#define APP_PACKAGE_HPP


//============================================================================
//============================================================================
class appPackage
{
	public:
		//------------------------------------------------------------------------
		//	Init must be called before you use the app package.  A good place to
		//	do this is in your main function, with your other package initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the app package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};
