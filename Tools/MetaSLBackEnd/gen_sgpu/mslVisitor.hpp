/*****************************************************************************
**	mslVisitor.hpp
**
**	 Visitor is the superclass of all syntax tree visitors. 
**	Taken from MentalMill sample code.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_VISITOR_HPP
#error mslVisitor.hpp multiply included
#endif
#define MSL_VISITOR_HPP

#ifndef MSL_DEFS_HPP
#include "mslDefs.hpp"
#endif

class mslQualifiedName;

//============================================================================
//============================================================================
class mslVisitor {

public:

    //! Print code to the code string.
    /*!
     *  \param  code    A printf-style format string.
     */
    virtual void print_code(const char *code,...) = 0;

    //! Begin compilation unit.
    virtual void begin_compilation_unit() = 0;

    //! End compilation unit.
    virtual void end_compilation_unit() = 0;

    //! Begin import.
    /*!
     *  \param  file_name   The file name of the import.
     */
    virtual void begin_import(const char *file_name) = 0;

    //! End import.
    virtual void end_import() = 0;

    //! Begin shader.
    /*!
     *  \param  superclass  The name of the superclass,
     *                      or NULL if there is none.
     *  \param  name        The name of the shader.
     */
    virtual void begin_shader(
                    const mslQualifiedName *superclass,
                    const char *name) = 0;

    //! End shader.
    virtual void end_shader() = 0;

    //! Begin brdf.
    /*!
     *  \param  superclass  The name of the superclass,
     *                      or NULL if there is none.
     *  \param  name        The name of the brdf.
     */
    virtual void begin_brdf(
                    const mslQualifiedName *superclass,
                    const char *name) = 0;

    //! End brdf.
    virtual void end_brdf() = 0;

    //! Begin technique.
    /*!
     *  \param  superclass  The name of the superclass,
     *                      or NULL if there is none.
     *  \param  name        The name of the technique.
     */
    virtual void begin_technique(
                    const mslQualifiedName *superclass,
                    const char *name) = 0;

    //! End technique.
    virtual void end_technique() = 0;

    //! Begin inputs section.
    virtual void begin_inputs() = 0;

    //! End inputs section.
    virtual void end_inputs() = 0;

    //! Begin outputs section.
    virtual void begin_outputs() = 0;

    //! End outputs section.
    virtual void end_outputs() = 0;

    //! Begin members section.
    virtual void begin_members() = 0;

    //! End members section.
    virtual void end_members() = 0;

    //! Begin declaration.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void begin_declaration(int depth) = 0;

    //! End declaration.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void end_declaration(int depth) = 0;

    //! Begin definition.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void begin_definition(int depth) = 0;

    //! End definition.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void end_definition(int depth) = 0;

    //! Variable declaration.
    /*!
     *  \param  storage     The storage type specifier,
     *                      or NULL if there is none.
     *  \param  type        The type of the variable.
     *  \param  name        The name of the variable.
     *  \param  dimension   The array dimension of the variable,
     *                      or NULL if it is not an array.
     *                      For dynamic arrays this is the empty string.
     */
    virtual void variable_declaration(
                    const char *storage,
                    const mslQualifiedName *type,
                    const char *name,
                    const char *dimension) = 0;

    //! Parameter declaration.
    /*!
     *  \param  position    The position of the parameter in the parameter
     *                      list, counting from zero.
     *  \param  mode        The mode of the parameter.
     *  \param  type        The type of the parameter.
     *  \param  name        The name of the parameter.
     *  \param  dimension   The array dimension of the parameter,
     *                      or NULL if it is not an array.
     *                      For dynamic arrays this is the empty string.
     */
    virtual void parameter_declaration(
                    int position,
                    const char *mode,
                    const mslQualifiedName *type,
                    const char *name,
                    const char *dimension) = 0;

    //! Begin function declaration.
    /*!
     *  \param  type        The type of the function.
     *  \param  name        The name of the function.
     */
    virtual void begin_function_declaration(
                    const mslQualifiedName *type,
                    const char *name) = 0;

    //! End function declaration.
    virtual void end_function_declaration() = 0;      

    //! Begin constructor declaration.
    /*!    
    *  \param  name        The name of the constructor.
    *  \param  is_static   true for static constructor.
    */
    virtual void begin_constructor_declaration(
	const char *name,
	bool is_static) = 0;

    //! End constructor declaration.
    virtual void end_constructor_declaration() = 0;

    //! Begin destructor declaration.
    /*!    
    *  \param  name        The name of the destructor.
    *  \param  is_static   true for static destructor.
    */
    virtual void begin_destructor_declaration(
	const char *name,
	bool is_static) = 0;

    //! End destructor declaration.
    virtual void end_destructor_declaration() = 0;

    //! Begin initializer.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void begin_initializer(int depth) = 0;

    //! End initializer.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void end_initializer(int depth) = 0;

    //! Begin block.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void begin_block(int depth) = 0;

    //! End block.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void end_block(int depth) = 0;

    //! Begin expression statement.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void begin_expression_statement(int depth) = 0;

    //! End expression statement.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void end_expression_statement(int depth) = 0;

    //! Boolean literal.
    /*!
     *  \param  value   The boolean value as a string.
     */
    virtual void bool_literal(const char *value) = 0;

    //! Integer literal.
    /*!
     *  \param  value   The integer value as a string.
     */
    virtual void int_literal(const char *value) = 0;

    //! Scalar literal.
    /*!
     *  \param  value   The scalar value as a string.
     */
    virtual void scalar_literal(const char *value) = 0;

    //! String literal.
    /*!
     *  \param  value   The string literal.
     */
    virtual void string_literal(const char *value) = 0;

    //! Begin array literal.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void begin_array_literal(int depth) = 0;

    //! Begin array literal item expression.
    /*!
     *  \param  depth       The indentation depth.
     *  \param  position    The position of the item expression in the
     *                      array literal, counting from zero.
     */
    virtual void begin_array_literal_item(int depth,int position) = 0;

    //! End array literal item expression.
    /*!
     *  \param  depth       The indentation depth.
     *  \param  position    The position of the item expression in the
     *                      array literal, counting from zero.
     */
    virtual void end_array_literal_item(int depth,int position) = 0;

    //! End array literal.
    /*!
     *  \param  depth   The indentation depth.
     */
    virtual void end_array_literal(int depth) = 0;

    //! Reference to variable.
    /*!
     *  \param  name        The name of the variable.
     *  \param  declaration The declaration of the variable.
     */
    virtual void var_ref(mslQualifiedName *name,
                            const IDeclaration *declaration) = 0;

    //! Reference to state variable.
    /*!
     *  \param  name        The name of the state variable.
     *  \param  declaration The declaration of the state variable.
     */
    virtual void state_var_ref(
                    const char *name,
                    const IDeclaration *declaration) = 0;

    //! Type.
    /*!
     *  \param  name        The name of the type.
     */
    virtual void type(const char *name) = 0;

    //! Begin enumeration.
    /*!
     *  \param  depth       The indentation depth.
     *  \param  name        The name of the enumeration.
     */
    virtual void begin_enum(int depth,mslQualifiedName *name) = 0;

    //! Enumeration value.
    /*!
     *  \param  position    The position of the enumeration value in the
     *                      enumeration, counting from zero.
     *  \param  name        The name of the enumeration value.
     */
    virtual void enum_value(int position,mslQualifiedName *name) = 0;

    //! Enumeration value.
    /*!
     *  \param  position    The position of the enumeration value in the
     *                      enumeration, counting from zero.
     *  \param  name        The name of the enumeration value.
     *  \param  value       The value of the enumeration value.
     */
    virtual void enum_value(int position,mslQualifiedName *name,int value) = 0;

    //! Begin enumeration.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_enum(int depth) = 0;

    //! Begin structure.
    /*!
     *  \param  depth       The indentation depth.
     *  \param  name        The name of the structure.
     */
    virtual void begin_struct(int depth,mslQualifiedName *name) = 0;

    //! End structure.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_struct(int depth) = 0;

    //! Begin structure field.
    /*!
     *  \param  depth       The indentation depth.
     *  \param  name        The name of the structure field.
     */
    virtual void begin_field(int depth,const char *name) = 0;

    //! End structure field.
    /*!
     *  \param  depth       The indentation depth.
     *  \param  name        The name of the structure field.
     */
    virtual void end_field(int depth,const char *name) = 0;

    //! Begin expression list.
    virtual void begin_expression_list() = 0;

    //! Begin expression list item.
    /*!
     *  \param  position    The position of the expression item
     *                      in the expression list, counting from zero.
     */
    virtual void begin_expression_list_item(int position) = 0;

    //! End expression list item.
    /*!
     *  \param  position    The position of the expression item
     *                      in the expression list, counting from zero.
     */
    virtual void end_expression_list_item(int position) = 0;

    //! End expression list.
    virtual void end_expression_list() = 0;

    //! Begin conditonal expression.
    virtual void begin_conditional_expression() = 0;

    //! Begin condition of conditonal expression.
    virtual void begin_conditional_condition() = 0;

    //! End condition of conditonal expression.
    virtual void end_conditional_condition() = 0;

    //! Begin then part of conditonal expression.
    virtual void begin_conditional_then() = 0;

    //! End then part of conditonal expression.
    virtual void end_conditional_then() = 0;

    //! Begin else part of conditonal expression.
    virtual void begin_conditional_else() = 0;

    //! End else part of conditonal expression.
    virtual void end_conditional_else() = 0;

    //! End conditonal expression.
    virtual void end_conditional_expression() = 0;

    //! Operator.
    /*!
     *  \param  type        The type of the expression.
     *  \param  name        The name of the operator.
     */
    virtual void op(IType *type,const char *name) = 0;

    //! Begin prefix operator expression.
    /*!
     *  \param  type        The type of the expression.
     *  \param  name        The name of the operator.
     *  \param  parentheses True if parentheses are required.
     */
    virtual void begin_pre_op(
                    IType *type,const char *name,bool parentheses = true) = 0;

    //! End prefix operator expression.
    /*!
     *  \param  type        The type of the expression.
     *  \param  name        The name of the operator.
     *  \param  parentheses True if parentheses are required.
     */
    virtual void end_pre_op(
                    IType *type,const char *name,bool parentheses = true) = 0;

    //! Begin postfix operator expression.
    /*!
     *  \param  type        The type of the expression.
     *  \param  name        The name of the operator.
     *  \param  parentheses True if parentheses are required.
     */
    virtual void begin_post_op(
                    IType *type,const char *name,bool parentheses = true) = 0;

    //! End postfix operator expression.
    /*!
     *  \param  type        The type of the expression.
     *  \param  name        The name of the operator.
     *  \param  parentheses True if parentheses are required.
     */
    virtual void end_post_op(
                    IType *type,const char *name,bool parentheses = true) = 0;

    //! Begin binary operator expression.
    /*!
     *  \param  type        The type of the expression.
     *  \param  name        The name of the operator.
     *  \param  parentheses True if parentheses are required.
     */
    virtual void begin_bin_op(
                    IType *type,const char *name,bool parentheses = true) = 0;

    //! End binary operator expression.
    /*!
     *  \param  type        The type of the expression.
     *  \param  name        The name of the operator.
     *  \param  parentheses True if parentheses are required.
     */
    virtual void end_bin_op(
                    IType *type,const char *name,bool parentheses = true) = 0;

    //! Begin cast expression.
    /*!
     *  \param  type        The name of the type to cast to.
     */
    virtual void begin_cast(const mslQualifiedName *type) = 0;

    //! End cast expression.
    /*!
     *  \param  type        The name of the type to cast to.
     */
    virtual void end_cast(const mslQualifiedName *type) = 0;

    //! Begin function call.
    /*!
     *  \param  name        The name of the function.
     *  \param  declaration The declaration of the function.
     */
    virtual void begin_call(
                    const mslQualifiedName *name,
                    const IDeclaration *declaration,
					bool i_bRequiresState = false) = 0;

    //! End function call.
    virtual void end_call() = 0;

    //! Begin state function call.
    /*!
     *  \param  name        The name of the function.
     *  \param  declaration The declaration of the function.
     */
    virtual void begin_state_call(
                    const mslQualifiedName *name,
                    const IDeclaration *declaration) = 0;

    //! End state function call.
    virtual void end_state_call() = 0;

    //! Begin call argument.
    /*!
     *  \param  position    The position of the call argument
     *                      in the argument list, counting from zero.
     */
    virtual void begin_argument(int position) = 0;

    //! End call argument.
    /*!
     *  \param  position    The position of the call argument
     *                      in the argument list, counting from zero.
     */
    virtual void end_argument(int position) = 0;

    //! Begin conditional statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_if(int depth) = 0;

    //! Begin condition of conditional statement.
    virtual void begin_if_condition() = 0;

    //! End condition of conditional statement.
    virtual void end_if_condition() = 0;

    //! Begin then part of conditional statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_then(int depth) = 0;

    //! End then part of conditional statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_then(int depth) = 0;

    //! Begin else part of conditional statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_else(int depth) = 0;

    //! End else part of conditional statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_else(int depth) = 0;

    //! End conditional statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_if(int depth) = 0;

    //! Begin switch statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_switch(int depth) = 0;

    //! Begin selector expression of switch statement.
    virtual void begin_switch_selector() = 0;

    //! End selector expression of switch statement.
    virtual void end_switch_selector() = 0;

    //! Begin case label in switch statement.
    virtual void begin_case_label(int depth) = 0;

    //! End case label in switch statement.
    virtual void end_case_label(int depth) = 0;

    //! Default label in switch statement.
    virtual void default_label(int depth) = 0;

    //! End switch statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_switch(int depth) = 0;

    //! Begin while loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_while(int depth) = 0;

    //! Begin condition of while loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_while_condition(int depth) = 0;

    //! End condition of while loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_while_condition(int depth) = 0;

    //! End while loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_while(int depth) = 0;

    //! Begin do-while loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_do_while(int depth) = 0;

    //! Begin condition of do-while loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_do_while_condition(int depth) = 0;

    //! End condition of do-while loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_do_while_condition(int depth) = 0;

    //! End do-while loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_do_while(int depth) = 0;

    //! Begin for loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_for(int depth) = 0;

    //! Begin initialization of for loop.
    virtual void begin_for_init() = 0;

    //! End initialization of for loop.
    virtual void end_for_init() = 0;

    //! Begin condition of for loop.
    virtual void begin_for_condition() = 0;

    //! End condition of for loop.
    virtual void end_for_condition() = 0;

    //! Begin update of for loop.
    virtual void begin_for_update() = 0;

    //! End update of for loop.
    virtual void end_for_update() = 0;

    //! End for loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_for(int depth) = 0;

    //! Begin foreach loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_foreach(int depth) = 0;

    //! Begin iterator expression of foreach loop.
    virtual void begin_foreach_iterator() = 0;

    //! End iterator expression of foreach loop.
    virtual void end_foreach_iterator() = 0;

    //! End foreach loop.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_foreach(int depth) = 0;

    //! Break statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void break_statement(int depth) = 0;

    //! Continue statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void continue_statement(int depth) = 0;

    //! Begin return statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_return_statement(int depth) = 0;

    //! End return statement.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_return_statement(int depth) = 0;

    //! Begin annotation.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void begin_annotation(int depth) = 0;

    //! End annotation.
    /*!
     *  \param  depth       The indentation depth.
     */
    virtual void end_annotation(int depth) = 0;

};
