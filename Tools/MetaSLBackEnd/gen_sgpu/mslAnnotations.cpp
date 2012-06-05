/*****************************************************************************
**	mslAnnotations.cpp
**
**	 see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslAnnotations.hpp"

#include "mslVisitorHLSL.hpp"

#include "Core/dbg/dbgMsg.hpp"


namespace mslAnnotations
{

	namespace 
	{
		// Get the first argument to the function, if string literal.
		// Return true if the argument was found.
		// Might want to return a vector of arguments later.
		bool get_function_argument(ISyntax_tree *expression,
									std::string &o_Argument)
		{
			bool bResult = false;
			if (0 < expression->get_child_count()) {
                ISyntax_tree *args = expression->get_child(0);
				if (0 < args->get_child_count())
				{
                    ISyntax_tree *argument = args->get_child(0);
					o_Argument = argument->get_string_value();
					if (!o_Argument.empty())
						bResult = true;
                    argument->release();
                }
                args->release();
            }
			return bResult;
		}

		// Get control name in our syntax from MetaSL type
		bool get_control_name(const std::string &i_TypeName,  
							  bool i_bHasRange, 
							  std::string &o_ControlName)
		{
			if (i_TypeName == "Color")
			{
				o_ControlName = "ColorPicker";
				return true;
			}
			else if ((i_TypeName == "texture2D") || 
					 (i_TypeName == "texture3D") || 
					 (i_TypeName == "texture1D"))
			{
				o_ControlName = "FilePicker";
				return true;
			}
			else if (i_TypeName == "bool")
			{
				o_ControlName = "Checkbox";
				return true;
			}
			else if ((i_TypeName == "float")
					|| (i_TypeName == "int")
					|| (i_TypeName == "double")
					|| (i_TypeName == "half"))
			{
				// can uncomment this when we support Numeric controls in machstudio.
//				if (i_bHasRange)
					o_ControlName = "Slider";
//				else
//					o_ControlName = "Numeric";
				return true;
			}
			return false;
		}
	}

	
	//------------------------------------------------------------------------
	//	WriteAnnotation - write shader constant with annotations for the
	//	given MetaSL input.
	//------------------------------------------------------------------------
	void WriteAnnotation(mslVisitor *visitor,
						 ISyntax_tree *annotation,
						 ISyntax_tree *initializer,
						 const std::string &i_VariableName,
						 const std::string &i_TypeName)
	{
		const int depth = 0;
		bool bHasRange = false;
		visitor->begin_annotation(depth);
		int count = annotation->get_child_count();
		std::string display_name = i_VariableName;
		for(int i = 0; i < count; i++) {
			ISyntax_tree *expression = annotation->get_child(i);
			ISyntax_tree::Node_type type = expression->get_node_type();
			if (type == ISyntax_tree::FUNCTION_CALL)
			{
				std::string function_name = expression->get_string_value();
				if (function_name == "display_name")
				{
					std::string function_arg;
					if (get_function_argument(expression, function_arg))
						display_name = function_arg;
				}
				else if ((function_name == "soft_range") || (function_name == "hard_range"))
				{
					bHasRange = true;
				}
			}
			expression->release();
		}

		// Write DisplayName in our format
		visitor->print_code("\n\tstring SasUiLabel = \"%s\";", display_name.c_str());

		// Define the type of control to use based on variable's type
		std::string control_name;
		if (get_control_name(i_TypeName, bHasRange, control_name))
		{
			visitor->print_code("\n\tstring SasUiControl = \"%s\";", control_name.c_str());
		}
		
		visitor->print_code("\n\tstring UiCategory = \"MetaSL\";");
		visitor->print_code("\n\tstring SasUiDescription = \"Input from custom shader.\";");

		// Default values for texture file names need 
		// to be specified as annotation "name"
		if (initializer)
		{
			if (initializer->get_node_type() == ISyntax_tree::STRING_LITERAL)
			{
				std::string default_texture = initializer->get_string_value();
				visitor->print_code("\n\tstring name = \"%s\";", default_texture.c_str());
			}
		}

		visitor->end_annotation(depth);
	}

}	// end of namespace

