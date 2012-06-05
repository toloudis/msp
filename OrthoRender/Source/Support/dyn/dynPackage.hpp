/*****************************************************************************
**  dynPackage.hpp
**
**      System Cmra
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DYN_PACKAGE_HPP
#error dynPackage.hpp multiply included
#endif
#define DYN_PACKAGE_HPP


namespace dynPackage
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
