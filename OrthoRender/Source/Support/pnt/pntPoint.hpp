/*****************************************************************************
**  pntPoint.hpp
**
**      The pntPoint describes a point.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PNT_POINT_HPP
#error pntPoint.hpp multiply included
#endif
#define PNT_POINT_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>


//====================================================================
//	
//====================================================================
class pntPoint
{
	public:
		// Type for callback function
		class PointChangedCallback
		{
		public:
			virtual void PointChanged(pntPoint*) = 0;
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pntPoint();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~pntPoint();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetPosition(const maPoint3d & i_Pos);
		const maPoint3d& GetPosition() const;

		//--------------------------------------------------------------------
		// Name for curve
		//--------------------------------------------------------------------
		const std::string& GetName() const;
		void SetName(const char* i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		maPoint3d Evaluate() const;

		//--------------------------------------------------------------------
		// Set callback for when point changes value.
		//--------------------------------------------------------------------
		void SetCallback(PointChangedCallback *i_pCallback);

	private:
		maPoint3d	m_Point;
		std::string m_Name;
		PointChangedCallback* m_pCallback;
};


