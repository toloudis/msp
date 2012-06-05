/*****************************************************************************
**  mayPackage.hpp
**
**      mayPackage contains the initialization functions
**	for the may (maya import) package.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MAY_PACKAGE_HPP
#error mayPackage.hpp multiply included
#endif
#define MAY_PACKAGE_HPP

class mayPackage
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
