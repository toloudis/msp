/*****************************************************************************
**  xtraPackage.hpp
**
**      Init and CleanUp of xtraPackage
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef XTRA_PACKAGE_HPP
#error xtraPackage.hpp multiply included
#endif
#define XTRA_PACKAGE_HPP


namespace xtraPackage
{
		//--------------------------------------------------------------------
		// Init
		//--------------------------------------------------------------------
		void Init();

		//--------------------------------------------------------------------
		// CleanUp -- cleanup system
		//--------------------------------------------------------------------
		void CleanUp();
};
