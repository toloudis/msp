/*****************************************************************************
**  entPackage.hpp
**
**      entPackage contains the initialization functions
**	for the mat package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ENT_PACKAGE_HPP
#error entPackage.hpp multiply included
#endif
#define ENT_PACKAGE_HPP


//============================================================================
//============================================================================
class entPackage
{
	public:
		//------------------------------------------------------------------------
		//	Init must be called before you use the mat package.  A good place to
		//	do this is in your main function, with your other package
		//	initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the mat package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};
