/*****************************************************************************
**  fogSystem.hpp
**
**      System for adding static object with no transformation into level
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef FOG_SYSTEM_HPP
#error fogSystem.hpp multiply included
#endif
#define FOG_SYSTEM_HPP


namespace fogSystem
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
