/****************************************************************************\
**  MeshUtil.cpp
**
**      MeshUtil contains function for manipulating triangle meshes.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include "MeshUtil.hpp"


#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MStatus.h>

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"

#include <map>
#include <set>

//============================================================================
//============================================================================
namespace MeshUtil
{

//============================================================================
//============================================================================
namespace
{

const float c_SmoothThreshold = 0.6f;
const int c_CodeBreak = 8;

// control of smoothing
bool l_bRedoNormals = true;
bool l_bAllSmooth = false;	
bool l_bFacetAngledFaces = true;
//bool l_bTwinMeshWelding = false;

		inline bool close_enough(const maPoint3d& i_A, const maPoint3d& i_B)
		{
			static maPoint3d C;
			C = i_A - i_B;
			if (fabsf(C.m_X) > maConstants::c_fEpsilon) return false;
			if (fabsf(C.m_Y) > maConstants::c_fEpsilon) return false;
			if (fabsf(C.m_Z) > maConstants::c_fEpsilon) return false;
			return (C.LengthSqr() < maConstants::c_fEpsilon);
		}

		inline int get_code(const maPoint3d& i_A, const maPoint3d& i_Off, 
							const maVector3d &i_Mul)
		{
			static maPoint3d vec;
			static float code;
			vec = i_A - i_Off;
			code = vec.m_X * i_Mul.m_X;
			code = code * c_CodeBreak + vec.m_Y * i_Mul.m_Y;
			code = code * c_CodeBreak + vec.m_Z * i_Mul.m_Z;
			return int(code);
		}

		// Find vertices with same geometric position,
		// create re-indexing
/*		void create_reindex(const maPoint3d* i_Vertices,
							int i_NumVertices,
							std::vector<IndexType> &o_Reindex)
		{
			maAxisBox box;
			box.Union(i_Vertices, i_NumVertices);
			maPoint3d offset = box.GetBoxPoint(7);
			maVector3d diff = box.GetBoxPoint(0) - offset;
			if (diff.m_X > 0.0f) diff.m_X = float(c_CodeBreak) / diff.m_X;
			if (diff.m_Y > 0.0f) diff.m_Y = float(c_CodeBreak) / diff.m_Y;
			if (diff.m_Z > 0.0f) diff.m_Z = float(c_CodeBreak) / diff.m_Z;

			o_Reindex.resize(i_NumVertices);
			std::vector<int> code(i_NumVertices);

			int i, j;
			for (i=0; i<i_NumVertices; i++)
			{
				code[i] = get_code(i_Vertices[i], offset, diff);
				o_Reindex[i] = i;
				for (j=0; j<i; j++)
				{
					if ((o_Reindex[j] == j) && (code[j] == code[i]) &&
							close_enough(i_Vertices[i], i_Vertices[j]))
					{
						o_Reindex[i] = j;
						break;
					}
				}
			}
		}
*/
		struct edge_vertex
		{
			edge_vertex() {}
			edge_vertex(IndexType i_Gi,IndexType i_Ni, IndexType i_Ti)
				: m_Gi(i_Gi), m_Ni(i_Ni), m_Ti(i_Ti) { }
			IndexType m_Gi, m_Ni, m_Ti;

			bool operator==(const edge_vertex& i_V) const
			{
				return (i_V.m_Gi == m_Gi && i_V.m_Ni == m_Ni && i_V.m_Ti == m_Ti);
			}
		};

		struct edge_side
		{
			edge_side() {}
			edge_side(const edge_vertex& i_V1, const edge_vertex& i_V2)
				: m_V1(i_V1), m_V2(i_V2), m_bReverse(false) {}

			
			edge_vertex m_V1, m_V2;	// geom, normal, texture indices	
			bool m_bReverse;

			void swap()
			{
				maFunctions::Swap(m_V1, m_V2);
				m_bReverse = true;
			}
		};

		// Typedef for strange data structures
		typedef std::vector<edge_side> EdgeSideVector;
		typedef std::map<IndexType, EdgeSideVector> CornerMap;

				
		// Structure for recording how many times an edge occurs
		struct edge_rec
		{
			edge_rec() : m_I0(0), m_I1(1), m_Count(0), m_ReverseCount(0) {}
			edge_rec(IndexType i_I0, IndexType i_I1) : m_I0(i_I0), m_I1(i_I1), 
									m_Count(0), m_ReverseCount(0) {}

			IndexType m_I0;	//	m_I0 < m_I1
			IndexType m_I1;

			//	these members are mutable so that they can be modified in place in a set
			//	The < operator does not depend on them
			mutable int m_Count;			//	number of times this edge occurs
			mutable int m_ReverseCount;		//	number of times the twin of this edge occurs
			mutable std::vector<edge_side> m_Sides;

			inline bool operator < (const edge_rec& i_CompareTo) const
			{
				if( m_I0 != i_CompareTo.m_I0 )
					return m_I0 < i_CompareTo.m_I0;
				else
					return m_I1 < i_CompareTo.m_I1;
			}

			inline void add_count(bool i_Reverse)
			{
				if (i_Reverse) m_ReverseCount++;
				else m_Count++;
			}
		};




		void debug_indices(const maPoint3d* i_Vertices,
							int i_NumVertices,
							std::vector<IndexType>& io_Indices)
		{
			int num_inds = io_Indices.size();
			cout << "Num indices " << num_inds << endl;
			for (int i=0; i<num_inds; i+=3)
			{
				cout << " I: " << io_Indices[i] << " " <<  io_Indices[i+1] << " " << io_Indices[i+2] << endl;
			}
		}

		
		void add_triangle(std::vector<IndexType>& io_GeomIndices, 
						  std::vector<IndexType>& io_NormIndices, 
						  std::vector<IndexType>& io_UVIndices, 
						  const edge_vertex& v0, 
						  const edge_vertex& v1, 
						  const edge_vertex& v2)
		{
			io_GeomIndices.push_back(v0.m_Gi);
			io_GeomIndices.push_back(v1.m_Gi);
			io_GeomIndices.push_back(v2.m_Gi);
			io_NormIndices.push_back(v0.m_Ni);
			io_NormIndices.push_back(v1.m_Ni);
			io_NormIndices.push_back(v2.m_Ni);
			io_UVIndices.push_back(v0.m_Ti);
			io_UVIndices.push_back(v1.m_Ti);
			io_UVIndices.push_back(v2.m_Ti);
		}

		void submit_corner_edge(CornerMap &io_CornerMap,
								IndexType reindex, 
								const edge_vertex& v0, 
								const edge_vertex& v1)
		{
			EdgeSideVector &side_vector = io_CornerMap[reindex];
			side_vector.push_back(edge_side(v0, v1));
		}

		int get_next_edge(const EdgeSideVector &i_SideVector, 
						  const edge_vertex& i_Vertex)
		{
			for (int i=0; i<i_SideVector.size(); i++)
				if (i_SideVector[i].m_V1 == i_Vertex)
					return i;
			return -1;
		}
		int get_next_edge_relaxed(const EdgeSideVector &i_SideVector, 
						  const edge_vertex& i_Vertex)
		{
			for (int i=0; i<i_SideVector.size(); i++)
				if (i_SideVector[i].m_V1.m_Ni == i_Vertex.m_Ni)
					return i;
			return -1;
		}
		
}

	//========================================================================
	//	FillNonWeldedEdges adds polygons to a mesh between edges that are not
	//	welded together.  This helps to make the mesh a two-manifold.
	//	The function does not need to add any new vertices; 
	//  indices are added for zero-area triangles for welding.
	//	Polygons are added between edges and also at areas where more than
	//	two non-welded vertices meet.
	//========================================================================
	void FillNonWeldedEdges(MFnMesh &mesh, 
							std::vector<IndexType>& o_GeomIndices, 
							std::vector<IndexType>& o_NormIndices, 
							std::vector<IndexType>& o_UVIndices)
	{

		MStatus status;

		int nVerts = mesh.numVertices(&status);
		int nPolys = mesh.numPolygons(&status);
		int nUVs = mesh.numUVs(&status);
		int nNormals = mesh.numNormals(&status);

		std::set<int> int_set;

		// Find matching edges
		std::set<edge_rec> edge_set;
		typedef std::set<edge_rec>::iterator set_it;

		MItMeshPolygon polyIter(mesh.object(), &status);
		for (; !polyIter.isDone(); polyIter.next()) 
		{
			int numVerts = polyIter.polygonVertexCount();
			for (int v = 0; v < numVerts; v++) 
			{
				int nv = (v+1) % numVerts;

				IndexType vi0 = polyIter.vertexIndex(v);
				IndexType vi1 = polyIter.vertexIndex(nv);
				IndexType ni0 = polyIter.normalIndex(v);
				IndexType ni1 = polyIter.normalIndex(nv);

				int ti0=0, ti1=0;
				polyIter.getUVIndex(v, ti0, NULL ) ;
				polyIter.getUVIndex(nv, ti1, NULL ) ;


				bool reverse = false;
				edge_side eside(edge_vertex(vi0, ni0, ti0), edge_vertex(vi1, ni1, ti1));
				if ( vi1 > vi0 )
				{
					reverse = true;
					maFunctions::Swap(vi0, vi1);
					eside.swap();
				}

				edge_rec new_edge_rec(vi0, vi1);
				std::pair<set_it, bool> pairib = edge_set.insert(new_edge_rec);
				pairib.first->add_count(reverse);
				pairib.first->m_Sides.push_back(eside);
			}
		}

		// Corner map handles corners where sharp edges meet
		CornerMap corner_map;				

		// now check each edge in set to see where
		// welding is needed
		set_it cur_it = edge_set.begin();
		set_it end_it = edge_set.end();

		int ccorrect = 0, csingle1 = 0, csingle2 = 0, cdouble = 0;
		for( ; cur_it != end_it ; ++cur_it )
		{
			const edge_rec& edge_data = *cur_it;

			if (edge_data.m_Count == 1 && edge_data.m_ReverseCount == 1)
			{
				if (edge_data.m_Sides.size() != 2)
					MayaUtil::PrintWarning("Wrong edge side count.");

				edge_vertex v01 = edge_data.m_Sides[0].m_V1;
				edge_vertex v02 = edge_data.m_Sides[0].m_V2;
				edge_vertex v11 = edge_data.m_Sides[1].m_V1;
				edge_vertex v12 = edge_data.m_Sides[1].m_V2;

				if (v01.m_Ni == v11.m_Ni && v02.m_Ni == v12.m_Ni)
					ccorrect++;
				else if (v01.m_Ni != v11.m_Ni && v02.m_Ni != v12.m_Ni)
				{
					cdouble++;
					if (edge_data.m_Sides[0].m_bReverse) 
						maFunctions::Swap(v01, v02);
					if (edge_data.m_Sides[1].m_bReverse) 
						maFunctions::Swap(v11, v12);

					add_triangle(o_GeomIndices,o_NormIndices,o_UVIndices, v02, v01, v11);
					add_triangle(o_GeomIndices,o_NormIndices,o_UVIndices, v12, v11, v01);

					submit_corner_edge(corner_map, v02.m_Gi, v02, v11);
					submit_corner_edge(corner_map, v01.m_Gi, v12, v01);
				}
				else
				{
					// One of vertices matches
					if (edge_data.m_Sides[0].m_bReverse) 
						maFunctions::Swap(v01, v02);
					if (edge_data.m_Sides[1].m_bReverse) 
						maFunctions::Swap(v11, v12);

					if (v01.m_Ni == v12.m_Ni)
					{
						csingle1++;
									
						add_triangle(o_GeomIndices,o_NormIndices,o_UVIndices, v02, v01, v11);
						submit_corner_edge(corner_map, v02.m_Gi, v02, v11);

						//DBG_WARNING3("Single1: %d %d %d", i02, i01, i11);
					}
					else 
					{
						if (v02.m_Ni != v11.m_Ni)
							MayaUtil::PrintError("Something has to match!");
						csingle2++;
									
						add_triangle(o_GeomIndices,o_NormIndices,o_UVIndices, v02, v01, v12);
						submit_corner_edge(corner_map, v01.m_Gi, v12, v01);

						//DBG_WARNING3("Single2: %d %d %d", i02, i01, i12);
					}
				}
			}
		}
		
		// Fill in corners
		CornerMap::iterator corner_it = corner_map.begin();
		for( ; corner_it != corner_map.end() ; ++corner_it )
		{
			EdgeSideVector &side_vector = corner_it->second;
			if (side_vector.size() > 2)
			{
				// debug corner info
				/*cout << "Corner at: " << corner_it->first << endl;
				MVector norm;
				float u, v;	
				for (int si=0; si<side_vector.size(); si++)
				{
					edge_side &side = side_vector[si];
					cout << "  Side: ";
					cout << "(" << side.m_V1.m_Gi << "," << side.m_V1.m_Ni << "," << side.m_V1.m_Ti << ") ";
					mesh.getVertexNormal(side.m_V1.m_Ni, norm);
					cout << "[" << norm[0] << "," << norm[1] << "," << norm[2] << "]";
					mesh.getUV(side.m_V1.m_Ti, u, v);
					cout << "[" << u << "," << v << "]";
					cout << "(" << side.m_V2.m_Gi << "," << side.m_V2.m_Ni << "," << side.m_V2.m_Ti << ") ";
					mesh.getVertexNormal(side.m_V2.m_Ni, norm);
					cout << "[" << norm[0] << "," << norm[1] << "," << norm[2] << "]";
					mesh.getUV(side.m_V2.m_Ti, u, v);
					cout << "[" << u << "," << v << "]";
					cout << endl;
				}*/

				std::vector<edge_vertex> corner;
				edge_vertex ind = side_vector[0].m_V2;
				while (!side_vector.empty())
				{
					int next_edge = get_next_edge(side_vector, ind);
					if (next_edge < 0)
					{		
						next_edge = get_next_edge_relaxed(side_vector, ind);
						if (next_edge < 0)
						{		
							MString str("Non 2-manifold shape at corner #");
							str += ind.m_Gi;
							str += " in mesh ";
							str += mesh.name();
							MayaUtil::PrintWarning(str);
							break;
						}
						//else cout << "Got relaxed edge" << endl;
					}
					
					ind = side_vector[next_edge].m_V2;
					corner.push_back(ind);
					//cout << "  Corner (" << ind.m_Gi << "," << ind.m_Ni << "," << ind.m_Ti << ")" << endl;
					side_vector.erase(side_vector.begin() + next_edge);
				}

				if (corner_it->second.empty())
				{
					// Got all edges in order
					for (int i=2; i<corner.size(); i++)
					{		
						//DBG_WARNING3(" Corner tri: %d %d %d", corner[0], corner[i-1], corner[i]);
						add_triangle(o_GeomIndices,o_NormIndices,o_UVIndices, corner[0], corner[i-1], corner[i]);
					}	
				}
			}
		}

		
	}

}

