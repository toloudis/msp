/*****************************************************************************
**  matPackage.hpp
**
**      matPackage contains the initialization functions
**	for the mat package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_PACKAGE_HPP
#error matPackage.hpp multiply included
#endif
#define MAT_PACKAGE_HPP

class matPackage
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
