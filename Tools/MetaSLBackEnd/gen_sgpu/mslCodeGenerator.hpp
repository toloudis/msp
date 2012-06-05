/*****************************************************************************
**	mslCodeGenerator.hpp
**
**	 mslCodeGenerator implements MentalMill's ICode_generator
** to generate our DirectX 11 shader code from a syntax tree.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_CODEGENERATOR_HPP
#error mslCodeGenerator.hpp multiply included
#endif
#define MSL_CODEGENERATOR_HPP

#ifndef MSL_DEFS_HPP
#include "mslDefs.hpp"
#endif

#include <string>
#include <map>
#include <set>

class mslVisitor;

//============================================================================
//! mslCodeGenerator implements the code generator of this plugin.
//============================================================================
class mslCodeGenerator : public ICode_generator
{

public:

    //! Convert typecode to string.
    /*!
     *  \param  code    The type code.
     *  \returns        The string representation of the type code.
     */
    static const char *type_code_to_c_str(IType::Type_code code);

    //! Convert node type to string.
    /*!
     *  \param  type    The node type.
     *  \returns        The string representation of the node type.
     */
    static const char *node_type_to_c_str(ISyntax_tree::Node_type type);

    //! The constructor.
    mslCodeGenerator();

    //! The destructor.
    virtual ~mslCodeGenerator();

    //! Determine the priority of an operator.
    /*!
     *  \param  op              The operator to determine the priority of.
     *  \returns                The priority of the operator.
     */
    virtual int get_priority(const char *op);

    //! Generates a shader with MetaSL shader code.
    /*!
     *  \param  log             The log interface.
     *  \param  comp_unit       The compilation unit to generate code frome.
     *  \param  comp_options    The compilation options to use.
     */
    virtual IGenerated_shader* generate_code(
        ILog *log,
	const ICompilation_unit *comp_unit,
	const ICompiler_options *comp_options);

    //! Increment the reference count.
    virtual void reference();
    
    //! Decrement the reference count.
    virtual void release();
    
    //! Get an interface from this interface.
    /*!
     *  \param  interface_id    The identifier of the interface to get.
     *  \returns                The interface, or NULL if it is not available.
     */
    virtual Interface *get_interface(
	                    int interface_id);

    //! Get the identifier of the object which is the target of this interface.
    /*!
     *  \returns    The identifier of the object which is the target of
     *              this interface.
     */
    virtual Uint64 get_interface_target() const;

    //! Visit a variable declaration.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  specifiers      The declaration specifiers.
     *  \param  id              The variable id.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_variable_declaration(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            const ISyntax_tree *specifiers,
            const ISyntax_tree *id,
            int depth,
            mslVisitor *visitor);

    //! Visit a function declaration.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  declaration     The declaration to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_function_declaration(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            const ISyntax_tree *declaration,
            int depth,
            mslVisitor *visitor);

    //! Visit a constructor declaration.
    /*!
    *  \param  log             The logger interface.
    *  \param  comp_unit       The compilation unit.
    *  \param  comp_options    The compilation options.
    *  \param  declaration     The declaration to visit.
    *  \param  depth           The indentation depth.
    *  \param  visitor         The visitor to use.
    */
    void visit_constructor_declaration(
	ILog *log,
	const ICompilation_unit *comp_unit,
	const ICompiler_options *comp_options,
	const ISyntax_tree *declaration,
	int depth,
	mslVisitor *visitor);

    //! Visit a constructor declaration.
    /*!
    *  \param  log             The logger interface.
    *  \param  comp_unit       The compilation unit.
    *  \param  comp_options    The compilation options.
    *  \param  declaration     The declaration to visit.
    *  \param  depth           The indentation depth.
    *  \param  visitor         The visitor to use.
    */
    void visit_destructor_declaration(
	ILog *log,
	const ICompilation_unit *comp_unit,
	const ICompiler_options *comp_options,
	const ISyntax_tree *declaration,
	int depth,
	mslVisitor *visitor);

    //! Visit a parameter declaration.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  position        The position of the parameter.
     *  \param  parameter       The parameter to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_parameter_declaration(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            int position,
            const ISyntax_tree *parameter,
            int depth,
            mslVisitor *visitor);

    //! Visit a type definition.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  type            The type of the definition to visit.
     *  \param  name            The name of the definition to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_type_definition(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            IType *type,
            const char *name,
            int depth,
            mslVisitor *visitor);

    //! Visit a declaration.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  declaration     The declaration to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_declaration(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            ISyntax_tree *declaration,
            int depth,
            mslVisitor *visitor);

    //! Visit an input that needs to be written as Sas annotation
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  declaration     The declaration to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_input(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            ISyntax_tree *declaration,
            int depth,
            mslVisitor *visitor);

    //! Visit a statement block.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  block           The statement block to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_statement_block(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            const ISyntax_tree *block,
            int depth,
            mslVisitor *visitor);

    //! Visit a conditional statement.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  statement       The conditional statement to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_conditional_statement(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            const ISyntax_tree *statement,
            int depth,
            mslVisitor *visitor);

    //! Visit a conditional statement.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  statement       The switch statement to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_switch_statement(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            const ISyntax_tree *statement,
            int depth,
            mslVisitor *visitor);

    //! Visit an expression statement.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  statement       The expression statement to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_expression_statement(
        ILog *log,
        const ICompilation_unit *comp_unit,
        const ICompiler_options *comp_options,
        const ISyntax_tree *statement,
        int depth,
        mslVisitor *visitor);

    //! Visit a do-while loop.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  statement       The loop statement to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_do_while_loop(
        ILog *log,
        const ICompilation_unit *comp_unit,
        const ICompiler_options *comp_options,
        const ISyntax_tree *statement,
        int depth,
        mslVisitor *visitor);

    //! Visit a while loop.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  statement       The loop statement to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_while_loop(
        ILog *log,
        const ICompilation_unit *comp_unit,
        const ICompiler_options *comp_options,
        const ISyntax_tree *statement,
        int depth,
        mslVisitor *visitor);

    //! Visit a for loop.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  statement       The loop statement to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_for_loop(
        ILog *log,
        const ICompilation_unit *comp_unit,
        const ICompiler_options *comp_options,
        const ISyntax_tree *statement,
        int depth,
        mslVisitor *visitor);

    //! Visit a foreach loop.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  statement       The loop statement to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_foreach_loop(
        ILog *log,
        const ICompilation_unit *comp_unit,
        const ICompiler_options *comp_options,
        const ISyntax_tree *statement,
        int depth,
        mslVisitor *visitor);

    //! Visit a statement.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  statement       The statement to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_statement(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            const ISyntax_tree *statement,
            int depth,
            mslVisitor *visitor);

    //! Visit a condition.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  condition       The condition to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     */
    void visit_condition(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            const ISyntax_tree *condition,
            int depth,
            mslVisitor *visitor);

    //! Visit an expression.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  expression      The expression to visit.
     *  \param  depth           The indentation depth.
     *  \param  visitor         The visitor to use.
     *  \param  priority        The priority of the context.
     */
    void visit_expression(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            const ISyntax_tree *expression,
            int depth,
            mslVisitor *visitor,
            int priority = 0);

    //! Visit the compilation unit.
    /*!
     *  \param  log             The logger interface.
     *  \param  comp_unit       The compilation unit.
     *  \param  comp_options    The compilation options.
     *  \param  visitor         The visitor to use.
     */
    virtual void visit(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            mslVisitor *visitor);

private:

    //! The reference count.
    int m_ref_count;

    //! The type of the priority map.
    typedef std::map<std::string,int> Priority_map;

    //! The priority map.
    Priority_map m_priority_map;

    //! Flag to indicate that visitor is inside of a shader.
    bool m_inside_shader;

	// All function definitions in shader
	std::set<std::string> m_FunctionDefinitions;

};

