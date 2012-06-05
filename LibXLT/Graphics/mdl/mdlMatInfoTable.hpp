/*****************************************************************************
**	mdlMatInfoTable.h
**
**		mdlMatInfoTable represents a table of materials
**	loaded or to be written to a file.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MATINFOTABLE_HPP
#error mdlMatInfoTable.hpp multiply included
#endif
#define MDL_MATINFOTABLE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <map>
#include <string>


//============================================================================
// forward declarations
//============================================================================
struct mdlMatInfo;

//============================================================================
// Material Table is a typedef for a mapping from strings to 
//	shared pointers of material information structures
//============================================================================
typedef std::map<std::string, shared_ptr<mdlMatInfo> > mdlMatInfoTable;

