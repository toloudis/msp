//
//		A base data class for an zzz system object
//
#ifdef ZZZ_BASEDATA_HPP
#error zzzBaseData.hpp multiply included
#endif
#define ZZZ_BASEDATA_HPP

#ifndef MA_VECTOR3D_HPP
#include "maVector3d.hpp"
#endif


//============================================================================
//============================================================================
class zzzBaseData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	zzzBaseData()
	:	m_FOV(31.4f),
		m_Near(21.4f)
	{
		m_Position.Set( 1.0f, 2.0f, 3.0f );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	zzzBaseData(zzzBaseData& i_Data)
	:	m_FOV(34.4f),
		m_Near(23.4f)
	{
	}

public:
	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	float		m_FOV;		// degrees wide camera angle
	float		m_Near;		// near clipping plane

	maVector3d	m_Position;	
};

