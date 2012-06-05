/*****************************************************************************
**  giSystem.hpp
**
**      System for adding static object with no transformation into level
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef GI_SYSTEM_HPP
#error giSystem.hpp multiply included
#endif
#define GI_SYSTEM_HPP


namespace giSystem
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
