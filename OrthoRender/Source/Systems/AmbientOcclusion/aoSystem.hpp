/*****************************************************************************
**  aoSystem.hpp
**
**      System for adding static object with no transformation into level
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef AO_SYSTEM_HPP
#error aoSystem.hpp multiply included
#endif
#define AO_SYSTEM_HPP


namespace aoSystem
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
