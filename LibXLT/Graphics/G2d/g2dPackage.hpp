/*****************************************************************************
**  g2dPackage.hpp
**
**      g2dPackage contains the initialization functions
**	for the g2d package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_PACKAGE_HPP
#error g2dPackage.hpp multiply included
#endif
#define G2D_PACKAGE_HPP


//============================================================================
//============================================================================
class g2dPackage
{
	public:
		//------------------------------------------------------------------------
		//	Init must be called before you use the g2d package.  A good place to
		//	do this is in your main function, with your other package initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the g2d package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};
