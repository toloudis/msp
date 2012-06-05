/********************************************************************************************\
**  grpsGroupData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef GRPS_GROUPDATA_HPP
#error grpsGroupData.hpp multiply included
#endif
#define GRPS_GROUPDATA_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef SEL3D_OBJECT_HPP
#include "Tool/sel3d/sel3dObject.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class grpsGroupData
{
public:
	nameString m_Name;

	std::vector<nameString>			m_ObjectNames;
//	std::vector<sel3dObject*>	m_Objects;
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

