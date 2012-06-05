/*****************************************************************************
**  OrderingUtil.hpp
**
**      Namespace with functions for supplying an ordering to the 
**	selection in Maya. The ordering usually needs to match in order
**	to match up geometry and animation files.
**
**	Extra Large Technology
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef ORDERINGUTIL_HPP
#error OrderingUtil.hpp multiply included
#endif
#define ORDERINGUTIL_HPP

class MItSelectionList;
class MDagPathArray;

#include <string>
#include <map>

namespace OrderingUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	typedef std::map< std::string, int> UniqueNameMap;

	//------------------------------------------------------------------------
	// Given a selection iterator type criteria,  return a list of 
	// dagPaths so that if the same selection in a different
	// order would produce the same ordering of paths.
	//------------------------------------------------------------------------
	void DetermineSelectionOrdering(MFn::Type i_Type,
									MDagPathArray &o_PathsInOrder);

	//------------------------------------------------------------------------
	// Make sure name is unique in the map. If not, then add some counter
	// to the name.
	//------------------------------------------------------------------------
	void CheckUniqueName(std::string &io_Name,
						 UniqueNameMap &io_UniqueNameMap);
}

