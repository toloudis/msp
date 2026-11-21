/*****************************************************************************
**  optxAO.cu
**
**      The progressive ambient occlusion programs: see optxAOParams.h.
**
**	Compiled to OptiX IR by nvcc at build time and embedded as "optxAO"
**	(optxEmbeddedPrograms). Entry points: __raygen__ao and
**	__raygen__resolve (one launch each; __raygen__ao resolves too with
**	m_Resolve), __miss__camera (miss record 0),
**	__miss__occlusion (miss record 1), __closesthit__camera (every hit
**	group record: one per build input of the biggest GAS) with
**	__anyhit__opacity (cut-out and transparent materials; build inputs of
**	opaque materials disable it). Occlusion rays run no closest hit
**	(TERMINATE_ON_FIRST_HIT, DISABLE_CLOSESTHIT).
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
#ifndef GOX_OPTXMATERIALTYPES_H
#include "optxMaterialTypes.h"
#endif
#ifndef GOX_OPTXMATERIAL_H
#include "optxMaterial.h"
#endif

using namespace gox;

//	The launch parameters: OptiX finds them by name (pipelineLaunchParamsVariableName)
extern "C"
{
	__constant__ optxAOParams params;
}

namespace
{
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

	//	The colour and depth buffers' texels of a pixel: i_Sum over
	//	i_Weight (the samples' weights' sum), and i_Depth (in [0, 1])
	__forceinline__ __device__ void write_pixel(unsigned int i_Pixel, float3 i_Sum, float i_Weight, float i_Depth)
	{
		const float scale = i_Weight > 0.0f ? 1.0f / i_Weight : 0.0f;
		const unsigned int r = to_byte(i_Sum.x * scale);
		const unsigned int g = to_byte(i_Sum.y * scale);
		const unsigned int b = to_byte(i_Sum.z * scale);
		unsigned int* color = reinterpret_cast<unsigned int*>(params.m_Color);
		color[i_Pixel] = params.m_ColorOrder == e_optxColorBGRA ? (b | (g << 8) | (r << 16) | (255u << 24))
																: (r | (g << 8) | (b << 16) | (255u << 24));
		unsigned int* depth = reinterpret_cast<unsigned int*>(params.m_Depth);
		//	(depth * 16777215 + 0.5 in float would round 1 up to 2^24)
		depth[i_Pixel] = params.m_DepthLayout == e_optxDepthFloat ? __float_as_uint(i_Depth)
																  : min(__float2uint_rn(i_Depth * 16777215.0f), 16777215u);
	}

	//	Whether an occlusion ray from i_Origin along i_Direction hits anything
	//	within the radius
	__forceinline__ __device__ bool occluded(float3 i_Origin, float3 i_Direction)
	{
		unsigned int hit = 1;
		optixTrace(params.m_Handle, i_Origin, i_Direction, 0.0f, params.m_Radius, 0.0f,
				   OptixVisibilityMask(e_optxMaskOcclusion),
				   OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT | OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT,
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
	const unsigned int count = params.m_Width * params.m_Height;
	const float3 background = make_float3(params.m_Background[0], params.m_Background[1], params.m_Background[2]);

	float3 sum = make_float3(0.0f, 0.0f, 0.0f);
	float weights = 0.0f;
	float first_depth = 1.0f;		// sample 0's, if this launch has it
	for (unsigned int s = 0; s < params.m_Samples; ++s)
	{
		const unsigned int sample = params.m_FirstSample + s;
		optxq::Sampler sampler(params.m_Sequence, params.m_ImageX + index.x, params.m_ImageY + index.y, sample);
		const optxq::Pair at = sampler.camera_pixel();
		const float dx = optxf::offset(params.m_Filter, at.u), dy = optxf::offset(params.m_Filter, at.v);
		const float weight = optxf::weight(params.m_Filter, dx, dy);

		//	D3D's viewport: y down in pixels, up in NDC; depth 0 near, 1 far
		const float nx = (index.x + 0.5f + dx) / params.m_Width * 2.0f - 1.0f;
		const float ny = 1.0f - (index.y + 0.5f + dy) / params.m_Height * 2.0f;
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
					   OptixVisibilityMask(e_optxMaskCamera), OPTIX_RAY_FLAG_NONE,
					   0, 1, 0,		// SBT offset, stride, miss index 0
					   t, n0, n1, n2, hit);
		}

		float depth = 1.0f;
		float3 value = background;
		if (hit)
		{
			const float3 position = near_point + direction * __uint_as_float(t);
			const float3 normal = make_float3(__uint_as_float(n0), __uint_as_float(n1), __uint_as_float(n2));
			const float3 origin = position + normal * params.m_Epsilon;
			unsigned int open = 0;
			for (unsigned int r = 0; r < params.m_AORays; ++r)
			{
				const optxq::Pair u = sampler.direction(0, r);
				if (!occluded(origin, cosine_direction(normal, u.u, u.v)))
				{
					++open;
				}
			}
			const float ao = params.m_AORays ? float(open) / float(params.m_AORays) : 1.0f;
			value = make_float3(ao, ao, ao);
			depth = project(params.m_ViewProj, position.x, position.y, position.z, w).z;
		}
		sum = sum + value * weight;
		weights += weight;

		if (sample == 0)
		{
			first_depth = fminf(fmaxf(depth, 0.0f), 1.0f);
		}
	}

	float4* accum = reinterpret_cast<float4*>(params.m_Accum) + pixel;
	float depth = first_depth;
	if (params.m_FirstSample != 0)
	{
		const float4 before = accum[0];
		sum = sum + make_float3(before.x, before.y, before.z);
		depth = before.w;
		weights += accum[count].x;
	}
	accum[0] = make_float4(sum.x, sum.y, sum.z, depth);
	accum[count] = make_float4(weights, 0.0f, 0.0f, 0.0f);
	if (params.m_Resolve)
	{
		write_pixel(pixel, sum, weights, depth);
	}
}

extern "C" __global__ void __raygen__resolve()
{
	const uint3 index = optixGetLaunchIndex();
	if (index.x >= params.m_Width || index.y >= params.m_Height || params.m_FirstSample == 0)
	{
		return;
	}
	const unsigned int pixel = index.y * params.m_Width + index.x;
	const float4* accum = reinterpret_cast<const float4*>(params.m_Accum) + pixel;
	const float4 color = accum[0];
	write_pixel(pixel, make_float3(color.x, color.y, color.z), accum[params.m_Width * params.m_Height].x, color.w);
}

extern "C" __global__ void __miss__camera()
{
	optixSetPayload_4(0);
}

extern "C" __global__ void __miss__occlusion()
{
	optixSetPayload_0(0);
}

//	Cut-out and transparent materials let rays through (optxMaterial.h)
extern "C" __global__ void __anyhit__opacity()
{
	if (!optxm::opacity_passes(params.m_Instances, params.m_Materials, params.m_Textures, params.m_Seed))
	{
		optixIgnoreIntersection();
	}
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
