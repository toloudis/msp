/*****************************************************************************
**  splnSpline.cpp
**
**      The splnSpline describes a path through points
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "Support/spln/splnSpline.hpp"



namespace
{

float compute_percents(std::vector<splnSpline::SplinePoint> &i_Points,
					  bool i_bClosed)
{
	int num_pts = i_Points.size();
	if (num_pts == 0) return 0;

	// Set up percents to be lengths along path
	int i;
	i_Points[0].percent = 0;
	if (num_pts == 1) return 0;

	if (i_bClosed)
		i_Points[0].percent = (i_Points[0].pos - i_Points[num_pts-1].pos).Length();

	for (i=1; i<num_pts; i++)
	{
		maVector3d diff = i_Points[i].pos - i_Points[i-1].pos;
		i_Points[i].percent = i_Points[i-1].percent + diff.Length();
	}

	// Get total length, divide each percent by total_length
	// to get values from 0-1
	//
	float total_length = i_Points[num_pts-1].percent;
	for (i=0; i<i_Points.size(); i++)
	{
		//DBG_LOG3("%d - Length  %f, Percent %f", i, i_Points[i].percent, i_Points[i].percent / total_length);
		i_Points[i].percent /= total_length;
	}

	return total_length;
}

float compute_param_from_percent(
				const std::vector<splnSpline::SplinePoint> &i_Points,
				float i_Percent,
				bool i_bClosed)
{
	if (i_Percent > 1) return 1;
	else if (i_Percent < 0) return 0;

	int num_pts = i_Points.size();

	float delta = 1.0f / (float) (i_bClosed ? num_pts : num_pts-1);
	float param = 0.0f;
	int start_ind = i_bClosed ? 0 : 1;
	for (int i=start_ind; i<num_pts; i++)
	{
		if (i_Percent < i_Points[i].percent)
		{
			float prev_percent = (i==start_ind) ? 0 : i_Points[i-1].percent;
			float alpha = (i_Percent - prev_percent) /
							(i_Points[i].percent - prev_percent);
			param += (alpha * delta);
			break;
		}
		param += delta;
	}

	return param;
}

maPoint3d do_bezier(float	i_Alpha,
					 const maPoint3d &p1,
					 const maPoint3d &p2,
					 const maPoint3d &p3,
					 const maPoint3d &p4)
{
	float u = i_Alpha;
	float u_2 = u * u;
	float u_3 = u * u_2;

	float B1 = (-1*u_3 + 3*u_2 - 3*u + 1);
	float B2 = ( 3*u_3 - 6*u_2 + 3*u + 0);
	float B3 = (-3*u_3 + 3*u_2 + 0*u + 0);
	float B4 = ( 1*u_3 + 0*u_2 + 0*u + 0);

	maPoint3d vec = p1 * B1 + p2 * B2 + p3 * B3 + p4 * B4;
	return vec;
}

maPoint3d do_bspline(float	i_Alpha,
					 const maPoint3d &p1,
					 const maPoint3d &p2,
					 const maPoint3d &p3,
					 const maPoint3d &p4)
{
	float u = i_Alpha;
	float u_2 = u * u;
	float u_3 = u * u_2;

	float B1 = (-1*u_3 + 3*u_2 - 3*u + 1);
	float B2 = ( 3*u_3 - 6*u_2 + 0*u + 4);
	float B3 = (-3*u_3 + 3*u_2 + 3*u + 1);
	float B4 = ( 1*u_3 + 0*u_2 + 0*u + 0);

	maPoint3d vec = (p1 * B1 + p2 * B2 + p3 * B3 + p4 * B4) / 6;
	return vec;
}

maPoint3d do_catmull(float	i_Alpha,
					 const maPoint3d &p1,
					 const maPoint3d &p2,
					 const maPoint3d &p3,
					 const maPoint3d &p4)
{
	float u = i_Alpha;
	float u_2 = u * u;
	float u_3 = u * u_2;

	float B1 = (-1*u_3 + 2*u_2 - 1*u + 0);
	float B2 = ( 3*u_3 - 5*u_2 + 0*u + 2);
	float B3 = (-3*u_3 + 4*u_2 + 1*u + 0);
	float B4 = ( 1*u_3 - 1*u_2 + 0*u + 0);

	maPoint3d vec = (p1 * B1 + p2 * B2 + p3 * B3 + p4 * B4) / 2;
	return vec;
}

maPoint3d tangent_catmull(float	i_Alpha,
					 const maPoint3d &p1,
					 const maPoint3d &p2,
					 const maPoint3d &p3,
					 const maPoint3d &p4)
{
	float u = i_Alpha;
	float u_2 = u * u;
	//float u_3 = u * u_2;

	//float B1 = (-3*u_3 - 1*u_2 + 4*u - 1);
	//float B2 = ( 9*u_3 + 3*u_2 - 10*u + 0);
	//float B3 = (-9*u_3 - 3*u_2 + 8*u + 1);
	//float B4 = ( 3*u_3 + 1*u_2 - 2*u + 0);

	float B1 = (-3*u_2 + 4*u - 1);
	float B2 = ( 9*u_2 - 10*u + 0);
	float B3 = (-9*u_2 + 8*u + 1);
	float B4 = ( 3*u_2 - 2*u + 0);

	maPoint3d vec = (p1 * B1 + p2 * B2 + p3 * B3 + p4 * B4) / 2;
	return vec;
}

// get relevant control points for given segment
void get_cpts(int i_Segment, int i_NumPts, bool i_bClosed,
			  int &o_A, int &o_B, int &o_C, int &o_D)
{
	if (i_bClosed)
	{
		o_A = (i_Segment+i_NumPts-1) % i_NumPts;
		o_B = i_Segment;
		o_C = (i_Segment+1) % i_NumPts;
		o_D = (i_Segment+2) % i_NumPts;
	}
	else
	{
		o_A = (i_Segment > 0) ? i_Segment-1 : 0;
		o_B = i_Segment;
		o_C = i_Segment+1;
		o_D = (i_Segment < i_NumPts-2) ? i_Segment+2 : i_NumPts-1;
	}
}


} // local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
splnSpline::splnSpline( SplineType i_Type )
:	m_SplineType(i_Type),
	m_bUseChordLength(true),
	m_bClosed(false),
	m_Length(0)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
splnSpline::~splnSpline()
{
}

//--------------------------------------------------------------------
//	SetPoints sets control points for spline
//--------------------------------------------------------------------
void splnSpline::SetPoints(const maPoint3d *i_pPoints, int i_NumPoints)
{
	m_Points.resize(i_NumPoints);

	for (int i=0; i<i_NumPoints; i++)
	{
		m_Points[i].pos = i_pPoints[i];
	}

	m_Length = compute_percents(m_Points, m_bClosed);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float splnSpline::GetLength() const
{
	return m_Length;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int splnSpline::GetNumPoints() const
{
	return m_Points.size();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maPoint3d splnSpline::GetPointPos(int i_Index) const
{
	return m_Points[i_Index].pos;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
float splnSpline::GetPointPercent(int i_Index) const
{
	return m_Points[i_Index].percent;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void splnSpline::SetPointPos(int i_Index, const maPoint3d & i_Pos)
{
	m_Points[i_Index].pos = i_Pos;
	m_Length = compute_percents(m_Points, m_bClosed);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void splnSpline::AppendPoint(const maPoint3d & i_Pos)
{
	SplinePoint pt;
	pt.pos = i_Pos;
	m_Points.push_back(pt);
	m_Length = compute_percents(m_Points, m_bClosed);
}

//--------------------------------------------------------------------
// Insert point after given index
//--------------------------------------------------------------------
void splnSpline::InsertPoint(int i_Index, const maPoint3d & i_Pos)
{
	SplinePoint pt;
	pt.pos = i_Pos;
	m_Points.insert(m_Points.begin()+(i_Index+1), pt);
	m_Length = compute_percents(m_Points, m_bClosed);
}

//--------------------------------------------------------------------
//	Delete a point
//--------------------------------------------------------------------
void splnSpline::DeletePoint(int i_Index)
{
	int num_pts = m_Points.size();
	m_Points.erase(m_Points.begin() + i_Index);
	m_Length = compute_percents(m_Points, m_bClosed);
}

//--------------------------------------------------------------------
// Remove last point
//--------------------------------------------------------------------
void splnSpline::TruncatePoint()
{
	int num_pts = m_Points.size();
	m_Points.erase(m_Points.begin() + (num_pts-1));
	m_Length = compute_percents(m_Points, m_bClosed);
}

//--------------------------------------------------------------------
//  Closed curves connect curve back to first point.
//	First and last points should not be equal.
//--------------------------------------------------------------------
void splnSpline::SetClosed(bool i_Value)
{
	if (!m_Points.empty() && m_bClosed != i_Value)
		m_Length = compute_percents(m_Points, i_Value);

	m_bClosed = i_Value;
}
bool splnSpline::IsClosed() const
{
	return m_bClosed;
}

//--------------------------------------------------------------------
//  SplineType
//--------------------------------------------------------------------
void splnSpline::SetSplineType(SplineType i_Type)
{
	// changing spline type doesn't actually change
	// the pre-computed percentages
	m_SplineType = i_Type;
}
splnSpline::SplineType splnSpline::GetSplineType() const
{
	return m_SplineType;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
maPoint3d splnSpline::Evaluate(float i_Percent, maVector3d *o_pTangent) const
{
	int num_pts = m_Points.size();
	DBG_ASSERT(num_pts > 0, "Need to set points of spline before evaluating");

	if (num_pts == 1)
	{
		if (o_pTangent)
			o_pTangent->Set(0, 0, 1);
		return m_Points[0].pos;
	}

	float param = i_Percent;
	if (m_bUseChordLength)
		param = compute_param_from_percent(m_Points, i_Percent, m_bClosed);

	int num_segs = m_bClosed ? num_pts : num_pts-1;

	SplineType spline_type = m_SplineType;
	if (spline_type != splnSpline::e_Linear && num_pts < 2)
	{
		// spline type but not enough points, so
		// force back to linear
		spline_type = splnSpline::e_Linear;
	}

	int segment = int(param * num_segs);

	float alpha = (param * num_segs) - float(segment);

	maPoint3d pos;
	int A,B,C,D;

	switch (spline_type)
	{
	default:
	case splnSpline::e_Linear:
		if (segment >= num_segs)
			pos = (m_bClosed) ? m_Points[0].pos : m_Points[num_pts-1].pos;
		else if (segment < 0)
			pos =  m_Points[0].pos;
		else if(m_bClosed)
			pos = m_Points[(segment+num_pts-1) % num_pts].pos +
				(m_Points[segment].pos - m_Points[(segment+num_pts-1) % num_pts].pos) * alpha;
		else
			pos = m_Points[segment].pos +
				(m_Points[segment+1].pos - m_Points[segment].pos) * alpha;
		break;
	case splnSpline::e_BSpline:
		if (!m_bClosed && num_pts == 4)
		{
			pos = do_bezier(param, m_Points[0].pos, m_Points[1].pos,
								m_Points[2].pos, m_Points[3].pos);
		}
		else
		{
			if (segment >= num_segs)
			{
				segment = num_segs - 1;
				alpha = 1;
			}

			get_cpts(segment, num_pts, m_bClosed, A,B,C,D);
			pos = do_bspline(alpha,
							m_Points[A].pos,
							m_Points[B].pos,
							m_Points[C].pos,
							m_Points[D].pos);
		}
		break;
	case splnSpline::e_Catmull:
		if (m_bClosed)
			segment = (segment+num_pts-1) % num_pts;

		if (segment >= num_segs)
		{
			segment = num_segs - 1;
			alpha = 1;
		}

		get_cpts(segment, num_pts, m_bClosed, A,B,C,D);
		pos = do_catmull(alpha,
						m_Points[A].pos,
						m_Points[B].pos,
						m_Points[C].pos,
						m_Points[D].pos);

		if (o_pTangent)
		{
			*o_pTangent = tangent_catmull(alpha,
						m_Points[A].pos,
						m_Points[B].pos,
						m_Points[C].pos,
						m_Points[D].pos);
			o_pTangent->Normalize();
		}
		break;
	}

	return pos;
}

//--------------------------------------------------------------------
// Name for curve
//--------------------------------------------------------------------
const std::string& splnSpline::GetName() const
{
	return m_Name;
}
void splnSpline::SetName(const char* i_Name)
{
	m_Name = i_Name;
}

