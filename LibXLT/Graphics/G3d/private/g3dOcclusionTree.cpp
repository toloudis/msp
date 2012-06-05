/****************************************************************************\
**	g3dOcclusionTree.cpp
**
**
**  \author Jared Hoberock
**  \brief Implementation of g3dOcclusionTree class.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dOcclusionTree.hpp"

#include <limits>
#include <algorithm>
#include <assert.h>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dOcclusionTree::build(const maPoint3d* positions, 
							 int i_nVertices,
							 const g3dIndexPtr i_Indices, 
							 int i_nIndices,
							 maPoint3d& pmin, 
							 maPoint3d& pmax)
{
	m_NumPositions = i_nVertices;
	m_Positions = new maPoint3d[m_NumPositions];
	memcpy(m_Positions, positions, m_NumPositions*sizeof(maPoint3d));

//    maPoint3d c = (pmax + pmin) / 2.0f;
  //  float invDiagonalLength = 1.0f / (pmax - c).Length();
	//for (int i = 0; i < m_NumPositions; i++)
//	{
//		m_Positions[i] = 10.0f * invDiagonalLength * (positions[i] - c);
//	}

	m_Triangles.resize(i_nIndices/3);
	if (i_Indices.Is32BitIndices())
	{
		const envType::UInt32 *buffer = i_Indices.GetIndices32();
		for (int i = 0; i < m_Triangles.size(); i++)
		{
			m_Triangles[i][0] = buffer[i*3];
			m_Triangles[i][1] = buffer[i*3+1];
			m_Triangles[i][2] = buffer[i*3+2];
		}
	}
	else
	{
		const envType::UInt16 *buffer = i_Indices.GetIndices16();
		for (int i = 0; i < m_Triangles.size(); i++)
		{
			m_Triangles[i][0] = buffer[i*3];
			m_Triangles[i][1] = buffer[i*3+1];
			m_Triangles[i][2] = buffer[i*3+2];
		}
	}

	// tmp index list to pass to build()
	Triangles triangles = m_Triangles;

	// we will sort an array of indices
	// these indices are to triangles, not vertices!
	std::vector<unsigned int> triIndices(triangles.size());
	for (unsigned i = 0; i < triIndices.size(); ++i)
	{
		triIndices[i] = i;
	}

	// initialize
	// We start out with at least this many leaf nodes
	// more will be added as we create interior nodes
	m_Nodes.resize(m_Triangles.size());

	// recurse
	m_TreeDepth = 0;
	m_RootIndex = build(NULL_NODE,
		triIndices.begin(),
		triIndices.end(),
		triangles, 0);
	
	// for each node, compute the index of the next
	// node in a depth-first traversal
	for (NodeIndex i = 0;
		i != m_Nodes.size();
		++i)
	{
		m_Nodes[i].m_NextNode = computeNextIndex(i);
	} // end for i
} // end OcclusionTree::build()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dOcclusionTree::NodeIndex g3dOcclusionTree::build(const NodeIndex parent,
													std::vector<unsigned int>::iterator &begin,
													std::vector<unsigned int>::iterator &end,
													const Triangles& triangles, int lvl)
{
	assert(begin <= end);

	if (lvl > m_TreeDepth)
		m_TreeDepth = lvl;

	// base case
	if (begin + 1 == end)
	{
		// add a leaf node: these are stored
		// in order at the beginning of the array
		Node node;
		node.m_Occlusion = 1.0f;
		node.m_Parent = parent;
		node.m_LeftChild = node.m_RightChild = NULL_NODE;
		node.m_Height = lvl;
		createApproximatingDisc(triangles[*begin], node.m_Disc);

		if (node.m_Disc.m_Area == 0)
		{
			DBG_WARNING("g3dOcclusionTree::build(): zero area triangle: " << *begin);
		} // end if

		if (node.m_Disc.m_Normal[0] != node.m_Disc.m_Normal[0])
		{
			DBG_WARNING("g3dOcclusionTree::build(): nan normal on triangle: " << *begin);
			node.m_Disc.m_Normal = float3(0,0,0);
		} // end if

		// set the Node
		m_Nodes[*begin] = node;

		NodeIndex result = static_cast<NodeIndex>(*begin);
		return result;
	}
	else if (begin == end)
	{
		DBG_WARNING("g3dOcclusionTree::build(): empty base case.");
		return NULL_NODE;
	}

	// find the bounds of the points
	Point min, max;
	findBounds(begin, end, triangles, min, max);

	unsigned int axis = findPrincipalAxis(min, max);

	// add a new node
	NodeIndex nodeLocation = static_cast<NodeIndex>(m_Nodes.size());
	m_Nodes.push_back(Node());
	m_Nodes.back().m_Parent = parent;
	m_Nodes.back().m_Occlusion = 1.0f;
	m_Nodes.back().m_Height = lvl;

	// sort along this axis
	sort(begin, end, m_Positions, triangles, axis);

	unsigned int diff = static_cast<unsigned int>(end - begin);

	// find the element to split on
	std::vector<unsigned int>::iterator split
		= begin + (end - begin) / 2;

	// recurse
	assert(begin <= split);
	assert(split <= end);
	assert(begin <= end);
	NodeIndex leftChild = build(nodeLocation, begin, split, triangles, lvl+1);
	m_Nodes[nodeLocation].m_LeftChild = leftChild;
	NodeIndex rightChild = build(nodeLocation, split, end, triangles, lvl+1);
	m_Nodes[nodeLocation].m_RightChild = rightChild;

	// create an approximating disc
	assert(leftChild != NULL_NODE);
	assert(rightChild != NULL_NODE);
	createApproximatingDisc(m_Nodes[leftChild],
						  m_Nodes[rightChild],
						  m_Nodes[nodeLocation].m_Disc);

	return nodeLocation;
} // end g3dOcclusionTree::build()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dOcclusionTree::Node::Node(void)
{
	;
} // end Node::Node()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dOcclusionTree::findBounds(const std::vector<unsigned int>::iterator &begin,
								  const std::vector<unsigned int>::iterator &end,
								  const Triangles& triangles,
								  Point &minCorner, 
								  Point &maxCorner)
{
	float inf = std::numeric_limits<float>::infinity();
	Point inf3(inf,inf,inf);
	minCorner = inf3;
	maxCorner = -inf3;
	  
	for (std::vector<unsigned int>::iterator t = begin;
		t != end;
		++t)
	{
		const Triangle &tri = triangles[*t];
		Point centroid = m_Positions[tri[0]]
			+ m_Positions[tri[1]]
			+ m_Positions[tri[2]];
		centroid /= 3.0f;

		for (unsigned int i =0;
			i < 3;
			++i)
		{
			const float &x = centroid[i];

			if (x < minCorner[i])
			{
				minCorner[i] = x;
			} // end if

			if (x > maxCorner[i])
			{
				maxCorner[i] = x;
			} // end if
		} // end for i
	} // end for t
} // end g3dOcclusionTree::findBounds()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
unsigned int g3dOcclusionTree::findPrincipalAxis(const Point &min,
												 const Point &max) const
{
	// find the principal axis of the points
	unsigned int axis = 0;
	float maxLength = -1.0f;
	float temp;
	for (int i = 0; i < 3; ++i)
	{
		temp = max[i] - min[i];
		if (temp > maxLength)
		{
			maxLength = temp;
			axis = i;
		}
	}

	return axis;
} // end g3dOcclusionTree::findPrincipalAxis()


//============================================================================
//============================================================================
struct SortTriangles
{
	unsigned int axis;
	inline bool operator()(const unsigned int &lhs,
						 const unsigned int &rhs) const
	{
		float lhsCentroid = 0, rhsCentroid = 0;
		for (int i = 0; i < 3; ++i)
		{
			lhsCentroid += mVertices[mTriangles[lhs][i]][axis];
			rhsCentroid += mVertices[mTriangles[rhs][i]][axis];
		} // end for i

		lhsCentroid /= 3.0f;
		rhsCentroid /= 3.0f;

		return lhsCentroid < rhsCentroid;
	}

	const maPoint3d* mVertices;
	const std::vector<g3dOcclusionTree::Triangle> &mTriangles;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dOcclusionTree::sort(std::vector<unsigned int>::iterator &begin,
							std::vector<unsigned int>::iterator &end,
							const maPoint3d* vertexPositions,
							const Triangles& triangles,
							const unsigned int axis)
{
	// sort points along this axis
	SortTriangles sorter = {axis, vertexPositions, triangles};
	std::sort(begin, end, sorter);
} // end g3dOcclusionTree::sort()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dOcclusionTree::createApproximatingDisc(const Triangle &tri,
												Disc &disc) const
{
	computeTriangleAreaAndNormal(tri, disc.m_Area, disc.m_Normal);

	disc.m_Centroid = m_Positions[tri[0]]
		+ m_Positions[tri[1]]
		+ m_Positions[tri[2]];
	disc.m_Centroid /= 3.0f;
} // end g3dOcclusionTree::createApproximatingDisc()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dOcclusionTree::computeTriangleAreaAndNormal(const Triangle &tri,
													float &area,
													Normal &n) const
{
	const float3 &v0 = m_Positions[tri[0]];
	const float3 &v1 = m_Positions[tri[1]];
	const float3 &v2 = m_Positions[tri[2]];

	float3 e1 = v1 - v0;
	float3 e2 = v2 - v0;

	n = e1.Cross(e2);
	float length = n.Length();
	n /= length;
	area = 0.5f * length;
} // end g3dOcclusionTree::computeTriangleAreaAndNormal()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dOcclusionTree::createApproximatingDisc(const Disc &d0,
												const Disc &d1,
												Disc &disc) const
{
	disc.m_Area = d0.m_Area + d1.m_Area;
	float w0 = d0.m_Area / disc.m_Area;
	float w1 = d1.m_Area / disc.m_Area;

	disc.m_Normal = w0 * d0.m_Normal + w1 * d1.m_Normal;
	disc.m_Normal.Normalize();

	disc.m_Centroid = w0 * d0.m_Centroid + w1 * d1.m_Centroid;

	if (disc.m_Centroid[0] != disc.m_Centroid[0])
	{
		DBG_WARNING("g3dOcclusionTree::createApproximatingDisc(): nan centroid.");
		disc.m_Centroid = (d0.m_Centroid + d1.m_Centroid) / 2.0f;
	} // end if

	if (disc.m_Normal[0] != disc.m_Normal[0])
	{
		DBG_WARNING("g3dOcclusionTree::createApproximatingDisc(): nan normal.");
		disc.m_Normal = disc.m_Centroid;
		disc.m_Normal.Normalize();
	} // end if
} // end g3dOcclusionTree::createApproximatingDisc()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dOcclusionTree::createApproximatingDisc(const Node &n0,
												const Node &n1,
												Disc &disc) const
{
	createApproximatingDisc(n0.m_Disc, n1.m_Disc, disc);
} // end g3dOcclusionTree::createApproximatingDisc()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dOcclusionTree::NodeIndex g3dOcclusionTree::computeNextIndex(const NodeIndex i) const
{
	NodeIndex result = m_RootIndex;

	// case 1
	// there is no next node to visit after the root
	if (i == m_RootIndex)
	{
		result = NULL_NODE;
	}
	else
	{
		// case 2
		// if i am my parent's left child, return my brother
		result = computeRightBrotherIndex(i);
		if (result == NULL_NODE)
		{ 
			// case 3
			// return my father's next
			result = computeNextIndex(m_Nodes[i].m_Parent);
		}
	}

	return result;
} // end g3dOcclusionTree::computeNextIndex()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dOcclusionTree::NodeIndex g3dOcclusionTree::computeRightBrotherIndex(const NodeIndex i) const
{
	NodeIndex result = NULL_NODE;

	const Node &node = m_Nodes[i];
	if (i == m_Nodes[node.m_Parent].m_LeftChild)
	{
		result = m_Nodes[node.m_Parent].m_RightChild;
	}

	return result;
} // g3dOcclusionTree::computeRightBrotherIndex()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static float rsqrt(float v)
{
	return 1.0f / sqrt(v);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static float saturate(float v)
{
	return std::max<float>(0.0f, std::min<float>(1.0f, v));
}

//============================================================================
//============================================================================
#ifndef PI
#define PI 3.14159265f
#endif // PI

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static float solidAngle(float3 v, float d2, float3 receiverNormal,
                        float3 emitterNormal, float emitterArea)
{
  //return (1.0f - rsqrt(emitterArea / (PI * d2) + 1.0f)) *
  //  saturate(emitterNormal.dot(v)) *
  //  saturate(3.0f * receiverNormal.dot(v));
	float result = emitterArea
		* saturate(emitterNormal.Dot(-v))
		* saturate(receiverNormal.Dot(v))
		/ (d2 + emitterArea / PI); 
	return result / PI;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float g3dOcclusionTree::computeOcclusion(const float3 &p,
										const float3 &n,
										const float epsilon) const
{
	float result = 0.0f;
	float3 v;
	float3 bentNormal = n;
	float d2;
	float contribution;
	float eArea;

	unsigned int currentNode = m_RootIndex;
	while(currentNode != NULL_NODE)
	{
		const Node &node = m_Nodes[currentNode];

		Disc disc = node.m_Disc;

		v = disc.m_Centroid - p;
		d2 = v.Dot(v) + 1e-16f;
		eArea = disc.m_Area;

		// we have to stop if:
		// there's no left child under this node
		// or we are approximating and we are far enough away from this element
		if (node.m_LeftChild == NULL_NODE
		   || (d2 >= epsilon*eArea))
		{
			// compute contribution from this element
			v /= sqrt(d2);
			contribution = solidAngle(v, d2, n,
									disc.m_Normal, eArea);

			// modulate by last result
			contribution *= node.m_Occlusion;
			bentNormal -= contribution * v;
			result += contribution;

			// step across the hierarchy
			currentNode = node.m_NextNode;
		}
		else
		{
			// traverse deeper
			currentNode = node.m_LeftChild;
		}
	}

	result = saturate(1.0f - result);
	return result;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void visibleQuad(const float3 &p, const float3 &n,
				const float3 &v0, const float3 &v1, const float3 &v2,
				float3 &q0, float3 &q1, float3 &q2, float3 &q3,
				const bool verbose)
{
	static const float epsilon = 1e-6f;
	float c = n.Dot(p);

	// Compute the signed distances from the vertices to the plane.
	float sd[3];
	sd[0] = n.Dot(v0) - c;
	if (fabs(sd[0]) <= epsilon) sd[0] = 0;
	sd[1] = n.Dot(v1) - c;
	if (fabs(sd[1]) <= epsilon) sd[1] = 0;
	sd[2] = n.Dot(v2) - c;
	if (fabs(sd[2]) <= epsilon) sd[2] = 0;

	if (sd[0] > 0)
	{
		if (sd[1] > 0)
		{
			if (sd[2] > 0)
			{
				// +++
				q0 = v0;
				q1 = v1;
				q2 = v2;
				q3 = q2;
			}
			else if (sd[2] < 0)
			{
				// ++-
				q0 = v0;
				q1 = v1;
				q2 = v1+(sd[1]/(sd[1]-sd[2]))*(v2-v1);
				q3 = v0+(sd[0]/(sd[0]-sd[2]))*(v2-v0);
			}
			else
			{
				// ++0
				q0 = v0;
				q1 = v1;
				q2 = v2;
				q3 = q2;
			}
		}
		else if (sd[1] < 0)
		{
			if (sd[2] > 0)
			{
				// +-+
				q0 = v0;
				q1 = v0+(sd[0]/(sd[0]-sd[1]))*(v1-v0);
				q2 = v1+(sd[1]/(sd[1]-sd[2]))*(v2-v1);
				q3 = v2;
			}
			else if (sd[2] < 0)
			{
				// +--
				q0 = v0;
				q1 = v0+(sd[0]/(sd[0]-sd[1]))*(v1-v0);
				q2 = v0+(sd[0]/(sd[0]-sd[2]))*(v2-v0);
				q3 = q2;
			}
			else
			{
				// +-0
				q0 = v0;
				q1 = v0+(sd[0]/(sd[0]-sd[1]))*(v1-v0);
				q2 = v2;
				q3 = q2;
			}
		}
		else
		{
			if (sd[2] > 0)
			{
				// +0+
				q0 = v0;
				q1 = v1;
				q2 = v2;
				q3 = q2;
			}
			else if (sd[2] < 0)
			{
				// +0-
				q0 = v0;
				q1 = v1;
				q2 = v0+(sd[0]/(sd[0]-sd[2]))*(v2-v0);
				q3 = q2;
			}
			else
			{
				// +00
				q0 = v0;
				q1 = v1;
				q2 = v2;
				q3 = q2;
			}
		}
	}
	else if (sd[0] < 0)
	{
		if (sd[1] > 0)
		{
			if (sd[2] > 0)
			{
				// -++
				q0 = v0+(sd[0]/(sd[0]-sd[1]))*(v1-v0);
				q1 = v1;
				q2 = v2;
				q3 = v0+(sd[0]/(sd[0]-sd[2]))*(v2-v0);
			}
			else if (sd[2] < 0)
			{
				// -+-
				q0 = v0+(sd[0]/(sd[0]-sd[1]))*(v1-v0);
				q1 = v1;
				q2 = v1+(sd[1]/(sd[1]-sd[2]))*(v2-v1);
				q3 = q2;
			}
			else
			{
				// -+0
				q0 = v0+(sd[0]/(sd[0]-sd[1]))*(v1-v0);
				q1 = v1;
				q2 = v2;
				q3 = q2;
			}
		}
		else if (sd[1] < 0)
		{
			if (sd[2] > 0)
			{
				// --+
				q0 = v0+(sd[0]/(sd[0]-sd[2]))*(v2-v0);
				q1 = v1+(sd[1]/(sd[1]-sd[2]))*(v2-v1);
				q2 = v2;
				q3 = q2;
			}
			else if (sd[2] < 0)
			{
				// ---
				q0 = q1 = q2 = q3 = p;
			}
			else
			{
				// --0
				q0 = q1 = q2 = q3 = p;
			}
		}
		else
		{
			if (sd[2] > 0)
			{
				// -0+
				q0 = v0+(sd[0]/(sd[0]-sd[2]))*(v2-v0);
				q1 = v1;
				q2 = v2;
				q3 = q2;
			}
			else if (sd[2] < 0)
			{
				// -0-
				q0 = q1 = q2 = q3 = p;
			}
			else
			{
				// -00
				q0 = q1 = q2 = q3 = p;
			}
		}
	}
	else
	{
		if (sd[1] > 0)
		{
			if (sd[2] > 0)
			{
				// 0++
				q0 = v0;
				q1 = v1;
				q2 = v2;
				q3 = q2;
			}
			else if (sd[2] < 0)
			{
				// 0+-
				q0 = v0;
				q1 = v1;
				q2 = v1+(sd[1]/(sd[1]-sd[2]))*(v2-v1);
				q3 = q2;
			}
			else
			{
				// 0+0
				q0 = v0;
				q1 = v1;
				q2 = v2;
				q3 = q2;
			}
		}
		else if (sd[1] < 0)
		{
			if (sd[2] > 0)
			{
				// 0-+
				q0 = v0;
				q1 = v1+(sd[1]/(sd[1]-sd[2]))*(v2-v1);
				q2 = v2;
				q3 = q2;
			}
			else if (sd[2] < 0)
			{
				// 0--
				q0 = q1 = q2 = q3 = p;
			}
			else
			{
				// 0-0
				q0 = q1 = q2 = q3 = p;
			}
		}
		else
		{
			if (sd[2] > 0)
			{
				// 00+
				q0 = v0;
				q1 = v1;
				q2 = v2;
				q3 = q2;
			}
			else if (sd[2] < 0)
			{
				// 00-
				q0 = q1 = q2 = q3 = p;
			}
			else
			{
				// 000
				q0 = q1 = q2 = q3 = p;
			}
		}
	}
} // end visibleQuad()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float clamp(float m, float M, float val)
{
	return std::max(m, std::min(M, val));
} // end clamp()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float g3dOcclusionTree::computeFormFactor(const float3 &p, const float3 &n,
											const float3 &q0, const float3 &q1,
											const float3 &q2, const float3 &q3,
											const bool verbose)
{
  float3 r0 = q0 - p;
//  r0 = r0.normalize();
  r0.Normalize();

  float3 r1 = q1 - p;
//  r1 = r1.normalize();
  r1.Normalize();

  float3 r2 = q2 - p;
//  r2 = r2.normalize();
  r2.Normalize();

  float3 r3 = q3 - p;
//  r3 = r3.normalize();
  r3.Normalize();

  float3 g0 = r1.Cross(r0); g0.Normalize();
  float3 g1 = r2.Cross(r1); g1.Normalize();
  float3 g2 = r3.Cross(r2); g2.Normalize();
  float3 g3 = r0.Cross(r3); g3.Normalize();

  float a = acosf(clamp(-1.0f, 1.0f, r0.Dot(r1)));
  float dot = clamp(-1.0f, 1.0f, n.Dot(g0));
  float contrib = a * dot;
  float result = contrib;

  if (verbose)
  {
    DBG_WARNING("a: " << a);
    DBG_WARNING("dot: " << dot);
    DBG_WARNING("contrib: " << contrib);
    DBG_WARNING("result: " << result);
  } // end if

  a = acosf(clamp(-1.0f, 1.0f, r1.Dot(r2)));
  dot = clamp(-1.0f, 1.0f, n.Dot(g1));
  contrib = a * dot;
  result += contrib;

  if (verbose)
  {
    DBG_WARNING("a: " << a);
    DBG_WARNING("dot: " << dot);
    DBG_WARNING("contrib: " << contrib);
    DBG_WARNING("result: " << result);
  } // end if

  a = acosf(clamp(-1.0f, 1.0f, r2.Dot(r3)));
  dot = clamp(-1.0f, 1.0f, n.Dot(g2));
  contrib = a * dot;
  result += contrib;

  if (verbose)
  {
    DBG_WARNING("a: " << a);
    DBG_WARNING("dot: " << dot);
    DBG_WARNING("contrib: " << contrib);
    DBG_WARNING("result: " << result);
  } // end if

  a = acosf(clamp(-1.0f, 1.0f, r3.Dot(r0)));
  dot = clamp(-1.0f, 1.0f, n.Dot(g3));
  contrib = a * dot;
  result += contrib;

  if (verbose)
  {
    DBG_WARNING("a: " << a);
    DBG_WARNING("dot: " << dot);
    DBG_WARNING("contrib: " << contrib);
    DBG_WARNING("result: " << result);
  } // end if

  result *= 0.5f;
  result /= PI;

  return std::max(0.0f, result);
} // end g3dOcclusionTree::computeFormFactor()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float g3dOcclusionTree::computeFormFactor(const float3 &p, const float3 &n,
										  const float3 &v0, const float3 &v1, const float3 &v2)
{
  float3 r0 = v0 - p;
//  r0 = r0.normalize();
  r0.Normalize();

  float3 r1 = v1 - p;
//  r1 = r1.normalize();
  r1.Normalize();

  float3 r2 = v2 - p;
//  r2 = r2.normalize();
  r2.Normalize();

  float3 g0 = r1.Cross(r0); g0.Normalize();
  float3 g1 = r2.Cross(r1); g1.Normalize();
  float3 g2 = r0.Cross(r2); g2.Normalize();

  float a = acosf(clamp(-1.0f, 1.0f, r0.Dot(r1)));
  float dot = clamp(-1.0f, 1.0f, n.Dot(g0));
  float contrib = a * dot;
  float result = contrib;

  a = acosf(clamp(-1.0f, 1.0f, r1.Dot(r2)));
  dot = clamp(-1.0f, 1.0f, n.Dot(g1));
  contrib = a * dot;
  result += contrib;

  a = acosf(clamp(-1.0f, 1.0f, r2.Dot(r0)));
  dot = clamp(-1.0f, 1.0f, n.Dot(g2));
  contrib = a * dot;
  result += contrib;

  result *= 0.5f;
  result /= PI;

  return std::max(0.0f, result);
} // end g3dOcclusionTree::computeFormFactor()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float g3dOcclusionTree::computeFormFactor(const float3 &p, const float3 &n,
										  const Triangle &tri, const float eArea, const bool verbose) const
{
  float3 triNorm = (m_Positions[tri[1]] - m_Positions[tri[0]])
    .Cross(m_Positions[tri[2]] - m_Positions[tri[0]]);
  triNorm.Normalize();
  
  float3 q0,q1,q2,q3;
  visibleQuad(p,n,
              m_Positions[tri[0]], m_Positions[tri[1]], m_Positions[tri[2]],
              q0, q1, q2, q3, verbose);
  return computeFormFactor(p,n,q0,q1,q2,q3,verbose);
} // end g3dOcclusionTree::computeFormFactor()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float g3dOcclusionTree::computeOcclusionUseTriangles(const float3 &p,
													 const float3 &n,
													 const float epsilon) const
{
  float result = 0.0f;
  float3 v;
  float3 bentNormal = n;
  float d2;
  float contribution = 0.0f;
  float eArea;

  unsigned int currentNode = m_RootIndex;
  while(currentNode != NULL_NODE)
  {
    const Node &node = m_Nodes[currentNode];

    Disc disc = node.m_Disc;

    v = disc.m_Centroid - p;
    d2 = v.Dot(v) + 1e-16f;
    eArea = disc.m_Area;

    // we have to stop if:
    // there's no left child under this node
    // or we are approximating and we are far enough away from this element
    if (node.m_LeftChild == NULL_NODE
       || (d2 >= epsilon*eArea))
    {
      // compute contribution from this element
      v /= sqrt(d2);

      if (node.m_LeftChild == NULL_NODE
         && node.m_RightChild == NULL_NODE)
      {
        // compute the contribution of the triangle
        contribution = computeFormFactor(p, n, m_Triangles[currentNode], eArea, false);
      } // end if
      else
      {
        contribution = solidAngle(v, d2, n,
                                  disc.m_Normal, eArea);
      } // end else


      // modulate by last result
      contribution *= node.m_Occlusion;


      bentNormal -= contribution * v;
      result += contribution;

      // step across the hierarchy
      currentNode = node.m_NextNode;
    }
    else
    {
      // traverse deeper
      currentNode = node.m_LeftChild;
    }
  }

  result = saturate(1.0f - result);
  return result;
} // end g3dOcclusionTree::computeOcclusionUseTriangles()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dOcclusionTree::computeOcclusionPasses(const unsigned int numPasses,
											  const float epsilon)
{
  std::vector<float> occlusion(m_Nodes.size());

  for (unsigned int pass = 0; pass < numPasses; ++pass)
  {
    for (unsigned int i = 0;i != m_Nodes.size(); ++i)
    {
      occlusion[i] = computeOcclusion(m_Nodes[i].m_Disc.m_Centroid,
                                      m_Nodes[i].m_Disc.m_Normal,
                                      epsilon);
    } // end for i

    // now assign occlusion
    for (unsigned int i = 0; i != occlusion.size(); ++i)
    {
      if (pass == numPasses - 1)
      {
        // force convergence
        float m = std::min(m_Nodes[i].m_Occlusion, occlusion[i]);
        float M = std::max(m_Nodes[i].m_Occlusion, occlusion[i]);

        // since this method tends to overestimate occlusion, bias towards the smaller value
        m_Nodes[i].m_Occlusion = 0.70f * m + 0.30f * M;
      } // end if
      else
      {
        m_Nodes[i].m_Occlusion = occlusion[i];
      } // end else
    }

    DBG_WARNING("g3dOcclusionTree::computeOcclusionPasses(): Finished pass " << pass);
  } // end for pass
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dOcclusionTree::~g3dOcclusionTree()
{
	delete [] m_Positions;
}

