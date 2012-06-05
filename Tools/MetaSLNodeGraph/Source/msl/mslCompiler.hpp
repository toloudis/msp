/*****************************************************************************
**	mslCompiler.hpp
**
**	 mslCompiler takes a MetaSL graph and converts it to HLSL.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_COMPILER_HPP
#error mslCompiler.hpp multiply included
#endif
#define MSL_COMPILER_HPP

#ifndef MSL_DEFS_HPP
#include "mslDefs.hpp"
#endif

#include <string>

//============================================================================
//============================================================================
namespace mslCompiler 
{
	//--------------------------------------------------------------------
	// Compile
	//--------------------------------------------------------------------
	bool  Compile(const std::string & i_ShaderName,
				   IMill_compiler *i_pMillCompiler,
				   IGraph_library *i_pGraphLibrary,
				   std::string &o_ShaderString,
				   std::string &o_MetaSLString);

}
