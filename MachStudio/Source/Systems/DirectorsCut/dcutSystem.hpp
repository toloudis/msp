/*****************************************************************************
**  dcutSystem.hpp
**
**      System DirectorsCut
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef DCUT_SYSTEM_HPP
#error dcutSystem.hpp multiply included
#endif
#define DCUT_SYSTEM_HPP

class g2dSystem;
class fsLocator;
class itString;

namespace dcutSystem
{
		//--------------------------------------------------------------------
		// Init  - needs the system for creating rendering views
		//--------------------------------------------------------------------
		void Init(g2dSystem *i_pSystem);

		//--------------------------------------------------------------------
		// CleanUp -- cleanup system
		//--------------------------------------------------------------------
		void CleanUp();
};
