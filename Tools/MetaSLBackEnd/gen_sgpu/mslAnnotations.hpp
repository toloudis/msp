/*****************************************************************************
**	mslAnnotations.hpp
**
**	 mslAnnotations handles the conversion of MetaSL inputs into
**	the FX shader constant annotations that our parser will convert 
**	into properties for the material.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MSL_ANNOTATIONS_HPP
#error mslAnnotations.hpp multiply included
#endif
#define MSL_ANNOTATIONS_HPP

#ifndef MSL_DEFS_HPP
#include "mslDefs.hpp"
#endif

class mslVisitor;

#include <string>

//============================================================================
//============================================================================
namespace mslAnnotations 
{
	//------------------------------------------------------------------------
	//	WriteAnnotation - write shader constant with annotations for the
	//	given MetaSL input.
	//------------------------------------------------------------------------
	void WriteAnnotation(mslVisitor *visitor,
						 ISyntax_tree *annotation,
						 ISyntax_tree *initializer,
						 const std::string &i_VariableName,
						 const std::string &i_TypeName);
};

