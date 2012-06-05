/*****************************************************************************
**  skySystem.hpp
**
**      System for adding static object with no transformation into level
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef SKY_SYSTEM_HPP
#error skySystem.hpp multiply included
#endif
#define SKY_SYSTEM_HPP


namespace skySystem
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
