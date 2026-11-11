/*****************************************************************************
**  optxRayQuery.cu
**
**      The ray query program: one ray per launch index, closest hit only
**	(any-hit programs are off), the result written to m_pHits. See
**	optxRayQueryParams.h.
**
**	Compiled to OptiX IR by nvcc at build time and embedded as
**	"optxRayQuery" (optxEmbeddedPrograms). Entry points: __raygen__rayQuery,
**	__miss__rayQuery, __closesthit__rayQuery; one ray type, one SBT record
**	of each.
**
**	Extra Large Technology
\****************************************************************************/
#include <optix.h>

#ifndef GOX_OPTXRAYQUERYPARAMS_H
#include "optxRayQueryParams.h"
#endif

using namespace gox;

//	The launch parameters: OptiX finds them by name (pipelineLaunchParamsVariableName)
extern "C"
{
	__constant__ optxRayQueryParams params;
}

extern "C" __global__ void __raygen__rayQuery()
{
	const unsigned int i = optixGetLaunchIndex().x;
	if (i >= params.m_Count)
	{
		return;
	}
	const optxRayQueryRay& ray = params.m_pRays[i];

	unsigned int t = 0, instance = 0, primitive = 0, hit = 0;
	optixTrace(params.m_Handle,
			   make_float3(ray.m_Origin[0], ray.m_Origin[1], ray.m_Origin[2]),
			   make_float3(ray.m_Direction[0], ray.m_Direction[1], ray.m_Direction[2]),
			   ray.m_TMin, ray.m_TMax, 0.0f,
			   OptixVisibilityMask(ray.m_Mask & 0xFF),
			   OPTIX_RAY_FLAG_DISABLE_ANYHIT,
			   0, 1, 0,
			   t, instance, primitive, hit);

	optxRayQueryHit& out = params.m_pHits[i];
	out.m_T = __uint_as_float(t);
	out.m_InstanceId = instance;
	out.m_PrimitiveIndex = primitive;
	out.m_Hit = hit;
}

extern "C" __global__ void __miss__rayQuery()
{
	optixSetPayload_3(0);
}

extern "C" __global__ void __closesthit__rayQuery()
{
	optixSetPayload_0(__float_as_uint(optixGetRayTmax()));
	optixSetPayload_1(optixGetInstanceId());
	optixSetPayload_2(optixGetPrimitiveIndex());
	optixSetPayload_3(1);
}
