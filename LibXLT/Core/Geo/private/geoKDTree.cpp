/*****************************************************************************
**  geoKDTree.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/geo/geoKDTree.hpp"

#include "Core/geo/geoRayIntersection.hpp"
#include "Core/geo/geoPickedPoint.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{

const int c_DesiredPolysInLeaves = 4;

// Ray intersection stats
int l_LeavesTested = 0;
int	l_TrisTested = 0;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
struct TriRef
{
	int index;
	maPoint3d points[3];
	maVector3d normal;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
struct kdTreeNode 
{
	kdTreeNode() { front = NULL;  back = NULL; }

	kdTreeNode *front;
	kdTreeNode *back;
	std::vector<int> tris;
	int axis;
	float dist;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void
clamp(float &io_Val, const float i_Precision)
{
	io_Val = floor(io_Val * i_Precision) / i_Precision;
}
void
clamp(maPoint3d &io_Point, const float i_Precision)
{
	clamp(io_Point.m_X, i_Precision);
	clamp(io_Point.m_Y, i_Precision);
	clamp(io_Point.m_Z, i_Precision);

	// testing kdTree errors
	//io_Point.m_Y += (io_Point.m_X + io_Point.m_Z) / 1000.0f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void 
ClearTree(kdTreeNode &io_Node)
{
	if (io_Node.front) 
	{
		ClearTree(*io_Node.front);
		delete io_Node.front;
		io_Node.front = NULL;
	}
	if (io_Node.back) 
	{
		ClearTree(*io_Node.back);
		delete io_Node.back;
		io_Node.front = NULL;
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline void 
SplitBBox(	float i_Split, 
			int i_Axis, 
			maPoint3d &io_MaxBack, 
			maPoint3d &io_MinFront)
{
	io_MaxBack[i_Axis] = i_Split;
	io_MinFront[i_Axis] = i_Split;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool 
TriInsideBox(const maPoint3d &p0, const maPoint3d &p1, const maPoint3d &p2, 
			 const maPoint3d &min, const maPoint3d &max, const maVector3d &normal)
{
	int axis = 0;

	if(p0[axis] < min[axis] && p1[axis] < min[axis] && p2[axis] < min[axis])
		return false;
	axis++;

	if(p0[axis] < min[axis] && p1[axis] < min[axis] && p2[axis] < min[axis])
		return false;
	axis++;

	if(p0[axis] < min[axis] && p1[axis] < min[axis] && p2[axis] < min[axis])
		return false;
	axis++;

	axis = 0;

	if(p0[axis] > max[axis] && p1[axis] > max[axis] && p2[axis] > max[axis])
		return false;
	axis++;

	if(p0[axis] > max[axis] && p1[axis] > max[axis] && p2[axis] > max[axis])
		return false;
	axis++;

	if(p0[axis] > max[axis] && p1[axis] > max[axis] && p2[axis] > max[axis])
		return false;

	maPoint3d pts[8];

	pts[0] = min;
	pts[1].Set(min.m_X, min.m_Y, max.m_Z);
	pts[2].Set(min.m_X, max.m_Y, max.m_Z);
	pts[3].Set(min.m_X, max.m_Y, min.m_Z);
	pts[4].Set(max.m_X, min.m_Y, min.m_Z);
	pts[5].Set(max.m_X, min.m_Y, max.m_Z);
	pts[6].Set(max.m_X, max.m_Y, min.m_Z);
	pts[7] = max;

	float d = -(p0 * normal);
	bool side = (d + pts[0] * normal) >= 0;
	for (int i = 1; i < 8; i++) 
	{
		if (side != ((d + pts[i] * normal) >= 0)) 
		{
			return true;
		}
	}

	return false;

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void 
Split(	const std::vector<TriRef> &i_TriRefs, 
		const std::vector<int> &i_TriList, 
		std::vector<int> &o_FrontList, 
		std::vector<int> &o_BackList, 
		int i_Axis, 
		float i_Dist, 
		const maPoint3d &i_Min, 
		const maPoint3d &i_Max)
{
	float center = (i_Max[i_Axis] + i_Min[i_Axis]) * 0.5f;

	maPoint3d maxBack = i_Max;
	maPoint3d minFront = i_Min;

	SplitBBox(center, i_Axis, maxBack, minFront);

	//DBG_ASSERT(center == i_Dist, "Inconsistent splitting.");

	int i;
	int trilist_size = i_TriList.size();
	for (i = 0; i < trilist_size ; i++)
	{
		const maPoint3d& p0 = i_TriRefs[i_TriList[i]].points[0];
		const maPoint3d& p1 = i_TriRefs[i_TriList[i]].points[1];
		const maPoint3d& p2 = i_TriRefs[i_TriList[i]].points[2];

		if (p0[i_Axis] < i_Dist || p1[i_Axis] < i_Dist || p2[i_Axis] < i_Dist)
		{
			if (TriInsideBox(p0, p1, p2, i_Min, maxBack, i_TriRefs[i_TriList[i]].normal))
				o_BackList.push_back(i_TriList[i]);
		}
		if (p0[i_Axis] >= i_Dist || p1[i_Axis] >= i_Dist || p2[i_Axis] >= i_Dist)
		{
			if (TriInsideBox(p0, p1, p2, minFront, i_Max, i_TriRefs[i_TriList[i]].normal))
				o_FrontList.push_back(i_TriList[i]);
		}
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
kdTreeNode *
MakeTree(	const std::vector<TriRef> &i_TriRefs, 
			std::vector<int> &list, 
			int axis, 
			const maPoint3d& min, 
			const maPoint3d& max,
			const maPoint3d& i_MinNodeBBox,
			const int i_DesiredPolysInLeaves)
{
	if (list.size() == 0)
	{
		return NULL;
	}

	maPoint3d box = max - min;

	if (	(list.size() < i_DesiredPolysInLeaves	) || 
			(	box[0] < i_MinNodeBBox[0] && 
				box[1] < i_MinNodeBBox[1] && 
				box[2] < i_MinNodeBBox[2])	)
	{
		// Base of recursion, Make node for whole list
		//
		kdTreeNode *node = new kdTreeNode;

		node->front = NULL;
		node->back = NULL;
		node->tris = list;	//bga - this must make a copy of the list
		
		//axis and dist shouldn't matter, but put non-crazy values in anyway
		node->axis = 0;
		node->dist = 0.0f;

		return node;
	}

	// Don't allow a split to reduce the axis of a bbox
	// if that dimension is already below minimum.
	// If all 3 were below minimum, we wouldn't be here 
	// (see "if" above)
	//
	while (box[axis] < i_MinNodeBBox[axis])
	{
		axis = (axis + 1) % 3;
	}

	// Split list of triangles along an axis
	//
	float center = box[axis]/2.0f + min[axis];
	std::vector<int> frontList;
	std::vector<int> backList;

	Split(i_TriRefs, list, frontList, backList, axis, center, min, max);

	kdTreeNode *node = new kdTreeNode;

	int nextAxis = (axis + 1) % 3;

	maPoint3d maxBack = max;
	maPoint3d minFront = min;

	SplitBBox(center, axis, maxBack, minFront);

	node->front = MakeTree(i_TriRefs, frontList, nextAxis, minFront, max, i_MinNodeBBox, i_DesiredPolysInLeaves);
	node->back = MakeTree(i_TriRefs, backList, nextAxis, min, maxBack, i_MinNodeBBox, i_DesiredPolysInLeaves);
	node->axis = axis;
	node->dist = center;
	return node;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void 
GetStats(kdTreeNode &node, int &numLeaves, int &numTris, int &numFatLeaves)
{
	if (node.front == 0 && node.back == 0) 
	{
		numLeaves++;
		numTris += node.tris.size();
		if (node.tris.size() > 100) 
		{
			numFatLeaves++;
		}
	}
	else 
	{
		if (node.front) 
		{
			GetStats(*node.front, numLeaves, numTris, numFatLeaves);
		}
		if (node.back) 
		{
			GetStats(*node.back, numLeaves, numTris, numFatLeaves);
		}
	}

}

void get_groups(kdTreeNode &node, std::vector< std::vector<int> > &o_TriGroups)
{
	if (node.tris.size() > 0)
	{
		DBG_LOG("Triangle group has " << node.tris.size() << " triangles." );
		o_TriGroups.push_back(node.tris);
	}
	if (node.front) 
	{
		get_groups(*node.front, o_TriGroups);
	}
	if (node.back) 
	{
		get_groups(*node.back, o_TriGroups);
	}
	
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool 
TriListIntersection(const std::vector<TriRef> &i_TriRefs, 
					const kdTreeNode *i_pNode, 
					const maPoint3d &i_RayStart, 
					const maPoint3d &i_RayEnd, 
					geoPickedPoint &o_PickPt)
{
	l_LeavesTested++;
	l_TrisTested += i_pNode->tris.size();

	maVector3d RayDir = i_RayEnd - i_RayStart;
	bool bStraightDown = ((RayDir.m_X == 0) && (RayDir.m_Z == 0));

	bool intersected = false;

	int i;
	int num_node_tris = i_pNode->tris.size();
	for (i = 0; i < num_node_tris ; i++)
	{
		int cur_tri = i_pNode->tris[i];
		const maPoint3d& Ap = i_TriRefs[cur_tri].points[0];
		const maPoint3d& Bp = i_TriRefs[cur_tri].points[1];
		const maPoint3d& Cp = i_TriRefs[cur_tri].points[2];

		if (bStraightDown)
		{
			// If the ray is straight down, 
			// we can do trivial reject on the triangle in XZ plane
			if ((i_RayStart.m_X > Ap.m_X) && (i_RayStart.m_X > Bp.m_X) 
					&& (i_RayStart.m_X > Cp.m_X)) continue;
			if ((i_RayStart.m_X < Ap.m_X) && (i_RayStart.m_X < Bp.m_X) 
					&& (i_RayStart.m_X < Cp.m_X)) continue;
			if ((i_RayStart.m_Z > Ap.m_Z) && (i_RayStart.m_Z > Bp.m_Z) 
					&& (i_RayStart.m_Z > Cp.m_Z)) continue;
			if ((i_RayStart.m_Z < Ap.m_Z) && (i_RayStart.m_Z < Bp.m_Z) 
					&& (i_RayStart.m_Z < Cp.m_Z)) continue;
		}

		// plane normal is precalculated
		maPoint3d fnormal = i_TriRefs[i_pNode->tris[i]].normal;

		// Check for if how the polygon's plane is intersected by the
		// ray segment.  In general, the i_RayStart point needs to be
		// on the positive side, and the i_RayEnd point needs to be on the
		// negative side for there to be an intersection.
		//
		float i_RayStartval = fnormal * ( i_RayStart - Ap );
		if (i_RayStartval < 0.0f) continue;
		float i_RayEndval = fnormal * ( i_RayEnd - Ap );
		if (i_RayEndval > 0.0f) continue;

		float tval = 0.0f;

		// get intersect point, we know one point on plane 
		if (!geoRayIntersection::ProjectLineToPlane(i_RayStart, RayDir, 
					Ap, fnormal, tval)) continue;

		// Check to see if tval is within bounds
		//
		if (tval < 0.0f) continue;
		if (tval >= 1.0f) continue;

		// check to see if after already found intersection
		if (o_PickPt.IsFoundIntersect() && (o_PickPt.GetTVal() < tval)) continue;

		// Now compute the point on the plane
		//
		maPoint3d pt = i_RayStart + RayDir * tval;

		if ( geoRayIntersection::IsInsideTriangle( Ap, Bp, Cp, pt, 
										fnormal ) )
		{
			// Found intersection
			o_PickPt.SetFoundIntersect(true);
			o_PickPt.SetPickPos(pt);
			o_PickPt.SetTVal(tval);
			o_PickPt.SetPickNormal(fnormal);

			// if find closest, have to keep going, otherwise
			// can return first intersection found
			if (o_PickPt.IsPickClosest()) 
				intersected = true;
			else
				return true; 
		}
	}

	return intersected;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool 
NodeRayIntersection(const std::vector<TriRef> &i_TriRefs, 
					const kdTreeNode *i_pNode, 
					const maPoint3d &i_RayStart, 
					const maPoint3d &i_RayEnd, 
					geoPickedPoint &o_PickPt)
{
	if (!i_pNode)
	{
		return false;
	}

	// check if leaf
	if (i_pNode->front == NULL && i_pNode->back == NULL)
	{
		return TriListIntersection(i_TriRefs, i_pNode, i_RayStart, i_RayEnd, o_PickPt);
	}


	const float epsilon = 0.0f;

	if (i_RayStart[i_pNode->axis] + epsilon < i_pNode->dist
	      && i_RayEnd[i_pNode->axis] + epsilon < i_pNode->dist)
	{
		// both start and end in back
		return NodeRayIntersection(i_TriRefs, i_pNode->back, i_RayStart, i_RayEnd, o_PickPt);
	}
	else if(i_RayStart[i_pNode->axis] - epsilon >= i_pNode->dist
			&& i_RayEnd[i_pNode->axis] - epsilon >= i_pNode->dist)
	{
		//both start and end in front
		return NodeRayIntersection(i_TriRefs, i_pNode->front, i_RayStart, i_RayEnd, o_PickPt);
	}
	else
	{
		//points on opposite side, or within epsilon of plane
		maVector3d delta = i_RayEnd - i_RayStart;
		if (delta[i_pNode->axis] == 0.0f)
		{
			//this should not happen but just in case ...
			//return collision with front
			return NodeRayIntersection(i_TriRefs, i_pNode->front, i_RayStart, i_RayEnd, o_PickPt);
		}

		// find where ray hits dividing plane, and split
		// ray into two segments
		float t = (i_pNode->dist - i_RayStart[i_pNode->axis]) / delta[i_pNode->axis];
		maPoint3d intersectionPt = i_RayStart + delta * t;

		if (i_RayStart[i_pNode->axis] < i_pNode->dist) 
		{
			if ( NodeRayIntersection(i_TriRefs, i_pNode->back, i_RayStart, 
						intersectionPt, o_PickPt))
			{
				return true;
			}
			else
			{
				return NodeRayIntersection(i_TriRefs, i_pNode->front, 
							intersectionPt, i_RayEnd, o_PickPt);
			}
		}
		else 
		{
			if (NodeRayIntersection(i_TriRefs, i_pNode->front, i_RayStart, 
						intersectionPt, o_PickPt))
			{
				return true;
			}
			else
			{
				return NodeRayIntersection(i_TriRefs, i_pNode->back, 
							intersectionPt, i_RayEnd, o_PickPt);
			}
		}
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool 
TriListBboxIntersection(const std::vector<TriRef> &i_TriRefs, 
						const kdTreeNode *i_pNode, 
						const maPoint3d &i_MinPt,  
						const maPoint3d &i_MaxPt)
{
	l_LeavesTested++;
	int num_node_tris = i_pNode->tris.size();
	l_TrisTested += num_node_tris;

	int i;
	for (i = 0; i < num_node_tris ; i++)
	{

		const maPoint3d& Ap = i_TriRefs[i_pNode->tris[i]].points[0];
		const maPoint3d& Bp = i_TriRefs[i_pNode->tris[i]].points[1];
		const maPoint3d& Cp = i_TriRefs[i_pNode->tris[i]].points[2];
		
		if (TriInsideBox(Ap, Bp, Cp, i_MinPt, i_MaxPt,  
					i_TriRefs[i_pNode->tris[i]].normal))
		{
			return true;
		}

	}

	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool 
NodeBBoxIntersection(	const std::vector<TriRef> &i_TriRefs, 
						const kdTreeNode *i_pNode, 
						const maPoint3d &i_MinPt,  
						const maPoint3d &i_MaxPt)
{
	if (!i_pNode)	return false;


	// check if leaf
	if (i_pNode->front == NULL && i_pNode->back == NULL)
	{
		return TriListBboxIntersection(i_TriRefs, i_pNode, i_MinPt, i_MaxPt);
	}

	if (i_MaxPt[i_pNode->axis] < i_pNode->dist)
	{
		// both min and max in back
		return NodeBBoxIntersection(i_TriRefs, i_pNode->back, i_MinPt, i_MaxPt);
	}
	else if (i_MinPt[i_pNode->axis] >= i_pNode->dist)
	{
		// both min and max in front
		return NodeBBoxIntersection(i_TriRefs, i_pNode->front, i_MinPt, i_MaxPt);
	}
	else
	{
		// find where bbox hits dividing plane, and split
		// bbox into two boxes
		
		maPoint3d maxBack = i_MaxPt;
		maPoint3d minFront = i_MinPt;

		SplitBBox(i_pNode->dist, i_pNode->axis, maxBack, minFront);

		if ( NodeBBoxIntersection(i_TriRefs, i_pNode->back, i_MinPt, maxBack) )
		{
			return true;
		}
		else
		{
			return NodeBBoxIntersection(i_TriRefs, i_pNode->front, minFront, i_MaxPt);
		}
	}

}

//--------------------------------------------------------------------
// TriListInBox - finds triangles that might intersect 
//  axis aligned bounding box and add to o_Tris vector
//--------------------------------------------------------------------
void 
TriListInBbox(const std::vector<TriRef> &i_TriRefs, 
				const kdTreeNode *i_pNode, 
				const maPoint3d &i_MinPt,  
				const maPoint3d &i_MaxPt,
				std::vector<maPoint3d> &o_Tris)
{
	l_LeavesTested++;
	int num_node_tris = i_pNode->tris.size();
	l_TrisTested += num_node_tris;

	int i;
	for (i = 0; i < num_node_tris ; i++)
	{

		const maPoint3d& Ap = i_TriRefs[i_pNode->tris[i]].points[0];
		const maPoint3d& Bp = i_TriRefs[i_pNode->tris[i]].points[1];
		const maPoint3d& Cp = i_TriRefs[i_pNode->tris[i]].points[2];
		
		if (TriInsideBox(Ap, Bp, Cp, i_MinPt, i_MaxPt,  
					i_TriRefs[i_pNode->tris[i]].normal))
		{
			o_Tris.push_back(Ap);
			o_Tris.push_back(Bp);
			o_Tris.push_back(Cp);
			o_Tris.push_back(i_TriRefs[i_pNode->tris[i]].normal);
		}

	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void 
NodeTrianglesInBBox(	const std::vector<TriRef> &i_TriRefs, 
						const kdTreeNode *i_pNode, 
						const maPoint3d &i_MinPt,  
						const maPoint3d &i_MaxPt,
						std::vector<maPoint3d> &o_Tris)
{
	if (!i_pNode)	return;

	// check if leaf
	if (i_pNode->front == NULL && i_pNode->back == NULL)
	{
		TriListInBbox(i_TriRefs, i_pNode, i_MinPt, i_MaxPt, o_Tris);
	}

	if (i_MaxPt[i_pNode->axis] < i_pNode->dist)
	{
		// both min and max in back
		NodeTrianglesInBBox(i_TriRefs, i_pNode->back, 
						i_MinPt, i_MaxPt, o_Tris);
	}
	else if (i_MinPt[i_pNode->axis] >= i_pNode->dist)
	{
		// both min and max in front
		NodeTrianglesInBBox(i_TriRefs, i_pNode->front, 
						i_MinPt, i_MaxPt, o_Tris);
	}
	else
	{
		// find where bbox hits dividing plane, and split
		// bbox into two boxes
		
		maPoint3d maxBack = i_MaxPt;
		maPoint3d minFront = i_MinPt;

		SplitBBox(i_pNode->dist, i_pNode->axis, maxBack, minFront);

		NodeTrianglesInBBox(i_TriRefs, i_pNode->back, 
								i_MinPt, maxBack, o_Tris);
		NodeTrianglesInBBox(i_TriRefs, i_pNode->front, 
								minFront, i_MaxPt, o_Tris);
	}

}

}

//--------------------------------------------------------------------
// Private implementation structure
//--------------------------------------------------------------------
struct geoKDTreeImp
{
	std::vector<TriRef>	m_TriRefs;
	kdTreeNode*			m_pNode;
	maPoint3d			m_MinNodeBBox;
	maAxisBox			m_AxisBox;
	int					m_DesiredPolysInLeaves;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
geoKDTree::geoKDTree()
{
	m_pImp = new geoKDTreeImp;
	m_pImp->m_pNode = NULL;
	m_pImp->m_MinNodeBBox.Set(100, 100, 100);
	m_pImp->m_DesiredPolysInLeaves = c_DesiredPolysInLeaves;
	//m_pImp->m_MinNodeBBox.Set(10000, 10000, 10000);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
geoKDTree::~geoKDTree()
{
	Clear();
	delete m_pImp;
}

//--------------------------------------------------------------------
// Clear prepares set for receiving triangles
//--------------------------------------------------------------------
void	
geoKDTree::Clear()
{
	if (m_pImp->m_pNode)
	{
		ClearTree(*m_pImp->m_pNode);
		delete m_pImp->m_pNode;
		m_pImp->m_pNode = NULL;
	}

	m_pImp->m_TriRefs.clear();
}


//--------------------------------------------------------------------
// Add Triangles to set
//--------------------------------------------------------------------
void	
geoKDTree::AddTriangles(const maPoint3d*			i_Vertices,
						int							i_NumVertices,
						const envType::UInt32*		i_Indices,
						int							i_NumIndices)
{
	int triCount = (i_NumIndices / 3);

	// Add to triangle count for running total
	//
	int triIndx = m_pImp->m_TriRefs.size();
	triCount += triIndx;
	m_pImp->m_TriRefs.resize(triCount);

	for (int ii = 0; ii < i_NumIndices; ii += 3)
	{
		// is index used?
		m_pImp->m_TriRefs[triIndx].index = ii;

		m_pImp->m_TriRefs[triIndx].points[0] = i_Vertices[i_Indices[ii]];
		m_pImp->m_TriRefs[triIndx].points[1] = i_Vertices[i_Indices[ii + 1]];
		m_pImp->m_TriRefs[triIndx].points[2] = i_Vertices[i_Indices[ii + 2]];

		// Clamp vertices to 4 decimal places, this helps
		// the ray intersection algorithm handle the 
		// numerics in IsInsideTriangle()
		//
		clamp(m_pImp->m_TriRefs[triIndx].points[0], 1.0e4);
		clamp(m_pImp->m_TriRefs[triIndx].points[1], 1.0e4);
		clamp(m_pImp->m_TriRefs[triIndx].points[2], 1.0e4);

		// Compute face normal
		maVector3d v1 = m_pImp->m_TriRefs[triIndx].points[1] - m_pImp->m_TriRefs[triIndx].points[0];
		maVector3d v2 = m_pImp->m_TriRefs[triIndx].points[2] - m_pImp->m_TriRefs[triIndx].points[0];
		m_pImp->m_TriRefs[triIndx].normal = v1 / v2;
		if (m_pImp->m_TriRefs[triIndx].normal.LengthSqr() > 0.0f)
		{
			m_pImp->m_TriRefs[triIndx].normal.Normalize(); //should check for degenerates here
		}
		else m_pImp->m_TriRefs[triIndx].normal.Set(0,1,0);

		triIndx++;
	}

}

//--------------------------------------------------------------------
//	SetMinBox allows the user to set the size of the largest
//	allowable subsection of the tree.
//--------------------------------------------------------------------
void geoKDTree::SetMinBox(float i_X, float i_Y, float i_Z)
{
	m_pImp->m_MinNodeBBox.Set(i_X, i_Y, i_Z);
}

//--------------------------------------------------------------------
//	SetNumDesiredPolysInLeaves allows the user to set the 
//	desired number of polygons per leaf.
//--------------------------------------------------------------------
void geoKDTree::SetNumDesiredPolysInLeaves(int i_Num)
{
	m_pImp->m_DesiredPolysInLeaves  = i_Num;
}

//--------------------------------------------------------------------
//	GetWorldBox returns a bounding box in world space for tree.
//--------------------------------------------------------------------
const maAxisBox& 
geoKDTree::GetWorldBox() const
{
	return m_pImp->m_AxisBox;
}

//--------------------------------------------------------------------
// Create finishes structure, no more triangles may be added.
//--------------------------------------------------------------------
void	
geoKDTree::Create()
{
	int triCount = m_pImp->m_TriRefs.size();
//	DBG_LOG("Creating kdTree with " << triCount << " tris." );
	
	if (triCount == 0)
	{
		return;
	}

	std::vector<int> triList;
	triList.resize(triCount);
	int i;
	for (i = 0; i < triCount; i++)
	{
		triList[i] = i;		
	}

	// Compute Bounding box of points
	//
	maPoint3d bbmin = m_pImp->m_TriRefs[0].points[0];	// start with first point
	maPoint3d bbmax = bbmin;

	maPoint3d pt;
	for (i = 0; i < triCount; i++)
	{
		for (int p=0; p<3; p++)
		{
			pt = m_pImp->m_TriRefs[i].points[p];
			
			if (pt.m_X < bbmin.m_X)		bbmin.SetX( pt.m_X );
			else if (pt.m_X > bbmax.m_X)	bbmax.SetX( pt.m_X );
			if (pt.m_Y < bbmin.m_Y)		bbmin.SetY( pt.m_Y );
			else if (pt.m_Y > bbmax.m_Y)	bbmax.SetY( pt.m_Y );
			if (pt.m_Z < bbmin.m_Z)		bbmin.SetZ( pt.m_Z );
			else if (pt.m_Z > bbmax.m_Z)	bbmax.SetZ( pt.m_Z );
		}
	}

	// Expand bbox slightly to make sure it has
	// some width in all dimensions
	bbmin += maVector3d(-1,-1,-1);
	bbmax += maVector3d(1,1,1);

//	DBG_LOG3("Min BBox kdTree %f %f %f", bbmin.m_X, bbmin.m_Y, bbmin.m_Z);
//	DBG_LOG3("Max BBox kdTree %f %f %f", bbmax.m_X, bbmax.m_Y, bbmax.m_Z);
	m_pImp->m_AxisBox.Set(bbmin, bbmax);

	// cycle among axial planes
	int axis = 0;

	m_pImp->m_pNode = MakeTree(m_pImp->m_TriRefs, triList, axis, bbmin, bbmax, 
		m_pImp->m_MinNodeBBox, m_pImp->m_DesiredPolysInLeaves);

	int numLeaves = 0;
	int numTris = 0;
	int numFatLeaves = 0;
	GetStats(*m_pImp->m_pNode, numLeaves, numTris, numFatLeaves);
//	DBG_LOG3("Finished kdTree with %d leaves, %d tris, %d fat leaves.",
//					numLeaves, numTris, numFatLeaves);
}


//--------------------------------------------------------------------
//	ComputeRayIntersection, returns true if ray from ray start
//  to ray end intersects triangles.
//  Make sure Create() has been called first
//--------------------------------------------------------------------
bool	
geoKDTree::ComputeRayIntersection(const maPoint3d &i_RayStart, 
			const maVector3d &i_RayEnd, geoPickedPoint &o_PickPt) const
{
	l_LeavesTested = 0;
	l_TrisTested = 0;

	bool bHit =  NodeRayIntersection(m_pImp->m_TriRefs, m_pImp->m_pNode, 
		i_RayStart, i_RayEnd, o_PickPt);


	int leaves_tested = l_LeavesTested;
	int tris_tested = l_TrisTested;

	return bHit;
}
		
//--------------------------------------------------------------------
//	IntersectBBox, returns true if axis-aligned bbox intersects
//  with triangles.
//  Make sure Create() has been called first
//--------------------------------------------------------------------
bool	
geoKDTree::IntersectBBox(const maPoint3d &i_MinPt, 
						 const maPoint3d &i_MaxPt) const
{
	bool bHit =  NodeBBoxIntersection(m_pImp->m_TriRefs, m_pImp->m_pNode, 
						i_MinPt, i_MaxPt);

	return bHit;
}


//--------------------------------------------------------------------
//	GetTrianglesInBBox, returns triangles that intersect 
//  axis-aligned bbox into vector. This checks for 
//  overlap between triangle's bbox and given bbox, not
//  exact collision between triangle and bbox.
//  Triangles are returned as 3 point and 1 normal vector.
//  Make sure Create() has been called first
//--------------------------------------------------------------------
void
geoKDTree::GetTrianglesInBBox(const maPoint3d &i_MinPt, 
							  const maPoint3d &i_MaxPt,
							  std::vector<maPoint3d> &o_Tris) const
{
	o_Tris.clear();
	NodeTrianglesInBBox(m_pImp->m_TriRefs, m_pImp->m_pNode, 
						i_MinPt, i_MaxPt, o_Tris);

}

//--------------------------------------------------------------------
// Get groups of triangles, spatially partitioned. Each index
//	is the index of the triangle in the original list.
//--------------------------------------------------------------------
void geoKDTree::GetGroups(std::vector< std::vector<int> > &o_TriGroups)
{
	if (m_pImp && m_pImp->m_pNode)
	{
		get_groups(*m_pImp->m_pNode, o_TriGroups);
	}
}