/*****************************************************************************
**  optxTextureProbeParams.h
**
**      The launch parameters of the texture probe (optxTextureProbe.cu):
**	one launch of m_Count threads, each sampling texture m_Index of a
**	texture table (optxMaterialTypes.h's optxTexture, with its swizzle) at
**	one query (u, v, level; float4 with w unused), writing the float4 it
**	reads. For tests: what CUDA reads from a Vulkan texture.
**
**	Plain C structs: the host and nvcc must lay them out the same way.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXTEXTUREPROBEPARAMS_H
#error optxTextureProbeParams.h multiply included
#endif
#define GOX_OPTXTEXTUREPROBEPARAMS_H

namespace gox
{
	struct optxTextureProbeParams
	{
		unsigned long long	m_Textures;		// device address of the optxTexture table
		unsigned long long	m_Queries;		// device address of m_Count float4 (u, v, level, -)
		unsigned long long	m_Results;		// device address of m_Count float4
		unsigned int		m_Index;		// the texture's index in the table
		unsigned int		m_Count;
	};
}
