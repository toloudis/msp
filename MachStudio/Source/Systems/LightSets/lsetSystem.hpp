/*****************************************************************************
**  lsetSystem.hpp
**
**      System for grouping lights and objects into light sets
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef LSET_SYSTEM_HPP
#error lsetSystem.hpp multiply included
#endif
#define LSET_SYSTEM_HPP


namespace lsetSystem
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
