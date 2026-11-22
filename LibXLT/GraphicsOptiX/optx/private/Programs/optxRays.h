/*****************************************************************************
**  optxRays.h
**
**      Rays for the lighting programs (optxLighting.cu): projection
**	through the view-projection matrices, the camera-ray hit (Hit, as __closesthit__camera reports it) and the
**	occlusion test. Device only (nvcc); include in a program after its
**	launch parameters `params` (an optxLightingParams: m_Handle) are
**	declared, after optix.h, optxVector.h and the scene types.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXRAYS_H
#error optxRays.h multiply included
#endif
#define GOX_OPTXRAYS_H

namespace gox
{
namespace optxr
{
	using namespace optxv;

	__forceinline__ __device__ float3 read_float3(unsigned long long i_Address)
	{
		const float* p = reinterpret_cast<const float*>(i_Address);
		return make_float3(p[0], p[1], p[2]);
	}

	//	(x, y, z, 1) * M, divided by w (row vectors, row-major M)
	__forceinline__ __device__ float3 project(const float* M, float x, float y, float z, float& o_W)
	{
		const float px = x * M[0] + y * M[4] + z * M[8] + M[12];
		const float py = x * M[1] + y * M[5] + z * M[9] + M[13];
		const float pz = x * M[2] + y * M[6] + z * M[10] + M[14];
		o_W = x * M[3] + y * M[7] + z * M[11] + M[15];
		const float inv = 1.0f / o_W;
		return make_float3(px * inv, py * inv, pz * inv);
	}

	struct Hit
	{
		bool			m_bHit;
		float			m_T;
		float3			m_Geometric;	// geometric normal, facing the ray
		float3			m_Shading;		// interpolated vertex normal, on the geometric normal's side
		unsigned int	m_Instance;
		unsigned int	m_Material;		// index, or c_optxNoMaterial
		float2			m_UV;			// transformed by the material's UV transform
		float3			m_S;			// world tangents (raster's tangent and binormal), flipped with the normal
		float3			m_T3;
		float			m_LodBase;		// 0.5 log2(texel area / world area) of the triangle
	};

	__forceinline__ __device__ Hit trace_camera(float3 i_Origin, float3 i_Direction, float i_TMax)
	{
		unsigned int p[19] = {};
		if (params.m_Handle)
		{
			optixTrace(params.m_Handle, i_Origin, i_Direction, 0.0f, i_TMax, 0.0f,
					   OptixVisibilityMask(e_optxMaskCamera), OPTIX_RAY_FLAG_NONE,
					   0, 1, 0,		// SBT offset, stride, miss index 0
					   p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14],
					   p[15], p[16], p[17], p[18]);
		}
		Hit result;
		result.m_bHit = p[8] != 0;
		result.m_T = __uint_as_float(p[0]);
		result.m_Geometric = make_float3(__uint_as_float(p[1]), __uint_as_float(p[2]), __uint_as_float(p[3]));
		result.m_Shading = make_float3(__uint_as_float(p[4]), __uint_as_float(p[5]), __uint_as_float(p[6]));
		result.m_Instance = p[7];
		result.m_Material = p[9];
		result.m_UV = make_float2(__uint_as_float(p[10]), __uint_as_float(p[11]));
		result.m_S = make_float3(__uint_as_float(p[12]), __uint_as_float(p[13]), __uint_as_float(p[14]));
		result.m_T3 = make_float3(__uint_as_float(p[15]), __uint_as_float(p[16]), __uint_as_float(p[17]));
		result.m_LodBase = __uint_as_float(p[18]);
		return result;
	}

	//	Whether anything that casts shadows (i_Mask: the instances of that
	//	visibility mask, e_optxMaskCamera for every surface the camera sees)
	//	lies along the ray before i_TMax
	__forceinline__ __device__ bool occluded(float3 i_Origin, float3 i_Direction, float i_TMax,
											 unsigned int i_Mask = e_optxMaskOcclusion)
	{
		if (!params.m_Handle)
		{
			return false;
		}
		unsigned int hit = 1;
		optixTrace(params.m_Handle, i_Origin, i_Direction, 0.0f, i_TMax, 0.0f,
				   OptixVisibilityMask(i_Mask),
				   OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT | OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT,
				   0, 1, 1,		// SBT offset, stride, miss index 1
				   hit);
		return hit != 0;
	}

	//	A shadow ray's length towards a point i_Distance away (c_Infinity:
	//	a direction), stopping short of it
	__forceinline__ __device__ float shadow_tmax(float i_Distance)
	{
		return i_Distance < c_Infinity ? i_Distance * (1.0f - 1e-4f) - params.m_Epsilon : c_Infinity;
	}
}
}
