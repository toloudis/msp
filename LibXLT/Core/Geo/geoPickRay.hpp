/*****************************************************************************
**  g3oPickRay.hpp
**
**	a Ray used for picking.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GEO_PICKRAY_HPP
#error geoPickRay.hpp multiply included
#endif
#define GEO_PICKRAY_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif


//============================================================================
//============================================================================
class geoPickRay
{
	public:
		//--------------------------------------------------------------------
		//	constructors
		//--------------------------------------------------------------------
		geoPickRay();
		geoPickRay( maPoint3d& i_RayStart, maVector3d& i_RayDir );

		//--------------------------------------------------------------------
		//	destructor
		//--------------------------------------------------------------------
		~geoPickRay();

		//--------------------------------------------------------------------
		//	SetFromStartEnd() - create the ray information from a start and
		//	end point.
		//--------------------------------------------------------------------
		void SetFromStartEnd( const maPoint3d& i_RayStart, const maPoint3d& i_RayEnd );

		//--------------------------------------------------------------------
		// shortens ray to given t value 
		// (after picking against occluders and before passing to objects)
		//--------------------------------------------------------------------
		void Shorten( float i_Tval );

		//--------------------------------------------------------------------
		//	GetRayStart()
		//--------------------------------------------------------------------
		const maPoint3d& GetRayStart() const;

		//--------------------------------------------------------------------
		//	GetRayEnd()
		//--------------------------------------------------------------------
		const maPoint3d GetRayEnd() const;

		//--------------------------------------------------------------------
		//	GetRayDir()
		//--------------------------------------------------------------------
		const maVector3d& GetRayDir() const;

	private:
		maPoint3d	m_RayStart;
		maVector3d	m_RayDir;
		float		m_fTVal;

		bool	m_bInfiniteStart;
		bool	m_bInfiniteEnd;
};
