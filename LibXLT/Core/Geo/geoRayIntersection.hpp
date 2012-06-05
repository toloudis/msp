/*****************************************************************************
**  geoRayIntersection.hpp
**
**		Support for 3D geometry ray intersection.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GEO_RAYINTERSECTION_HPP
#error geoRayIntersection.hpp multiply included
#endif
#define GEO_RAYINTERSECTION_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif


//============================================================================
//============================================================================
namespace geoRayIntersection 
{
	//-----------------------------------------------------------------------------
	// IntersectLineSphere() checks whether a ray from RayStart along
	// RayDir intersects sphere defined by SphereCenter and SphereRadius.
	// If so, parameter from 0 to 1 is returned in tval for where along
	// ray intersection occurred.
	//-----------------------------------------------------------------------------
	bool IntersectLineSphere(	const maPoint3d &i_RayStart, 
								const maVector3d &i_RayDir, 
								const maPoint3d &i_SphereCenter, 
								float i_SphereRadius,  
								float &o_Tval);

	//-----------------------------------------------------------------------------
	// IntersectLineBBox() checks whether a ray from RayStart along
	// RayDir intersects axis-aligned bounding box defined by MinPoint 
	// and MaxPoint. If so, parameter from 0 to 1 is returned in tval for 
	// where along ray intersection occurred.
	//-----------------------------------------------------------------------------
	bool IntersectLineBBox(	const maPoint3d &i_RayStart, 
							const maVector3d &i_RayDir, 
							const maPoint3d &i_MinPoint, 
							const maPoint3d &i_MaxPoint,  
							float &o_Tval);

	//-----------------------------------------------------------------------------
	// IntersectLineThickCircle() checks whether a ray from RayStart along
	// RayDir intersects a thick (hollow) circle.
	// If so, parameter from 0 to 1 is returned in tval for where along
	// ray intersection occurred.  By passing in the Radius as the thickness,
	// the whole circle will be checked.
	//-----------------------------------------------------------------------------
	bool IntersectLineThickCircle(	const maPoint3d &i_RayStart, 
									const maVector3d &i_RayDir, 
									const maPoint3d &i_CircleCenter, 
									const maVector3d &i_CircleAxis, 
									float i_fCircleRadius,  
									float i_fCircleThickness,
									float &o_Tval);

	//-----------------------------------------------------------------------------
	// IntersectLineThickSegment() checks whether a ray from RayStart along
	// RayDir intersects cylinder defined by CylinderPoint, CylinderAxis,
	// and CylinderRadius. Note: this only checks sides of cylinder, not caps.
	// If so, parameter from 0 to 1 is returned in tval for where along
	// ray intersection occurred.
	//-----------------------------------------------------------------------------
	bool IntersectLineThickSegment(	const maPoint3d &i_RayStart, 
									const maVector3d &i_RayDir, 
									const maPoint3d &i_CylinderPoint, 
									const maVector3d &i_CylinderAxis, 
									float i_CylinderRadius,  
									float &o_Tval);

	//-----------------------------------------------------------------------------
	// IntersectLineTriangle() checks whether a ray from RayStart along
	// RayDir intersects triangle defined by three points. 
	// If so, tval parameter is returned for where along ray 
	// intersection has occurred, within range 0 to 1.
	//-----------------------------------------------------------------------------
	bool IntersectLineTriangle(	const maPoint3d &i_RayStart, 
								const maVector3d &i_RayDir, 
								const maPoint3d& i_Ap, 
								const maPoint3d& i_Bp, 
								const maPoint3d& i_Cp,  
								float &o_Tval,
								maVector3d* o_pNormal=NULL );

	//-----------------------------------------------------------------------------
	// IntersectLinePolygon() checks whether a ray from RayStart along
	// RayDir intersects polygon defined by vertices.  The
	// vertices are assumed to line within a single plane with 
	// given face normal (assumed to be normalized). 
	// If intersection, tval parameter is returned for where along ray 
	// intersection has occurred, within range 0 to 1.
	//-----------------------------------------------------------------------------
	bool IntersectLinePolygon(	const maPoint3d &i_RayStart, 
								const maVector3d &i_RayDir, 
								const maPoint3d* i_Vertices, 
								int	i_NumVertices, 
								const maVector3d &i_FaceNormal, 
								float &o_Tval,
								bool  i_bTwoSided = false);

	//-----------------------------------------------------------------------------
	// IntersectLineLine() determines if ray from RayStart along
	// RayDir and line defined by LinePoint and LineDir intersect.
	// The two tvals are returned in RayTval and LineTVal.
	// Returns false if parallel lines or if they do not intersect.
	//----------------------------------	-------------------------------------------
	bool IntersectLineLine(	const maPoint3d &i_RayStart, 
							const maVector3d &i_RayDir, 
							const maPoint3d &i_LinePoint, 
							const maVector3d &i_LineDir,  
							float &o_RayTval, 
							float &o_LineTval);

	//-----------------------------------------------------------------------------
	// ProjectLineToPlane() checks whether a line segment from RayStart along
	// SegDir intersects plane defined by PlanePoint and PlaneNormal.
	// If so, tval parameter is returned for where along the line segment 
	// intersection has occurred; which may be outside of range 0 to 1.
	//-----------------------------------------------------------------------------
	bool ProjectLineToPlane(const maPoint3d &i_SegStart, 
							const maVector3d &i_SegDir, 
							const maPoint3d &i_PlanePoint, 
							const maVector3d &i_PlaneNormal,  
							float &o_Tval);

	//-----------------------------------------------------------------------------
	// ProjectLineToLine() finds closest points on ray from RayStart along
	// RayDir and line defined by LinePoint and LineDir.
	// The two tvals are returned in RayTval and LineTVal.
	// Returns false if parallel lines.
	//-----------------------------------------------------------------------------
	bool ProjectLineToLine(	const maPoint3d &i_RayStart, 
							const maVector3d &i_RayDir, 
							const maPoint3d &i_LinePoint, 
							const maVector3d &i_LineDir,  
							float &o_RayTval, 
							float &o_LineTval);

	//-----------------------------------------------------------------------------
	// IsPointInsideTriangle() checks whether a point on the same plane 
	// formed by 3 vertices is inside triangle.  Returns the interpolation 
	// values in alpha and beta.
	//-----------------------------------------------------------------------------
	bool IsInsideTriangle(	const maPoint3d& i_Ap, 
							const maPoint3d& i_Bp, 
							const maPoint3d& i_Cp, 
							const maPoint3d& i_Point,
							const maVector3d& i_Normal );

	//============================================================================
	//	VsAxisBox will return the intersections between the given axis box and the
	//	line segment.
	//============================================================================
	bool SegmentVsAxisBox(	const maAxisBox& i_Box, 
							const maPoint3d& i_P0, 
							const maPoint3d& i_P1,
							int& o_Quantity,
							maVector3d o_Points[2]);
	
	//============================================================================
	//	ClipToAxisBoxInterior will return two points that represent the
	//	portion of the given segment that is inside the given axis box.
	//	It returns false if nothing is inside.
	//============================================================================
	bool ClipToAxisBoxInterior(	const maAxisBox& i_Box, 
								maPoint3d& io_P0, 
								maPoint3d& io_P1);

	//-----------------------------------------------------------------------------
	// PointToLineDistance() determines the distance between the line given by two
	// points and a point.  The tval of the point on the line the point is closest 
	// to is returned in LineTval
	//-----------------------------------------------------------------------------
	float PointToLineDistance(	const maPoint3d& i_Point, 
								const maPoint3d& i_LineA, 
								const maPoint3d& i_LineB, 
								float &o_LineTval );
	//-----------------------------------------------------------------------------
	// IntersectLineTorus() checks whether a ray from RayStart along
	// RayDir intersects a torus.
	// If so, parameter from 0 to 1 is returned in tval for where along
	// ray intersection occurred.
	//-----------------------------------------------------------------------------
	bool IntersectLineTorus(	const maPoint3d &i_RayStart, 
								const maVector3d &i_RayDir, 
								const maPoint3d &i_TorusCenter, 
								const maVector3d &i_TorusAxis, 
								float i_fMajorRadius,
								float i_fMinorPlanarRadius,
								float i_fMinorNormalRadius,
								float &o_Tval );
};
