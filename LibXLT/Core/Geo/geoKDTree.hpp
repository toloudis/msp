/*****************************************************************************
**  geoKDTree.hpp
**
**		A geoKDTree is a kd tree (k-dimensional tree) of triangles.  A
**	kd tree is a spatial sorting structure similar to a BSP tree except that
**	it is constrained to use axis-aligned split planes.
**		I apologize for this file (mostly the cpp) not being fully 
**	Terawatt-ized yet.
**
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef GEO_KDTREE_HPP
#error geoKDTree.hpp multiply included
#endif
#define GEO_KDTREE_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
struct geoKDTreeImp;
class maAxisBox;
class geoPickedPoint;


//============================================================================
//============================================================================
class geoKDTree
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		geoKDTree();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~geoKDTree();

		//--------------------------------------------------------------------
		// Clear prepares set for receiving triangles
		//--------------------------------------------------------------------
		void	Clear();

		//--------------------------------------------------------------------
		// Add Triangles to set
		//--------------------------------------------------------------------
		void	AddTriangles(	const maPoint3d*			i_Vertices,
								int							i_NumVertices,
								const envType::UInt32*		i_Indices,
								int							i_NumIndices);

		//--------------------------------------------------------------------
		//	SetMinBox allows the user to set the size of the largest
		//	allowable subsection of the tree.
		//--------------------------------------------------------------------
		void	SetMinBox(float i_X, float i_Y, float i_Z);

		//--------------------------------------------------------------------
		//	SetNumDesiredPolysInLeaves allows the user to set the 
		//	desired number of polygons per leaf.
		//--------------------------------------------------------------------
		void SetNumDesiredPolysInLeaves(int i_Num);

		//--------------------------------------------------------------------
		//	GetWorldBox returns a bounding box in world space for tree.
		//--------------------------------------------------------------------
		const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		// Create finishes structure, no more triangles may be added.
		//--------------------------------------------------------------------
		void	Create();

		//--------------------------------------------------------------------
		//	ComputeRayIntersection, returns true if ray from ray start
		//  to ray end intersects triangles.
		//  Make sure Create() has been called first
		//--------------------------------------------------------------------
		bool	ComputeRayIntersection(	const maPoint3d &i_RayStart, 
										const maVector3d &i_RayEnd, 
										geoPickedPoint &o_PickPt) const;

		//--------------------------------------------------------------------
		//	IntersectBBox, returns true if axis-aligned bbox intersects
		//  with triangles.
		//  Make sure Create() has been called first
		//--------------------------------------------------------------------
		bool	IntersectBBox(const maPoint3d &i_MinPt, const maPoint3d &i_MaxPt) const;

				
		//--------------------------------------------------------------------
		//	GetTrianglesInBBox, returns triangles that intersect 
		//  axis-aligned bbox into vector. This checks for 
		//  overlap between triangle's bbox and given bbox, not
		//  exact collision between triangle and bbox.
		//  Triangles are returned as 3 point and 1 normal vector.
		//  Make sure Create() has been called first
		//--------------------------------------------------------------------
		void	GetTrianglesInBBox(const maPoint3d &i_MinPt, 
								const maPoint3d &i_MaxPt,
								std::vector<maPoint3d> &o_Tris) const;

		//--------------------------------------------------------------------
		// Get groups of triangles, spatially partitioned. Each index
		//	is the index of the triangle in the original list.
		//--------------------------------------------------------------------
		void GetGroups(std::vector< std::vector<int> > &o_TriGroups);
	private:

		geoKDTreeImp*		m_pImp;
};
