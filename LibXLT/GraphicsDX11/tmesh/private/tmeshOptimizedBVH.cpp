/****************************************************************************\
**	tmeshOptimizedBVH.cpp
**
**  Implementation of BVH construction        
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/tmesh/tmeshOptimizedBVH.hpp"

tmeshOptimizedBvh::tmeshOptimizedBvh(int i_nVertices, const maPoint3d * i_Vertices,
									 int i_nIndices, const g3dIndexPtr i_Indices) 
{
	_rootNode = NULL;
	_nVertices = i_nVertices;
	_nIndices = i_nIndices;
	_Vertices = i_Vertices;
	_numLeaves = i_nIndices/3;
	_leafNodes = new OptimizedBvhNode[_numLeaves];
	_contiguousNodes = NULL;
}

tmeshOptimizedBvh::~tmeshOptimizedBvh() 
{
	delete [] _leafNodes;
	delete [] _contiguousNodes;
}

OptimizedBvhNode * tmeshOptimizedBvh::Build(const g3dIndexPtr i_Indices)
{
	//NodeTriangleCallback callback = new NodeTriangleCallback(_leafNodes);
	ProcessAllTriangles(i_Indices);

	_contiguousNodes = new OptimizedBvhNode[2 * _numLeaves];
	_curNodeIndex = 0;

	(_rootNode) = BuildTree(_leafNodes, 0, _numLeaves);

	DBG_ASSERT(_rootNode == _contiguousNodes, "root node is not the first of the contiguousnodes");
	return _rootNode;
}
int tmeshOptimizedBvh::GetNumNodes()
{
	return 2 * _numLeaves;
}


void tmeshOptimizedBvh::ProcessAllTriangles(const g3dIndexPtr i_Indices)
{
	int triangleIdx, indexBufferIdx;
	int numTriangles = _nIndices/3;

	int numMeshes = 1;
	maVector3d triangle[3];

	// Loop over meshes
	int meshId;
	for (meshId = 0; meshId < numMeshes; meshId++)
	{
		// Loop over triangles of current mesh
        for (triangleIdx = 0; triangleIdx < numTriangles; triangleIdx++)
        {
			indexBufferIdx = triangleIdx*3;

			triangle[0] = maVector3d( _Vertices[i_Indices.GetIndex(indexBufferIdx+0)].GetX() , 
									  _Vertices[i_Indices.GetIndex(indexBufferIdx+0)].GetY() , 
									  _Vertices[i_Indices.GetIndex(indexBufferIdx+0)].GetZ() ); //verts[indicies[triangleIdx * 3 + 0]];
            triangle[1] = maVector3d( _Vertices[i_Indices.GetIndex(indexBufferIdx+1)].GetX() , 
									  _Vertices[i_Indices.GetIndex(indexBufferIdx+1)].GetY() , 
									  _Vertices[i_Indices.GetIndex(indexBufferIdx+1)].GetZ() ); //verts[indicies[triangleIdx * 3 + 0]];
            triangle[2] = maVector3d( _Vertices[i_Indices.GetIndex(indexBufferIdx+2)].GetX() , 
									  _Vertices[i_Indices.GetIndex(indexBufferIdx+2)].GetY() , 
									  _Vertices[i_Indices.GetIndex(indexBufferIdx+2)].GetZ() ); //verts[indicies[triangleIdx * 3 + 0]];

			// Make a node per triangle
            MakeTriangleNode(triangle, meshId, triangleIdx, indexBufferIdx);
        }
	}
}


void tmeshOptimizedBvh::MakeTriangleNode(maVector3d* triangle, int partId, int triangleIdx, int indexBufferIdx)
{

	OptimizedBvhNode node;
	node._aabbMin = maVector3d(1e30f, 1e30f, 1e30f);
	node._aabbMax = maVector3d(-1e30f, -1e30f, -1e30f);

	node._aabbMin = SetMin(node._aabbMin, triangle[0]);
	node._aabbMax = SetMax(node._aabbMax, triangle[0]);
	node._aabbMin = SetMin(node._aabbMin, triangle[1]);
	node._aabbMax = SetMax(node._aabbMax, triangle[1]);
	node._aabbMin = SetMin(node._aabbMin, triangle[2]);
	node._aabbMax = SetMax(node._aabbMax, triangle[2]);

//	node._aabbMin = maVector4d(-3,-3,-3, 0);
//	node._aabbMax = maVector4d(3,3,3, 0);
	node._aabbMin -= maVector4d(.1f,.1f,.1f,0);
	node._aabbMax += maVector4d(.1f,.1f,.1f,0);

//	node._escapeIndex = -1;
	node._indices.m_X = -1;
//	node._leftChild = NULL;
//	node._rightChild = NULL;

	//for child nodes
	node._indices.m_Y = (float)partId;
	node._indices.m_Z = (float)triangleIdx;
	node._indices.m_W = (float)indexBufferIdx; 
//	node._subPart = partId;
//	node._triangleIdx = triangleIdx;
//	node._indexBufferIdx = indexBufferIdx; 

	//_triangleNodes.Add(node);
	//_leafNodes[(_numLeaves - 1) - triangleIdx] = node;
	_leafNodes[triangleIdx] = node;
}


OptimizedBvhNode * tmeshOptimizedBvh::BuildTree(OptimizedBvhNode * leafNodes, int startIndex, int endIndex)
{
	OptimizedBvhNode*  internalNode;

	int splitAxis, splitIndex, i;
	int numIndices = endIndex - startIndex;
	int curIndex = _curNodeIndex;

	if (numIndices <= 0)
	{
		// exception
	}
		
	if (numIndices == 1)
	{
		_contiguousNodes[_curNodeIndex++] = leafNodes[startIndex];
		return &(_contiguousNodes[_curNodeIndex]);//leafNodes[startIndex];
	}

	//calculate Best Splitting Axis and where to split it. Sort the incoming 'leafNodes' array within range 'startIndex/endIndex'.
	splitAxis = CalculateSplittingAxis(leafNodes, startIndex, endIndex);

	splitIndex = SortAndCalculateSplittingIndex(leafNodes, startIndex, endIndex, splitAxis);

	internalNode = &(_contiguousNodes[_curNodeIndex++]);

	internalNode->_aabbMax = maVector3d(-1e30f, -1e30f, -1e30f);
	internalNode->_aabbMin = maVector3d(1e30f, 1e30f, 1e30f);

	for (i = startIndex; i < endIndex; i++)
	{
		internalNode->_aabbMax = SetMax(internalNode->_aabbMax, leafNodes[i]._aabbMax);
		internalNode->_aabbMin = SetMin(internalNode->_aabbMin, leafNodes[i]._aabbMin);
	}

	internalNode->_aabbMin -= maVector4d(AABB_INCREASE,AABB_INCREASE,AABB_INCREASE,0);
	internalNode->_aabbMax += maVector4d(AABB_INCREASE,AABB_INCREASE,AABB_INCREASE,0);

	/*internalNode->_leftChild = */BuildTree(leafNodes, startIndex, splitIndex);
	/*internalNode->_rightChild = */BuildTree(leafNodes, splitIndex, endIndex);

	internalNode->_indices.m_X = (float)(_curNodeIndex - curIndex);
//	internalNode->_escapeIndex = _curNodeIndex - curIndex;

	DBG_ASSERT(internalNode->_indices.m_X >= 0, "Negative/bad escape index found" );

	return internalNode;
}

int tmeshOptimizedBvh::CalculateSplittingAxis(OptimizedBvhNode * leafNodes, int startIndex, int endIndex)
{
	maVector4d means = maVector4d();
	maVector4d variance = maVector4d();
	int numIndices = endIndex - startIndex;
	int i;

	for (i = startIndex; i < endIndex; i++)
	{
		maVector4d center = 0.5f * (leafNodes[i]._aabbMax + leafNodes[i]._aabbMin);
		means += center;
	}
	means *= (1.0f / (float)numIndices);

	for (i = startIndex; i < endIndex; i++)
	{
		maVector4d center = 0.5f * (leafNodes[i]._aabbMax + leafNodes[i]._aabbMin);
		maVector4d diff2 = center - means;
		diff2.SetX( diff2.GetX() * diff2.GetX() );
		diff2.SetY( diff2.GetY() * diff2.GetY() );
		diff2.SetZ( diff2.GetZ() * diff2.GetZ() );
		variance += diff2;
	}
	variance *= (1.0f / ((float)numIndices - 1));

	return MaxAxis(variance);
}

int tmeshOptimizedBvh::SortAndCalculateSplittingIndex(OptimizedBvhNode * leafNodes, int startIndex, int endIndex, int splitAxis)
{
	int splitIndex = startIndex;
	int numIndices = endIndex - startIndex;
	float splitValue;

	maVector4d means = maVector4d();
	for (int i = startIndex; i < endIndex; i++)
	{
		maVector4d center = 0.5f * (leafNodes[i]._aabbMax + leafNodes[i]._aabbMin);
		means += center;
	}
	means *= (1.0f / (float)numIndices);

	if (splitAxis == 0)
		splitValue = means.GetX();
	else if (splitAxis == 1)
		splitValue = means.GetY();
	else if (splitAxis == 2)
		splitValue = means.GetZ();
	else
	{
		//exception
	}		

	//sort leafNodes so all values larger then splitValue comes first, and smaller values start from 'splitIndex'.
	for (int i = startIndex; i < endIndex; i++)
	{
		maVector4d center = 0.5f * (leafNodes[i]._aabbMax + leafNodes[i]._aabbMin);
		float centerSplit;

		if (splitAxis == 0)
			centerSplit = means.GetX();
		else if (splitAxis == 1)
			centerSplit = means.GetY();
		else if (splitAxis == 2)
			centerSplit = means.GetZ();
		else
		{
			//exception
		}			

		if (centerSplit > splitValue)
		{
			//swap
			OptimizedBvhNode tmp = leafNodes[i];
			leafNodes[i] = leafNodes[splitIndex];
			leafNodes[splitIndex] = tmp;
			splitIndex++;
		}
	}
	if ((splitIndex == startIndex) || (splitIndex == (endIndex - 1)))
	{
		splitIndex = startIndex + (numIndices >> 1);
	}
	// bool unbalanced = ((splitIndex == startIndex) || (splitIndex == (endIndex - 1)));
	//int rangeBalancedIndices = numIndices / 3;
	//bool unbalanced = ((splitIndex <= startIndex+rangeBalancedIndices) || (splitIndex >= endIndex-1-rangeBalancedIndices));
	//if (unbalanced)
	//{
	//	splitIndex = startIndex + (numIndices >> 1);
	//}
	DBG_ASSERT(!(splitIndex == startIndex || splitIndex == endIndex), "bad bvh tree split" );
	return splitIndex;
}


maVector4d tmeshOptimizedBvh::SetMin(maVector4d self, maVector4d other)
{
	maVector4d retVal = self;

    if (other.GetX() < self.GetX())
        retVal.SetX(other.GetX());
    if (other.GetY() < self.GetY())
        retVal.SetY(other.GetY());
    if (other.GetZ() < self.GetZ())
        retVal.SetZ(other.GetZ());

	return retVal;
}


maVector4d tmeshOptimizedBvh::SetMax(maVector4d self, maVector4d other)
{
	maVector4d retVal = self;

    if (other.GetX() > self.GetX())
		retVal.SetX(other.GetX());
    if (other.GetY() > self.GetY())
        retVal.SetY(other.GetY());
    if (other.GetZ() > self.GetZ())
        retVal.SetZ(other.GetZ());

	return retVal;
}

int tmeshOptimizedBvh::MaxAxis(maVector4d v)
{
	return v.GetX() < v.GetY() ? (v.GetY() < v.GetZ() ? 2 : 1) : (v.GetX() < v.GetZ() ? 2 : 0);
} 

void tmeshOptimizedBvh::WalkStacklessTree(OptimizedBvhNode * rootNodeArray, OptimizedBvhNode * nodeCallback, maVector3d aabbMin, maVector3d aabbMax)
{
	int escapeIndex, curIndex = 0;
	int walkIterations = 0;
	bool aabbOverlap, isLeafNode;
    int rootNodeIndex = 0;
    OptimizedBvhNode rootNode = rootNodeArray[rootNodeIndex];

	while (curIndex < _curNodeIndex)
	{
		//catch bugs in tree data
		if (walkIterations >= _curNodeIndex)
		{
			// exception
		}
	
		walkIterations++;
		aabbOverlap = TestAabbAgainstAabb2(aabbMin, aabbMax, rootNode._aabbMin, rootNode._aabbMax);
		
		isLeafNode = rootNode._indices.m_X < 0;//   (rootNode._leftChild == NULL && rootNode._rightChild == NULL);
//		isLeafNode = rootNode._escapeIndex < 0;//   (rootNode._leftChild == NULL && rootNode._rightChild == NULL);

		if (isLeafNode && aabbOverlap)
		{
			//nodeCallback.ProcessNode(rootNode);
		}

		if (aabbOverlap || isLeafNode)
		{
			rootNodeIndex++; // this
			curIndex++;
            if (rootNodeIndex < (_nIndices/3))
                rootNode = rootNodeArray[rootNodeIndex];
        }
		else
		{
			//escapeIndex = rootNode._escapeIndex;
			escapeIndex = (int)rootNode._indices.m_X;
			rootNodeIndex += escapeIndex; // and this
			curIndex += escapeIndex;
            if (rootNodeIndex < (_nIndices/3))
                rootNode = rootNodeArray[rootNodeIndex];
		}

	}

	if (_maxIterations < walkIterations)
		_maxIterations = walkIterations;
}

// conservative test for overlap between two aabbs
bool tmeshOptimizedBvh::TestAabbAgainstAabb2(maVector4d aabbMinA, maVector4d aabbMaxA, maVector4d aabbMinB, maVector4d aabbMaxB)
{
	bool overlap = true;
	overlap = (aabbMinA.GetX() > aabbMaxB.GetX() || aabbMaxA.GetX() < aabbMinB.GetX()) ? false : overlap;
	overlap = (aabbMinA.GetZ() > aabbMaxB.GetZ() || aabbMaxA.GetZ() < aabbMinB.GetZ()) ? false : overlap;
	overlap = (aabbMinA.GetY() > aabbMaxB.GetY() || aabbMaxA.GetY() < aabbMinB.GetY()) ? false : overlap;
	return overlap;
}