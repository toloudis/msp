/*****************************************************************************
**  trfnSystem.hpp
**
**      System for grouping lights and objects into light sets
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef TRFN_SYSTEM_HPP
#error trfnSystem.hpp multiply included
#endif
#define TRFN_SYSTEM_HPP


namespace trfnSystem
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
