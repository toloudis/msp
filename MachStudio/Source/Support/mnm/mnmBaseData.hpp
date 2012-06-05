/********************************************************************************************\
**  mnmBaseData.hpp
**
**		Universal base data 
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef MNM_BASEDATA_HPP
#error mnmBaseData.hpp multiply included
#endif
#define MNM_BASEDATA_HPP

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif

#include <vector>

class tmlnBaseData;

//
//
class mnmBaseData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mnmBaseData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mnmBaseData( const itString& i_Filename );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mnmBaseData(const itString& i_Filename,
				 const maPoint3d& i_Position,
				 const maRotation& i_Orientation );

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const mnmBaseData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mnmBaseData& operator = (const mnmBaseData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mnmBaseData& operator = (const tmlnBaseData& i_Data);

	//
	//	data
	//
	bool		m_bEditorVisible;
	nameString	m_Name;
	itString	m_Filename;
	maPoint3d	m_Position;
	maRotation	m_Orientation;
	//float or maPoint3d m_Scale; (????)

	std::vector<tmlnDriverInfo*> m_Drivers;
};

