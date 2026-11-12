/*****************************************************************************
**  optxAO.cu
**
**      The progressive ambient occlusion programs: see optxAOParams.h.
**
**	Compiled to OptiX IR by nvcc at build time and embedded as "optxAO"
**	(optxEmbeddedPrograms). Entry points: __raygen__ao and
**	__raygen__resolve (one launch each), __miss__camera (miss record 0),
**	__miss__occlusion (miss record 1), __closesthit__camera (every hit
**	group record: one per build input of the biggest GAS). Occlusion rays
**	run no hit programs (TERMINATE_ON_FIRST_HIT, DISABLE_CLOSESTHIT).
**
**	Extra Large Technology
\****************************************************************************/
#include <optix.h>

#ifndef GOX_OPTXAOPARAMS_H
#include "optxAOParams.h"
#endif
#ifndef GOX_OPTXSCENETYPES_H
#include "optxSceneTypes.h"
#endif

using namespace gox;

//	The launch parameters: OptiX finds them by name (pipelineLaunchParamsVariableName)
extern "C"
{
	__constant__ optxAOParams params;
}

namespace
{
	//	PCG hash (Jarzynski and Olano, "Hash Functions for GPU Rendering")
	__forceinline__ __device__ unsigned int pcg(unsigned int v)
	{
		const unsigned int state = v * 747796405u + 2891336453u;
		const unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	struct Rng
	{
		unsigned int m_State;

		__forceinline__ __device__ Rng(unsigned int i_Pixel, unsigned int i_Sample, unsigned int i_Seed)
			: m_State(pcg(i_Pixel ^ pcg(i_Sample ^ pcg(i_Seed))))
		{
		}

		//	[0, 1)
		__forceinline__ __device__ float Next()
		{
			m_State = pcg(m_State);
			return (m_State >> 8) * (1.0f / 16777216.0f);
		}
	};

	__forceinline__ __device__ float3 operator+(float3 a, float3 b) { return make_float3(a.x + b.x, a.y + b.y, a.z + b.z); }
	__forceinline__ __device__ float3 operator-(float3 a, float3 b) { return make_float3(a.x - b.x, a.y - b.y, a.z - b.z); }
	__forceinline__ __device__ float3 operator*(float3 a, float s) { return make_float3(a.x * s, a.y * s, a.z * s); }
	__forceinline__ __device__ float dot(float3 a, float3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	__forceinline__ __device__ float3 cross(float3 a, float3 b)
	{
		return make_float3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}
	__forceinline__ __device__ float3 normalize(float3 a) { return a * rsqrtf(dot(a, a)); }

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

	__forceinline__ __device__ float3 read_float3(unsigned long long i_Address)
	{
		const float* p = reinterpret_cast<const float*>(i_Address);
		return make_float3(p[0], p[1], p[2]);
	}

	//	A direction about n, cosine-weighted
	__forceinline__ __device__ float3 cosine_direction(float3 n, float u1, float u2)
	{
		const float r = sqrtf(u1);
		const float phi = 6.28318530718f * u2;
		const float x = r * __cosf(phi);
		const float y = r * __sinf(phi);
		const float z = sqrtf(fmaxf(0.0f, 1.0f - u1));
		//	Orthonormal basis (Duff et al., "Building an Orthonormal Basis, Revisited")
		const float sign = copysignf(1.0f, n.z);
		const float a = -1.0f / (sign + n.z);
		const float b = n.x * n.y * a;
		const float3 t = make_float3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
		const float3 s = make_float3(b, sign + n.y * n.y * a, -n.y);
		return t * x + s * y + n * z;
	}

	__forceinline__ __device__ float linear_to_srgb(float c)
	{
		c = fminf(fmaxf(c, 0.0f), 1.0f);
		return c <= 0.0031308f ? 12.92f * c : 1.055f * __powf(c, 1.0f / 2.4f) - 0.055f;
	}

	__forceinline__ __device__ unsigned int to_byte(float c)
	{
		return static_cast<unsigned int>(linear_to_srgb(c) * 255.0f + 0.5f);
	}

	//	Whether an occlusion ray from i_Origin along i_Direction hits anything
	//	within the radius
	__forceinline__ __device__ bool occluded(float3 i_Origin, float3 i_Direction)
	{
		unsigned int hit = 1;
		optixTrace(params.m_Handle, i_Origin, i_Direction, 0.0f, params.m_Radius, 0.0f,
				   OptixVisibilityMask(e_optxMaskOcclusion),
				   OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT | OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT | OPTIX_RAY_FLAG_DISABLE_ANYHIT,
				   0, 1, 1,		// SBT offset, stride, miss index 1
				   hit);
		return hit != 0;
	}
}

extern "C" __global__ void __raygen__ao()
{
	const uint3 index = optixGetLaunchIndex();
	if (index.x >= params.m_Width || index.y >= params.m_Height)
	{
		return;
	}
	const unsigned int pixel = index.y * params.m_Width + index.x;
	const float3 background = make_float3(params.m_Background[0], params.m_Background[1], params.m_Background[2]);

	float3 sum = make_float3(0.0f, 0.0f, 0.0f);
	for (unsigned int s = 0; s < params.m_Samples; ++s)
	{
		const unsigned int sample = params.m_FirstSample + s;
		Rng rng(pixel, sample, params.m_Seed);
		float jx = 0.5f, jy = 0.5f;
		if (sample != 0)
		{
			jx = rng.Next();
			jy = rng.Next();
		}

		//	D3D's viewport: y down in pixels, up in NDC; depth 0 near, 1 far
		const float nx = (index.x + jx) / params.m_Width * 2.0f - 1.0f;
		const float ny = 1.0f - (index.y + jy) / params.m_Height * 2.0f;
		//	The direction from the near plane to depth 0.5 (about twice the
		//	near distance in perspective): the far plane's w is tiny, and
		//	unprojecting it in float bends the ray enough to show in depth
		float w;
		const float3 near_point = project(params.m_InvViewProj, nx, ny, 0.0f, w);
		const float3 mid_point = project(params.m_InvViewProj, nx, ny, 0.5f, w);
		const float3 far_point = project(params.m_InvViewProj, nx, ny, 1.0f, w);
		const float3 step = mid_point - near_point;
		const float3 direction = step * rsqrtf(dot(step, step));
		const float length = dot(far_point - near_point, direction);

		unsigned int t = 0, n0 = 0, n1 = 0, n2 = 0, hit = 0;
		if (params.m_Handle)
		{
			optixTrace(params.m_Handle, near_point, direction, 0.0f, length, 0.0f,
					   OptixVisibilityMask(e_optxMaskCamera), OPTIX_RAY_FLAG_DISABLE_ANYHIT,
					   0, 1, 0,		// SBT offset, stride, miss index 0
					   t, n0, n1, n2, hit);
		}

		float depth = 1.0f;
		if (!hit)
		{
			sum = sum + background;
		}
		else
		{
			const float3 position = near_point + direction * __uint_as_float(t);
			const float3 normal = make_float3(__uint_as_float(n0), __uint_as_float(n1), __uint_as_float(n2));
			const float3 origin = position + normal * params.m_Epsilon;
			unsigned int open = 0;
			for (unsigned int r = 0; r < params.m_AORays; ++r)
			{
				const float u1 = rng.Next();
				const float u2 = rng.Next();
				if (!occluded(origin, cosine_direction(normal, u1, u2)))
				{
					++open;
				}
			}
			const float ao = params.m_AORays ? float(open) / float(params.m_AORays) : 1.0f;
			sum = sum + make_float3(ao, ao, ao);
			depth = project(params.m_ViewProj, position.x, position.y, position.z, w).z;
		}

		if (sample == 0)
		{
			unsigned int* out = reinterpret_cast<unsigned int*>(params.m_Depth);
			depth = fminf(fmaxf(depth, 0.0f), 1.0f);
			//	(depth * 16777215 + 0.5 in float would round 1 up to 2^24)
			out[pixel] = params.m_DepthLayout == e_optxDepthFloat ? __float_as_uint(depth)
																  : min(__float2uint_rn(depth * 16777215.0f), 16777215u);
		}
	}

	float* accum = reinterpret_cast<float*>(params.m_Accum) + 4 * pixel;
	if (params.m_FirstSample != 0)
	{
		sum = sum + make_float3(accum[0], accum[1], accum[2]);
	}
	accum[0] = sum.x;
	accum[1] = sum.y;
	accum[2] = sum.z;
	accum[3] = 1.0f;
}

extern "C" __global__ void __raygen__resolve()
{
	const uint3 index = optixGetLaunchIndex();
	if (index.x >= params.m_Width || index.y >= params.m_Height || params.m_FirstSample == 0)
	{
		return;
	}
	const unsigned int pixel = index.y * params.m_Width + index.x;
	const float* accum = reinterpret_cast<const float*>(params.m_Accum) + 4 * pixel;
	const float scale = 1.0f / params.m_FirstSample;
	const unsigned int r = to_byte(accum[0] * scale);
	const unsigned int g = to_byte(accum[1] * scale);
	const unsigned int b = to_byte(accum[2] * scale);
	unsigned int* out = reinterpret_cast<unsigned int*>(params.m_Color);
	out[pixel] = params.m_ColorOrder == e_optxColorBGRA ? (b | (g << 8) | (r << 16) | (255u << 24))
														: (r | (g << 8) | (b << 16) | (255u << 24));
}

extern "C" __global__ void __miss__camera()
{
	optixSetPayload_4(0);
}

extern "C" __global__ void __miss__occlusion()
{
	optixSetPayload_0(0);
}

//	The hit's distance and world-space geometric normal, facing the ray
extern "C" __global__ void __closesthit__camera()
{
	const optxInstanceData& instance = reinterpret_cast<const optxInstanceData*>(params.m_Instances)[optixGetInstanceId()];
	const optxGeometryInput& input = reinterpret_cast<const optxGeometryInput*>(instance.m_Inputs)[optixGetSbtGASIndex()];
	const unsigned int triangle = optixGetPrimitiveIndex();
	unsigned int i0, i1, i2;
	if (input.m_IndexSize == 2)
	{
		const unsigned short* indices = reinterpret_cast<const unsigned short*>(input.m_Indices) + 3 * triangle;
		i0 = indices[0];
		i1 = indices[1];
		i2 = indices[2];
	}
	else
	{
		const unsigned int* indices = reinterpret_cast<const unsigned int*>(input.m_Indices) + 3 * triangle;
		i0 = indices[0];
		i1 = indices[1];
		i2 = indices[2];
	}
	const float3 p0 = read_float3(instance.m_Vertices + (unsigned long long)i0 * instance.m_VertexStride);
	const float3 p1 = read_float3(instance.m_Vertices + (unsigned long long)i1 * instance.m_VertexStride);
	const float3 p2 = read_float3(instance.m_Vertices + (unsigned long long)i2 * instance.m_VertexStride);

	const float3 direction = optixGetWorldRayDirection();
	float3 normal = optixTransformNormalFromObjectToWorldSpace(cross(p1 - p0, p2 - p0));
	const float length2 = dot(normal, normal);
	if (!(length2 > 1e-30f))
	{
		normal = direction * -1.0f;		// degenerate: face the ray
	}
	else
	{
		normal = normal * rsqrtf(length2);
		if (dot(normal, direction) > 0.0f)
		{
			normal = normal * -1.0f;
		}
	}

	optixSetPayload_0(__float_as_uint(optixGetRayTmax()));
	optixSetPayload_1(__float_as_uint(normal.x));
	optixSetPayload_2(__float_as_uint(normal.y));
	optixSetPayload_3(__float_as_uint(normal.z));
	optixSetPayload_4(1);
}
