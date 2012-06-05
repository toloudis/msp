/********************************************************************************************\
**  cmraDriverDataSubjectInfo.hpp
**
**		Data structure for cmra Subject info.
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERDATASUBJECTINFO_HPP
#error cmraDriverDataSubjectInfo.hpp multiply included
#endif
#define CMRA_DRIVERDATASUBJECTINFO_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class cmraDriverDataSubjectInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverDataSubjectInfo();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverDataSubjectInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual cmraDriverDataSubjectInfo* Clone();

	//--------------------------------------------------------------------
	//	copy the information
	//--------------------------------------------------------------------
	cmraDriverDataSubjectInfo& operator = (const cmraDriverDataSubjectInfo& i_Info);

public:
	std::vector<nameString>		m_ObjectNames;
};
