/*****************************************************************************
**  billSystem.hpp
**
**      System for adding dynamic billboards to the world.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_SYSTEM_HPP
#error billSystem.hpp multiply included
#endif
#define BILL_SYSTEM_HPP

class fsLocator;
class itString;


namespace billSystem
{
		//--------------------------------------------------------------------
		// Init -- initialize system with directory to use to look for
		//		geometry files
		//--------------------------------------------------------------------
		void Init(const fsLocator& i_AppDir, const itString& i_BillboardDataDir);

		//--------------------------------------------------------------------
		// CleanUp -- cleanup system
		//--------------------------------------------------------------------
		void CleanUp();
};
