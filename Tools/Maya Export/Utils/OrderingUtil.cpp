/*****************************************************************************
**  OrderingUtil.cpp
**
**
**	Extra Large Technology
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "MayaUtil.hpp"
#include "OrderingUtil.hpp"

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>

#include <sstream>

namespace OrderingUtil
{
	namespace
	{
		// find index for path within path array or return -1 for "not found"
		int find(MDagPathArray &i_PathArray, MDagPath &i_Path)
		{
			const int num_paths = i_PathArray.length();
			for (int i=0; i<num_paths; ++i)
			{
				if (i_PathArray[i] == i_Path)
					return i;
			}
			return -1;
		}

	}	// end of namespace


	//------------------------------------------------------------------------
	// Given a selection iterator already setup for the criteria desired
	// return a list of dagPaths so that if the same selection in a different
	// order would produce the same ordering of paths.
	//------------------------------------------------------------------------
	void DetermineSelectionOrdering(MFn::Type i_Type,
									MDagPathArray &o_PathsInOrder)
	{
		MStatus status;	
		
		MSelectionList activeList;
		status = MGlobal::getActiveSelectionList(activeList);
		MItSelectionList sel_it( activeList, i_Type );

		// Gather paths from the selection list
		MDagPathArray sel_order;
		//std::cout << "Selection Order:" << std::endl;
		for ( ; !sel_it.isDone(); sel_it.next() ) 
		{
			MDagPath dagPath;			
			MObject component;
			sel_it.getDagPath( dagPath, component );
			sel_order.append( dagPath );
			//std::cout << dagPath.fullPathName() << std::endl;
		}

		// Traverse the full DAG and try to find the paths in the selection list.
		// The order the paths are found in the DAG will not change even though the
		// order in the selection list might.
		MItDag dag_it( MItDag::kDepthFirst, i_Type);
		MDagPath cur_path;
		int index = -1;
		//std::cout << "DAG Order:" << std::endl;
		for ( ; !dag_it.isDone(); dag_it.next() ) 
		{
			dag_it.getPath(cur_path);
			index = find(sel_order, cur_path);
			if (index >= 0)
			{
				//std::cout << cur_path.fullPathName() << std::endl;
				//std::cout << "  Found index " << index << std::endl;
				o_PathsInOrder.append(cur_path);
				sel_order.remove(index);
			}
		}

		// If there are paths still in the selection that we did not find,
		// then this whole principle is flawed. But, we gotta do something, so
		// just append the left overs to the end.
		const int num_leftover = sel_order.length();
		//std::cout << "Left over:" << num_leftover << std::endl;
		for (int i=0; i<num_leftover; ++i)
		{
			//std::cout << sel_order[i].fullPathName() << std::endl;
			o_PathsInOrder.append(sel_order[i]);
		}
	}

	//------------------------------------------------------------------------
	// Make sure name is unique in the map. If not, then add some counter
	// to the name.
	//------------------------------------------------------------------------
	void CheckUniqueName(std::string &io_Name,
						 UniqueNameMap &io_UniqueNameMap)
	{
		UniqueNameMap::iterator it = io_UniqueNameMap.find(io_Name);
		if (it == io_UniqueNameMap.end())
		{
			// First use of this name
			io_UniqueNameMap[io_Name] = 1;
		}
		else
		{
			// Add one to the count and use this number for the
			// suffix (so first suffix is "_2"
			int count = ++it->second;
			std::ostringstream sstream;
			sstream << io_Name << "#" << count;
			io_Name = sstream.str();
			std::cout << "Duplicate name found, new name assigned, " << io_Name << std::endl;
		}
	}

} // end of namespace


