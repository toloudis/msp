/*****************************************************************************
**  chDefs.hpp
**
**      chDefs contains several definitions global to the ch package,
**	and a useful function.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CH_DEFS_HPP
#error chDefs.hpp multiply included
#endif
#define CH_DEFS_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
//============================================================================
namespace chDefs
{
	typedef uint32_t Name;
	typedef uint32_t Size;
	typedef uint16_t Version;

	//------------------------------------------------------------------------
	//	MakeName returns a chDefs::Name made from the 4 given characters.
	//	The characters will appear in the order given on 
	//	little-endian machines.
	//------------------------------------------------------------------------
	Name MakeName(char i_1, char i_2, char i_3, char i_4);

//----------------------------------------------------------------------------
//	Implementation
//----------------------------------------------------------------------------
	
inline Name MakeName(char i_1, char i_2, char i_3, char i_4)
{
	Name ret_val = i_4;

	ret_val <<= 8;
	ret_val |= i_3;

	ret_val <<= 8;
	ret_val |= i_2;

	ret_val <<= 8;
	ret_val |= i_1;

	return ret_val;
}

}
