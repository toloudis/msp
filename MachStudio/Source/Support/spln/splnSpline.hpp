/*****************************************************************************
**  splnSpline.hpp
**
**      The splnSpline describes a path through points
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/
#ifdef SPLN_SPLINE_HPP
#error splnSpline.hpp multiply included
#endif
#define SPLN_SPLINE_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>
#include <vector>


//====================================================================
//	Forward References
//====================================================================


//====================================================================
//====================================================================
class splnSpline
{
	public:
		enum SplineType		// how to compute path from points
		{
			e_Catmull,		// default
			e_BSpline,
			e_Linear
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		splnSpline(SplineType i_Type = e_Catmull);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~splnSpline();

		//--------------------------------------------------------------------
		//	SetPoints sets control points for spline
		//--------------------------------------------------------------------
		void SetPoints(const maPoint3d *i_pPoints, int i_NumPoints);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		float GetLength() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetNumPoints() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		maPoint3d GetPointPos(int i_Index) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		float GetPointPercent(int i_Index) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetPointPos(int i_Index, const maPoint3d & i_Pos);

		//--------------------------------------------------------------------
		// Add given point to end of list
		//--------------------------------------------------------------------
		void AppendPoint(const maPoint3d & i_Pos);

		//--------------------------------------------------------------------
		// Insert point after given index
		//--------------------------------------------------------------------
		void InsertPoint(int i_Index, const maPoint3d & i_Pos);

		//--------------------------------------------------------------------
		//	Delete a point
		//--------------------------------------------------------------------
		void DeletePoint(int i_Index);

		//--------------------------------------------------------------------
		// Remove last point
		//--------------------------------------------------------------------
		void TruncatePoint();

		//--------------------------------------------------------------------
		//  Closed curves connect curve back to first point.
		//	First and last points should not be equal.
		//--------------------------------------------------------------------
		void SetClosed(bool i_Value);
		bool IsClosed() const;

		//--------------------------------------------------------------------
		//  SplineType
		//--------------------------------------------------------------------
		void SetSplineType(SplineType i_Type);
		SplineType GetSplineType() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		maPoint3d Evaluate(float i_Percent, maVector3d *o_pTangent = NULL) const;

		//--------------------------------------------------------------------
		// Name for curve
		//--------------------------------------------------------------------
		const std::string& GetName() const;
		void SetName(const char* i_Name);


		struct SplinePoint
		{
			maPoint3d pos;
			float	percent;
		};

	private:

		std::vector<SplinePoint>	m_Points;
		float		m_Length;
		SplineType	m_SplineType;
		bool		m_bUseChordLength;
		bool		m_bClosed;
		std::string m_Name;
};


