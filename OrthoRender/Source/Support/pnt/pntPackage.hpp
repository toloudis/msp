/*****************************************************************************
**  pntPackage.hpp
**
**      Timeline Package
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PNT_PACKAGE_HPP
#error pntPackage.hpp multiply included
#endif
#define PNT_PACKAGE_HPP


class pntPackage
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
