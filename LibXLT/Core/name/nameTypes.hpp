/****************************************************************************\
**	nameTypes.hpp
**
**		name component data types
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef NAME_TYPES_HPP
#error nameTypes.hpp multiply included
#endif
#define NAME_TYPES_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <vector>


//============================================================================
//	typedefs
//============================================================================
typedef std::vector<const nameString*> nameList;

