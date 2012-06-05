/****************************************************************************\
**	tmeshOptimizedBVH.hpp
**
**  Implementation of BVH construction        
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef TMESH_OPTIMIZEDBVH_HPP
#error tmeshOptimizedBVH.hpp multiply included
#endif
#define TMESH_OPTIMIZEDBVH_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif

#ifndef G3D_TYPE_HPP
#include "Graphics/g3d/g3dType.hpp"
#endif 

#ifndef G3D_INDEXPTR_HPP
#include "Graphics/g3d/g3dIndexPtr.hpp"
#endif

#define AABB_INCREASE 0

struct OptimizedBvhNode {

	// the bounding box
	maVector4d _aabbMin;
	maVector4d _aabbMax;

	// these 2 pointers are obsolete, the stackless traversal just uses the escape index
//	OptimizedBvhNode * _leftChild;
//	OptimizedBvhNode * _rightChild;

	maVector4d _indices;
/*
	int _escapeIndex;

	// for child nodes
	int _subPart;
	int _triangleIdx;
	int _indexBufferIdx;
*/
};


class tmeshOptimizedBvh
{

public:
    tmeshOptimizedBvh(int i_nVertices, const maPoint3d * i_Vertices,
					  int i_nIndices, const g3dIndexPtr i_Indices);
	~tmeshOptimizedBvh();

	OptimizedBvhNode * Build(const g3dIndexPtr i_Indices);
	OptimizedBvhNode * BuildTree(OptimizedBvhNode * leafNodes, int startIndex, int endIndex);	

	int MaxAxis(maVector4d v);
	maVector4d SetMin(maVector4d self, maVector4d other);
	maVector4d SetMax(maVector4d self, maVector4d other);

	int CalculateSplittingAxis(OptimizedBvhNode * leafNodes, int startIndex, int endIndex);
	int SortAndCalculateSplittingIndex(OptimizedBvhNode * leafNodes, int startIndex, int endIndex, int splitAxis);

	void ProcessAllTriangles(const g3dIndexPtr i_Indices);
	void MakeTriangleNode(maVector3d* triangle, int partId, int triangleIndex, int indexBufferIdx);

	void WalkStacklessTree(OptimizedBvhNode * rootNodeArray, OptimizedBvhNode * nodeCallback, maVector3d aabbMin, maVector3d aabbMax);
	bool TestAabbAgainstAabb2(maVector4d aabbMinA, maVector4d aabbMaxA, maVector4d aabbMinB, maVector4d aabbMaxB);
	
	int GetNumNodes();
private:
	OptimizedBvhNode * _rootNode;

	OptimizedBvhNode * _contiguousNodes;
	int _curNodeIndex;

	OptimizedBvhNode * _leafNodes;
	int _numLeaves;

	int _maxIterations;

	int _nVertices;
	int _nIndices;
	const maPoint3d * _Vertices;
};
