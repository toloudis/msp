/*****************************************************************************
**  todSystem.hpp
**
**      System for adding static object with no transformation into level
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef TOD_SYSTEM_HPP
#error todSystem.hpp multiply included
#endif
#define TOD_SYSTEM_HPP


namespace todSystem
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
