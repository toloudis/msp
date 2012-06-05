/*****************************************************************************
**	scrtysPackage.hpp
**
**		Initializes Software Shield implementation for scrty
**	user interface package
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SCRTYS_PACKAGE_HPP
#error scrtysPackage.hpp multiply included
#endif
#define SCRTYS_PACKAGE_HPP


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace scrtysPackage
{
	//---------------------------------------------------------------------------
	// Initialize() - initialize the package
	//---------------------------------------------------------------------------
	void Initialize();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void DeInitialize();
};

