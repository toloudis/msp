/********************************************************************************************\
**  grpsGroupData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef GRPS_GROUPDATA_HPP
#error grpsGroupData.hpp multiply included
#endif
#define GRPS_GROUPDATA_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class grpsGroupData
{
public:
	nameString m_Name;

	std::vector<nameString>			m_ObjectNames;
//	std::vector<pick3dPickObject*>	m_Objects;
};


//============================================================================
//============================================================================
class grpsGroupsData
{
public:
	std::vector<grpsGroupData> m_Groups;

	//----------------------------------------------------------------------------
	// Add data as new group, or merge data into 
	// existing group with the same name.
	//----------------------------------------------------------------------------
	void AddGroupData(const grpsGroupData& i_Data);
};

