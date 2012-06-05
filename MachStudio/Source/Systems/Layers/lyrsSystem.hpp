/*****************************************************************************
**  lyrsSystem.hpp
**
**      System for grouping lights and objects into light sets
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef LYRS_SYSTEM_HPP
#error lyrsSystem.hpp multiply included
#endif
#define LYRS_SYSTEM_HPP


namespace lyrsSystem
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
