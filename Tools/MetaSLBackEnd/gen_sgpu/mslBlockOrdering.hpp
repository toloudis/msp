/*****************************************************************************
**	mslBlockOrdering.hpp
**
**	 mslBlockOrdering handles the ordering of functions so that
**	the functions used are declared first.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_BLOCKORDERING_HPP
#error mslBlockOrdering.hpp multiply included
#endif
#define MSL_BLOCKORDERING_HPP

#ifndef MSL_DEFS_HPP
#include "mslDefs.hpp"
#endif

#include <vector>
#include <map>
#include <set>

//============================================================================
//============================================================================
namespace mslBlockOrdering 
{
	//------------------------------------------------------------------------
	//	GetBlockOrdering - determine the correct order to export the
	//		given functions declarations.
	//------------------------------------------------------------------------
	void GetBlockOrdering(ISyntax_tree *blocks,
						 std::vector<int> &o_BlockOrdering);

	//------------------------------------------------------------------------
	//	GetAllFunctionDefinitions - find all functions defined in the compilation
	//	unit and its imports. Functions are returned as multimap from 
	//  function name to definitions (can have multiple functions with same
	//	name but different arguments).
	//------------------------------------------------------------------------
	void GetAllFunctionDefinitions(const ICompilation_unit *i_CompilationUnit,
						  std::multimap<std::string, ISyntax_tree*> &o_FunctionDefinitions);

	//------------------------------------------------------------------------
	//	GetAllFunctionsCalled - find all functions used by the main function
	//	in the given function definition list, recursively looking for 
	//	function calls within those functions.
	//------------------------------------------------------------------------
	void GetAllFunctionsCalled(const std::multimap<std::string, ISyntax_tree*> &i_FunctionDefinitions,
							   std::set<std::string> &o_FunctionNames);	
	
	//------------------------------------------------------------------------
	// GetFunctionName - Recurse through tree looking for the function declaration
	// in order to get the name of the function.
	//------------------------------------------------------------------------
	bool GetFunctionName(ISyntax_tree *tree,
					     std::string &o_FunctionName);
};

