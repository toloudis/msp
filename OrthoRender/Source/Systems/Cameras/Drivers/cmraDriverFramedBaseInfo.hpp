/********************************************************************************************\
**  cmraDriverFramedBaseInfo.hpp
**
**		Data structure for cmra FramedBase
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERFRAMEDBASEINFO_HPP
#error cmraDriverFramedBaseInfo.hpp multiply included
#endif
#define CMRA_DRIVERFRAMEDBASEINFO_HPP

#ifndef CMRA_DRIVERDATAPOSITIONINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataPositionInfo.hpp"
#endif
#ifndef CMRA_DRIVERDATASUBJECTINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataSubjectInfo.hpp"
#endif
#ifndef CMRA_DRIVERDATAVIEWTYPEINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataViewTypeInfo.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif


class cmraDriverFramedBaseInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverFramedBaseInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverFramedBaseInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void display_values() const;

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool		m_bPositionRestrict;
	maPoint3d	m_PositionTolerance;
	bool		m_bAngleRestrict;
	maPoint3d	m_AngleTolerance;
	bool		m_bDistanceRestrict;
	maPoint3d	m_DistanceTolerance;
	bool		m_bDirectionRestrict;
	float		m_fDirectionTolerance;

	cmraDriverDataPositionInfo	m_StartPositionData;
	cmraDriverDataSubjectInfo	m_SubjectData;
	cmraDriverDataViewTypeInfo	m_ViewTypeData;
};
