/*****************************************************************************
**	mslBlockOrdering.cpp
**
**	 see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslBlockOrdering.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include <map>
#include <set>

namespace mslBlockOrdering
{

	namespace 
	{
		// Recurse through tree looking for the function declaration
		// in order to get the name of the function.
		bool get_function_name(ISyntax_tree *tree,
							   std::string &o_FunctionName)
		{
			ISyntax_tree::Node_type type = tree->get_node_type();
			if (type == ISyntax_tree::FUNCTION_DECLARATOR)
			{
				if (tree->get_child_count() >= 1)
				{
					bool bFound = false;
					ISyntax_tree *child = tree->get_child(0);
					if (child->get_node_type() == ISyntax_tree::IDENTIFIER)
					{
						o_FunctionName = child->get_string_value();
						bFound = true;
					}
					child->release();
					if (bFound)
						return true;
				}
			}

			int child_count = tree->get_child_count();
			for(int i = 0; i < child_count; i++) {
				ISyntax_tree *child = tree->get_child(i);
				bool bFound = get_function_name(child, o_FunctionName);
				child->release();

				if (bFound) 
					return true;
			}
			return false;
		}

		// Get all method functions called by the function defined in the
		// given block.
		void get_functions_called(ISyntax_tree *tree,
								  std::set<std::string> &o_FunctionCallSet)
		{
			ISyntax_tree::Node_type type = tree->get_node_type();
			if (type == ISyntax_tree::FUNCTION_CALL)
			{
				std::string function_name = tree->get_string_value();
				IDeclaration *declaration = tree->get_declaration();
				if (declaration) {
					// Don't want to limit to methods anymore
					//if (declaration->is_method()) {
						o_FunctionCallSet.insert(function_name);
					//}
				}
			}
			int child_count = tree->get_child_count();
			for(int i = 0; i < child_count; i++) {
				ISyntax_tree *child = tree->get_child(i);
				get_functions_called(child, o_FunctionCallSet);
				child->release();
			}
		}

		// Look through syntax tree and find all function definitions.
		void get_all_function_definitions(ISyntax_tree *i_Tree,
						  std::multimap<std::string, ISyntax_tree*> &o_FunctionDefinitions)
		{
			ISyntax_tree::Node_type type = i_Tree->get_node_type();
			if (type == ISyntax_tree::FUNCTION_DEFINITION)
			{
				std::string function_name;
				if (get_function_name(i_Tree, function_name))
				{
					i_Tree->reference();
					o_FunctionDefinitions.insert(make_pair(function_name, i_Tree));
				}
			}
			int child_count = i_Tree->get_child_count();
			for(int i = 0; i < child_count; i++) {
				ISyntax_tree *child = i_Tree->get_child(i);
				get_all_function_definitions(child, o_FunctionDefinitions);
				child->release();
			}
		}

		// Add the block indices for the functions called by this function
		// to the o_BlockOrdering. Recurses on function calls in order to
		// add used functions before functions that use them.
		void add_function_calls(ISyntax_tree *blocks, 
								int i_BlockIndex,
								std::vector<int> &o_BlockOrdering, 
								const std::map<std::string, int> &i_BlockIndexMap,
								std::set<int> &o_BlocksConsidered)
		{
			// Blocks Considered makes sure we don't make a loop
			// by making sure we don't consider the same block more than once.
			o_BlocksConsidered.insert(i_BlockIndex);

			// Get functions called by this function
			ISyntax_tree *function = blocks->get_child(i_BlockIndex);
			std::set<std::string> function_call_set;
			get_functions_called(function, function_call_set);

			// Consider each of these functions
			std::set<std::string>::const_iterator it;
			for (it = function_call_set.begin(); it != function_call_set.end(); ++it)
			{
				// Get block index for this function name
				std::map<std::string, int>::const_iterator bi = i_BlockIndexMap.find(*it);
				if (bi != i_BlockIndexMap.end())
				{
					int func_ind = bi->second;
					// Only consider this function index if we have not already considered it
					if (o_BlocksConsidered.find(func_ind) == o_BlocksConsidered.end())
					{
						// Recurse on the function calls in this function first and then
						// add the index for this function
						add_function_calls(blocks, func_ind, o_BlockOrdering, i_BlockIndexMap, o_BlocksConsidered);
					}
				}
			}
						
			o_BlockOrdering.push_back(i_BlockIndex);

			function->release();
		}

		
		// Find the names of functions called by the function definition 
		// in the given syntax tree. Recurse on the functions that have not already 
		// been added.
		void add_function_calls(ISyntax_tree *i_Tree, 
								const std::multimap<std::string, ISyntax_tree*> &i_FunctionDefinitions,
								std::set<std::string> &o_FunctionNames)
		{
			// Find functions called by this function
			std::set<std::string> functions;
			get_functions_called(i_Tree, functions);

			std::set<std::string>::iterator it;
			for (it = functions.begin(); it != functions.end(); ++it)
			{
				std::string function_name = (*it);

				// See if the function has been added already
				if (o_FunctionNames.find(function_name) == o_FunctionNames.end())
				{
					bool bInserted = false;

					// There can be more than one function definition with the same
					// name. So, we have to look through all of them.
					std::multimap<std::string, ISyntax_tree*>::const_iterator bi;
					for (bi = i_FunctionDefinitions.lower_bound(function_name);
						 bi != i_FunctionDefinitions.upper_bound(function_name); ++bi)
					{
						// Insert function name only once, but only if the function
						// was found. This separates out the standard function calls
						// from our global and shader function definition calls.
						if (!bInserted)
						{
							o_FunctionNames.insert(function_name);
							bInserted = true;
						}

						// Recursively add function calls within this function definition
						add_function_calls(bi->second, i_FunctionDefinitions, o_FunctionNames);
					}
				}
			}
		}
	}

	
	//------------------------------------------------------------------------
	//	GetBlockOrdering - determine the correct order to export the
	//		given functions declarations.
	//------------------------------------------------------------------------
	void GetBlockOrdering(ISyntax_tree *blocks,
						 std::vector<int> &o_BlockOrdering)
	{
		int n_blocks = blocks->get_child_count();
		if (n_blocks < 2)
			return;
		
		// Simple implementation, reverses order and leaves out index "0"
		//for (int b = n_blocks-1; b >= 1; b--) {
		//	o_BlockOrdering.push_back(b);
		//}

		// More complicated and correct implementation, detect which 
		// functions call which others and define ordering from that information.
		// Also allows us to throw away functions blocks that we don't use
		// that were included from a shared import msl.

		// First get the function name for each block as a map.
		std::map<std::string, int> block_index_map;
		for (int b=1; b<n_blocks; b++)
		{
			ISyntax_tree *function = blocks->get_child(b);
			std::string function_name;
			if (get_function_name(function,function_name))
				block_index_map[function_name] = b;
			function->release();
		}

		// Then recursively add functions, starting with the main function (index 0)	
		int main_index = 0;
		std::set<int> blocks_considered;
		add_function_calls(blocks, main_index, o_BlockOrdering, block_index_map, blocks_considered);
	}


	//------------------------------------------------------------------------
	//	GetAllFunctionDefinitions - find all functions defined in the compilation
	//	unit and its imports. Functions are returned as multimap from 
	//  function name to definitions (can have multiple functions with same
	//	name but different arguments).
	//------------------------------------------------------------------------
	void GetAllFunctionDefinitions(const ICompilation_unit *i_CompilationUnit,
						  std::multimap<std::string, ISyntax_tree*> &o_FunctionDefinitions)
	{

		// Recurse through imported compilation units
		int import_count = i_CompilationUnit->get_import_count();
		for(int i = 0; i < import_count; i++) {
			const char *file_name = i_CompilationUnit->get_import_file_name(i);
			if (strcmp(file_name, "standard")!=0) {
				ICompilation_unit *i_cunit = i_CompilationUnit->get_import_compilation_unit(i);
				GetAllFunctionDefinitions(i_cunit, o_FunctionDefinitions);
				i_cunit->release();
			}
		}
		
		// Get all global function definitions for this compilation unit
		int global_declaration_count = i_CompilationUnit->get_global_declaration_count();
		for(int i = 0; i < global_declaration_count; i++) {
			ISyntax_tree *tree = i_CompilationUnit->get_global_declaration_syntax_tree(i);
			get_all_function_definitions(tree, o_FunctionDefinitions);
			tree->release();
		}
		
		// Get all shader function definitions for this compilation unit
		int shader_count = i_CompilationUnit->get_shader_count();
		for(int i = 0; i < shader_count; i++) {
			ISyntax_tree *tree = i_CompilationUnit->get_shader_syntax_tree(i);
			get_all_function_definitions(tree, o_FunctionDefinitions);
			tree->release();
		}
	}

	//------------------------------------------------------------------------
	//	GetAllFunctionsCalled - find all functions used by the main function
	//	in the given function definition list, recursively looking for 
	//	function calls within those functions.
	//------------------------------------------------------------------------
	void GetAllFunctionsCalled(const std::multimap<std::string, ISyntax_tree*> &i_FunctionDefinitions,
							   std::set<std::string> &o_FunctionNames)
	{
		// Start with the main function:
		std::multimap<std::string, ISyntax_tree*>::const_iterator bi = i_FunctionDefinitions.find("main");
		if (bi != i_FunctionDefinitions.end())
		{
			 add_function_calls(bi->second, i_FunctionDefinitions, o_FunctionNames);
		}
	}


	//------------------------------------------------------------------------
	// GetFunctionName - Recurse through tree looking for the function declaration
	// in order to get the name of the function.
	//------------------------------------------------------------------------
	bool GetFunctionName(ISyntax_tree *tree,
					     std::string &o_FunctionName)
	{
		return get_function_name(tree, o_FunctionName);
	}

}	// end of namespace

