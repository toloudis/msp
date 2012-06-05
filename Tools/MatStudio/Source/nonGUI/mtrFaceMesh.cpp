/****************************************************************************\
**	mtrFaceMesh.hpp
**
**	A mtrFaceMesh keeps track of faces in mesh for picking
**	materials
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "mtrFaceMesh.hpp"

#include "dbgLog.hpp"
#include "geoRayIntersection.hpp"
#include "maConstants.hpp"
#include "maFunctions.hpp"
#include "maMatrix4x4.hpp"

namespace
{
}

//============================================================================
//============================================================================
mtrFaceMesh::FaceRef::FaceRef(envType::UInt32 i0, envType::UInt32 i1, envType::UInt32 i2)
{
	index[0] = i0;
	index[1] = i1;
	index[2] = i2;
}


//============================================================================
//============================================================================
mtrFaceMesh::mtrFaceMesh()
{
}

//============================================================================
//============================================================================
mtrFaceMesh::mtrFaceMesh(const maPoint3d* i_Points, 
				   int i_NumPoints,
				   const envType::UInt32* i_Indices, 
				   int i_NumIndices)
{
	int num_tris = i_NumIndices / 3;
	DBG_ASSERT0(3*num_tris == i_NumIndices, "Indices not in triangles");
	//DBG_ASSERT0(num_tris < 32768, "Too many tris for 'short'.");

	m_Points.resize(i_NumPoints);
	for (int i=0; i<i_NumPoints; i++)
		m_Points[i] = i_Points[i];

	m_Faces.resize(num_tris);
	envType::UInt32 i0, i1, i2;
	i=0;
	for (int t=0; t<num_tris; t++, i+=3)
	{
		i0 = i_Indices[i];
		i1 = i_Indices[i+1];
		i2 = i_Indices[i+2];
		m_Faces[t] = FaceRef(i0, i1, i2);

		// could compute face normal here to optimize
	}

}

//============================================================================
//============================================================================
int mtrFaceMesh::PickFace( const maPoint3d &i_RayStart, 
						  const maVector3d &i_RayDir,
					      float &o_Tval)
{

	int num_faces = m_Faces.size();
	float tval;
	o_Tval = 999999.9f;
	int pick_face = -1;
	for (int i=0; i<num_faces; i++)
	{
		FaceRef &face = m_Faces[i];
		if (geoRayIntersection::IntersectLineTriangle(	i_RayStart, i_RayDir, 
						m_Points[face.index[0]], m_Points[face.index[1]], 
						m_Points[face.index[2]], tval))
		{
			if (tval < o_Tval)
			{
				pick_face = i;
				o_Tval = tval;

//				DBG_LOG3("Picked face: %d %d %d", face.index[0], face.index[1], face.index[2]);
			}
		}
	}
		
	return pick_face;
}
