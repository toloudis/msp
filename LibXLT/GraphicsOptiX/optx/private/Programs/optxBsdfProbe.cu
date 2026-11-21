/*****************************************************************************
**  optxBsdfProbe.cu
**
**      The BRDF probe: see optxBsdfProbeParams.h. Compiled to OptiX IR by
**	nvcc at build time and embedded as "optxBsdfProbe". Entry points:
**	__raygen__bsdf, and an empty miss and closest hit (the pipeline
**	traces no rays, but its shader binding table has a record of each
**	kind).
**
**	Extra Large Technology
\****************************************************************************/
#include <optix.h>

#ifndef GOX_OPTXVECTOR_H
#include "optxVector.h"
#endif
#ifndef GOX_OPTXBSDF_H
#include "optxBsdf.h"
#endif
#ifndef GOX_OPTXBSDFPROBEPARAMS_H
#include "optxBsdfProbeParams.h"
#endif

using namespace gox;
using namespace gox::optxv;

extern "C"
{
	__constant__ optxBsdfProbeParams params;
}

namespace
{
	__forceinline__ __device__ void store3(float* o_P, float3 i_V)
	{
		o_P[0] = i_V.x;
		o_P[1] = i_V.y;
		o_P[2] = i_V.z;
	}
}

extern "C" __global__ void __raygen__bsdf()
{
	const unsigned int i = optixGetLaunchIndex().x;
	if (i >= params.m_Count)
	{
		return;
	}
	const optxBsdfQuery& q = reinterpret_cast<const optxBsdfQuery*>(params.m_Queries)[i];
	const optxb::Bsdf bsdf = optxb::make_bsdf(load3(q.m_BaseColor), q.m_Metallic, q.m_Roughness, q.m_Specular,
											  load3(q.m_SpecularColor), q.m_Ior);
	const float3 n = load3(q.m_Normal);
	const float3 wo = load3(q.m_Wo);
	const optxb::Value value = optxb::evaluate(bsdf, n, wo, load3(q.m_Wi));
	const optxb::Sample sample = optxb::sample(bsdf, n, wo, q.m_U[0], q.m_U[1], q.m_U[2]);

	optxBsdfResult r;
	store3(r.m_Diffuse, value.m_Diffuse);
	r.m_Pdf = value.m_Pdf;
	store3(r.m_Specular, value.m_Specular);
	r.m_Lobe = sample.m_Lobe;
	store3(r.m_SampleWi, sample.m_Wi);
	r.m_SamplePdf = sample.m_Lobe != optxb::e_LobeNone ? sample.m_Value.m_Pdf : 0.0f;
	store3(r.m_SampleWeight, sample.m_Weight);
	r.m_Pad = 0.0f;
	reinterpret_cast<optxBsdfResult*>(params.m_Results)[i] = r;
}

extern "C" __global__ void __miss__none()
{
}

extern "C" __global__ void __closesthit__none()
{
}
