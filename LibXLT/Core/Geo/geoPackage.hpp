/*****************************************************************************
**  geoPackage.hpp
**
**      The geoPackage namespace contains the initialization functions
**	for the geo package.  
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GEO_PACKAGE_HPP
#error geoPackage.hpp multiply included
#endif
#define GEO_PACKAGE_HPP


//============================================================================
//============================================================================
class geoPackage
{
	public:

		//------------------------------------------------------------------------
		//	Init must be called before you use this package.  A good place to
		//	do this is in your geoin function, with your other package initializers.
		//------------------------------------------------------------------------
		static void Init();

		//------------------------------------------------------------------------
		//	CleanUp should be called after you are done with this package.
		//	A good place to do this is in your geoin function, after you are done
		//	with other deinitialization and cleanup tasks.
		//------------------------------------------------------------------------
		static void CleanUp();
};
