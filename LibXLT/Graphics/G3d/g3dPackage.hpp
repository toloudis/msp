/*****************************************************************************
**	g3dPackage.hpp
**
**		g3dPackage contains the initialization functions
**	for the g3d package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_PACKAGE_HPP
#error g3dPackage.hpp multiply included
#endif
#define G3D_PACKAGE_HPP

#include <vector>


//============================================================================
//============================================================================
class g3dPackage
{
	public:
		//------------------------------------------------------------------------
		//	Init must be called before you use the g3d package.  A good place to
		//	do this is in your main function, with your other package
		//	initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with the g3d package.
		//	A good place to do this is in your main function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};

