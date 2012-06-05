/*****************************************************************************
**	mslDumpAST.hpp
**
**	 mslDumpAST prints out debugging info about an abstract syntax tree.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_DUMPAST_HPP
#error mslDumpAST.hpp multiply included
#endif
#define MSL_DUMPAST_HPP

#ifndef MSL_DEFS_HPP
#include "mslDefs.hpp"
#endif

class mslGeneratedShader;

//============================================================================
//============================================================================
namespace mslDumpAST
{

	 void dump_ast(   ILog *log,
					  const MSDK::ICompilation_unit *comp_unit,
                      const MSDK::ICompiler_options *comp_options,
                      mslGeneratedShader *shader);
};

