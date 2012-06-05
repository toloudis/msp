/*****************************************************************************
**	mslDumpAST.cpp
**
**	 see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslDumpAST.hpp"

#include "mslCodeGenerator.hpp"
#include "mslGeneratedShader.hpp"

#include "Core/dbg/dbgMsg.hpp"

namespace 
{

	//! Indent output.
	/*!
	 *  \param  shader  The shader.
	 *  \param  depth   The depth to which to indent.
	 */
	static void indent(
		mslGeneratedShader *shader,
		int depth)
	{
		for(int i = 0; i < depth; i++)
			shader->print_code("  ");
	}

	//! Dump type.
	/*!
	 *  \param  shader  The shader.
	 *  \param  type    The type.
	 *  \param  depth   The depth to which to indent.
	 */
	static void dump_type(
		mslGeneratedShader *shader,
		const IType *type,
		int depth = 0)
	{
		IType::Type_code type_code = type->get_typecode();
		indent(shader,depth);
		shader->print_code("type code:\t'%s'\n",
							mslCodeGenerator::type_code_to_c_str(type_code));
		const char *type_name = type->get_type_name();
		if(type_name) {
			indent(shader,depth);
			shader->print_code("type name:\t'%s'\n",type_name);
		}
		const char *member_name = type->get_member_name();
		if(member_name) {
			indent(shader,depth);
			shader->print_code("member name:\t'%s'\n",member_name);
		}
		int array_size = type->get_array_size();
		indent(shader,depth);
		shader->print_code("array size:\t%d\n",array_size);
		int value_size = type->get_value_size();
		indent(shader,depth);
		shader->print_code("value size:\t%d\n",value_size);
		int alignment = type->get_alignment();
		indent(shader,depth);
		shader->print_code("alignment:\t%d\n",alignment);
		int element_size = type->get_element_size();
		indent(shader,depth);
		shader->print_code("element size:\t%d\n",element_size);
		IType *t = type->get_child();
		while(t) {
			shader->print_code("\n");
			dump_type(shader,t,depth+1);
			IType *n = t->get_next();
			t->release();
			t = n;
		}
	}

	//! Dump type.
	/*!
	 *  \param  shader  The shader.
	 *  \param  tree    The tree.
	 *  \param  depth   The depth to which to indent.
	 */
	static void dump_tree(
		mslGeneratedShader *shader,
		const ISyntax_tree *tree,
		int depth = 0)
	{
		indent(shader,depth);
		const char *node_type = 
					mslCodeGenerator::node_type_to_c_str(tree->get_node_type());
		shader->print_code("%s",node_type);
		if(tree->get_data_size() < 0)
			shader->print_code(" \"%s\"",tree->get_string_value());
		shader->print_code("\n");
		IDeclaration *declaration = tree->get_declaration();
		if(declaration) {
			if(declaration->is_input()) {
				indent(shader,depth);
				shader->print_code("input\n");
			}
			if(declaration->is_output()) {
				indent(shader,depth);
				shader->print_code("output\n");
			}
			if(declaration->is_member()) {
				indent(shader,depth);
				shader->print_code("member\n");
			}
			if(declaration->is_local()) {
				indent(shader,depth);
				shader->print_code("local variable\n");
			}
			if(declaration->is_method()) {
				indent(shader,depth);
				shader->print_code("method\n");
			}
			if(declaration->is_function()) {
				indent(shader,depth);
				shader->print_code("function\n");
			}
			if(declaration->is_parameter()) {
				indent(shader,depth);
				shader->print_code("parameter\n");
			}
			if(declaration->is_array()) {
				indent(shader,depth);
				if(declaration->is_dynamic()) {
					shader->print_code("array[]\n");
				} else {
					shader->print_code("array[%d]\n",declaration->get_size());
				}
			}
		}
		int child_count = tree->get_child_count();
		for(int i = 0; i < child_count; i++) {
			ISyntax_tree *child = tree->get_child(i);
			dump_tree(shader,child,depth+1);
			child->release();
		}
	}
} // end of namespace

//! Dump abstract syntax tree.
/*!
 *  \param  log             The log.
 *  \param  comp_unit       The compilation unit.
 *  \param  comp_options    The compilation options.
 *  \param  shader          The shader.
 */
void mslDumpAST::dump_ast(
	ILog *log,
	const ICompilation_unit *comp_unit,
	const ICompiler_options *comp_options,
	mslGeneratedShader *shader)
{

	int import_count = comp_unit->get_import_count();
	for(int i = 0; i < import_count; i++) {
		const char *file_name = comp_unit->get_import_file_name(i);
		ICompilation_unit *i_cunit = comp_unit->get_import_compilation_unit(i);
		shader->print_code("/" "*** import \"%s\" ***/\n\n",file_name);
		dump_ast(log,i_cunit,comp_options,shader);
		shader->print_code("/" "*** end import ***/\n\n\n");
		i_cunit->release();
	}

	int type_definition_count = comp_unit->get_type_definition_count();
	for(int i = 0; i < type_definition_count; i++) {
		IType *type = comp_unit->get_type_definition_type(i);
		const char *name = comp_unit->get_type_definition_name(i);
		shader->print_code("/" "*** type definition %s\n\n",name);
		dump_type(shader,type);
		shader->print_code("\n***" "/\n\n");
		type->release();
	}

	int global_declaration_count = comp_unit->get_global_declaration_count();
	for(int i = 0; i < global_declaration_count; i++) {
		ISyntax_tree *tree = comp_unit->get_global_declaration_syntax_tree(i);
		shader->print_code("/" "*** global declaration\n\n");
		dump_tree(shader,tree);
		shader->print_code("\n***" "/\n\n");
		shader->print_code("\n\n");
		tree->release();
	}

	int shader_count = comp_unit->get_shader_count();
	for(int i = 0; i < shader_count; i++) {
		ISyntax_tree *tree = comp_unit->get_shader_syntax_tree(i);
		const char *name = comp_unit->get_shader_name(i);
		shader->print_code("/*** shader definition %s\n\n",name);
		dump_tree(shader,tree);
		shader->print_code("\n***/\n\n");
		tree->release();
	}
}
