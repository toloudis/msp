/*****************************************************************************
**  fbxPackage.hpp
**
**      fbxPackage contains the initialization functions
**	for the FBX importer package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_PACKAGE_HPP
#error fbxPackage.hpp multiply included
#endif
#define FBX_PACKAGE_HPP

class fbxPackage
{
	public:

		//------------------------------------------------------------------------
		//	Init must be called before you use the package.  A good place to
		//	do this is in your main function, with your other package
		//	initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};
