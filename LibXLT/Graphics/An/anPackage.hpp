/*****************************************************************************
**  anPackage.hpp
**
**      anPackage contains the initialization functions
**	for the an package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef AN_PACKAGE_HPP
#error anPackage.hpp multiply included
#endif
#define AN_PACKAGE_HPP


//============================================================================
//============================================================================
class anPackage
{
	public:
		//------------------------------------------------------------------------
		//	Init must be called before you use the an package.  A good place to
		//	do this is in your main function, with your other package initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the an package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};
