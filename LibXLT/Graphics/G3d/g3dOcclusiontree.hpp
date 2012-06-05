/****************************************************************************\
**	g3dOcclusionTree.hpp
**
**	\file OcclusionTree.h
**	\author Jared Hoberock
**	\brief Defines the interface to a hierarchy for triangles.
**
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_OCCLUSIONTREE_HPP
#error g3dOcclusionTree.hpp multiply included
#endif
#define G3D_OCCLUSIONTREE_HPP

#ifndef G3D_INDEXPTR_HPP
#include "Graphics/g3d/g3dIndexPtr.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>


//============================================================================
// supporting structures
//============================================================================
typedef maVector3d float3;


//============================================================================
// triangle face is an array of 3 uint indices:
//============================================================================
class uint3
{
public:
	uint3(){u[0]=u[1]=u[2]=0;}
	unsigned int u[3];

	const unsigned int& operator [] ( const int i_Index ) const         // ALLOWS VECTOR ACCESS AS AN ARRAY.
	{
		DBG_ASSERT((i_Index<3)&&(i_Index>=0), "Index out of range - " << i_Index);
		return u[i_Index];
	}
	unsigned int& operator [] ( const int i_Index ) // ALLOWS VECTOR ACCESS AS AN ARRAY.
	{
		DBG_ASSERT((i_Index<3)&&(i_Index>=0), "Index out of range - " << i_Index);
		return u[i_Index];
	}
};

//============================================================================
//============================================================================
class g3dOcclusionTree
{
public:
	typedef float3 Point;
	typedef float3 Normal;

	struct Disc
	{
		Point m_Centroid;
		Normal m_Normal;
		float m_Area;
	}; // end Disc

	/*! A Triangle is a triplet of vertex indices.
	 */
	typedef uint3 Triangle;
	typedef std::vector<Triangle> Triangles;

	typedef unsigned int NodeIndex;
	static const NodeIndex NULL_NODE = UINT_MAX;

	struct Node
	{
		Disc m_Disc;
		float m_Occlusion;
		int m_Height;
		NodeIndex m_Parent;
		NodeIndex m_LeftChild;
		NodeIndex m_RightChild;
		NodeIndex m_NextNode;

		// null constructor does nothing
		Node(void);
	}; // end Node

	~g3dOcclusionTree();
	float computeOcclusion(const float3 &p,
						   const float3 &n,
						   const float epsilon = 4.0f) const;

	float computeOcclusionUseTriangles(const float3 &p,
									   const float3 &n,
									   const float epsilon = 4.0f) const;

	void computeOcclusionPasses(const unsigned int numPasses,
								const float epsilon = 4.0f);

	float computeFormFactor(const float3 &p, const float3 &n,
							const Triangle &tri, const float triArea, const bool verbose) const;

	static float computeFormFactor(const float3 &p, const float3 &n,
								   const float3 &q0, const float3 &q1,
								   const float3 &q2, const float3 &q3, const bool verbose);

	static float computeFormFactor(const float3 &p, const float3 &n,
								   const float3 &v0, const float3 &v1, const float3 &v2);

	void build(const maPoint3d* positions, int i_nVertices,
		const g3dIndexPtr i_Indices, int i_nIndices, maPoint3d& pmin, maPoint3d& pmax);

	// recursive version of build().
	NodeIndex build(const NodeIndex parent,
					std::vector<unsigned int>::iterator &begin,
					std::vector<unsigned int>::iterator &end,
					const Triangles& triangles, int lvl);

	/*! This method computes the index of the next node in a
	 *  depth first traversal of this tree, from node i.
	 *  \param i The Node of interest.
	 *  \return The index of the next Node from i, if it exists;
	 *          UINT_MAX, otherwise.
	 */
	NodeIndex computeNextIndex(const NodeIndex i) const;

	/*! This method computes the index of a Node's brother to the right,
	 *  if it exists.
	 *  \param i The index of the Node of interest.
	 *  \return The index of Node i's brother to the right, if it exists;
	 *          UINT_MAX, otherwise.
	 */
	NodeIndex computeRightBrotherIndex(const NodeIndex i) const;

	void computeTriangleAreaAndNormal(const Triangle &tri,
									  float &area,
									  Normal &n) const;

	void findBounds(const std::vector<unsigned int>::iterator &begin,
					const std::vector<unsigned int>::iterator &end,
					const Triangles& triangles,
					Point &minCorner, Point &maxCorner);

	unsigned int findPrincipalAxis(const Point &min,
								   const Point &max) const;

	void createApproximatingDisc(const Triangle &tri,
								 Disc &disc) const;

	void createApproximatingDisc(const Disc &d0,
								 const Disc &d1,
								 Disc &disc) const;

	void createApproximatingDisc(const Node &n0,
								 const Node &n1,
								 Disc &disc) const;

	static void sort(std::vector<unsigned int>::iterator &begin,
					 std::vector<unsigned int>::iterator &end,
					 const maPoint3d* vertexPositions,
					 const Triangles& triangles,
					 const unsigned int axis);

	int m_NumPositions;
	maPoint3d* m_Positions;

	//std::vector<Point> mVertexPositions;
	Triangles m_Triangles;

	std::vector<Node> m_Nodes;

	NodeIndex m_RootIndex;
	int m_TreeDepth;
}; // end class g3dOcclusionTree

