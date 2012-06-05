/*****************************************************************************
**  shdwPackage.hpp
**
**      shdwPackage contains the initialization functions
**	for the shdw package.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_PACKAGE_HPP
#error shdwPackage.hpp multiply included
#endif
#define SHDW_PACKAGE_HPP

class shdwPackage
{
	public:

		//------------------------------------------------------------------------
		//	Init must be called before you use the shdw package.  A good place to
		//	do this is in your main function, with your other package
		//	initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the shdw package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();

		//------------------------------------------------------------------------
		//	InitGraphics - should be called after device is created
		//------------------------------------------------------------------------
		static void InitGraphics();

		//------------------------------------------------------------------------
		//	CleanUpGraphics - should be called before device is destroyed
		//------------------------------------------------------------------------
		static void CleanUpGraphics();
};
