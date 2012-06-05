/*****************************************************************************
**	mslVisitorHLSL.hpp
**
**	 mslVisitorHLSL implements mslVisitor for HLSL syntax
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_VISITORHLSL_HPP
#error mslVisitorHLSL.hpp multiply included
#endif
#define MSL_VISITORHLSL_HPP

#ifndef MSL_VISITOR_HPP
#include "mslVisitor.hpp"
#endif

#include <string>

class mslGeneratedShader;

//============================================================================
//! Msl_visitor is a concrete subclass of Visitor that dumps the shader
//! in HLSL format.
//============================================================================
class mslVisitorHLSL : public mslVisitor 
{
public:

    //! Constructor.
    /*!
     *  \param  log     The logging interface.
     *  \param  shader  The generated shader.
     */
    mslVisitorHLSL(ILog *log, mslGeneratedShader *shader);

    //! Print code to the code string.
    /*!
     *  \param  code    A printf-style format string.
     */
    virtual void print_code(const char *code,...);

    //! Print tabs to the code string.
    /*!
     *  \param  tabs    The number of tabs to print.
     */
    virtual void print_tabs(int tabs);

    virtual void begin_shader_file();

    virtual void end_shader_file();

    virtual void begin_compilation_unit();

    virtual void end_compilation_unit();

    virtual void begin_import(const char *file_name);

    virtual void end_import();

    virtual void begin_shader(
                    const mslQualifiedName *superclass,
                    const char *name);

    virtual void end_shader();

    virtual void begin_brdf(
                    const mslQualifiedName *superclass,
                    const char *name);

    virtual void end_brdf();

    virtual void begin_technique(
                    const mslQualifiedName *superclass,
                    const char *name);

    virtual void end_technique();

    virtual void begin_inputs();

    virtual void end_inputs();

    virtual void begin_outputs();

    virtual void end_outputs();

    virtual void begin_members();

    virtual void end_members();

    virtual void begin_declaration(int depth);

    virtual void end_declaration(int depth);

    virtual void begin_definition(int depth);

    virtual void end_definition(int depth);

    virtual void variable_declaration(
                    const char *storage,
                    const mslQualifiedName *type,
                    const char *name,
                    const char *dimension);

    virtual void parameter_declaration(
                    int position,
                    const char *mode,
                    const mslQualifiedName *type,
                    const char *name,
                    const char *dimension);

    virtual void begin_function_declaration(
                    const mslQualifiedName *type,
                    const char *name);

    virtual void end_function_declaration();

    virtual void begin_constructor_declaration(	
	const char *name,
	bool is_static);

    virtual void end_constructor_declaration();

    virtual void begin_destructor_declaration(	
	const char *name,
	bool is_static);

    virtual void end_destructor_declaration();

    virtual void begin_initializer(int depth);

    virtual void end_initializer(int depth);

    virtual void begin_block(int depth);

    virtual void end_block(int depth);

    virtual void begin_expression_statement(int depth);

    virtual void end_expression_statement(int depth);

    virtual void bool_literal(const char *value);

    virtual void int_literal(const char *value);

    virtual void scalar_literal(const char *value);

    virtual void string_literal(const char *value);

    virtual void begin_array_literal(int depth);

    virtual void begin_array_literal_item(int depth,int position);

    virtual void end_array_literal_item(int depth,int position);

    virtual void end_array_literal(int depth);

    virtual void var_ref(mslQualifiedName *name,const IDeclaration *declaration);

    virtual void state_var_ref(
                    const char *name,
                    const IDeclaration *declaration);

    virtual void type(const char *name);

    virtual void begin_enum(int depth,mslQualifiedName *name);

    virtual void enum_value(int position,mslQualifiedName *name);

    virtual void enum_value(int position,mslQualifiedName *name,int value);

    virtual void end_enum(int depth);

    virtual void begin_struct(int depth,mslQualifiedName *name);

    virtual void end_struct(int depth);

    virtual void begin_field(int depth,const char *name);

    virtual void end_field(int depth,const char *name);

    virtual void begin_expression_list();

    virtual void begin_expression_list_item(int position);

    virtual void end_expression_list_item(int position);

    virtual void begin_conditional_expression();

    virtual void begin_conditional_condition();

    virtual void end_conditional_condition();

    virtual void begin_conditional_then();

    virtual void end_conditional_then();

    virtual void begin_conditional_else();

    virtual void end_conditional_else();

    virtual void end_conditional_expression();

    virtual void end_expression_list();

    virtual void op(IType *type,const char *name);

    virtual void begin_pre_op(
                    IType *type,const char *name,bool parentheses = true);

    virtual void end_pre_op(
                    IType *type,const char *name,bool parentheses = true);

    virtual void begin_post_op(
                    IType *type,const char *name,bool parentheses = true);

    virtual void end_post_op(
                    IType *type,const char *name,bool parentheses = true);

    virtual void begin_bin_op(
                    IType *type,const char *name,bool parentheses = true);

    virtual void end_bin_op(
                    IType *type,const char *name,bool parentheses = true);

    virtual void begin_cast(const mslQualifiedName *type);

    virtual void end_cast(const mslQualifiedName *type);

    virtual void begin_call(
                    const mslQualifiedName *name,
                    const IDeclaration *declaration,
					bool i_bRequiresState = false);

    virtual void end_call();

    virtual void begin_state_call(
                    const mslQualifiedName *name,
                    const IDeclaration *declaration);

    virtual void end_state_call();

    virtual void begin_argument(int position);

    virtual void end_argument(int position);

    virtual void begin_if(int depth);

    virtual void begin_if_condition();

    virtual void end_if_condition();

    virtual void begin_then(int depth);

    virtual void end_then(int depth);

    virtual void begin_else(int depth);

    virtual void end_else(int depth);

    virtual void end_if(int depth);

    virtual void begin_switch(int depth);

    virtual void begin_switch_selector();

    virtual void end_switch_selector();

    virtual void begin_case_label(int depth);

    virtual void end_case_label(int depth);

    virtual void default_label(int depth);

    virtual void end_switch(int depth);

    virtual void begin_while(int depth);

    virtual void begin_while_condition(int depth);

    virtual void end_while_condition(int depth);

    virtual void end_while(int depth);

    virtual void begin_do_while(int depth);

    virtual void begin_do_while_condition(int depth);

    virtual void end_do_while_condition(int depth);

    virtual void end_do_while(int depth);

    virtual void begin_for(int depth);

    virtual void begin_for_init();

    virtual void end_for_init();

    virtual void begin_for_condition();

    virtual void end_for_condition();

    virtual void begin_for_update();

    virtual void end_for_update();

    virtual void end_for(int depth);

    virtual void begin_foreach(int depth);

    virtual void begin_foreach_iterator();

    virtual void end_foreach_iterator();

    virtual void end_foreach(int depth);

    virtual void break_statement(int depth);

    virtual void continue_statement(int depth);

    virtual void begin_return_statement(int depth);

    virtual void end_return_statement(int depth);

    virtual void begin_annotation(int depth);

    virtual void end_annotation(int depth);

private:
    //! The logging interface.
    ILog *m_log;

    //! The generated shader.
    mslGeneratedShader *m_shader;

};
