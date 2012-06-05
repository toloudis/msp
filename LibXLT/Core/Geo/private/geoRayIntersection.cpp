/*****************************************************************************
**  geoRayIntersection.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/geo/geoRayIntersection.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/ma/maRotation.hpp"

//============================================================================
//============================================================================
namespace
{


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool clip(float i_Denom, float i_Numer, float& o_T0, float& o_T1)
{
	// Return value is 'true' if line segment intersects the current test
	// plane.  Otherwise 'false' is returned in which case the line segment
	// is entirely clipped.
	if ( i_Denom > 0.0f )
	{
		if ( i_Numer > i_Denom * o_T1 )
			return false;
		if ( i_Numer > i_Denom * o_T0 )
			o_T0 = i_Numer / i_Denom;
		return true;
	}
	else if ( i_Denom < 0.0f )
	{
		if ( i_Numer > i_Denom * o_T0 )
			return false;
		if ( i_Numer > i_Denom * o_T1 )
			o_T1 = i_Numer / i_Denom;
		return true;
	}
	else
		return i_Numer <= 0.0f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool find_intersection(	const maVector3d& i_Origin,
						const maVector3d& i_Direction, 
						const maVector3d& i_Extents, 
						float& o_T0,
						float& o_T1)
{
    float fSaveT0 = o_T0, fSaveT1 = o_T1;

    bool bNotEntirelyClipped =
        clip( +i_Direction.m_X, -i_Origin.m_X - i_Extents[0], o_T0, o_T1) &&
        clip( -i_Direction.m_X, +i_Origin.m_X - i_Extents[0], o_T0, o_T1) &&
        clip( +i_Direction.m_Y, -i_Origin.m_Y - i_Extents[1], o_T0, o_T1) &&
        clip( -i_Direction.m_Y, +i_Origin.m_Y - i_Extents[1], o_T0, o_T1) &&
        clip( +i_Direction.m_Z, -i_Origin.m_Z - i_Extents[2], o_T0, o_T1) &&
        clip( -i_Direction.m_Z, +i_Origin.m_Z - i_Extents[2], o_T0, o_T1);

    return bNotEntirelyClipped && ( o_T0 != fSaveT0 || o_T1 != fSaveT1 );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int area_sign( const maPoint2d& i_PointA, const maPoint2d& i_PointB, const maPoint2d& i_PointC )
{
	float area2;

	area2 = ( i_PointB[0] - i_PointA[0] ) * ( i_PointC[1] - i_PointA[1] ) -
			( i_PointC[0] - i_PointA[0] ) * ( i_PointB[1] - i_PointA[1] ); 

	if( area2 > 0.0f )
	{
		return 1;
	}
	else if( area2 < -0.0f )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

//----------------------------------------------------------------------------
// epsilon surrounding for near zero values 
//----------------------------------------------------------------------------
inline bool IsZero(double x)
{
	const double	c_dEQN_EPS		= 1.0e-9;
	return (x > -c_dEQN_EPS && x < c_dEQN_EPS);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline double cbrt(double x)
{
	return  (x > 0.0 ? pow((double)x, 1.0/3.0) :
			(x < 0.0 ? -pow((double)(-x), 1.0/3.0) : 0.0));
}

//----------------------------------------------------------------------------
// Roots solver from Graphics Gems I, Jochen Schwarze.
//
//  Utility functions to find cubic and quartic roots,
//  coefficients are passed like this:
//
//      c[0] + c[1]*x + c[2]*x^2 + c[3]*x^3 + c[4]*x^4 = 0
//
//  The functions return the number of non-complex roots and
//  put the values into the s array.
//----------------------------------------------------------------------------
int SolveQuadric(double c[ 3 ],  double s[ 2 ])
{
	double p, q, D;

	// normal form: x^2 + px + q = 0 
	p = c[ 1 ] / (2 * c[ 2 ]);
	q = c[ 0 ] / c[ 2 ];

	D = p * p - q;

	if (IsZero(D))
	{
		s[ 0 ] = - p;
		return 1;
	}
	else if (D < 0)
	{
		return 0;
	}
	else //if (D > 0)
	{
		double sqrt_D = sqrt(D);

		s[ 0 ] =   sqrt_D - p;
		s[ 1 ] = - sqrt_D - p;
		return 2;
	}
}

//----------------------------------------------------------------------------
// Roots solver from Graphics Gems I, Jochen Schwarze
//----------------------------------------------------------------------------
int SolveCubic(double c[ 4 ], double s[ 3 ])
{
    int     i, num;
    double  sub;
    double  A, B, C;
    double  sq_A, p, q;
    double  cb_p, D;

    // normal form: x^3 + Ax^2 + Bx + C = 0 
    A = c[ 2 ] / c[ 3 ];
    B = c[ 1 ] / c[ 3 ];
    C = c[ 0 ] / c[ 3 ];

    //  substitute x = y - A/3 to eliminate quadric term:
	// x^3 +px + q = 0 
    sq_A = A * A;
    p = 1.0/3 * (- 1.0/3 * sq_A + B);
    q = 1.0/2 * (2.0/27 * A * sq_A - 1.0/3 * A * B + C);

    // use Cardano's formula 
    cb_p = p * p * p;
    D = q * q + cb_p;

    if (IsZero(D))
    {
		if (IsZero(q)) // one triple solution 
		{
			s[ 0 ] = 0;
			num = 1;
		}
		else // one single and one double solution 
		{
			double u = cbrt(-q);
			s[ 0 ] = 2 * u;
			s[ 1 ] = - u;
			num = 2;
		}
    }
    else if (D < 0) // Casus irreducibilis: three real solutions
    {
		double phi = 1.0/3 * acos(-q / sqrt(-cb_p));
		double t = 2 * sqrt(-p);

		s[ 0 ] =   t * cos(phi);
		s[ 1 ] = - t * cos(phi + maConstants::c_dPI / 3);
		s[ 2 ] = - t * cos(phi - maConstants::c_dPI / 3);
		num = 3;
    }
    else // one real solution 
    {
		double sqrt_D = sqrt(D);
		double u = cbrt(sqrt_D - q);
		double v = - cbrt(sqrt_D + q);

		s[ 0 ] = u + v;
		num = 1;
    }

    // resubstitute
    sub = 1.0/3 * A;

    for (i = 0; i < num; ++i)
		s[ i ] -= sub;

    return num;
}

//----------------------------------------------------------------------------
// Roots solver from Graphics Gems I, Jochen Schwarze
//----------------------------------------------------------------------------
int SolveQuartic(double c[ 5 ], double s[ 4 ])
{
    double  coeffs[ 4 ];
    double  z, u, v, sub;
    double  A, B, C, D;
    double  sq_A, p, q, r;
    int     i, num;

    // normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 
    A = c[ 3 ] / c[ 4 ];
    B = c[ 2 ] / c[ 4 ];
    C = c[ 1 ] / c[ 4 ];
    D = c[ 0 ] / c[ 4 ];

    //  substitute x = y - A/4 to eliminate cubic term:
	// x^4 + px^2 + qx + r = 0 
    sq_A = A * A;
    p = - 3.0/8 * sq_A + B;
    q = 1.0/8 * sq_A * A - 1.0/2 * A * B + C;
    r = - 3.0/256*sq_A*sq_A + 1.0/16*sq_A*B - 1.0/4*A*C + D;

    if (IsZero(r))
    {
		// no absolute term: y(y^3 + py + q) = 0 
		coeffs[ 0 ] = q;
		coeffs[ 1 ] = p;
		coeffs[ 2 ] = 0;
		coeffs[ 3 ] = 1;

		num = SolveCubic(coeffs, s);

		s[ num++ ] = 0;
    }
    else
    {
		// solve the resolvent cubic ...
		coeffs[ 0 ] = 1.0/2 * r * p - 1.0/8 * q * q;
		coeffs[ 1 ] = - r;
		coeffs[ 2 ] = - 1.0/2 * p;
		coeffs[ 3 ] = 1;

		(void) SolveCubic(coeffs, s);

		// ... and take the one real solution ...
		z = s[ 0 ];

		// ... to build two quadric equations
		u = z * z - r;
		v = 2 * z - p;

		if (IsZero(u))
			u = 0;
		else if (u > 0)
			u = sqrt(u);
		else
			return 0;

		if (IsZero(v))
			v = 0;
		else if (v > 0)
			v = sqrt(v);
		else
			return 0;

		coeffs[ 0 ] = z - u;
		coeffs[ 1 ] = q < 0 ? -v : v;
		coeffs[ 2 ] = 1;

		num = SolveQuadric(coeffs, s);

		coeffs[ 0 ]= z + u;
		coeffs[ 1 ] = q < 0 ? v : -v;
		coeffs[ 2 ] = 1;

		num += SolveQuadric(coeffs, s + num);
    }

    // resubstitute 
    sub = 1.0/4 * A;

    for (i = 0; i < num; ++i)
		s[ i ] -= sub;

    return num;
}

//----------------------------------------------------------------------------
// Intersect a ray with a sphere from Graphics Gems II.
// Used by the ray-torus intersection because this version returns
// the tvals of the two solutions in order to bound the ray segment
// for later planar tests.
//----------------------------------------------------------------------------
bool intersect_sphere(	const maPoint3d &i_RayStart, 
						const maVector3d &i_RayDir, 
						const maPoint3d &i_SphereCenter, 
						float i_SphereRadius,  
						double &o_Tval_In,  
						double &o_Tval_Out)
{
	// Ray base to sphere i_SphereCenter
	double dx   = i_RayStart.m_X - i_SphereCenter.m_X;
	double dy   = i_RayStart.m_Y - i_SphereCenter.m_Y;
	double dz   = i_RayStart.m_Z - i_SphereCenter.m_Z;
	double bsq  = dx*i_RayDir.m_X + dy*i_RayDir.m_Y + dz*i_RayDir.m_Z; 
	double u    = dx*dx + dy*dy + dz*dz - i_SphereRadius*i_SphereRadius;
	double disc = bsq*bsq - u;
  
	bool hit  = (disc >= 0.0);
	if  (hit) // If ray hits sphere
	{ 				
		double root  =  ::sqrtf(disc);
	    o_Tval_In  = -bsq - root;		//    entering distance
	    o_Tval_Out = -bsq + root;		//    leaving distance	
	}
  
	return (hit);
}

}

//-----------------------------------------------------------------------------
// IntersectLineSphere() checks whether a ray from RayStart along
// RayDir intersects sphere defined by SphereCenter and SphereRadius.
// If so, parameter from 0 to 1 is returned in tval for where along
// ray intersection occurred.
//-----------------------------------------------------------------------------
bool 
geoRayIntersection::IntersectLineSphere(const maPoint3d &i_RayStart, 
										const maVector3d &i_RayDir, 
										const maPoint3d &i_SphereCenter, 
										float i_SphereRadius,  
										float &o_Tval)
{
    maVector3d diff = i_SphereCenter - i_RayStart;
    o_Tval = diff * i_RayDir;

    if ( o_Tval <= 0.0f )
    {
        o_Tval = 0.0f;
    }
	else
    {
        o_Tval /= i_RayDir.LengthSqr();
        diff -= o_Tval * i_RayDir;
    }

	float fRadiusSqr = i_SphereRadius * i_SphereRadius;

	return diff.LengthSqr() <= fRadiusSqr;
}

//-----------------------------------------------------------------------------
// IntersectLineBBox() checks whether a ray from RayStart along
// RayDir intersects axis-aligned bounding box defined by MinPoint 
// and MaxPoint. If so, parameter from 0 to 1 is returned in tval for 
// where along ray intersection occurred.
//-----------------------------------------------------------------------------
bool 
geoRayIntersection::IntersectLineBBox(	const maPoint3d &i_RayStart, 
										const maVector3d &i_RayDir, 
										const maPoint3d &i_MinPoint, 
										const maPoint3d &i_MaxPoint,  
										float &o_Tval)
{
	int i;
	float planes[3], tvals[3];
	for (i=0; i<3; i++)
	{
		// Based on direction vector, find plane closest to ray
		//
		if (i_RayDir[i] > 0) planes[i] = i_MinPoint[i];
		else planes[i] = i_MaxPoint[i];

		// Find t value for intersection between ray and plane
		//
		if (i_RayDir[i] == 0) tvals[i] = 0;
		else tvals[i] = (planes[i] - i_RayStart[i]) / i_RayDir[i];
	}

	// Find greatest "t" value, this will be point of intersection
	//
	int which_plane = 0;
	float maxt = tvals[0];
	for (i=1; i<3; i++)
	{
		if (tvals[i] > maxt)
		{
			maxt = tvals[i];
			which_plane = i;
		}
	}

	// Check to see if t value is within range of ray
	//
	if ((maxt < 0) || (maxt > 1)) return false;

	// See if this intersection point is actually in bbox
	//
	float coord;
	for (i=0; i<3; i++)
	{
		if ((maxt == 0) || (i != which_plane))
		{
			coord = i_RayStart[i] + i_RayDir[i] * maxt;
			if ((coord < i_MinPoint[i]) || (coord > i_MaxPoint[i]))
			{
				return false;
			}
		}
	}

	o_Tval = maxt;
	return true;
}

//-----------------------------------------------------------------------------
// IntersectLineThickCircle() checks whether a ray from RayStart along
// RayDir intersects a thick (hollow) circle.
// If so, parameter from 0 to 1 is returned in tval for where along
// ray intersection occurred.
//-----------------------------------------------------------------------------
bool geoRayIntersection::IntersectLineThickCircle(	const maPoint3d &i_RayStart, 
													const maVector3d &i_RayDir, 
													const maPoint3d &i_CircleCenter, 
													const maVector3d &i_CircleAxis, 
													float i_fCircleRadius,
													float i_fCircleThickness,
													float &o_Tval )
{
	float ray_t;
	if (ProjectLineToPlane( i_RayStart, 
							i_RayDir, 
							i_CircleCenter, 
							i_CircleAxis, 
							ray_t ) )
	{
		// clamp ray t val to segment to see if
		// end is close enough
		if (ray_t < 0.0f) ray_t = 0.0f;
		else if (ray_t > 1.0f) ray_t = 1.0f;

		maVector3d diff = (i_RayStart + i_RayDir * ray_t) - i_CircleCenter;

		float diffsqr = diff * diff;
		float radsqr  = (i_fCircleRadius * i_fCircleRadius);

		float inner_radsqr  = (i_fCircleRadius-i_fCircleThickness) * (i_fCircleRadius-i_fCircleThickness);
		float outer_radsqr  = (i_fCircleRadius+i_fCircleThickness) * (i_fCircleRadius+i_fCircleThickness);

		//DBG_LOG3( "inner(%6.2f) diffsqr(%6.2f) outer(%6.2f)", inner_radsqr, diffsqr, outer_radsqr );

		if ( diffsqr >= inner_radsqr && diffsqr <= outer_radsqr)
		{
			// This t value is closest to line, not actual
			// intersection with Circle; which is closer
			// to meaning intended by this function.
			//
			o_Tval = ray_t;
			return true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------
// IntersectLineThickSegment() checks whether a ray from RayStart along
// RayDir intersects cylinder defined by CylinderPoint, CylinderAxis,
// and CylinderRadius. Note: this only checks sides of cylinder, not caps.
// If so, parameter from 0 to 1 is returned in tval for where along
// ray intersection occurred.
//-----------------------------------------------------------------------------
bool 
geoRayIntersection::IntersectLineThickSegment(	const maPoint3d &i_RayStart, 
												const maVector3d &i_RayDir, 
												const maPoint3d &i_CylinderPoint, 
												const maVector3d &i_CylinderAxis, 
												float i_CylinderRadius,  
												float &o_Tval)
{
	float ray_t, cyl_t;
	if (ProjectLineToLine(i_RayStart, i_RayDir, i_CylinderPoint, 
				i_CylinderAxis, ray_t, cyl_t))
	{
		if ((cyl_t < 0) || (cyl_t > 1)) return false;

		maPoint3d pt1 = i_CylinderPoint + i_CylinderAxis * cyl_t;

		// clamp ray t val to segment to see if
		// end is close enough
		if (ray_t < 0) ray_t = 0;
		else if (ray_t > 1) ray_t = 1;

		maPoint3d pt2 = i_RayStart + i_RayDir * ray_t;

		maVector3d diff = pt2 - pt1;
		if (diff * diff < i_CylinderRadius * i_CylinderRadius)
		{
			// This t value is closest to line, not actual
			// intersection with cylinder; which is closer
			// to meaning intended by this function.
			//
			o_Tval = ray_t;
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// IntersectLineTriangle() checks whether a ray from RayStart along
// RayDir intersects triangle defined by three points. 
// If so, tval parameter is returned for where along ray 
// intersection has occurred, within range 0 to 1.
//-----------------------------------------------------------------------------
bool 
geoRayIntersection::IntersectLineTriangle(	const maPoint3d &i_RayStart, 
											const maVector3d &i_RayDir, 
											const maPoint3d& i_Ap, 
											const maPoint3d& i_Bp, 
											const maPoint3d& i_Cp,  
											float &o_Tval,
											maVector3d* o_pNormal )
{
	maPoint3d ray_end = i_RayDir + i_RayStart;

	bool bStraightDown = ((i_RayDir.GetX() == 0) && (i_RayDir.GetZ() == 0));	
	if (bStraightDown)
	{
		// If the ray is straight down, 
		// we can do trivial reject on the triangle in XZ plane
		if ((i_RayStart.GetX() > i_Ap.GetX()) && 
			(i_RayStart.GetX() > i_Bp.GetX()) && 
			(i_RayStart.GetX() > i_Cp.GetX())) 
		{
			return false;
		}

		if ((i_RayStart.GetX() < i_Ap.GetX()) && 
			(i_RayStart.GetX() < i_Bp.GetX()) && 
			(i_RayStart.GetX() < i_Cp.GetX())) 
		{
			return false;
		}

		if ((i_RayStart.GetZ() > i_Ap.GetZ()) && 
			(i_RayStart.GetZ() > i_Bp.GetZ()) && 
			(i_RayStart.GetZ() > i_Cp.GetZ())) 
		{
			return false;
		}

		if ((i_RayStart.GetZ() < i_Ap.GetZ()) && 
			(i_RayStart.GetZ() < i_Bp.GetZ()) && 
			(i_RayStart.GetZ() < i_Cp.GetZ())) 
		{
			return false;
		}
	}
	else 
	{
		float maxy = maFunctions::Highest( i_Ap.GetY(), i_Bp.GetY(), i_Cp.GetY());
		if ((i_RayStart.GetY() > maxy) && (ray_end.GetY() > maxy)) 
		{
			return false;
		}

		float miny = maFunctions::Lowest( i_Ap.GetY(), i_Bp.GetY(), i_Cp.GetY());
		if ((i_RayStart.GetY() < miny) && (ray_end.GetY() < miny))
		{
			return false;
		}

		float maxx = maFunctions::Highest( i_Ap.GetX(), i_Bp.GetX(), i_Cp.GetX());
		if ((i_RayStart.GetX() > maxx) && (ray_end.GetX() > maxx)) 
		{
			return false;
		}

		float minx = maFunctions::Lowest( i_Ap.GetX(), i_Bp.GetX(), i_Cp.GetX());
		if ((i_RayStart.GetX() < minx) && (ray_end.GetX() < minx))
		{
			return false;
		}

		float maxz = maFunctions::Highest( i_Ap.GetZ(), i_Bp.GetZ(), i_Cp.GetZ());
		if ((i_RayStart.GetZ() > maxz) && (ray_end.GetZ() > maxz))
		{
			return false;
		}

		float minz = maFunctions::Lowest( i_Ap.GetZ(), i_Bp.GetZ(), i_Cp.GetZ());
		if ((i_RayStart.GetZ() < minz) && (ray_end.GetZ() < minz))
		{
			return false;
		}
	}

	// calc plane normal, it could be precalculated
	maVector3d fnormal = (i_Bp - i_Ap) / (i_Cp - i_Ap);
	
	// Actually, we can delay the Normalize() until after
	// the back face cull check

	// Check for if how the polygon's plane is intersected by the
	// ray segment.  The start point needs to be
	// on the positive side, and the end point needs to be on the
	// negative side for there to be an intersection.
	//
	float startval = fnormal * ( i_RayStart - i_Ap );
	if (startval < 0.0f) return false;
	float endval = fnormal * ( ray_end - i_Ap );
	if (endval > 0.0f) return false;

	// Now normalize the face normal
	//
	float nlen = fnormal.Length();
	if (nlen == 0) return false;
	fnormal /= nlen;

	// get intersect point
	// we know one point on plane   
	// startpoint, lineend, planepoint, planenormal
	float tval;
	if (!ProjectLineToPlane( i_RayStart, i_RayDir, i_Ap, fnormal, tval )) 
	{
		return false;
	}

	// Check to see if tval is within bounds
	//
	if ((tval < 0.0f) || (tval > 1.0f)) 
	{
		return false;
	}

	// Is there some way to check tvalue
	// against already found intersections?
	//

	// Now compute the point on the plane
	//
	maPoint3d pt = i_RayStart + i_RayDir * tval;

	if ( IsInsideTriangle( i_Ap, i_Bp, i_Cp, pt, fnormal ) )
	{
		o_Tval = tval;
		if ( o_pNormal )
		{
			*o_pNormal = fnormal;
		}
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// IntersectLinePolygon() checks whether a ray from RayStart along
// RayDir intersects polygon defined by vertices.  The
// vertices are assumed to line within a single plane with 
// given face normal (assumed to be normalized). 
// If intersection, tval parameter is returned for where along ray 
// intersection has occurred, within range 0 to 1.
//-----------------------------------------------------------------------------
bool 
geoRayIntersection::IntersectLinePolygon(	const maPoint3d &i_RayStart, 
											const maVector3d &i_RayDir, 
											const maPoint3d* i_Vertices, 
											int	i_NumVertices, 
											const maVector3d &i_FaceNormal, 
											float &o_Tval,
											bool  i_bTwoSided)
{
	if (i_NumVertices < 3) return false; // gotta have a triangle at least

	maPoint3d ray_end = i_RayDir + i_RayStart;
	maPoint3d first_point = i_Vertices[0];

	// Check for if how the polygon's plane is intersected by the
	// ray segment.  
	if (i_bTwoSided)
	{
		// If two sided, ray start and end need to be on opposite 
		// sides of the plane.
		// 
		float startval = i_FaceNormal * ( i_RayStart - first_point );
		float endval = i_FaceNormal * ( ray_end - first_point );
		// startval*endval is positive if both positive or both negative
		if (startval * endval > 0.0f) return false;  

	}
	else
	{
		// If not two sided, the start point needs to be
		// on the positive side, and the end point needs to be on the
		// negative side for there to be an intersection.
		//
		float startval = i_FaceNormal * ( i_RayStart - first_point );
		if (startval < 0.0f) return false;
		float endval = i_FaceNormal * ( ray_end - first_point );
		if (endval > 0.0f) return false;
	}


	// get intersection betweeen ray and plane
	float tval;
	if (!ProjectLineToPlane( i_RayStart, i_RayDir, first_point, i_FaceNormal, tval )) 
	{
		return false;
	}

	// Check to see if tval is within bounds
	//
	if ((tval < 0.0f) || (tval > 1.0f)) 
	{
		return false;
	}

	// Is there some way to check tvalue
	// against already found intersections?
	//

	// Now compute the point on the plane
	//
	maPoint3d pt = i_RayStart + i_RayDir * tval;

	// Now fan polygon into triangles
	//
	for (int v=2; v<i_NumVertices; v++)
	{
		if ( IsInsideTriangle( i_Vertices[0], i_Vertices[v-1], i_Vertices[v], 
								pt, i_FaceNormal ) )
		{
			// Could use alpha,beta to get interpolated normals or
			// texture coordinates here
			o_Tval = tval;
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// IntersectLineLine() determines if ray from RayStart along
// RayDir and line defined by LinePoint and LineDir intersect.
// The two tvals are returned in RayTval and LineTVal.
// Returns false if parallel lines or if they do not intersect.
//----------------------------------	-------------------------------------------
bool geoRayIntersection::IntersectLineLine(	const maPoint3d &i_RayStart, 
						const maVector3d &i_RayDir, 
						const maPoint3d &i_LinePoint, 
						const maVector3d &i_LineDir,  
						float &o_RayTval, 
						float &o_LineTval)
{
	if ( !ProjectLineToLine( i_RayStart, i_RayDir, i_LinePoint, i_LineDir, o_RayTval,o_LineTval) )
	{
		return false;
	}

	// if either of the tvals are not in [0,1] then return false
	if ( o_RayTval < 0.0f || o_RayTval > 1.0f )
	{
		return false;
	}
	if ( o_LineTval < 0.0f || o_LineTval > 1.0f )
	{
		return false;
	}

	maPoint3d ray_point = i_RayStart + i_RayDir * o_RayTval;
	maPoint3d line_point = i_LinePoint + i_LineDir * o_LineTval;

	maVector3d diff = ray_point - line_point;
	float len_sqr = diff.LengthSqr();

	// if the points are really close together then there is an intersection
	if ( len_sqr < maConstants::c_fEpsilon )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// ProjectLineToPlane() checks whether a line segment from RayStart along
// SegDir intersects plane defined by PlanePoint and PlaneNormal.
// If so, tval parameter is returned for where along the line segment 
// intersection has occurred; which may be outside of range 0 to 1.
//-----------------------------------------------------------------------------
bool 
geoRayIntersection::ProjectLineToPlane(	const maPoint3d &i_SegStart, 
										const maVector3d &i_SegDir, 
										const maPoint3d &i_PlanePoint, 
										const maVector3d &i_PlaneNormal,  
										float &o_Tval)
{
	maVector3d Vec1 = i_PlanePoint - i_SegStart;

	float StartDistFromPlane = Vec1 * i_PlaneNormal;
	if (StartDistFromPlane == 0)	
	{
		// point is in plane
		o_Tval = 0.0f;
		return true;				
	}

	float ProjectedLineLength = i_SegDir * i_PlaneNormal;
	if (ProjectedLineLength == 0.0f)
	{
		return false;
	}

	o_Tval = StartDistFromPlane / ProjectedLineLength;
	return true;
}

//-----------------------------------------------------------------------------
// ProjectLineToLine() finds closest points on ray from RayStart along
// RayDir and line defined by LinePoint and LineDir.
// The two tvals are returned in RayTval and LineTVal.
// Returns false if parallel lines.
//-----------------------------------------------------------------------------
bool 
geoRayIntersection::ProjectLineToLine(	const maPoint3d &i_RayStart, 
										const maVector3d &i_RayDir, 
										const maPoint3d &i_LinePoint, 
										const maVector3d &i_LineDir,  
										float &o_RayTval, 
										float &o_LineTval)
{
	// Get vector perpindicular to both lines
	maVector3d cross = i_RayDir / i_LineDir;
	
	// Check for cross equals zero within epsilon
	float clen = cross.Length();
	if (clen < maConstants::c_fEpsilon)
	{
		// parallel lines
		o_RayTval = o_LineTval = 0;
		return false;
	}
	cross /= clen;	// Normalize

    // form plane containing ray, parallel to cross
	maVector3d anorm =  cross / i_RayDir;
	anorm.Normalize();

    // form plane containing line, parallel to cdir
	maVector3d bnorm =  cross / i_LineDir;
	bnorm.Normalize();

    // intersect ray with line's plane (bnorm)
	ProjectLineToPlane(i_RayStart, i_RayDir, i_LinePoint, bnorm, o_RayTval);

    // intersect line with ray's plane (anorm)
	ProjectLineToPlane(i_LinePoint, i_LineDir, i_RayStart, anorm, o_LineTval);

	return true;
}

//-----------------------------------------------------------------------------
// IsPointInsideTriangle() checks whether a point on the same plane 
// formed by 3 vertices is inside triangle.  Returns the interpolation 
// values in alpha and beta.
//-----------------------------------------------------------------------------
bool 
geoRayIntersection::IsInsideTriangle(	const maPoint3d& i_Ap, 
										const maPoint3d& i_Bp, 
										const maPoint3d& i_Cp, 
										const maPoint3d& i_Point,
										const maVector3d& i_Normal )
{
	// Find the dominant plane
	int nDomPlane;
	if ( fabs( i_Normal.m_X ) > fabs( i_Normal.m_Y ))
	{
		nDomPlane = ( fabs( i_Normal.m_X ) > fabs( i_Normal.m_Z ) ) ? 0 : 2;
	}
	else
	{
		nDomPlane = ( fabs( i_Normal.m_Y ) > fabs( i_Normal.m_Z ) ) ? 1 : 2;
	}

	maPoint3d triangle[3] = { i_Ap, i_Bp, i_Cp };
	maPoint2d proj_point;
	maPoint2d proj_tri[3];

	// Project out the dominant coordinate, the point, and the triangle face
	int i, j = 0, k;
	for( i = 0; i < 3; ++i )
	{
		if( i != nDomPlane )
		{
			proj_point[ j ] = i_Point[ i ];

			for( k = 0; k < 3; ++ k )
			{
				proj_tri[ k ][ j ] = triangle[ k ][ i ];
			}

			++j;
		}
	}

	int area0 = area_sign( proj_point, proj_tri[0], proj_tri[1] );
	int area1 = area_sign( proj_point, proj_tri[1], proj_tri[2] );
	int area2 = area_sign( proj_point, proj_tri[2], proj_tri[0] );

	// On edge of traingle
	if( ( area0 == 0 && area1 > 0 && area2 > 0 ) ||
		( area1 == 0 && area0 > 0 && area2 > 0 ) ||
		( area2 == 0 && area0 > 0 && area1 > 0 ) )
	{
		return true;
	}

	// On edge of traingle
	if( ( area0 == 0 && area1 < 0 && area2 < 0 ) ||
		( area1 == 0 && area0 < 0 && area2 < 0 ) ||
		( area2 == 0 && area0 < 0 && area1 < 0 ) )
	{
		return true;
	}
	
	// Inside of triangle
	if( ( area0 > 0 && area1 > 0 && area2 > 0 ) ||
		( area0 < 0 && area1 < 0 && area2 < 0 ) )
	{
		return true;
	}

//	DBG_ASSERT( !( area0 == 0 && area1 == 0 && area2 == 0 ), "Error in triangle" );

	// On vertex of triangle
	if( ( area0 == 0 && area1 == 0 ) ||
		( area0 == 0 && area2 == 0 ) ||
		( area1 == 0 && area2 == 0 ) )
	{
		return true;
	}

	// Not in triangle
	return false;

}

//----------------------------------------------------------------------------
//	VsAxisBox will return the intersections between the given axis box and the
//	line segment.
//----------------------------------------------------------------------------
bool geoRayIntersection::SegmentVsAxisBox(	const maAxisBox& i_Box, 
											const maPoint3d& i_P0, 
											const maPoint3d& i_P1,
											int& o_Quantity,
											maVector3d o_Points[2])
{
	// convert segment to box coordinates
	maVector3d box_extent(	i_Box.GetDiffX() * 0.5f,
							i_Box.GetDiffY() * 0.5f,
							i_Box.GetDiffZ() * 0.5f );

	maVector3d seg_direction = i_P1 - i_P0;
	maVector3d seg_offset = i_P0 - i_Box.GetCenter();

	float fT0 = 0.0f, fT1 = 1.0f;
	bool bIntersects = find_intersection(	seg_offset,
											seg_direction,
											box_extent,
											fT0,
											fT1);
	if ( bIntersects )
	{
		if ( fT0 > 0.0f )
		{
			if ( fT1 < 1.0f )
			{
				o_Quantity = 2;
				o_Points[0] = i_P0 + fT0 * seg_direction;
				o_Points[1] = i_P0 + fT1 * seg_direction;
			}
			else
			{
				o_Quantity = 1;
				o_Points[0] = i_P0 + fT0 * seg_direction;
			}
		}
		else  // fT0 == 0
		{
			if ( fT1 < 1.0f )
			{
				o_Quantity = 1;
				o_Points[0] = i_P0 + fT1 * seg_direction;
			}
			else  // fT1 == 1
			{
				// segment entirely in box
				o_Quantity = 0;
			}
		}
	}
	else
		o_Quantity = 0;

	return bIntersects;
}

//----------------------------------------------------------------------------
//	ClipToAxisBoxInterior will return two points that represent the
//	portion of the given segment that is inside the given axis box.
//	It returns false if nothing is inside.
//----------------------------------------------------------------------------
bool geoRayIntersection::ClipToAxisBoxInterior(	const maAxisBox& i_Box, 
												maPoint3d& io_P0, 
												maPoint3d& io_P1)
{
	bool p0_inside = i_Box.ContainsPoint(io_P0);
	bool p1_inside = i_Box.ContainsPoint(io_P1);

	if( p0_inside && p1_inside )
	{
		//	all inside, so we're done
		return true;
	}

	int num_intersect;
	maVector3d intersections[2];
	geoRayIntersection::SegmentVsAxisBox(	i_Box,
											io_P0,
											io_P1,
											num_intersect,
											intersections);
	if( num_intersect == 0 )
	{
		//	hmm...we may be being fooled by numerical problems at edges
		//	test center point to make sure
		if( i_Box.ContainsPoint( (io_P0 + io_P1) * 0.5f) )
			return true;	//	segment is actually all inside
		else
		{
			//	might have infinitely thin box
			if( i_Box.GetMinY() == i_Box.GetMaxY() )
			{
				//	find intersection with plane
				float t;
				maPoint3d start = io_P0;
				maVector3d dir = io_P1 - io_P0;
				
				if( ProjectLineToPlane(	start,
										dir,
										i_Box.GetCenter(),
										maVector3d(0.0f, 1.0f, 0.0f),
										t) )
				{
					maPoint3d intersection = start + dir * t;
					if( (intersection.m_X < i_Box.GetMaxX()) &&
						(intersection.m_X > i_Box.GetMinX()) &&
						(intersection.m_Z < i_Box.GetMaxZ()) &&
						(intersection.m_Z > i_Box.GetMinZ()) )
					{
						io_P0 = intersection;
						io_P1 = intersection;
						return true;
					}
				}
			}

			return false;
		}
	}
	if( num_intersect == 2 )
	{
		//	try to sort, so we don't reverse the order somehow
		maVector3d delta = io_P1 - io_P0;
		float t1 = (intersections[0] - io_P0) * delta;
		float t2 = (intersections[1] - io_P0) * delta;

		if( t2 > t1 )
		{
			io_P0 = intersections[0];
			io_P1 = intersections[1];
		}
		else
		{
			io_P0 = intersections[1];
			io_P1 = intersections[0];
		}
	}
	else
	{
		//	one intersects
		//	which end got clipped?
		if( (io_P0 - intersections[0]).LengthSqr() < 0.000001f )
			return true;		//	none, really - numerical slipping
		
		if( (io_P1 - intersections[0]).LengthSqr() < 0.000001f )
			return true;		//	none, really - numerical slipping

		if( p0_inside )
			io_P1 = intersections[0];
		else
			io_P0 = intersections[0];
	}

	return true;
}

//-----------------------------------------------------------------------------
// PointToLineDistance() determines the distance between the line given by two
// points and a point.  The tval of the point on the line the point is closest 
// to is returned in LineTval
//-----------------------------------------------------------------------------
float geoRayIntersection::PointToLineDistance(	const maPoint3d& i_Point, 
							const maPoint3d& i_LineA, 
							const maPoint3d& i_LineB, 
							float &o_LineTval )
{
	maVector3d line_seg = i_LineB - i_LineA;
	float line_seg_length = line_seg.Length();

	maVector3d line_to_point = i_LineA - i_Point;

	maVector3d cross_prod = line_seg.Cross( line_to_point );

	float distance = cross_prod.Length() / line_seg_length;

	o_LineTval = ( line_to_point * line_seg ) / ( line_seg_length * line_seg_length );

	return distance;
}


//-----------------------------------------------------------------------------
// IntersectLineTorus() checks whether a ray from RayStart along
// RayDir intersects a torus.
// If so, parameter from 0 to 1 is returned in tval for where along
// ray intersection occurred.
// Implementation from GraphicsGems II example code.
//-----------------------------------------------------------------------------
bool geoRayIntersection::IntersectLineTorus(	const maPoint3d &i_RayStart, 
												const maVector3d &i_RayDir, 
												const maPoint3d &i_TorusCenter, 
												const maVector3d &i_TorusAxis, 
												float i_fMajorRadius,
												float i_fMinorPlanarRadius,
												float i_fMinorNormalRadius,
												float &o_Tval )
{
	// Algorithm needs unit length ray direction
	float ray_len = i_RayDir.Length();
	if (ray_len == 0) return false;
	maVector3d unit_ray = i_RayDir / ray_len;

	//	Compute the intersection of the ray with a bounding sphere,
	// looking for trivial rejection
	float radius_sphere = i_fMajorRadius + maFunctions::Highest(i_fMinorPlanarRadius,i_fMinorNormalRadius);
	double tval_in = -1.0f, tval_out = -1.0f;
	// If ray misses bounding sphere, return false
	//if (!IntersectLineSphere( i_RayStart, i_RayDir, i_TorusCenter, radius_sphere, tval))
	//	return false;
	// switched to version that returns tval_in and tval_out used below
	if (!intersect_sphere( i_RayStart, unit_ray, i_TorusCenter, radius_sphere, tval_in, tval_out))
		return false;

	// Transform the intersection ray such that the torus is oriented at the
	// XZ plane and then do intersection with this ray and the standard torus.
	// This could be done by passing in the matrix, but for now construct
	// a rotation matrix from i_TorusAxis and translate by -i_TorusCenter.
	maMatrix4x4 shift;
	shift.MakeTranslate(-i_TorusCenter);
	maRotation orient;
	orient.SetValue(i_TorusAxis, maVector3d(0,1,0));
	maMatrix4x4	xform = shift * orient.GetMatrix();

	maPoint3d ray_base = i_RayStart;
	xform.Transform(ray_base);
	maVector3d ray_dir = unit_ray;
	xform.TransformDir(ray_dir);

	//	Bound the torus by two parallel planes i_fMinorNormalRadius from the x-z plane.
	double yin  = ray_base.m_Y + tval_in * ray_dir.m_Y;
	double yout = ray_base.m_Y + tval_out * ray_dir.m_Y;
	if (yin >  i_fMinorNormalRadius && yout >  i_fMinorNormalRadius)
		return false;
	if (yin < -i_fMinorNormalRadius && yout < -i_fMinorNormalRadius)
		return false;

	//bga - Testing intersection code. If it got this far, then the ray
	// has intersected a slice of a sphere and the rest of the code tries to indentify
	// the actual ray-torus intersection.
	//DBG_TRACE("Ray-Torus, raypos: " << i_RayStart << " raydir: " << i_RayDir);
	//DBG_TRACE(" torus: " << i_TorusCenter << " axis: " << i_TorusAxis);
	//DBG_TRACE(" radii: " << i_fMajorRadius << " minor: " << i_fMinorPlanarRadius << "  " << i_fMinorNormalRadius);


	//	Compute constants related to the torus.	
	double rho = i_fMinorPlanarRadius*i_fMinorPlanarRadius / (i_fMinorNormalRadius*i_fMinorNormalRadius);
	double a0  = 4. * i_fMajorRadius*i_fMajorRadius;
	double b0  = i_fMajorRadius*i_fMajorRadius - i_fMinorPlanarRadius*i_fMinorPlanarRadius;

	//	Compute ray dependent terms.
	double f = 1. - ray_dir.m_Y*ray_dir.m_Y;
	double l = 2. * (ray_base.m_X*ray_dir.m_X + ray_base.m_Z*ray_dir.m_Z);
	double t = ray_base.m_X*ray_base.m_X + ray_base.m_Z*ray_base.m_Z;
	double g = f + rho * ray_dir.m_Y*ray_dir.m_Y;
	double q = a0 / (g*g);
	double m = (l + 2.*rho*ray_dir.m_Y*ray_base.m_Y) / g;
	double u = (t +    rho*ray_base.m_Y*ray_base.m_Y + b0) / g;

	//	Compute the coefficients of the quartic.
	double	C[5];
	C[4] = 1.0;
	C[3] = 2. * m;
	C[2] = m*m + 2.*u - q*f;
	C[1] = 2.*m*u - q*l;
	C[0] = u*u - q*t;
	
	//	Use quartic root solver found in "Graphics Gems" by Jochen Schwarze.
	double	rhits[4];
	int nhits = SolveQuartic(C,rhits);

	//	SolveQuartic returns root pairs in reversed order.	
	m = rhits[0]; u = rhits[1]; rhits[0] = u; rhits[1] = m;
	m = rhits[2]; u = rhits[3]; rhits[2] = u; rhits[3] = m;
	
	//DBG_TRACE("Num hits: " << nhits);

	// Need to pick the lowest root greater than 0
	for (int h=0; h<nhits; ++h)
	{
		if (rhits[h] >= 0)
		{
			o_Tval = rhits[h] / ray_len;
			//DBG_TRACE("T-val: " << o_Tval);
			return true;
		}
	}
	
	return false;
}
