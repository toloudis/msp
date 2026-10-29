/*****************************************************************************
**  billSystem.hpp
**
**      System for adding dynamic billboards to the world.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_SYSTEM_HPP
#error billSystem.hpp multiply included
#endif
#include <string>
#define BILL_SYSTEM_HPP

class fsLocator;


namespace billSystem
{
		//--------------------------------------------------------------------
		// Init -- initialize system with directory to use to look for
		//		geometry files
		//--------------------------------------------------------------------
		void Init(const fsLocator& i_AppDir, const std::string& i_BillboardDataDir);

		//--------------------------------------------------------------------
		// CleanUp -- cleanup system
		//--------------------------------------------------------------------
		void CleanUp();
};
