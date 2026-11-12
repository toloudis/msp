/*****************************************************************************
**  optxSceneTypes.h
**
**      What the scene BVH (g3dSceneBVHOptiX) gives device programs besides
**	the traversable: the instance table, one entry per IAS instance,
**	indexed by the instance ID (optixGetInstanceId()), and the visibility
**	mask bits rays trace with.
**
**	A program finds a hit's vertices through the table (the triangle's
**	three indices, then the vertices at m_VertexStride apart, position at
**	offset 0, normal after it: g3dType::BumpTex1Vertex), so no GAS needs
**	OPTIX_BUILD_FLAG_ALLOW_RANDOM_VERTEX_ACCESS. Material data goes here
**	later.
**
**	Plain C structs: the host and nvcc must lay them out the same way.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXSCENETYPES_H
#error optxSceneTypes.h multiply included
#endif
#define GOX_OPTXSCENETYPES_H

namespace gox
{
	//	Visibility mask bits: camera rays see everything but shadow hulls;
	//	occlusion rays see what casts shadows
	enum optxRayMask
	{
		e_optxMaskCamera	= 1u << 0,
		e_optxMaskOcclusion	= 1u << 1
	};

	struct optxInstanceData
	{
		unsigned long long	m_Vertices;		// device address of the fragment's vertices
		unsigned long long	m_Indices;		// device address of its triangles (3 x uint32 each)
		unsigned int		m_VertexStride;	// bytes from one vertex to the next
		unsigned int		m_NumTriangles;
	};
}
