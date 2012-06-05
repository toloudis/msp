/*****************************************************************************
**	smdlPackage.hpp
**
**		smdlPackage contains the initialization functions
**	for the smdl (Scene Model) package.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_PACKAGE_HPP
#error smdlPackage.hpp multiply included
#endif
#define SMDL_PACKAGE_HPP


//============================================================================
//============================================================================
class smdlPackage
{
	public:
		//------------------------------------------------------------------------
		//	Init must be called before you use this package.  A good place to
		//	do this is in your main function, with your other package initializers.
		//------------------------------------------------------------------------
		static void InitGraphics();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with this package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUpGraphics();
};
