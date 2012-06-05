/*****************************************************************************
**  emdlPackage.hpp
**
**      emdlPackage contains the initialization functions
**	for the emdl (Entity Model) package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef EMDL_PACKAGE_HPP
#error emdlPackage.hpp multiply included
#endif
#define EMDL_PACKAGE_HPP


//============================================================================
//============================================================================
class emdlPackage
{
	public:
		//------------------------------------------------------------------------
		//	Init must be called before you use this package.  A good place to
		//	do this is in your main function, with your other package initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with this package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp();
};
