/*****************************************************************************
**  g3oPickRay.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/geo/geoPickRay.hpp"


//----------------------------------------------------------------------------
//	constructor
//----------------------------------------------------------------------------
geoPickRay::geoPickRay()
:	m_bInfiniteStart( false ),
	m_bInfiniteEnd( true ),
	m_fTVal( 9999.0f )
{
}

geoPickRay::geoPickRay( maPoint3d& i_RayStart, maVector3d& i_RayDir )
:	m_bInfiniteStart( false ),
	m_bInfiniteEnd( true ),
	m_fTVal( 9999.0f )
{
	m_RayStart	= i_RayStart;
	m_RayDir	= i_RayDir;
}


//----------------------------------------------------------------------------
//	destructor
//----------------------------------------------------------------------------
geoPickRay::~geoPickRay()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void geoPickRay::SetFromStartEnd( const maPoint3d& i_RayStart, const maPoint3d& i_RayEnd )
{
	m_RayStart = i_RayStart;
	m_RayDir = i_RayEnd - i_RayStart;
	//? m_RayDir.Normalize();
}

//----------------------------------------------------------------------------
// shortens ray to given t value 
// (after picking against occluders and before passing to objects)
//----------------------------------------------------------------------------
void geoPickRay::Shorten( float i_Tval )
{
	m_fTVal = i_Tval;
}

//--------------------------------------------------------------------
//	GetRayStart()
//--------------------------------------------------------------------
const maPoint3d& geoPickRay::GetRayStart() const
{
	return m_RayStart;
}

//--------------------------------------------------------------------
//	GetRayEnd()
//--------------------------------------------------------------------
const maPoint3d geoPickRay::GetRayEnd() const
{
	maPoint3d endPt = m_RayDir;

	endPt.Normalize();
	endPt *= m_fTVal;

	return endPt;
}

//--------------------------------------------------------------------
//	GetRayDir()
//--------------------------------------------------------------------
const maVector3d& geoPickRay::GetRayDir() const
{
	return m_RayDir;
}


