/*****************************************************************************
**	mslCodeGenerator.cpp
**
**	 see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslCodeGenerator.hpp"

#include "mslAnnotations.hpp"
#include "mslBlockOrdering.hpp"
#include "mslDumpAST.hpp"
#include "mslGeneratedShader.hpp"
#include "mslQualifiedName.hpp"
#include "mslVisitorHLSL.hpp"

#include "Core/dbg/dbgMsg.hpp"

using namespace std;

namespace
{
	// Convert type name from MetaSL to HLSL		
	const char *type_convert(const char* i_pMetaSLType)
	{
		if (!strcmp(i_pMetaSLType, "Color"))
			return "float4";
		//bga - comment out light_iterator as type? May be better in other place
		// because I really only want to comment out the light iterator when used in light loops
		else if (!strcmp(i_pMetaSLType, "Light_iterator"))
			return "//Light_iterator";	
		return i_pMetaSLType;
	}

	// Convert function name from MetaSL to safe name in HLSL
	// possibly to a function of our design.
	const char *function_convert(const char* i_pFunctionName)
	{
		// msl_tex2D is a function of ours that converts to DX11 texture sampling
		if (!strcmp(i_pFunctionName, "tex2D"))	
			return "msl_tex2D";
		else if (!strcmp(i_pFunctionName, "tex2d"))	
			return "msl_tex2D";
		else if (!strcmp(i_pFunctionName, "tex1D"))	
			return "msl_tex1D";
		else if (!strcmp(i_pFunctionName, "tex1d"))	
			return "msl_tex1D";
		else if (!strcmp(i_pFunctionName, "tex3D"))	
			return "msl_tex3D";
		else if (!strcmp(i_pFunctionName, "tex3d"))	
			return "msl_tex3D";

		// There are constructor functions like "diffuse = Color(0,0,0,0)"
		// that are represented in MEtaSL as function calls. So, this
		// means that we need to go through the type_convert function to handle them.
		return type_convert(i_pFunctionName);
	}

	// Certain strings are keywords in HLSL and cannot be a variable name
	const char *get_variable_name(const char* i_pVariableName)
	{
		//bga - Others will likely be needed here...
		if (!strcmp(i_pVariableName, "texture"))
			return "msl_texture";
		else if (!strcmp(i_pVariableName, "point"))
			return "msl_point";
		else if (!strcmp(i_pVariableName, "vector"))
			return "msl_vector";
		return i_pVariableName;
	}
}


const char *mslCodeGenerator::type_code_to_c_str(
    IType::Type_code code)
{
    switch(code) {
	case IType::TYPE_UNDEFINED:	return "UNDEFINED";
	case IType::TYPE_STRUCT:	return "STRUCT";
	case IType::TYPE_SCALAR:	return "SCALAR";
	case IType::TYPE_INTEGER:	return "INTEGER";
	case IType::TYPE_BOOLEAN:	return "BOOLEAN";
	case IType::TYPE_VECTOR2:	return "VECTOR2";
	case IType::TYPE_VECTOR3:	return "VECTOR3";
	case IType::TYPE_VECTOR4:	return "VECTOR4";
	case IType::TYPE_STRING:	return "STRING";
	case IType::TYPE_COLOR:         return "COLOR";
	case IType::TYPE_VECTOR2I:	return "VECTOR2I";
	case IType::TYPE_VECTOR3I:	return "VECTOR3I";
	case IType::TYPE_VECTOR4I:	return "VECTOR4I";
	case IType::TYPE_VECTOR2B:	return "VECTOR2B";
	case IType::TYPE_VECTOR3B:	return "VECTOR3B";
	case IType::TYPE_VECTOR4B:	return "VECTOR4B";
	case IType::TYPE_MATRIX2X2:	return "MATRIX2X2";
	case IType::TYPE_MATRIX2X3:	return "MATRIX2X3";
	case IType::TYPE_MATRIX3X2:	return "MATRIX3X2";
	case IType::TYPE_MATRIX3X3:	return "MATRIX3X3";
	case IType::TYPE_MATRIX4X3:	return "MATRIX4X3";
	case IType::TYPE_MATRIX3X4:	return "MATRIX3X4";
	case IType::TYPE_MATRIX4X4:	return "MATRIX4X4";
	case IType::TYPE_MATRIX4X2:	return "MATRIX4X2";
	case IType::TYPE_MATRIX2X4:	return "MATRIX2X4";
	case IType::TYPE_TEXTURE1D:	return "TEXTURE1D";
	case IType::TYPE_TEXTURE2D:	return "TEXTURE2D";
	case IType::TYPE_TEXTURE3D:	return "TEXTURE3D";
	case IType::TYPE_TEXTURE_CUBE:	return "TEXTURE_CUBE";
	default:  return "<unknown type code>";
    }
}

const char *mslCodeGenerator::node_type_to_c_str(
    ISyntax_tree::Node_type type)
{
    switch(type) {
        case ISyntax_tree::DECLARATION_LIST:
			return "DECLARATION_LIST";
        case ISyntax_tree::IDENTIFIER:
			return "IDENTIFIER";
        case ISyntax_tree::INIT_IDENTIFIER_LIST:
			return "INIT_IDENTIFIER_LIST";
        case ISyntax_tree::INIT_IDENTIFIER:
			return "INIT_IDENTIFIER";
        case ISyntax_tree::ARRAY_DIMENSION:
			return "ARRAY_DIMENSION";
        case ISyntax_tree::ARRAY_LITERAL:
			return "ARRAY_LITERAL";
        case ISyntax_tree::FUNCTION_DECLARATION:
			return "FUNCTION_DECLARATION";
        case ISyntax_tree::FUNCTION_DECLARATOR:
			return "FUNCTION_DECLARATOR";
        case ISyntax_tree::PARAMETER_LIST:
			return "PARAMETER_LIST";
        case ISyntax_tree::PARAMETER_DECLARATION:
			return "PARAMETER_DECLARATION";
        case ISyntax_tree::PARAMETER_MODE:
			return "PARAMETER_MODE";
        case ISyntax_tree::FUNCTION_DEFINITION:
			return "FUNCTION_DEFINITION";
        case ISyntax_tree::VARIABLE_DECLARATION:
			return "VARIABLE_DECLARATION";
        case ISyntax_tree::VARIABLE_SPECIFIERS:
			return "VARIABLE_SPECIFIERS";
        case ISyntax_tree::STORAGE_CLASS_SPECIFIER:
			return "STORAGE_CLASS_SPECIFIER";
        case ISyntax_tree::SIMPLE_TYPE:
			return "SIMPLE_TYPE";
        case ISyntax_tree::NESTED_TYPE:
			return "NESTED_TYPE";
        case ISyntax_tree::CLASS_DECLARATION:
			return "CLASS_DECLARATION";
        case ISyntax_tree::CLASS_BLOCK_LIST:
			return "CLASS_BLOCK_LIST";
        case ISyntax_tree::CLASS_BLOCK:
			return "CLASS_BLOCK";
        case ISyntax_tree::SUPERCLASS:
			return "SUPERCLASS";
        case ISyntax_tree::BLOCK:
			return "BLOCK";
        case ISyntax_tree::CONDITIONAL_STATEMENT:
			return "CONDITIONAL_STATEMENT";
        case ISyntax_tree::CONDITION:
			return "CONDITION";
        case ISyntax_tree::EXPRESSION_STATEMENT:
			return "EXPRESSION_STATEMENT";
        case ISyntax_tree::DO_WHILE_LOOP:
			return "DO_WHILE_LOOP";
        case ISyntax_tree::WHILE_LOOP:
			return "WHILE_LOOP";
        case ISyntax_tree::FOR_LOOP:
			return "FOR_LOOP";
        case ISyntax_tree::FOREACH_LOOP:
			return "FOREACH_LOOP";
        case ISyntax_tree::JUMP_STATEMENT:
			return "JUMP_STATEMENT";
        case ISyntax_tree::FOR_INIT:
			return "FOR_INIT";
        case ISyntax_tree::FOR_CONDITION:
			return "FOR_CONDITION";
        case ISyntax_tree::FOR_ITERATOR:
			return "FOR_ITERATOR";
        case ISyntax_tree::EXPRESSION_LIST:
			return "EXPRESSION_LIST";
	case ISyntax_tree::ASSIGNMENT_EXPRESSION:
			return "ASSIGNMENT_EXPRESSION";
	case ISyntax_tree::CONDITIONAL_EXPRESSION:
			return "CONDITIONAL_EXPRESSION";
	case ISyntax_tree::OR_EXPRESSION:
			return "OR_EXPRESSION";
	case ISyntax_tree::XOR_EXPRESSION:
			return "XOR_EXPRESSION";
	case ISyntax_tree::AND_EXPRESSION:
			return "AND_EXPRESSION";
	case ISyntax_tree::EQUALITY_EXPRESSION:
			return "EQUALITY_EXPRESSION";
	case ISyntax_tree::RELATIONAL_EXPRESSION:
			return "RELATIONAL_EXPRESSION";
	case ISyntax_tree::ADDITIVE_EXPRESSION:
			return "ADDITIVE_EXPRESSION";
	case ISyntax_tree::MULTIPLICATIVE_EXPRESSION:
			return "MULTIPLICATIVE_EXPRESSION";
	case ISyntax_tree::PRE_INCREMENT_EXPRESSION:
			return "PRE_INCREMENT_EXPRESSION";
	case ISyntax_tree::PRE_DECREMENT_EXPRESSION:
			return "PRE_DECREMENT_EXPRESSION";
	case ISyntax_tree::UNARY_EXPRESSION:
			return "UNARY_EXPRESSION";
	case ISyntax_tree::POST_INCREMENT_EXPRESSION:
			return "POST_INCREMENT_EXPRESSION";
	case ISyntax_tree::POST_DECREMENT_EXPRESSION:
			return "POST_DECREMENT_EXPRESSION";
        case ISyntax_tree::SELECTION_EXPRESSION:
			return "SELECTION_EXPRESSION";
        case ISyntax_tree::INDEX_EXPRESSION:
			return "INDEX_EXPRESSION";
        case ISyntax_tree::FUNCTION_CALL:
			return "FUNCTION_CALL";
        case ISyntax_tree::METHOD_CALL:
			return "METHOD_CALL";
        case ISyntax_tree::CONSTRUCTOR_CALL:
			return "CONSTRUCTOR_CALL";
        case ISyntax_tree::CALL_ARGUMENTS:
			return "CALL_ARGUMENTS";
        case ISyntax_tree::INT_LITERAL_DEC:
			return "INT_LITERAL_DEC";
        case ISyntax_tree::INT_LITERAL_OCT:
			return "INT_LITERAL_OCT";
        case ISyntax_tree::INT_LITERAL_HEX:
			return "INT_LITERAL_HEX";
        case ISyntax_tree::SCALAR_LITERAL:
			return "SCALAR_LITERAL";
        case ISyntax_tree::BOOL_LITERAL:
			return "BOOL_LITERAL";
        case ISyntax_tree::STRING_LITERAL:
			return "STRING_LITERAL";
        case ISyntax_tree::VARIABLE_ID:
			return "VARIABLE_ID";
        case ISyntax_tree::MEMBER_VARIABLE_ID:
			return "MEMBER_VARIABLE_ID";
        case ISyntax_tree::STATE_VARIABLE_REFERENCE:
			return "STATE_VARIABLE_REFERENCE";
        case ISyntax_tree::STATE_FUNCTION_REFERENCE:
			return "STATE_FUNCTION_REFERENCE";
        case ISyntax_tree::ENUM_DECLARATION:
			return "ENUM_DECLARATION";
        case ISyntax_tree::ENUM_MEMBER:
			return "ENUM_MEMBER";
        case ISyntax_tree::SWITCH_STATEMENT:
			return "SWITCH_STATEMENT";
        case ISyntax_tree::SWITCH_CASE:
			return "SWITCH_CASE";
        case ISyntax_tree::SWITCH_DEFAULT:
			return "SWITCH_DEFAULT";
        case ISyntax_tree::ANNOTATION_BLOCK:
			return "ANNOTATION_BLOCK";
		case ISyntax_tree::IMPORT_DECLARATION:
			return "IMPORT_DECLARATION";
        case ISyntax_tree::SKIP_STATEMENT:
            return "SKIP_STATEMENT";
        case ISyntax_tree::CONSTRUCTOR_DECLARATION:
			return "CONSTRUCTOR_DECLARATION";
        case ISyntax_tree::CONSTRUCTOR_DEFINITION:
			return "CONSTRUCTOR_DEFINITION";
        case ISyntax_tree::DESTRUCTOR_DECLARATION:
			return "DESTRUCTOR_DECLARATION";
        case ISyntax_tree::DESTRUCTOR_DEFINITION:
			return "DESTRUCTOR_DEFINITION";
	default:  return "<unknown type code>";
    }
}

mslCodeGenerator::mslCodeGenerator()
    : m_ref_count(1)
    , m_inside_shader(false)
{
    m_priority_map["="]     =  1;
    m_priority_map["*="]    =  1;
    m_priority_map["/="]    =  1;
    m_priority_map["%="]    =  1;
    m_priority_map["+="]    =  1;
    m_priority_map["-="]    =  1;

    m_priority_map["||"]    =  2;

    m_priority_map["&&"]    =  3;

    m_priority_map["^^"]    =  4;

    m_priority_map["=="]    =  5;
    m_priority_map["!="]    =  5;

    m_priority_map["<"]     =  6;
    m_priority_map["<="]    =  6;
    m_priority_map[">"]     =  6;
    m_priority_map[">="]    =  6;

    m_priority_map["+"]     =  7;
    m_priority_map["-"]     =  7;

    m_priority_map["*"]     =  8;
    m_priority_map["/"]     =  8;
    m_priority_map["%"]     =  8;

    m_priority_map["."]     =  9;

    m_priority_map["++"]    = 10;
    m_priority_map["--"]    = 10;

    m_priority_map["[]"]    = 11;
}

mslCodeGenerator::~mslCodeGenerator()
{
}

int mslCodeGenerator::get_priority(const char *op)
{
    return m_priority_map[op];
}

IGenerated_shader *mslCodeGenerator::generate_code(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options)
{
    mslGeneratedShader *shader = new mslGeneratedShader();

    mslVisitorHLSL visitor(log,shader);

#if DO_DUMP
	mslDumpAST::dump_ast(log,comp_unit,comp_options,shader);
#endif

	// First get all function definitions in compilation unit and import units
	std::multimap<std::string, ISyntax_tree*> function_definitions;
	mslBlockOrdering::GetAllFunctionDefinitions(comp_unit, function_definitions);

	//std::multimap<std::string, ISyntax_tree*>::iterator it;
	//for (it = function_definitions.begin(); it != function_definitions.end(); ++it)
	//{
	//	visitor.print_code("Found function: %s\n", it->first.c_str());
	//}

	m_FunctionDefinitions.clear();
	mslBlockOrdering::GetAllFunctionsCalled(function_definitions, m_FunctionDefinitions);

	//std::set<std::string>::iterator it;
	//for (it = m_FunctionDefinitions.begin(); it != m_FunctionDefinitions.end(); ++it)
	//{
	//	visitor.print_code("Calling function: %s\n", it->c_str());
	//}

    visitor.begin_shader_file();
    visit(log,comp_unit,comp_options,&visitor);
    visitor.end_shader_file();

    return shader;
}

void mslCodeGenerator::reference()
{
    ++m_ref_count;
}

void mslCodeGenerator::release()
{
    --m_ref_count;
    if(!m_ref_count)
        delete this;
}

Interface *mslCodeGenerator::get_interface(
    int interface_id)
{
    return 0;
}

Uint64 mslCodeGenerator::get_interface_target() const
{
    return reinterpret_cast<Uint64>(this);
}

void mslCodeGenerator::visit_variable_declaration(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *specifiers,
    const ISyntax_tree *id,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit variable declaration %d",depth);
    int count = specifiers->get_child_count();
    mslQualifiedName type_name;
    bool storage = false;
	string storage_specifier;
    for(int i = 0; i < count; i++) {
        ISyntax_tree *specifier = specifiers->get_child(i);
        ISyntax_tree::Node_type specifier_type = specifier->get_node_type();
        switch(specifier_type) {
        case ISyntax_tree::SIMPLE_TYPE:
            //type_name.append(specifier->get_string_value());
			type_name.append(type_convert(specifier->get_string_value()));
            break;
        case ISyntax_tree::NESTED_TYPE:
            {
                int count = specifier->get_child_count();
                for(int i = 0; i < count; i++) {
                    ISyntax_tree *child = specifier->get_child(i);
                    type_name.append(child->get_string_value());
                    child->release();
                }
            }
            break;
        case ISyntax_tree::STORAGE_CLASS_SPECIFIER:
            storage = true;
            storage_specifier = specifier->get_string_value();
            break;
	default:
	    log->error("expected type or storage class, found %s",
                        node_type_to_c_str(specifier_type));
        }
        specifier->release();
    }
    ISyntax_tree *dimension = 0;
    ISyntax_tree *annotation = 0;
    ISyntax_tree *initializer = 0;
    count = id->get_child_count();
    for(int i = 0; i < count; i++) {
        ISyntax_tree *child = id->get_child(i);
        ISyntax_tree::Node_type type = child->get_node_type();
        switch(type) {
        case ISyntax_tree::ARRAY_DIMENSION:
            dimension = child;
            break;
        case ISyntax_tree::ANNOTATION_BLOCK:
            annotation = child;
            break;
        default:
            initializer = child;
        }
    }
    string dim;
    if(dimension) {
        if(0 < dimension->get_child_count()) {
            ISyntax_tree *n = dimension->get_child(0);
            dim = n->get_string_value();
            n->release();
        }
    }
    visitor->variable_declaration(storage ? storage_specifier.c_str() : 0,
                                    &type_name,
                                    id->get_string_value(),
                                    dimension ? dim.c_str() : 0);
    if(dimension) {
        dimension->release();
    }
    if(initializer) {
        visitor->begin_initializer(depth);
        visit_expression(log,comp_unit,comp_options,
                            initializer,depth+1,visitor);
        visitor->end_initializer(depth);
        initializer->release();
    }
    if(annotation) {
        visitor->begin_annotation(depth+1);
        int count = annotation->get_child_count();
        for(int i = 0; i < count; i++) {
            ISyntax_tree *a = annotation->get_child(i);
            visitor->begin_declaration(depth+2);
            visit_expression(log,comp_unit,comp_options,a,depth+1,visitor);
            visitor->end_declaration(depth+2);
            a->release();
        }
        visitor->end_annotation(depth+1);
        annotation->release();
    }
//log->info("end visit variable declaration %d",depth);
}

void mslCodeGenerator::visit_function_declaration(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *declaration,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit function declaration %d",depth);
    ISyntax_tree *type = declaration->get_child(0);
    ISyntax_tree *declarator = declaration->get_child(1);
    mslQualifiedName type_name;
    switch(type->get_node_type()) {
    case ISyntax_tree::SIMPLE_TYPE:
        //type_name.append(type->get_string_value());
        type_name.append(type_convert(type->get_string_value()));
        break;
    case ISyntax_tree::NESTED_TYPE:
        {
            int count = type->get_child_count();
            for(int i = 0; i < count; i++) {
                ISyntax_tree *child = type->get_child(i);
                type_name.append(child->get_string_value());
                child->release();
            }
        }
        break;
    default:
        log->error("expected type, found %s",
                    node_type_to_c_str(type->get_node_type()));
    }
    ISyntax_tree *id = declarator->get_child(0);
    visitor->begin_function_declaration(&type_name,id->get_string_value());
    id->release();
    declarator->release();
    type->release();
    if(2 < declaration->get_child_count()) {
        ISyntax_tree *parameters = declaration->get_child(2);
        int count = parameters->get_child_count();
        for(int i = 0; i < count; i++) {
            ISyntax_tree *parameter = parameters->get_child(i);
            visit_parameter_declaration(log,comp_unit,comp_options,
                                            i+2,parameter,depth,visitor); //bga - i+2 implies state and light earlier
            parameter->release();
        }
        parameters->release();
    }
    visitor->end_function_declaration();
//log->info("end visit function declaration %d",depth);
}

void mslCodeGenerator::visit_constructor_declaration(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *declaration,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit constructor declaration %d",depth);
    ISyntax_tree *declarator = declaration->get_child(0);
    ISyntax_tree *id = declarator->get_child(0);
    bool is_static = !strcmp(declaration->get_string_value(),"static");
    visitor->begin_constructor_declaration(id->get_string_value(),is_static);
    id->release();
    declarator->release();
    visitor->end_constructor_declaration();
//log->info("end visit constructor declaration %d",depth);
}

void mslCodeGenerator::visit_destructor_declaration(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *declaration,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit destructor declaration %d",depth);
    ISyntax_tree *declarator = declaration->get_child(0);
    ISyntax_tree *id = declarator->get_child(0);
    bool is_static = !strcmp(declaration->get_string_value(),"static");
    visitor->begin_destructor_declaration(id->get_string_value(),is_static);
    id->release();
    declarator->release();
    visitor->end_destructor_declaration();
//log->info("end visit destructor declaration %d",depth);
}

void mslCodeGenerator::visit_parameter_declaration(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    int position,
    const ISyntax_tree *parameter,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit parameter declaration %d",depth);
    int count = parameter->get_child_count();
    ISyntax_tree *mode = 0;
    mslQualifiedName type_name;
    ISyntax_tree *dimension = 0;
    for(int i = 1; i < count; i++) {
        ISyntax_tree *child = parameter->get_child(i);
        ISyntax_tree::Node_type type = child->get_node_type();
        switch(type) {
        case ISyntax_tree::PARAMETER_MODE:
            mode = child;
            break;
        case ISyntax_tree::ARRAY_DIMENSION:
            dimension = child;
            break;
        default:
            child->release();
            log->error("unknown parameter qualifier %s",
                        node_type_to_c_str(type));
        }
    }
    ISyntax_tree *type = parameter->get_child(0);
    switch(type->get_node_type()) {
    case ISyntax_tree::SIMPLE_TYPE:
        //type_name.append(type->get_string_value());
        type_name.append(type_convert(type->get_string_value()));
        break;
    case ISyntax_tree::NESTED_TYPE:
        {
            int count = type->get_child_count();
            for(int i = 0; i < count; i++) {
                ISyntax_tree *child = type->get_child(i);
                type_name.append(child->get_string_value());
                child->release();
            }
        }
        break;
    default:
        log->error("expected type, found %s",
                    node_type_to_c_str(type->get_node_type()));
    }
    type->release();
    string dim;
    if(dimension) {
        if(0 < dimension->get_child_count()) {
            ISyntax_tree *n = dimension->get_child(0);
            dim = n->get_string_value();
            n->release();
        }
        dimension->release();
    }
    visitor->parameter_declaration(position,
                                    mode ? mode->get_string_value() : 0,
                                    &type_name,
									//bga - making sure variable names are valid
                                    //parameter->get_string_value(),
                                    get_variable_name(parameter->get_string_value()),
                                    dimension ? dim.c_str() : 0);
    if(mode)
        mode->release();
//log->info("end visit parameter declaration %d",depth);
}

void mslCodeGenerator::visit_type_definition(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            IType *type,
            const char *name,
            int depth,
            mslVisitor *visitor)
{
//log->info("begin visit type definition %d",depth);
    switch(type->get_typecode()) {
    case IType::TYPE_UNDEFINED:
	log->error("undefined type");
	break;
    case IType::TYPE_STRUCT:
        {
            mslQualifiedName name(type->get_type_name());
            visitor->begin_struct(depth,&name);
	    IType *t = type->get_child();
            while( t )
            {
                visitor->begin_declaration(depth+1);
                visitor->begin_field(depth+1,t->get_member_name());
                visit_type_definition(log,comp_unit,comp_options,
                                        t,t->get_type_name(),depth+1,visitor);
                visitor->end_field(depth+1,t->get_member_name());
                visitor->end_declaration(depth+1);

		// To make sure we are releasing properly the pointer
		IType *n = t->get_next();
		t->release();
		t = n;
            }
            visitor->end_struct(depth);
        }
        break;
    case IType::TYPE_SCALAR:
        visitor->type("float");
        break;
    case IType::TYPE_INTEGER:
        if(type->is_enum()) {
            mslQualifiedName name(type->get_type_name());
            visitor->begin_enum(depth,&name);
            int count = type->get_enum_value_count();
            for(int i = 0; i < count; i++) {
                mslQualifiedName value_name;
                for(int k = 0; k < name.size() - 1; k++)
                    value_name.append(name[k]);
                value_name.append(type->get_enum_name(i));
                visitor->enum_value(i,
                                    &value_name,
                                    type->get_enum_value(i));
            }
            visitor->end_enum(depth);
        } else {
            visitor->type("int");
        }
        break;
    case IType::TYPE_BOOLEAN:
        visitor->type("bool");
        break;
    case IType::TYPE_VECTOR2:
        visitor->type("float2");
        break;
    case IType::TYPE_VECTOR3:
        visitor->type("float3");
        break;
    case IType::TYPE_VECTOR4:
        visitor->type("float4");
        break;
    case IType::TYPE_COLOR:
        visitor->type("float4");
        break;

//bga - not sure what to do about these...
    case IType::TYPE_STRING:
        visitor->type("string");
        break;
    case IType::TYPE_VECTOR2I:
        visitor->type("int2");
        break;
    case IType::TYPE_VECTOR3I:
        visitor->type("int3");
        break;
    case IType::TYPE_VECTOR4I:
        visitor->type("int4");
        break;
    case IType::TYPE_VECTOR2B:
        visitor->type("bool2");
        break;
    case IType::TYPE_VECTOR3B:
        visitor->type("bool3");
        break;
    case IType::TYPE_VECTOR4B:
        visitor->type("bool4");
        break;
//---------------------------------------
    case IType::TYPE_MATRIX2X2:
        visitor->type("float2x2");
        break;
    case IType::TYPE_MATRIX2X3:
        visitor->type("float2x3");
        break;
    case IType::TYPE_MATRIX3X2:
        visitor->type("float3x2");
        break;
    case IType::TYPE_MATRIX4X3:
        visitor->type("float4x3");
        break;
    case IType::TYPE_MATRIX4X2:
        visitor->type("float4x2");
        break;
    case IType::TYPE_MATRIX2X4:
        visitor->type("float2x4");
        break;
    case IType::TYPE_MATRIX3X3:
        visitor->type("float3x3");
        break;
    case IType::TYPE_MATRIX3X4:
        visitor->type("float3x4");
        break;
    case IType::TYPE_MATRIX4X4:
        visitor->type("float4x4");
        break;
    case IType::TYPE_TEXTURE1D:
        visitor->type("texture1D");
        break;
    case IType::TYPE_TEXTURE2D:
        visitor->type("texture2D");
        break;
    case IType::TYPE_TEXTURE3D:
        visitor->type("texture3D");
        break;
    case IType::TYPE_TEXTURE_CUBE:
        visitor->type("textureCUBE");
        break;
    default:
	log->error("undefined type");
	break;
    }
//log->info("end visit type definition %d",depth);
}

void mslCodeGenerator::visit_declaration(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            ISyntax_tree *declaration,
            int depth,
            mslVisitor *visitor)
{
//log->info("begin visit declaration %d",depth);
    ISyntax_tree::Node_type type = declaration->get_node_type();
    switch(type) {
    case ISyntax_tree::ENUM_DECLARATION:
        {
            // Do not echo enum declarations local to a shader,
            // as they already appear in the global list of types.
            if(m_inside_shader)
                return;

            visitor->begin_declaration(depth);
            mslQualifiedName name(declaration->get_string_value());
            visitor->begin_enum(depth,&name);
            int count = declaration->get_child_count();
            for(int i = 0; i < count; i++) {
                ISyntax_tree *child = declaration->get_child(i);
                mslQualifiedName value_name;
                for(int k = 0; k < name.size() - 1; k++)
                    value_name.append(name[k]);
                value_name.append(child->get_string_value());
                visitor->enum_value(i,&value_name);
                if(0 < child->get_child_count()) {
                    ISyntax_tree *init = child->get_child(0);
                    visitor->begin_initializer(depth);
                    visit_expression(log,comp_unit,comp_options,
                                        init,depth,visitor);
                    visitor->end_initializer(depth);
                    init->release();
                }
                child->release();
            }
            visitor->end_enum(depth);
            visitor->end_declaration(depth);
        }
        break;
    case ISyntax_tree::CLASS_DECLARATION:
        {
            // Do not echo class declarations local to a shader,
            // as they already appear in the global list of types.
            if(m_inside_shader)
                return;

            const char *type = declaration->get_string_value();
            if(!strcmp(type,"shader")
            || !strcmp(type,"brdf")
            || !strcmp(type,"technique")) {
                int count = declaration->get_child_count();
                ISyntax_tree *name = declaration->get_child(0);
                ISyntax_tree *blocks = declaration->get_child(1);
                ISyntax_tree *annotation = 0;
                ISyntax_tree *superclass = 0;
                for(int i = 2; i < count; i++) {
                    ISyntax_tree *child = declaration->get_child(i);
                    switch(child->get_node_type()) {
                    case ISyntax_tree::SUPERCLASS:
                        superclass = child;
                        break;
                    case ISyntax_tree::ANNOTATION_BLOCK:
                        annotation = child;
                        break;
                    default:
                        log->error("unknown %s annotation %s",
                                    type,
                                    node_type_to_c_str(
                                                child->get_node_type()));
                    }
                }

                //visitor->begin_declaration(depth);

                mslQualifiedName superclass_name;
                if(superclass) {
                    superclass_name.append(superclass->get_string_value());
                    int count = superclass->get_child_count();
                    for(int i = 0; i < count; i++) {
                        ISyntax_tree *child = superclass->get_child(i);
                        superclass_name.append(child->get_string_value());
                        child->release();
                    }
                    superclass->release();
                    if(!strcmp(type,"shader")) {
                        visitor->begin_shader(&superclass_name,
                                                name->get_string_value());
                    } else if(!strcmp(type,"brdf")) {
                        visitor->begin_brdf(&superclass_name,
                                                name->get_string_value());
                    } else if(!strcmp(type,"technique")) {
                        visitor->begin_technique(&superclass_name,
                                                name->get_string_value());
                    }
                    m_inside_shader = true;
                } else {
                    if(!strcmp(type,"shader")) {
                        visitor->begin_shader(0,
                                                name->get_string_value());
                    } else if(!strcmp(type,"brdf")) {
                        visitor->begin_brdf(0,
                                                name->get_string_value());
                    } else if(!strcmp(type,"technique")) {
                        visitor->begin_technique(0,
                                                name->get_string_value());
                    }
                    m_inside_shader = true;
                }
                name->release();

                int n_blocks = blocks->get_child_count();
                blocks->release();

				// Get the index of the "outputs" block, we will use this
				// later in the "main" shader function.
				int output_block_index = -1;
                for (int i = 0; i < n_blocks; i++) {
                    ISyntax_tree *block = blocks->get_child(i);
                    const char *name = block->get_string_value();
					if(!strcmp(name,"output")) {
						output_block_index = i;
					}
					else if(!strcmp(name,"input")) {
						// bga - inputs need to be treated as shader constants
						int nInputs = block->get_child_count();
                        visitor->begin_inputs();
                        for(int b = 0; b < nInputs; b++) {
                            ISyntax_tree *child = block->get_child(b);
                            visit_input(log,comp_unit,comp_options,
                                                child,0,visitor);
                            child->release();
                        }
                        visitor->end_inputs();
                    }
                    block->release();
				}

                for(int i = 0; i < n_blocks; i++) {
                    ISyntax_tree *block = blocks->get_child(i);
                    const char *name = block->get_string_value();
                    int count = block->get_child_count();
                    if(!strcmp(name,"member")) {
                        visitor->begin_members();

						std::vector<int> block_ordering;
						mslBlockOrdering::GetBlockOrdering(block, block_ordering);

						for (int b=0; b<block_ordering.size(); ++b)
						{
							int block_index = block_ordering[b];
							if (block_index > 0)	// handle main function below in special case
							{
								ISyntax_tree *child = block->get_child(block_index);
								visit_declaration(log,comp_unit,comp_options,
													child,0,visitor);
								child->release();
							}
                        }
						// Count==0 is the "main" function, which we have to change
						// so that it handles our inputs and returns a color.
						if (count > 0)
						{
                            ISyntax_tree *child = block->get_child(0);
							if (ISyntax_tree::FUNCTION_DEFINITION == child->get_node_type())
							{
								visitor->begin_definition(0);

								//bga - Instead of the declaration in the AST, add in our own 
								// function declaration
								visitor->print_code("float4 sgpu_shader_main(State state, Light_iterator light)");
								//ISyntax_tree *decl = child->get_child(0);
								//visit_function_declaration(log,comp_unit,comp_options,
								//								decl,0,visitor);
								//decl->release();

								ISyntax_tree *body = child->get_child(1);
								visitor->begin_block(0);

								// Outputs of the shader need to be declared as local variables
								// in the main shader function. One of these outputs will be "result"
								if (output_block_index >= 0)
								{
									ISyntax_tree *outputs = blocks->get_child(output_block_index);
									for(int i = 0; i < outputs->get_child_count(); i++) {
										ISyntax_tree *child = outputs->get_child(i);
										visit_declaration(log,comp_unit,comp_options,
															child,1,visitor);
										child->release();
									}
									outputs->release();
								}

								//bga - based on code from visit_statement_block...
								// Write the statements within the main function
								int count = body->get_child_count();
								for(int i = 0; i < count; i++) {
									ISyntax_tree *statement = body->get_child(i);
									visit_statement(log,comp_unit,comp_options,statement,1,visitor);
									statement->release();
								}
								//bga - Force return value of "result"
								visitor->print_code("\n\treturn result;");
								visitor->end_block(0);

								body->release();


								visitor->end_definition(0);
							}
							else
							{
								visit_declaration(log,comp_unit,comp_options,
												 child,0,visitor);
							}
                            child->release();
						}
                        visitor->end_members();
                    }
                    block->release();
                }

                m_inside_shader = false;
                if(!strcmp(type,"shader")) {
                    visitor->end_shader();
                } else if(!strcmp(type,"brdf")) {
                    visitor->end_brdf();
                } else if(!strcmp(type,"technique")) {
                    visitor->end_technique();
                }

                if(annotation) {
                    int count = annotation->get_child_count();
                    for(int i = 0; i < count; i++) {
                        ISyntax_tree *a = annotation->get_child(i);
                        visitor->begin_annotation(1);
                        visitor->begin_declaration(depth+2);
                        visit_expression(log,comp_unit,comp_options,
                                            a,1,visitor);
                        visitor->end_declaration(depth+2);
                        visitor->end_annotation(1);
                        a->release();
                    }
                    annotation->release();
                }

                //visitor->end_declaration(depth);

            } else if(!strcmp(type,"struct")) {
                ISyntax_tree *id = declaration->get_child(0);
                mslQualifiedName name(id->get_string_value());
                visitor->begin_declaration(depth);
                visitor->begin_struct(depth,&name);
                id->release();
                ISyntax_tree *blocks = declaration->get_child(1);
                ISyntax_tree *block = blocks->get_child(0);
                blocks->release();
                int count = block->get_child_count();
                for(int i = 0; i < count; i++) {
                    ISyntax_tree *decl = block->get_child(i);
                    visit_declaration(log,comp_unit,comp_options,
                                        decl,depth+1,visitor);
                    if(decl)
                        decl->release();
                }
                block->release();
                visitor->end_struct(depth);
                visitor->end_declaration(depth);
            } else {
                log->error("unknown type of class declaration (%s)",type);
            }
        }
        break;
    case ISyntax_tree::VARIABLE_DECLARATION:
        {
            ISyntax_tree *specifiers = declaration->get_child(0);
            ISyntax_tree *id_list = declaration->get_child(1);
            int count = id_list->get_child_count();
            for(int i = 0; i < count; i++) {
                ISyntax_tree *id = id_list->get_child(i);
                visitor->begin_declaration(depth);
                visit_variable_declaration(log,comp_unit,comp_options,
                                            specifiers,id,
                                            depth,visitor);
                visitor->end_declaration(depth);
                id->release();
            }
            specifiers->release();
            id_list->release();
        }
        break;
    case ISyntax_tree::FUNCTION_DECLARATION:
        {
            visitor->begin_declaration(depth);
            visit_function_declaration(log,comp_unit,comp_options,
                                            declaration,depth,visitor);
            visitor->end_declaration(depth);
        }
        break;
    case ISyntax_tree::FUNCTION_DEFINITION:
        {
            visitor->begin_definition(depth);
            ISyntax_tree *decl = declaration->get_child(0);
            ISyntax_tree *body = declaration->get_child(1);
            visit_function_declaration(log,comp_unit,comp_options,
                                            decl,depth,visitor);
            visit_statement_block(log,comp_unit,comp_options,
                                            body,depth,visitor);
            decl->release();
            body->release();
            visitor->end_definition(depth);
        }
        break;
    case ISyntax_tree::CONSTRUCTOR_DECLARATION:
        {
            visitor->begin_declaration(depth);
            visit_constructor_declaration(log,comp_unit,comp_options,
                                            declaration,depth,visitor);
            visitor->end_declaration(depth);
        }
        break;
    case ISyntax_tree::CONSTRUCTOR_DEFINITION:
        {
            visitor->begin_definition(depth);
            ISyntax_tree *decl = declaration->get_child(0);
            ISyntax_tree *body = declaration->get_child(1);
            visit_constructor_declaration(log,comp_unit,comp_options,
                                            decl,depth,visitor);
            visit_statement_block(log,comp_unit,comp_options,
                                            body,depth,visitor);
            decl->release();
            body->release();
            visitor->end_definition(depth);
        }
        break;
    case ISyntax_tree::DESTRUCTOR_DECLARATION:
        {
            visitor->begin_declaration(depth);
            visit_destructor_declaration(log,comp_unit,comp_options,
                                            declaration,depth,visitor);
            visitor->end_declaration(depth);
        }
        break;
    case ISyntax_tree::DESTRUCTOR_DEFINITION:
        {
            visitor->begin_definition(depth);
            ISyntax_tree *decl = declaration->get_child(0);
            ISyntax_tree *body = declaration->get_child(1);
            visit_destructor_declaration(log,comp_unit,comp_options,
                                            decl,depth,visitor);
            visit_statement_block(log,comp_unit,comp_options,
                                            body,depth,visitor);
            decl->release();
            body->release();
            visitor->end_definition(depth);
        }
        break;
    default:
        log->error("unknown type of declaration (%d)",type);
    }
//log->info("end visit declaration %d",depth);
}

void mslCodeGenerator::visit_input(
            ILog *log,
            const ICompilation_unit *comp_unit,
            const ICompiler_options *comp_options,
            ISyntax_tree *declaration,
            int depth,
            mslVisitor *visitor)
{
//log->info("begin visit input %d",depth);
    ISyntax_tree::Node_type type = declaration->get_node_type();
    switch(type) {
    case ISyntax_tree::VARIABLE_DECLARATION:
        {
            ISyntax_tree *specifiers = declaration->get_child(0);
			int spec_count = specifiers->get_child_count();
            ISyntax_tree *id_list = declaration->get_child(1);
            int id_count = id_list->get_child_count();

			//bga - expecting our shader constants to have a single
			// specifier and a single identifier
			if (spec_count == 1 && id_count == 1)
			{
				visitor->begin_declaration(0);

				// Get type name of shader constant - needs to be simple type 
				// to be one of our supported inputs.
				mslQualifiedName type_name;
				ISyntax_tree *specifier = specifiers->get_child(0);
				ISyntax_tree::Node_type specifier_type = specifier->get_node_type();
				if (specifier_type == ISyntax_tree::SIMPLE_TYPE)
					type_name.append(type_convert(specifier->get_string_value()));

				// Look for initial value and annotations
				ISyntax_tree *id = id_list->get_child(0);
				ISyntax_tree *annotation = 0;
				ISyntax_tree *initializer = 0;
				int count = id->get_child_count();
				for(int i = 0; i < count; i++) {
					ISyntax_tree *child = id->get_child(i);
					ISyntax_tree::Node_type type = child->get_node_type();
					switch(type) {
					case ISyntax_tree::ARRAY_DIMENSION:
						//dimension = child; //bga - skipping dimension here
						break;
					case ISyntax_tree::ANNOTATION_BLOCK:
						annotation = child;
						break;
					default:
						initializer = child;
					}
				}

				// Write the variable declaration (type and variable name).
				std::string var_name = "sgpu_";	// prefix to avoid conflicts
				var_name += id->get_string_value();
			    visitor->variable_declaration(0, &type_name, var_name.c_str(), 0);


				// Annotations give guides to user interface for input,
				// we have to write them in our style, though.
				if(annotation) {
					mslAnnotations::WriteAnnotation(visitor, annotation, initializer,
													id->get_string_value(),			// input name
													specifier->get_string_value());	// type name
					annotation->release();
				}

				// Initializer is the default value, goes after the annotation block 
				if(initializer) {
					// Have to watch out for texture file names which need 
					// to be specified as resource name and not as 
					// default value with "="
					if (initializer->get_node_type() != ISyntax_tree::STRING_LITERAL)
					{
						visitor->begin_initializer(0);
						visit_expression(log,comp_unit,comp_options,
											initializer,1,visitor);
						visitor->end_initializer(0);
					}
					initializer->release();
				}

				visitor->end_declaration(0);
			}
			else
			{
				log->error("unexpected variable declaration in inputs");
				//// Write it out in traditional style
				//for(int i = 0; i < id_count; i++) {
				//	ISyntax_tree *id = id_list->get_child(i);
				//	visitor->begin_declaration(depth);
				//	visit_variable_declaration(log,comp_unit,comp_options,
				//								specifiers,id,
				//								depth,visitor);
				//	visitor->end_declaration(depth);
				//	id->release();
				//}
			}
            specifiers->release();
            id_list->release();
        }
        break;
    default:
        log->error("unknown type of declaration (%d)",type);
    }

//log->info("end visit input %d",depth);
}

void mslCodeGenerator::visit_statement_block(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *block,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit statement block %d",depth);
    visitor->begin_block(depth);
    int count = block->get_child_count();
    for(int i = 0; i < count; i++) {
        ISyntax_tree *statement = block->get_child(i);
        visit_statement(log,comp_unit,comp_options,statement,depth+1,visitor);
        statement->release();
    }
    visitor->end_block(depth);
//log->info("end visit statement block %d",depth);
}

void mslCodeGenerator::visit_conditional_statement(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *statement,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit conditional statement %d",depth);
    ISyntax_tree *condition = statement->get_child(0);
    visitor->begin_if(depth);
    visitor->begin_if_condition();
    visit_expression(log,comp_unit,comp_options,condition,depth,visitor);
    visitor->end_if_condition();
    condition->release();
    ISyntax_tree *then_part = statement->get_child(1);
    visitor->begin_then(depth);
    visit_statement(log,comp_unit,comp_options,then_part,depth+1,visitor);
    visitor->end_then(depth);
    then_part->release();
    if(2 < statement->get_child_count()) {
        ISyntax_tree *else_part = statement->get_child(2);
        visitor->begin_else(depth);
        visit_statement(log,comp_unit,comp_options,else_part,depth+1,visitor);
        visitor->end_else(depth);
        else_part->release();
    }
    visitor->end_if(depth);
//log->info("end visit conditional statement %d",depth);
}

void mslCodeGenerator::visit_switch_statement(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *statement,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit switch statement %d",depth);
    ISyntax_tree *condition = statement->get_child(0);
    visitor->begin_switch(depth);
    visitor->begin_switch_selector();
    visit_expression(log,comp_unit,comp_options,condition,depth,visitor);
    visitor->end_switch_selector();
    condition->release();
    int count = statement->get_child_count();
    for(int i = 1; i < count; i++) {
        ISyntax_tree *child = statement->get_child(i);
        ISyntax_tree::Node_type type = child->get_node_type();
        switch(type) {
        case ISyntax_tree::SWITCH_CASE:
            {
                ISyntax_tree *label = child->get_child(0);
                visitor->begin_case_label(depth);
                visit_expression(log,comp_unit,comp_options,
                                    label,depth,visitor);
                visitor->end_case_label(depth);
                if(1 < child->get_child_count()) {
                    ISyntax_tree *block = child->get_child(1);
                    visit_statement_block(log,comp_unit,comp_options,
                                            block,depth+1,visitor);
                    block->release();
                }
                label->release();
            }
            break;
        case ISyntax_tree::SWITCH_DEFAULT:
            {
                visitor->default_label(depth);
                if(child->get_child_count()) {
                    ISyntax_tree *block = child->get_child(0);
                    visit_statement_block(log,comp_unit,comp_options,
                                            block,depth+1,visitor);
                    block->release();
                }
            }
            break;
        default:
            log->error("unknown switch element (%d)",type);
        }
        child->release();
    }
    visitor->end_switch(depth);
//log->info("end visit switch statement %d",depth);
}

void mslCodeGenerator::visit_expression_statement(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *statement,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit expression statement %d",depth);
    if(statement->get_child_count()) {
        ISyntax_tree *e = statement->get_child(0);
        visitor->begin_expression_statement(depth);
        visit_expression(log,comp_unit,comp_options,e,depth+1,visitor);
        visitor->end_expression_statement(depth);
        e->release();
    }
//log->info("end visit expression statement %d",depth);
}

void mslCodeGenerator::visit_do_while_loop(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *statement,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit do-while loop %d",depth);
    ISyntax_tree *condition = statement->get_child(0);
    ISyntax_tree *body = statement->get_child(1);
    visitor->begin_do_while(depth);
    visit_statement(log,comp_unit,comp_options,body,depth+1,visitor);
    visitor->begin_do_while_condition(depth);
    visit_expression(log,comp_unit,comp_options,condition,depth,visitor);
    visitor->end_do_while_condition(depth);
    visitor->end_do_while(depth);
    condition->release();
    body->release();
//log->info("end visit do-while loop %d",depth);
}

void mslCodeGenerator::visit_while_loop(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *statement,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit while loop %d",depth);
    ISyntax_tree *test = statement->get_child(0);
    ISyntax_tree *body = statement->get_child(1);
    visitor->begin_while(depth);
    visitor->begin_while_condition(depth);
    visit_expression(log,comp_unit,comp_options,test,depth,visitor);
    visitor->end_while_condition(depth);
    visit_statement(log,comp_unit,comp_options,body,depth+1,visitor);
    visitor->end_while(depth);
    test->release();
    body->release();
//log->info("end visit while loop %d",depth);
}

void mslCodeGenerator::visit_for_loop(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *statement,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit for loop %d",depth);
    ISyntax_tree *for_init = statement->get_child(0);
    ISyntax_tree *for_cond = statement->get_child(1);
    ISyntax_tree *for_iter = statement->get_child(2);
    ISyntax_tree *for_body = statement->get_child(3);
    visitor->begin_for(depth);
    if(0 < for_init->get_child_count()) {
        ISyntax_tree *init = for_init->get_child(0);
        visitor->begin_for_init();
        visit_condition(log,comp_unit,comp_options,init,depth,visitor);
        visitor->end_for_init();
        init->release();
    }
    if(0 < for_cond->get_child_count()) {
        ISyntax_tree *cond = for_cond->get_child(0);
        visitor->begin_for_condition();
        visit_expression(log,comp_unit,comp_options,cond,depth,visitor);
        visitor->end_for_condition();
        cond->release();
    }
    if(0 < for_iter->get_child_count()) {
        ISyntax_tree *iter = for_iter->get_child(0);
        visitor->begin_for_update();
        visit_expression(log,comp_unit,comp_options,iter,depth,visitor);
        visitor->end_for_update();
        iter->release();
    }
    visit_statement(log,comp_unit,comp_options,for_body,depth+1,visitor);
    visitor->end_for(depth);
    for_init->release();
    for_cond->release();
    for_iter->release();
    for_body->release();
//log->info("end visit for loop %d",depth);
}

void mslCodeGenerator::visit_foreach_loop(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *statement,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit foreach loop %d",depth);
    ISyntax_tree *condition = statement->get_child(0);
    ISyntax_tree *body = statement->get_child(1);
    visitor->begin_foreach(depth);
    visitor->begin_foreach_iterator();
    visit_expression(log,comp_unit,comp_options,condition,depth,visitor);
    visitor->end_foreach_iterator();
    visit_statement(log,comp_unit,comp_options,body,depth+1,visitor);
    visitor->end_foreach(depth);
    condition->release();
    body->release();
//log->info("end visit foreach loop %d",depth);
}

void mslCodeGenerator::visit_statement(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *statement,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit statement %d",depth);
    ISyntax_tree::Node_type type = statement->get_node_type();
    switch(type) {
    case ISyntax_tree::VARIABLE_DECLARATION:
        {
            ISyntax_tree *specifiers = statement->get_child(0);
            ISyntax_tree *id_list = statement->get_child(1);
            int count = id_list->get_child_count();
            for(int i = 0; i < count; i++) {
                ISyntax_tree *id = id_list->get_child(i);
                visitor->begin_declaration(depth);
                visit_variable_declaration(log,comp_unit,comp_options,
                                            specifiers,id,depth,visitor);
                visitor->end_declaration(depth);
                id->release();
            }
            specifiers->release();
            id_list->release();
        }
        break;
    case ISyntax_tree::BLOCK:
        visit_statement_block(log,comp_unit,comp_options,
                                statement,depth,visitor);
        break;
    case ISyntax_tree::CONDITIONAL_STATEMENT:
        visit_conditional_statement(log,comp_unit,comp_options,
                                        statement,depth,visitor);
        break;
    case ISyntax_tree::SWITCH_STATEMENT:
        visit_switch_statement(log,comp_unit,comp_options,
                                        statement,depth,visitor);
        break;
    case ISyntax_tree::EXPRESSION_STATEMENT:
        visit_expression_statement(log,comp_unit,comp_options,
                                        statement,depth,visitor);
        break;
    case ISyntax_tree::DO_WHILE_LOOP:
        visit_do_while_loop(log,comp_unit,comp_options,
                                        statement,depth,visitor);
        break;
    case ISyntax_tree::WHILE_LOOP:
        visit_while_loop(log,comp_unit,comp_options,
                                        statement,depth,visitor);
        break;
    case ISyntax_tree::FOR_LOOP:
        visit_for_loop(log,comp_unit,comp_options,
                                        statement,depth,visitor);
        break;
    case ISyntax_tree::FOREACH_LOOP:
        visit_foreach_loop(log,comp_unit,comp_options,
                                        statement,depth,visitor);
        break;
    case ISyntax_tree::JUMP_STATEMENT:
        {
            const char *type = statement->get_string_value();

            if(!strcmp(type,"break")) {
                visitor->break_statement(depth);
            } else if(!strcmp(type,"continue")) {
                visitor->continue_statement(depth);
            } else if(!strcmp(type,"return")) {
                visitor->begin_return_statement(depth);
                if(0 < statement->get_child_count()) {
                    ISyntax_tree *e = statement->get_child(0);
                    visit_expression(log,comp_unit,comp_options,
                                        e,depth,visitor);
                    e->release();
                }
                visitor->end_return_statement(depth);
            } else {
                log->error("unknown type of jump statement: '%s'",type);
            }
        }
        break;
    default:
        log->error("unknown type of statement: (%d)",type);
    }
//log->info("end visit statement %d",depth);
}

void mslCodeGenerator::visit_condition(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *condition,
    int depth,
    mslVisitor *visitor)
{
//log->info("begin visit condition %d",depth);
    const char *type = condition->get_string_value();

    if(!strcmp(type,"declaration")) {
        ISyntax_tree *specifier = condition->get_child(0);
        ISyntax_tree *id = condition->get_child(1);
        ISyntax_tree::Node_type specifier_type = specifier->get_node_type();
        mslQualifiedName type_name;
        switch(specifier_type) {
        case ISyntax_tree::SIMPLE_TYPE:
            type_name.append(specifier->get_string_value());
            break;
        case ISyntax_tree::NESTED_TYPE:
            {
                int count = specifier->get_child_count();
                for(int i = 0; i < count; i++) {
                    ISyntax_tree *child = specifier->get_child(i);
                    type_name.append(child->get_string_value());
                    child->release();
                }
            }
            break;
	default:
	    log->error("expected type, found %s",
                        node_type_to_c_str(specifier_type));
        }
        specifier->release();
        visitor->variable_declaration(0,&type_name,id->get_string_value(),0);
        id->release();
        if(2 < condition->get_child_count()) {
            ISyntax_tree *initializer = condition->get_child(2);
            visitor->begin_initializer(depth);
            visit_expression(log,comp_unit,comp_options,
                                initializer,depth,visitor);
            visitor->end_initializer(depth);
            initializer->release();
        }
    } else if(!strcmp(type,"expression")) {
        ISyntax_tree *e = condition->get_child(0);
        visit_expression(log,comp_unit,comp_options,e,depth,visitor);
        e->release();
    } else {
        log->error("unknown type of condition '%s'",type);
    }
//log->info("end visit condition %d",depth);
}

bool is_matrix_type(MI::MSDK::IType::Type_code i_type)
{
	switch(i_type)
	{
		case MI::MSDK::IType::TYPE_MATRIX2X2: //!< 2 x 2 matrix
		case MI::MSDK::IType::TYPE_MATRIX2X3: //!< 3 x 3 matrix
		case MI::MSDK::IType::TYPE_MATRIX3X2: //!< 3 x 2 matrix
		case MI::MSDK::IType::TYPE_MATRIX3X3: //!< 3 x 3 matrix
		case MI::MSDK::IType::TYPE_MATRIX4X3: //!< 4 x 3 matrix
		case MI::MSDK::IType::TYPE_MATRIX3X4: //!< 3 x 4 matrix
		case MI::MSDK::IType::TYPE_MATRIX4X4: //!< 4 x 4 matrix
		case MI::MSDK::IType::TYPE_MATRIX4X2: //!< 4 x 2 matrix
		case MI::MSDK::IType::TYPE_MATRIX2X4: //!< 2 x 4 matrix
			return true;
		default:
			return false;
	}
}

void mslCodeGenerator::visit_expression(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    const ISyntax_tree *expression,
    int depth,
    mslVisitor *visitor,
    int priority)
{
//log->info("begin visit expression %d",depth);
    ISyntax_tree::Node_type type = expression->get_node_type();
    switch(type) {
    case ISyntax_tree::IDENTIFIER:
        {
            IDeclaration *decl = expression->get_declaration();
			//bga - making sure variable names are valid
            //mslQualifiedName name(expression->get_string_value());
            mslQualifiedName name(get_variable_name(expression->get_string_value()));
            visitor->var_ref(&name,decl);
            if(decl)
                decl->release();
        }
        break;
    case ISyntax_tree::CONDITION:
        visit_condition(log,comp_unit,comp_options,expression,depth,visitor);
        break;
    case ISyntax_tree::EXPRESSION_LIST:
        {
            visitor->begin_expression_list();
            int count = expression->get_child_count();
            for(int i = 0; i < count; i++) {
                ISyntax_tree *child = expression->get_child(i);
                visitor->begin_expression_list_item(i);
                visit_expression(log,comp_unit,comp_options,
                                    child,depth,visitor);
                visitor->end_expression_list_item(i);
                child->release();
            }
            visitor->end_expression_list();
        }
        break;
    case ISyntax_tree::CONDITIONAL_EXPRESSION:
        {
            ISyntax_tree *a = expression->get_child(0);
            ISyntax_tree *b = expression->get_child(1);
            ISyntax_tree *c = expression->get_child(2);
            visitor->begin_conditional_expression();
            visitor->begin_conditional_condition();
            visit_expression(log,comp_unit,comp_options,a,depth,visitor);
            visitor->end_conditional_condition();
            visitor->begin_conditional_then();
            visit_expression(log,comp_unit,comp_options,b,depth,visitor);
            visitor->end_conditional_then();
            visitor->begin_conditional_else();
            visit_expression(log,comp_unit,comp_options,c,depth,visitor);
            visitor->end_conditional_else();
            visitor->end_conditional_expression();
            a->release();
            b->release();
            c->release();
        }
        break;
    case ISyntax_tree::ASSIGNMENT_EXPRESSION:
    case ISyntax_tree::OR_EXPRESSION:
    case ISyntax_tree::XOR_EXPRESSION:
    case ISyntax_tree::AND_EXPRESSION:
    case ISyntax_tree::EQUALITY_EXPRESSION:
    case ISyntax_tree::RELATIONAL_EXPRESSION:
    case ISyntax_tree::ADDITIVE_EXPRESSION:
        {
            const char *op = expression->get_string_value();
            int pri = get_priority(op);
            IType *type = expression->get_type();
            ISyntax_tree *left = expression->get_child(0);
            ISyntax_tree *right = expression->get_child(1);
            visitor->begin_bin_op(type,op,pri <= priority);
            visit_expression(log,comp_unit,comp_options,
                                left,depth,visitor,pri);
            visitor->op(type,op);
            visit_expression(log,comp_unit,comp_options,
                                right,depth,visitor,pri);
            visitor->end_bin_op(type,op,pri <= priority);
            left->release();
            right->release();
            if(type)
                type->release();
        }
		break;
    case ISyntax_tree::MULTIPLICATIVE_EXPRESSION:
        {
            const char *op = expression->get_string_value();
            int pri = get_priority(op);
            IType *type = expression->get_type();
            ISyntax_tree *left = expression->get_child(0);
            ISyntax_tree *right = expression->get_child(1);
			if (is_matrix_type(left->get_type()->get_typecode()) ||  
				is_matrix_type(right->get_type()->get_typecode()))
			{
				if(strcmp(op,"[]") && (pri <= priority))
					visitor->print_code("(");
				visitor->print_code("mul(");

				visit_expression(log,comp_unit,comp_options,
									left,depth,visitor,pri);
				visitor->print_code(",");
				//visitor->op(type,op);
				visit_expression(log,comp_unit,comp_options,
									right,depth,visitor,pri);

				visitor->print_code(")");
				if(strcmp(op,"[]")) {
					if(pri <= priority)
						visitor->print_code(")");
				} else {
					visitor->print_code("]");
				}

			}
			else
			{


				visitor->begin_bin_op(type,op,pri <= priority);
				visit_expression(log,comp_unit,comp_options,
									left,depth,visitor,pri);
				visitor->op(type,op);
				visit_expression(log,comp_unit,comp_options,
									right,depth,visitor,pri);
				visitor->end_bin_op(type,op,pri <= priority);
			}
			left->release();
			right->release();
			if(type)
				type->release();
        }
        break;
    case ISyntax_tree::PRE_INCREMENT_EXPRESSION:
        {
            const char *op = "++";
            int pri = get_priority(op);
            IType *type = expression->get_type();
            ISyntax_tree *a = expression->get_child(0);
            visitor->begin_pre_op(type,op,pri <= priority);
            visit_expression(log,comp_unit,comp_options,a,depth,visitor,pri);
            visitor->end_pre_op(type,op,pri <= priority);
            a->release();
            type->release();
        }
        break;
    case ISyntax_tree::PRE_DECREMENT_EXPRESSION:
        {
            const char *op = "--";
            int pri = get_priority(op);
            IType *type = expression->get_type();
            ISyntax_tree *a = expression->get_child(0);
            visitor->begin_pre_op(type,op,pri <= priority);
            visit_expression(log,comp_unit,comp_options,a,depth,visitor,pri);
            visitor->end_pre_op(type,op,pri <= priority);
            a->release();
            type->release();
        }
        break;
    case ISyntax_tree::UNARY_EXPRESSION:
        {
            const char *op = expression->get_string_value();
            int pri = get_priority("++");
            IType *type = expression->get_type();
            ISyntax_tree *a = expression->get_child(0);
            visitor->begin_pre_op(type,op,pri <= priority);
            visit_expression(log,comp_unit,comp_options,a,depth,visitor,pri);
            visitor->end_pre_op(type,op,pri <= priority);
            a->release();
            type->release();
        }
        break;
    case ISyntax_tree::POST_INCREMENT_EXPRESSION:
        {
            const char *op = "++";
            int pri = get_priority("[]");
            IType *type = expression->get_type();
            ISyntax_tree *a = expression->get_child(0);
            visitor->begin_post_op(type,op,pri <= priority);
            visit_expression(log,comp_unit,comp_options,a,depth,visitor,pri);
            visitor->end_post_op(type,op,pri <= priority);
            a->release();
            type->release();
        }
        break;
    case ISyntax_tree::POST_DECREMENT_EXPRESSION:
        {
            const char *op = "--";
            int pri = get_priority("[]");
            IType *type = expression->get_type();
            ISyntax_tree *a = expression->get_child(0);
            visitor->begin_post_op(type,op,pri <= priority);
            visit_expression(log,comp_unit,comp_options,a,depth,visitor,pri);
            visitor->end_post_op(type,op,pri <= priority);
            a->release();
            type->release();
        }
        break;
    case ISyntax_tree::SELECTION_EXPRESSION:
        {
            const char *op = ".";
            int pri = get_priority(op);
            IType *type = expression->get_type();
            ISyntax_tree *left = expression->get_child(0);
            ISyntax_tree *right = expression->get_child(1);
            visitor->begin_bin_op(type,op,pri <= priority);
            visit_expression(log,comp_unit,comp_options,
                                left,depth,visitor,pri);
            visitor->op(type,op);
            visit_expression(log,comp_unit,comp_options,
                                right,depth,visitor,pri);
            visitor->end_bin_op(type,op,pri <= priority);
            left->release();
            right->release();
            type->release();
        }
        break;
    case ISyntax_tree::INDEX_EXPRESSION:
        {
            const char *op = "[]";
            int pri = get_priority(op);
            IType *type = expression->get_type();
            ISyntax_tree *left = expression->get_child(0);
            ISyntax_tree *right = expression->get_child(1);
            visitor->begin_bin_op(type,op,pri <= priority);
            visit_expression(log,comp_unit,comp_options,left,depth,visitor);
            visitor->op(type,op);
            visit_expression(log,comp_unit,comp_options,right,depth,visitor);
            visitor->end_bin_op(type,op,pri <= priority);
            left->release();
            right->release();
            type->release();
        }
        break;
    case ISyntax_tree::FUNCTION_CALL:
    case ISyntax_tree::METHOD_CALL:
        {
            IDeclaration *decl = expression->get_declaration();
			//bool bRequiresState = (decl && (decl->is_method()));
			bool bRequiresState = (m_FunctionDefinitions.find(expression->get_string_value()) != m_FunctionDefinitions.end());
            mslQualifiedName name(function_convert(expression->get_string_value()));
            visitor->begin_call(&name,decl, bRequiresState);
            int position = (bRequiresState) ? 2 : 0; //bga - method calls get state and light also
            if(decl)
                decl->release();
            if(0 < expression->get_child_count()) {
                ISyntax_tree *args = expression->get_child(0);
                int count = args->get_child_count();
                for(int i = 0; i < count; i++) {
                    ISyntax_tree *argument = args->get_child(i);
					visitor->begin_argument(i + position);
                    visit_expression(log,comp_unit,comp_options,
                                        argument,depth,visitor);
                    visitor->end_argument(i + position);
                    argument->release();
                }
                args->release();
            }
            visitor->end_call();
        }
        break;
    case ISyntax_tree::CONSTRUCTOR_CALL:
        {
            ISyntax_tree *type = expression->get_child(0);
            ISyntax_tree *args = expression->get_child(1);
            mslQualifiedName type_name;
            switch(type->get_node_type()) {
            case ISyntax_tree::SIMPLE_TYPE:
                // Color constructors from a single float value are allowed in MetaSL,
				// but not in HLSL. So, have to send them to a special support function.
				// This will send all Color constructors to the same function, even
				// with another color as argument. Not sure how to detect the type of the
				// arg in 1.0. This issue is resovled in 1.2 code some other way.
				if ((strcmp(type->get_string_value(), "Color") == 0) &&
					(args->get_child_count() == 1))
					type_name.append("__color_ctor");
				else
					type_name.append(type_convert(type->get_string_value()));
                break;
            case ISyntax_tree::NESTED_TYPE:
                {
                    int count = type->get_child_count();
                    for(int i = 0; i < count; i++) {
                        ISyntax_tree *child = type->get_child(i);
                        type_name.append(child->get_string_value());
                        child->release();
                    }
                }
                break;
	    default:
                log->error("expected type, found %s",
                            node_type_to_c_str(type->get_node_type()));
            }
            type->release();
            visitor->begin_call(&type_name,0);
            int count = args->get_child_count();
            for(int i = 0; i < count; i++) {
                ISyntax_tree *argument = args->get_child(i);
                visitor->begin_argument(i);
                visit_expression(log,comp_unit,comp_options,
                                    argument,depth,visitor);
                visitor->end_argument(i);
                argument->release();
            }
            args->release();
            visitor->end_call();
        }
        break;
    case ISyntax_tree::INT_LITERAL_DEC:
        visitor->int_literal(expression->get_string_value());
        break;
    case ISyntax_tree::INT_LITERAL_OCT:
        visitor->int_literal(expression->get_string_value());
        break;
    case ISyntax_tree::INT_LITERAL_HEX:
        visitor->int_literal(expression->get_string_value());
        break;
    case ISyntax_tree::SCALAR_LITERAL:
        visitor->scalar_literal(expression->get_string_value());
        break;
    case ISyntax_tree::BOOL_LITERAL:
        visitor->bool_literal(expression->get_string_value());
        break;
    case ISyntax_tree::STRING_LITERAL:
        visitor->string_literal(expression->get_string_value());
        break;
    case ISyntax_tree::ARRAY_LITERAL:
        {
            visitor->begin_array_literal(depth);
            int count = expression->get_child_count();
            for(int i = 0; i < count; i++) {
                ISyntax_tree *child = expression->get_child(i);
                visitor->begin_array_literal_item(depth,i);
                visit_expression(log,comp_unit,comp_options,
                                    child,depth,visitor);
                visitor->end_array_literal_item(depth,i);
                child->release();
            }
            visitor->end_array_literal(depth);
        }
        break;
    case ISyntax_tree::VARIABLE_ID:
    case ISyntax_tree::MEMBER_VARIABLE_ID:
        {
            IDeclaration *decl = expression->get_declaration();
			//bga - making sure variable names are valid
            //mslQualifiedName name(expression->get_string_value());
            mslQualifiedName name(get_variable_name(expression->get_string_value()));
            visitor->var_ref(&name,decl);
            if(decl)
                decl->release();
        }
        break;
    case ISyntax_tree::STATE_VARIABLE_REFERENCE:
        {
            IDeclaration *decl = expression->get_declaration();
            visitor->state_var_ref(expression->get_string_value(),decl);
            if(decl)
                decl->release();
        }
        break;
    case ISyntax_tree::STATE_FUNCTION_REFERENCE:
        {
            IDeclaration *decl = expression->get_declaration();
            mslQualifiedName name(expression->get_string_value());
            visitor->begin_state_call(&name,decl);
            if(decl)
                decl->release();
            if(0 < expression->get_child_count()) {
                ISyntax_tree *args = expression->get_child(0);
                int count = args->get_child_count();
                for(int i = 0; i < count; i++) {
                    ISyntax_tree *argument = args->get_child(i);
                    visitor->begin_argument(i);
                    visit_expression(log,comp_unit,comp_options,
                                        argument,depth,visitor);
                    visitor->end_argument(i);
                    argument->release();
                }
                args->release();
            }
            visitor->end_state_call();
        }
        break;
    default:
        log->error("unknown expression type (%d)",type);
    }
//log->info("end visit expression %d",depth);
}

void mslCodeGenerator::visit(
    ILog *log,
    const ICompilation_unit *comp_unit,
    const ICompiler_options *comp_options,
    mslVisitor *visitor)
{
//log->info("begin visit compilation unit");
    visitor->begin_compilation_unit();

    int import_count = comp_unit->get_import_count();
    for(int i = 0; i < import_count; i++) {
//log->info("begin visit import %d",i);
        const char *file_name = comp_unit->get_import_file_name(i);
		if (strcmp(file_name, "standard")!=0) {
			ICompilation_unit *i_cunit = comp_unit->get_import_compilation_unit(i);
			visitor->begin_import(file_name);
			visit(log,i_cunit,comp_options,visitor);
			visitor->end_import();
			i_cunit->release();
		}
//log->info("end visit import %d",i);
    }

    int type_definition_count = comp_unit->get_type_definition_count();
    for(int i = 0; i < type_definition_count; i++) {
//log->info("begin visit type definition %d",i);
        IType *type = comp_unit->get_type_definition_type(i);
        const char *name = comp_unit->get_type_definition_name(i);
        visitor->begin_declaration(0);
        visit_type_definition(log,comp_unit,comp_options,type,name,0,visitor);
        visitor->end_declaration(0);
        type->release();
//log->info("end visit type definition %d",i);
    }

    int global_declaration_count = comp_unit->get_global_declaration_count();
    for(int i = 0; i < global_declaration_count; i++) {
//log->info("begin visit global declaration %d",i);
        ISyntax_tree *tree = comp_unit->get_global_declaration_syntax_tree(i);

		// Only output global definitions that are used. The names of these
		// functions are stored in the member variable "m_FunctionDefinitions"
		// that were collected earlier.
		std::string function_name;
		mslBlockOrdering::GetFunctionName(tree, function_name);
		if (m_FunctionDefinitions.find(function_name) != m_FunctionDefinitions.end())
		{
			visit_declaration(log,comp_unit,comp_options,tree,0,visitor);
		}

        tree->release();
//log->info("end visit global declaration %d",i);
    }

    int shader_count = comp_unit->get_shader_count();
    for(int i = 0; i < shader_count; i++) {
//log->info("begin visit shader declaration %d",i);
        ISyntax_tree *tree = comp_unit->get_shader_syntax_tree(i);
        visit_declaration(log,comp_unit,comp_options,tree,0,visitor);
        tree->release();
//log->info("end visit shader declaration %d",i);
    }

    visitor->end_compilation_unit();
//log->info("end visit compilation unit");
}
