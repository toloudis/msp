/********************************************************************************************\
**  cmraDriverDataViewTypeInfo.hpp
**
**		Data structure for cmra ViewType info.
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERDATAVIEWTYPEINFO_HPP
#error cmraDriverDataViewTypeInfo.hpp multiply included
#endif
#define CMRA_DRIVERDATAVIEWTYPEINFO_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class cmraDriverDataViewTypeInfo
{
public:
	enum DataViewType
	{
		e_DataView_ExtremeCloseUp = 1,
		e_DataView_CloseUp,
		e_DataView_Medium,
		e_DataView_MediumLong,
		e_DataView_Long,
		e_DataView_Overhead,
		e_DataView_Aerial
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverDataViewTypeInfo();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverDataViewTypeInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual cmraDriverDataViewTypeInfo* Clone();

public:
	int		m_ViewType;
	float	m_fViewTolerance;
};


