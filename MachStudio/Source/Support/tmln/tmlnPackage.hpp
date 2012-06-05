/*****************************************************************************
**  tmlnPackage.hpp
**
**      Timeline Package
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_PACKAGE_HPP
#error tmlnPackage.hpp multiply included
#endif
#define TMLN_PACKAGE_HPP


class tmlnPackage
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
