/*****************************************************************************
**  optxTextureProbe.cu
**
**      The texture probe: see optxTextureProbeParams.h. Compiled to OptiX
**	IR by nvcc at build time and embedded as "optxTextureProbe". Entry
**	points: __raygen__probe, and an empty miss and closest hit (the
**	pipeline traces no rays, but its shader binding table has a record of
**	each kind).
**
**	Extra Large Technology
\****************************************************************************/
#include <optix.h>

#ifndef GOX_OPTXSCENETYPES_H
#include "optxSceneTypes.h"
#endif
#ifndef GOX_OPTXMATERIALTYPES_H
#include "optxMaterialTypes.h"
#endif
#ifndef GOX_OPTXMATERIAL_H
#include "optxMaterial.h"
#endif
#ifndef GOX_OPTXTEXTUREPROBEPARAMS_H
#include "optxTextureProbeParams.h"
#endif

using namespace gox;

extern "C"
{
	__constant__ optxTextureProbeParams params;
}

extern "C" __global__ void __raygen__probe()
{
	const unsigned int i = optixGetLaunchIndex().x;
	if (i >= params.m_Count)
	{
		return;
	}
	const float4 query = reinterpret_cast<const float4*>(params.m_Queries)[i];
	const optxTexture& texture = reinterpret_cast<const optxTexture*>(params.m_Textures)[params.m_Index];
	reinterpret_cast<float4*>(params.m_Results)[i] = optxm::sample(texture, query.x, query.y, query.z);
}

extern "C" __global__ void __miss__none()
{
}

extern "C" __global__ void __closesthit__none()
{
}
