/*****************************************************************************
**  hairPackage.hpp
**
**      hairPackage contains the initialization functions
**	for the hair package.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef HAIR_PACKAGE_HPP
#error hairPackage.hpp multiply included
#endif
#define HAIR_PACKAGE_HPP

class hairPackage
{
	public:

		//------------------------------------------------------------------------
		//	Init must be called before you use the hair package.  A good place to
		//	do this is in your main function, with your other package
		//	initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the hair package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};
