/*****************************************************************************
**  InstanceWork.hpp
**  The core algorithm for guessing the transformation
**  from one mesh to another mesh.
**
**  This is written independent of the 3ds Max API
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#if !defined( MAX_INSTANCEWORK_HPP )
#define  MAX_INSTANCEWORK_HPP 

#include <vector>

#define TIME_EXPORT_START 0


class InstanceWork
{
public:
	InstanceWork(){}
	//========================================================================
	//	Main function which does the job. 
	//  Each of the two input meshes has the same number of vertices.
	//	meshVerts1 is vector of floats of size 4*n, contains the vertex info
	//  for the first mesh.
	//  meshVerts1 = [ v0.x, v0.y, v0.z, 1.0, v1.x, v1.y, v1.z , 1.0, ...... vn-1.x, vn-1.y, vn-1.z, 1.0 ]
	//  meshVerts2 contains the vertex info for the second mesh.
	//  o_Xform is the transform that transforms the vrtices of the
	//  second mesh into the vrtices of the first mesh.
	//  This routine returns the maximum error involved in representing
	//  any component of any vertexof mesh1 using the transformation and
	// the corresponding vertex of mesh2.
	//========================================================================

	float Do(
		int nVert,  
		const std::vector< float > &i_MeshVerts1,
		const std::vector< float > &i_MeshVerts2,
		std::vector< float > &o_Xform);

	~InstanceWork(){}
	
};

#endif
