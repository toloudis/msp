/*****************************************************************************
**  splnPackage.hpp
**
**      Timeline Package
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef SPLN_PACKAGE_HPP
#error splnPackage.hpp multiply included
#endif
#define SPLN_PACKAGE_HPP


class splnPackage
{
	public:

		//--------------------------------------------------------------------
		// Init
		//--------------------------------------------------------------------
		static void Init();

		//--------------------------------------------------------------------
		// CleanUp
		//--------------------------------------------------------------------
		static void CleanUp();
};
