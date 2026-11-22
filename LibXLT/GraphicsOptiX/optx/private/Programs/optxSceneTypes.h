/*****************************************************************************
**  optxSceneTypes.h
**
**      What the scene BVH (g3dSceneBVHOptiX) gives device programs besides
**	the traversable: the instance table, one entry per IAS instance,
**	indexed by the instance ID (optixGetInstanceId()), and the visibility
**	mask bits rays trace with.
**
**	An instance's GAS has one build input per fragment (one, or a group's
**	fragments, which share their vertices: g3dGasCacheOptiX); each build
**	input has its own SBT record, so optixGetSbtGASIndex() says which
**	fragment a ray hit and indexes the instance's input array. A program
**	finds a hit's vertices through the table: the triangle's three indices
**	(16- or 32-bit, m_IndexSize), then the vertices at m_VertexStride
**	apart, position (float3) at offset 0 and the normal (float3) at
**	m_NormalOffset, the texture coordinates (float2) at m_TexCoordOffset
**	and the tangents S and T (float3, the raster's tangent and binormal)
**	at m_TangentOffset and m_BitangentOffset (c_optxNoAttribute where the
**	format has none); so no GAS needs OPTIX_BUILD_FLAG_ALLOW_RANDOM_VERTEX_-
**	ACCESS. Each build input names its material (an index into the
**	material table, optxMaterialTypes.h).
**
**	Each instance also names its light set (m_LightSet): the lights that
**	reach it (light linking), an index into the light table's sets
**	(optxLightTypes.h), or c_optxAllLights; and its flags
**	(optxInstanceFlags).
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

	//	An attribute offset of a vertex format without that attribute
	const unsigned int c_optxNoAttribute = 0xFFFFFFFFu;
	const unsigned int c_optxNoNormal = c_optxNoAttribute;

	//	m_LightSet of an instance every light reaches
	const unsigned int c_optxAllLights = 0xFFFFFFFFu;

	//	m_Flags bits
	enum optxInstanceFlags
	{
		e_optxInstanceReceivesShadow = 1u << 0	// shadow rays are traced for its points
	};

	//	One build input of a GAS: one fragment's triangles
	struct optxGeometryInput
	{
		unsigned long long	m_Indices;		// device address of its triangles (3 indices each)
		unsigned int		m_IndexSize;	// 2 or 4 bytes
		unsigned int		m_NumTriangles;
		unsigned int		m_Material;		// index into the material table, or c_optxNoMaterial
		unsigned int		m_Pad;
	};

	struct optxInstanceData
	{
		unsigned long long	m_Vertices;		// device address of the vertices
		unsigned long long	m_Inputs;		// device address of m_NumInputs optxGeometryInputs
		unsigned int		m_VertexStride;	// bytes from one vertex to the next
		unsigned int		m_NormalOffset;	// bytes from a vertex's start to its normal, or c_optxNoNormal
		unsigned int		m_NumInputs;
		unsigned int		m_NumTriangles;	// over all inputs
		unsigned int		m_LightSet;		// index of its light set, or c_optxAllLights
		unsigned int		m_Flags;		// optxInstanceFlags
		unsigned int		m_TexCoordOffset;	// or c_optxNoAttribute
		unsigned int		m_TangentOffset;
		unsigned int		m_BitangentOffset;
		unsigned int		m_Pad;
	};
}
