/****************************************************************************\
**	pythUserScriptUtil.hpp
**
**		Functions for executing python expressions defined by the 
**	user and getting return values from the expressions.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_USERSCRIPTUTIL_HPP
#error pythUserScriptUtil.hpp multiply included
#endif
#define PYTH_USERSCRIPTUTIL_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif 

#include <string>

//============================================================================
//============================================================================
namespace pythUserScriptUtil
{
	//--------------------------------------------------------------------
	// Execute the following code in the python interpretor.
	// If successful, returns true and sets the return value of
	// the expression in the second argument.
	//--------------------------------------------------------------------
	bool ExecuteUserScript(const std::string& i_Command,
						   bool &o_Value,
						   std::string& o_ErrorMessage);
	bool ExecuteUserScript(const std::string& i_Command,
						   float &o_Value,
						   std::string& o_ErrorMessage);
	bool ExecuteUserScript(const std::string& i_Command,
						   maFloatRGBA &o_Color,
						   std::string& o_ErrorMessage);
	bool ExecuteUserScript(const std::string& i_Command,
						   maPoint3d &o_Position,
						   std::string& o_ErrorMessage);
	bool ExecuteUserScript(const std::string& i_Command,
						   float &o_EulerX,
						   float &o_EulerY,
						   float &o_EulerZ,
						   std::string& o_ErrorMessage);

	//--------------------------------------------------------------------
	// Execute a property callback as a python script, passing in
	//	object name and property name.
	//--------------------------------------------------------------------
	bool ExecutePropertyCallback(const std::string& i_Command,
								 const std::string& i_ObjectName,
								 const std::string& i_PropertyName);

};
