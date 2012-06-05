/*****************************************************************************
**  geoPickedPoint.hpp
**
**      A geoPickedPoint continas information about ray intersection.
**  It determines which intersection is chosen and returns details
**  about the intersection point.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GEO_PICKEDPOINT_HPP
#error geoPickedPoint.hpp multiply included
#endif
#define GEO_PICKEDPOINT_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class geoPickedPoint
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		geoPickedPoint()
		{
			m_bPickClosest = true;
			m_bFoundIntersect = false;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~geoPickedPoint() {};

		//--------------------------------------------------------------------
		//	PickClosest is true if should pick closest intersection,
		//  false if any intersection is enough.
		//--------------------------------------------------------------------
		bool	IsPickClosest() const { return m_bPickClosest; }
		void	SetPickClosest(bool i_Val) { m_bPickClosest = i_Val; }

		//--------------------------------------------------------------------
		//	FoundIntersect is whether an intersection has been found yet
		//--------------------------------------------------------------------
		bool	IsFoundIntersect() const { return m_bFoundIntersect; }
		void	SetFoundIntersect(bool i_Val) { m_bFoundIntersect = i_Val; }

		//--------------------------------------------------------------------
		//	PickPos is the position of intersection
		//--------------------------------------------------------------------
		const maPoint3d& GetPickPos() const { return m_PickPos; }
		void  SetPickPos(const maPoint3d& i_Pos) { m_PickPos = i_Pos; }

		//--------------------------------------------------------------------
		//	PickNormal is the normal of intersection
		//--------------------------------------------------------------------
		const maVector3d& GetPickNormal() const { return m_PickNormal; }
		void  SetPickNormal(const maVector3d& i_Vec) { m_PickNormal = i_Vec; }

		//--------------------------------------------------------------------
		//	TVal is the parameter for percentage along ray where 
		//  intersection occurred.
		//--------------------------------------------------------------------
		float	GetTVal() const { return m_TVal; }
		void	SetTVal(float i_Val) { m_TVal = i_Val; }

	private:

		bool		m_bPickClosest;
		bool		m_bFoundIntersect;
		float		m_TVal;
		maPoint3d	m_PickPos;
		maVector3d	m_PickNormal;
};
