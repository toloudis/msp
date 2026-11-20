/*****************************************************************************
**  optxLighting.cu
**
**      The lighting programs: Illumination Only and Shadow Mask, traced.
**	See optxLightingParams.h (what each launch and mode does) and
**	optxLightTypes.h (the lights).
**
**	Compiled to OptiX IR by nvcc at build time and embedded as
**	"optxLighting" (optxEmbeddedPrograms). Entry points:
**	__raygen__lighting and __raygen__resolve, __miss__camera (miss
**	record 0), __miss__occlusion (miss record 1), __closesthit__camera
**	(every hit group record: one per build input of the biggest GAS).
**	Every ray is traced from a ray generation program (trace depth 1).
**	Occlusion rays run no hit programs.
**
**	Extra Large Technology
\****************************************************************************/
#include <optix.h>

#ifndef GOX_OPTXAOPARAMS_H
#include "optxAOParams.h"
#endif
#ifndef GOX_OPTXLIGHTINGPARAMS_H
#include "optxLightingParams.h"
#endif
#ifndef GOX_OPTXLIGHTTYPES_H
#include "optxLightTypes.h"
#endif
#ifndef GOX_OPTXSCENETYPES_H
#include "optxSceneTypes.h"
#endif

using namespace gox;

extern "C"
{
	__constant__ optxLightingParams params;
}

namespace
{
	constexpr float c_Pi = 3.14159265358979f;
	constexpr float c_Infinity = 1e30f;

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
	__forceinline__ __device__ float saturate(float x) { return fminf(fmaxf(x, 0.0f), 1.0f); }
	__forceinline__ __device__ float3 load3(const float* p) { return make_float3(p[0], p[1], p[2]); }

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

	//	An orthonormal basis (s, t) about the unit vector n (Duff et al.,
	//	"Building an Orthonormal Basis, Revisited")
	__forceinline__ __device__ void basis(float3 n, float3& o_S, float3& o_T)
	{
		const float sign = copysignf(1.0f, n.z);
		const float a = -1.0f / (sign + n.z);
		const float b = n.x * n.y * a;
		o_S = make_float3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
		o_T = make_float3(b, sign + n.y * n.y * a, -n.y);
	}

	//	A direction about n, cosine-weighted
	__forceinline__ __device__ float3 cosine_direction(float3 n, float u1, float u2)
	{
		const float r = sqrtf(u1);
		const float phi = 2.0f * c_Pi * u2;
		float3 s, t;
		basis(n, s, t);
		return s * (r * __cosf(phi)) + t * (r * __sinf(phi)) + n * sqrtf(fmaxf(0.0f, 1.0f - u1));
	}

	//	The resolve: the mean, clamped, as linear bytes (the raster copies
	//	its linear buffer into the 8-bit target the same way)
	__forceinline__ __device__ unsigned int to_byte(float c)
	{
		return static_cast<unsigned int>(saturate(c) * 255.0f + 0.5f);
	}

	__forceinline__ __device__ void write_pixel(unsigned int i_Pixel, float3 i_Sum, unsigned int i_nSamples, float i_Depth)
	{
		const float scale = 1.0f / i_nSamples;
		const unsigned int r = to_byte(i_Sum.x * scale);
		const unsigned int g = to_byte(i_Sum.y * scale);
		const unsigned int b = to_byte(i_Sum.z * scale);
		unsigned int* color = reinterpret_cast<unsigned int*>(params.m_Color);
		color[i_Pixel] = params.m_ColorOrder == e_optxColorBGRA ? (b | (g << 8) | (r << 16) | (255u << 24))
																: (r | (g << 8) | (b << 16) | (255u << 24));
		unsigned int* depth = reinterpret_cast<unsigned int*>(params.m_Depth);
		depth[i_Pixel] = params.m_DepthLayout == e_optxDepthFloat ? __float_as_uint(i_Depth)
																  : min(__float2uint_rn(i_Depth * 16777215.0f), 16777215u);
	}

	//------------------------------------------------------------------------
	//	Rays
	//------------------------------------------------------------------------

	struct Hit
	{
		bool			m_bHit;
		float			m_T;
		float3			m_Geometric;	// geometric normal, facing the ray
		float3			m_Shading;		// interpolated vertex normal, on the geometric normal's side
		unsigned int	m_Instance;
	};

	__forceinline__ __device__ Hit trace_camera(float3 i_Origin, float3 i_Direction, float i_TMax)
	{
		unsigned int t = 0, g0 = 0, g1 = 0, g2 = 0, s0 = 0, s1 = 0, s2 = 0, instance = 0, hit = 0;
		if (params.m_Handle)
		{
			optixTrace(params.m_Handle, i_Origin, i_Direction, 0.0f, i_TMax, 0.0f,
					   OptixVisibilityMask(e_optxMaskCamera), OPTIX_RAY_FLAG_DISABLE_ANYHIT,
					   0, 1, 0,		// SBT offset, stride, miss index 0
					   t, g0, g1, g2, s0, s1, s2, instance, hit);
		}
		Hit result;
		result.m_bHit = hit != 0;
		result.m_T = __uint_as_float(t);
		result.m_Geometric = make_float3(__uint_as_float(g0), __uint_as_float(g1), __uint_as_float(g2));
		result.m_Shading = make_float3(__uint_as_float(s0), __uint_as_float(s1), __uint_as_float(s2));
		result.m_Instance = instance;
		return result;
	}

	//	Whether anything that casts shadows lies along the ray before i_TMax
	__forceinline__ __device__ bool occluded(float3 i_Origin, float3 i_Direction, float i_TMax)
	{
		if (!params.m_Handle)
		{
			return false;
		}
		unsigned int hit = 1;
		optixTrace(params.m_Handle, i_Origin, i_Direction, 0.0f, i_TMax, 0.0f,
				   OptixVisibilityMask(e_optxMaskOcclusion),
				   OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT | OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT | OPTIX_RAY_FLAG_DISABLE_ANYHIT,
				   0, 1, 1,		// SBT offset, stride, miss index 1
				   hit);
		return hit != 0;
	}

	//------------------------------------------------------------------------
	//	Lights (Lighting.hlsli's formulas)
	//------------------------------------------------------------------------

	__forceinline__ __device__ float attenuation(const optxLight& i_Light, float i_Distance)
	{
		const float* f = i_Light.m_Falloff;
		if (i_Distance > i_Light.m_FalloffStart)
		{
			const float d = i_Distance - i_Light.m_FalloffStart;
			return 1.0f / fmaxf(1e-6f, f[0] + f[1] * d + f[2] * d * d + f[3] * d * d * d);
		}
		return 1.0f / fmaxf(1e-6f, f[0]);
	}

	__forceinline__ __device__ float smoothstep(float e0, float e1, float x)
	{
		const float t = saturate((x - e0) / (e1 - e0));
		return t * t * (3.0f - 2.0f * t);
	}

	//	EllipticalPenumbra (ARman section 14.3.1)
	__forceinline__ __device__ float elliptical_penumbra(float i_Inner, float i_Outer, float i_Aspect, float u, float v)
	{
		const float x = fabsf(u - 0.5f), y = fabsf(v - 0.5f);
		if (x == 0.0f && y == 0.0f)
		{
			return 1.0f;
		}
		const float a = 0.5f * tanf(i_Inner * 0.5f) / tanf(i_Outer * 0.5f);
		const float b = 0.5f * tanf(i_Aspect * i_Inner * 0.5f) / tanf(i_Aspect * i_Outer * 0.5f);
		const float A = 0.5f, B = 0.5f;
		const float q = a * b / sqrtf(b * b * x * x + a * a * y * y);
		const float r = A * B / sqrtf(B * B * x * x + A * A * y * y);
		return 1.0f - smoothstep(q, r + 0.001f, 1.0f);
	}

	//	What a light gives a point, unshadowed and before the cosine: its
	//	colour times attenuation and penumbra; and the way to it (towards
	//	its centre, unnormalized; for directional lights, against its
	//	direction) and its distance (c_Infinity for directional lights).
	//	False if the light doesn't reach the point.
	__forceinline__ __device__ bool light_at(const optxLight& i_Light, float3 i_P, float3& o_Color, float3& o_ToLight,
											 float& o_Distance)
	{
		const float3 color = load3(i_Light.m_Color);
		const float3 position = load3(i_Light.m_Position);
		const float3 direction = load3(i_Light.m_Direction);
		switch (i_Light.m_Type)
		{
			case e_optxLightPoint:
			{
				o_ToLight = position - i_P;
				o_Distance = sqrtf(dot(o_ToLight, o_ToLight));
				o_Color = color * attenuation(i_Light, o_Distance);
				return o_Distance > 0.0f;
			}
			case e_optxLightDirectional:
			{
				o_ToLight = direction * -1.0f;
				o_Distance = c_Infinity;
				o_Color = color;
				return true;
			}
			default:	// projected
			{
				const float* M = i_Light.m_Matrix;
				const float tx = i_P.x * M[0] + i_P.y * M[4] + i_P.z * M[8] + M[12];
				const float ty = i_P.x * M[1] + i_P.y * M[5] + i_P.z * M[9] + M[13];
				const float tz = i_P.x * M[2] + i_P.y * M[6] + i_P.z * M[10] + M[14];
				const float tw = i_P.x * M[3] + i_P.y * M[7] + i_P.z * M[11] + M[15];
				if (!(tz >= 0.0f && tz < i_Light.m_Far) || tw == 0.0f)
				{
					return false;
				}
				const float u = tx / tw, v = ty / tw;
				float factor;
				if (i_Light.m_InnerAngle <= i_Light.m_OuterAngle)
				{
					factor = elliptical_penumbra(i_Light.m_InnerAngle, i_Light.m_OuterAngle, i_Light.m_Aspect, u, v);
				}
				else
				{
					factor = (u >= 0.0f && u <= 1.0f && v >= 0.0f && v <= 1.0f) ? 1.0f : 0.0f;
				}
				if (i_Light.m_Type == e_optxLightProjected)
				{
					o_ToLight = position - i_P;
					o_Distance = sqrtf(dot(o_ToLight, o_ToLight));
					factor *= attenuation(i_Light, o_Distance);
				}
				else
				{
					o_ToLight = direction * -1.0f;
					o_Distance = c_Infinity;
				}
				o_Color = color * factor;
				return factor > 0.0f;
			}
		}
	}

	__forceinline__ __device__ bool is_area(const optxLight& i_Light)
	{
		return i_Light.m_Radius > 0.0f &&
			   (i_Light.m_Type == e_optxLightProjected || i_Light.m_Type == e_optxLightProjectedDirectional);
	}

	//	The solid angle of a directional area light's cone
	__forceinline__ __device__ float cone_solid_angle(const optxLight& i_Light)
	{
		return 2.0f * c_Pi * fmaxf(1e-7f, 1.0f - i_Light.m_CosCone);
	}

	//	A point on a disk light (centre c, radius r, facing p), and the
	//	disk's normal
	__forceinline__ __device__ float3 disk_point(const optxLight& i_Light, float3 i_P, float u1, float u2, float3& o_Normal)
	{
		const float3 centre = load3(i_Light.m_Position);
		o_Normal = normalize(i_P - centre);
		float3 s, t;
		basis(o_Normal, s, t);
		const float r = i_Light.m_Radius * sqrtf(u1);
		const float phi = 2.0f * c_Pi * u2;
		return centre + s * (r * __cosf(phi)) + t * (r * __sinf(phi));
	}

	//	A direction in a directional area light's cone
	__forceinline__ __device__ float3 cone_direction(const optxLight& i_Light, float u1, float u2)
	{
		const float3 axis = load3(i_Light.m_Direction) * -1.0f;
		const float cos_a = 1.0f - u1 * (1.0f - i_Light.m_CosCone);
		const float sin_a = sqrtf(fmaxf(0.0f, 1.0f - cos_a * cos_a));
		const float phi = 2.0f * c_Pi * u2;
		float3 s, t;
		basis(axis, s, t);
		return s * (sin_a * __cosf(phi)) + t * (sin_a * __sinf(phi)) + axis * cos_a;
	}

	__forceinline__ __device__ float power_heuristic(float a, float b)
	{
		const float a2 = a * a, b2 = b * b;
		return a2 + b2 > 0.0f ? a2 / (a2 + b2) : 0.0f;
	}

	//	The lights that reach an instance: indices i_First .. i_First +
	//	o_Count into the light set indices, or (o_bAll) every light
	__forceinline__ __device__ void light_range(const optxInstanceData& i_Instance, unsigned int& o_First,
												unsigned int& o_Count, bool& o_bAll)
	{
		o_bAll = i_Instance.m_LightSet == c_optxAllLights;
		if (o_bAll)
		{
			o_First = 0;
			o_Count = params.m_NumLights;
		}
		else
		{
			const optxLightSet set = reinterpret_cast<const optxLightSet*>(params.m_LightSets)[i_Instance.m_LightSet];
			o_First = set.m_First;
			o_Count = set.m_Count;
		}
	}

	__forceinline__ __device__ const optxLight& light_of(unsigned int i_First, unsigned int i, bool i_bAll)
	{
		const optxLight* lights = reinterpret_cast<const optxLight*>(params.m_Lights);
		if (i_bAll)
		{
			return lights[i];
		}
		return lights[reinterpret_cast<const unsigned int*>(params.m_LightIndices)[i_First + i]];
	}

	//	A point being shaded
	struct Surface
	{
		float3			m_P;
		float3			m_Geometric;	// facing the incoming ray
		float3			m_Shading;
		float3			m_Origin;		// off the surface, for secondary rays
		bool			m_bReceivesShadow;
		unsigned int	m_First;
		unsigned int	m_Count;
		bool			m_bAll;
	};

	__forceinline__ __device__ Surface make_surface(float3 i_P, const Hit& i_Hit)
	{
		Surface s;
		s.m_P = i_P;
		s.m_Geometric = i_Hit.m_Geometric;
		s.m_Shading = i_Hit.m_Shading;
		s.m_Origin = i_P + i_Hit.m_Geometric * params.m_Epsilon;
		const optxInstanceData& instance = reinterpret_cast<const optxInstanceData*>(params.m_Instances)[i_Hit.m_Instance];
		s.m_bReceivesShadow = (instance.m_Flags & e_optxInstanceReceivesShadow) != 0;
		light_range(instance, s.m_First, s.m_Count, s.m_bAll);
		return s;
	}

	__forceinline__ __device__ bool shadowed_pair(const Surface& i_Surface, const optxLight& i_Light)
	{
		return params.m_bShadows && i_Surface.m_bReceivesShadow && (i_Light.m_Flags & e_optxLightShadows);
	}

	//	Whether a light is sampled by both light samples and bounce rays
	//	(an area light whose shadows are traced, with the cosine on)
	__forceinline__ __device__ bool weighted(const Surface& i_Surface, const optxLight& i_Light)
	{
		return is_area(i_Light) && shadowed_pair(i_Surface, i_Light) && params.m_bUseCosine &&
			   params.m_Sampling != e_optxSampleLights;
	}

	__forceinline__ __device__ float cosine_term(const Surface& i_Surface, float3 i_Direction)
	{
		return params.m_bUseCosine ? saturate(dot(i_Direction, i_Surface.m_Shading)) : 1.0f;
	}

	//	The direct light at a surface by light samples (one per light), and
	//	whether any light there is also reached by bounce rays
	__forceinline__ __device__ float3 direct_light(const Surface& i_Surface, Rng& io_Rng, bool& o_bWeighted)
	{
		float3 sum = make_float3(0.0f, 0.0f, 0.0f);
		o_bWeighted = false;
		for (unsigned int i = 0; i < i_Surface.m_Count; ++i)
		{
			const optxLight& light = light_of(i_Surface.m_First, i, i_Surface.m_bAll);
			float3 color, to_light;
			float distance;
			if (!light_at(light, i_Surface.m_P, color, to_light, distance))
			{
				continue;
			}
			const bool shadows = shadowed_pair(i_Surface, light);
			const bool mis = weighted(i_Surface, light);
			o_bWeighted |= mis;
			if (mis && params.m_Sampling == e_optxSampleBounces)
			{
				continue;	// bounce rays alone
			}

			float3 wi;
			float pdf_light = 0.0f;		// solid angle, area lights
			float tmax = distance;
			const float u1 = io_Rng.Next(), u2 = io_Rng.Next();
			if (shadows && is_area(light))
			{
				if (light.m_Type == e_optxLightProjected)
				{
					float3 normal;
					const float3 y = disk_point(light, i_Surface.m_P, u1, u2, normal);
					const float3 to_y = y - i_Surface.m_P;
					const float d2 = dot(to_y, to_y);
					const float d = sqrtf(d2);
					wi = to_y * (1.0f / d);
					tmax = d;
					const float cos_l = fmaxf(1e-6f, -dot(wi, normal));
					pdf_light = d2 / (c_Pi * light.m_Radius * light.m_Radius * cos_l);
				}
				else
				{
					wi = cone_direction(light, u1, u2);
					tmax = c_Infinity;
					pdf_light = 1.0f / cone_solid_angle(light);
				}
			}
			else
			{
				wi = normalize(to_light);
			}

			const float cosine = cosine_term(i_Surface, wi);
			if (cosine <= 0.0f)
			{
				continue;
			}
			if (shadows)
			{
				if (dot(wi, i_Surface.m_Geometric) <= 0.0f ||
					occluded(i_Surface.m_Origin, wi, tmax < c_Infinity ? tmax * (1.0f - 1e-4f) - params.m_Epsilon : c_Infinity))
				{
					continue;
				}
			}
			float weight = 1.0f;
			if (mis)
			{
				weight = power_heuristic(pdf_light, cosine / c_Pi);
			}
			sum = sum + color * (cosine * weight);
		}
		return sum;
	}

	//	The light a bounce ray from a surface brings from the area lights it
	//	reaches before i_THit (the nearest geometry, c_Infinity for none),
	//	weighted against their light samples. With the cosine-weighted
	//	bounce pdf (cos / pi), a hit's estimate is the light's radiance:
	//	pi times the light at its centre over its solid angle density (a
	//	disk: d^2 / (area cos); a cone: 1 / solid angle), so that a light
	//	sample's estimate (colour times cosine) has the same expectation
	__forceinline__ __device__ float3 bounce_light(const Surface& i_Surface, float3 i_W, float i_THit)
	{
		float3 sum = make_float3(0.0f, 0.0f, 0.0f);
		const float pdf_bounce = saturate(dot(i_W, i_Surface.m_Shading)) / c_Pi;
		for (unsigned int i = 0; i < i_Surface.m_Count; ++i)
		{
			const optxLight& light = light_of(i_Surface.m_First, i, i_Surface.m_bAll);
			if (!weighted(i_Surface, light))
			{
				continue;
			}
			float3 color, to_light;
			float distance;
			if (!light_at(light, i_Surface.m_P, color, to_light, distance))
			{
				continue;
			}
			float value = 0.0f, pdf_light = 0.0f;
			if (light.m_Type == e_optxLightProjected)
			{
				const float3 centre = load3(light.m_Position);
				const float3 normal = normalize(i_Surface.m_P - centre);
				const float facing = dot(i_W, normal);
				if (facing >= 0.0f)
				{
					continue;
				}
				const float t = dot(centre - i_Surface.m_Origin, normal) / facing;
				if (!(t > 0.0f) || t >= i_THit)
				{
					continue;
				}
				const float3 y = i_Surface.m_Origin + i_W * t;
				const float3 off = y - centre;
				if (dot(off, off) > light.m_Radius * light.m_Radius)
				{
					continue;
				}
				const float3 to_y = y - i_Surface.m_P;
				const float d2 = dot(to_y, to_y);
				const float area_cos = c_Pi * light.m_Radius * light.m_Radius * (-facing);
				pdf_light = d2 / area_cos;
				value = c_Pi * pdf_light;
			}
			else
			{
				if (i_THit < c_Infinity || dot(i_W, load3(light.m_Direction)) > -light.m_CosCone)
				{
					continue;
				}
				pdf_light = 1.0f / cone_solid_angle(light);
				value = c_Pi * pdf_light;
			}
			const float weight = params.m_Sampling == e_optxSampleBounces ? 1.0f : power_heuristic(pdf_bounce, pdf_light);
			sum = sum + color * (value * weight);
		}
		return sum;
	}

	//	Shadow Mask: the product over the projected lights that reach the
	//	surface and face it of 1 - (1 - visibility) * cosine
	__forceinline__ __device__ float shadow_mask(const Surface& i_Surface, Rng& io_Rng)
	{
		float mask = 1.0f;
		for (unsigned int i = 0; i < i_Surface.m_Count; ++i)
		{
			const optxLight& light = light_of(i_Surface.m_First, i, i_Surface.m_bAll);
			if (light.m_Type != e_optxLightProjected && light.m_Type != e_optxLightProjectedDirectional)
			{
				continue;
			}
			const float u1 = io_Rng.Next(), u2 = io_Rng.Next();
			//	The mask ignores the light's colour, penumbra and attenuation:
			//	only whether the point is in its frustum (as the raster's
			//	IlluminateProjLightShadowsOnly)
			float3 to_light;
			float distance;
			const float* M = light.m_Matrix;
			const float3 p = i_Surface.m_P;
			const float tz = p.x * M[2] + p.y * M[6] + p.z * M[10] + M[14];
			if (!(tz >= 0.0f && tz < light.m_Far))
			{
				continue;
			}
			if (light.m_Type == e_optxLightProjected)
			{
				to_light = load3(light.m_Position) - p;
				distance = sqrtf(dot(to_light, to_light));
			}
			else
			{
				to_light = load3(light.m_Direction) * -1.0f;
				distance = c_Infinity;
			}
			const float3 l = normalize(to_light);
			const float cosine = dot(l, i_Surface.m_Shading);
			if (cosine <= 0.0f || !shadowed_pair(i_Surface, light))
			{
				continue;
			}
			float3 wi = l;
			float tmax = distance;
			if (is_area(light))
			{
				if (light.m_Type == e_optxLightProjected)
				{
					float3 normal;
					const float3 to_y = disk_point(light, p, u1, u2, normal) - p;
					tmax = sqrtf(dot(to_y, to_y));
					wi = to_y * (1.0f / tmax);
				}
				else
				{
					wi = cone_direction(light, u1, u2);
				}
			}
			const bool blocked = dot(wi, i_Surface.m_Geometric) <= 0.0f ||
								 occluded(i_Surface.m_Origin, wi, tmax < c_Infinity ? tmax * (1.0f - 1e-4f) - params.m_Epsilon : c_Infinity);
			if (blocked)
			{
				mask *= 1.0f - saturate(cosine);
			}
		}
		return mask;
	}
}

extern "C" __global__ void __raygen__lighting()
{
	const uint3 index = optixGetLaunchIndex();
	if (index.x >= params.m_Width || index.y >= params.m_Height)
	{
		return;
	}
	const unsigned int pixel = index.y * params.m_Width + index.x;
	const float3 background = make_float3(params.m_Background[0], params.m_Background[1], params.m_Background[2]);

	float3 sum = make_float3(0.0f, 0.0f, 0.0f);
	float first_depth = 1.0f;
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

		//	The camera ray, as the AO programs make it (optxAO.cu)
		const float nx = (index.x + jx) / params.m_Width * 2.0f - 1.0f;
		const float ny = 1.0f - (index.y + jy) / params.m_Height * 2.0f;
		float w;
		const float3 near_point = project(params.m_InvViewProj, nx, ny, 0.0f, w);
		const float3 mid_point = project(params.m_InvViewProj, nx, ny, 0.5f, w);
		const float3 far_point = project(params.m_InvViewProj, nx, ny, 1.0f, w);
		const float3 step = mid_point - near_point;
		const float3 direction = step * rsqrtf(dot(step, step));
		const float length = dot(far_point - near_point, direction);

		const Hit first = trace_camera(near_point, direction, length);
		float depth = 1.0f;
		if (!first.m_bHit)
		{
			sum = sum + background;
		}
		else
		{
			const float3 position = near_point + direction * first.m_T;
			depth = project(params.m_ViewProj, position.x, position.y, position.z, w).z;
			Surface surface = make_surface(position, first);

			if (params.m_Mode == e_optxLightingShadowMask)
			{
				const float mask = shadow_mask(surface, rng);
				sum = sum + make_float3(mask, mask, mask);
			}
			else
			{
				float3 radiance = make_float3(0.0f, 0.0f, 0.0f);
				float throughput = 1.0f;
				for (unsigned int bounce = 0;; ++bounce)
				{
					bool any_weighted = false;
					radiance = radiance + direct_light(surface, rng, any_weighted) * throughput;
					if (bounce >= params.m_Bounces && !any_weighted)
					{
						break;
					}
					const float u1 = rng.Next(), u2 = rng.Next();
					const float3 wi = cosine_direction(surface.m_Shading, u1, u2);
					if (dot(wi, surface.m_Geometric) <= 0.0f)
					{
						break;	// below the surface: the shading normal leans past the geometric one
					}
					const Hit next = trace_camera(surface.m_Origin, wi, c_Infinity);
					if (any_weighted)
					{
						radiance = radiance + bounce_light(surface, wi, next.m_bHit ? next.m_T : c_Infinity) * throughput;
					}
					if (bounce >= params.m_Bounces || !next.m_bHit)
					{
						break;
					}
					throughput *= params.m_Albedo;
					if (bounce + 1 >= 3)
					{
						const float survive = fminf(throughput, 0.95f);
						if (rng.Next() >= survive)
						{
							break;
						}
						throughput /= survive;
					}
					surface = make_surface(surface.m_Origin + wi * next.m_T, next);
				}
				sum = sum + radiance;
			}
		}

		if (sample == 0)
		{
			first_depth = fminf(fmaxf(depth, 0.0f), 1.0f);
		}
	}

	float4* accum = reinterpret_cast<float4*>(params.m_Accum) + pixel;
	float depth = first_depth;
	if (params.m_FirstSample != 0)
	{
		const float4 before = *accum;
		sum = sum + make_float3(before.x, before.y, before.z);
		depth = before.w;
	}
	*accum = make_float4(sum.x, sum.y, sum.z, depth);
	if (params.m_Resolve)
	{
		write_pixel(pixel, sum, params.m_FirstSample + params.m_Samples, depth);
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
	const float4 accum = reinterpret_cast<const float4*>(params.m_Accum)[pixel];
	write_pixel(pixel, make_float3(accum.x, accum.y, accum.z), params.m_FirstSample, accum.w);
}

extern "C" __global__ void __miss__camera()
{
	optixSetPayload_8(0);
}

extern "C" __global__ void __miss__occlusion()
{
	optixSetPayload_0(0);
}

namespace
{
	__forceinline__ __device__ float3 read_float3(unsigned long long i_Address)
	{
		const float* p = reinterpret_cast<const float*>(i_Address);
		return make_float3(p[0], p[1], p[2]);
	}
}

//	The hit's distance, world-space geometric normal (facing the ray) and
//	interpolated vertex normal (on the same side), and instance
extern "C" __global__ void __closesthit__camera()
{
	const unsigned int instance_id = optixGetInstanceId();
	const optxInstanceData& instance = reinterpret_cast<const optxInstanceData*>(params.m_Instances)[instance_id];
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
	const unsigned long long v0 = instance.m_Vertices + (unsigned long long)i0 * instance.m_VertexStride;
	const unsigned long long v1 = instance.m_Vertices + (unsigned long long)i1 * instance.m_VertexStride;
	const unsigned long long v2 = instance.m_Vertices + (unsigned long long)i2 * instance.m_VertexStride;
	const float3 p0 = read_float3(v0);
	const float3 p1 = read_float3(v1);
	const float3 p2 = read_float3(v2);

	const float3 direction = optixGetWorldRayDirection();
	float3 geometric = optixTransformNormalFromObjectToWorldSpace(cross(p1 - p0, p2 - p0));
	const float length2 = dot(geometric, geometric);
	if (!(length2 > 1e-30f))
	{
		geometric = direction * -1.0f;
	}
	else
	{
		geometric = geometric * rsqrtf(length2);
		if (dot(geometric, direction) > 0.0f)
		{
			geometric = geometric * -1.0f;
		}
	}

	float3 shading = geometric;
	if (instance.m_NormalOffset != c_optxNoNormal)
	{
		const float2 b = optixGetTriangleBarycentrics();
		const float3 n = read_float3(v0 + instance.m_NormalOffset) * (1.0f - b.x - b.y) +
						 read_float3(v1 + instance.m_NormalOffset) * b.x + read_float3(v2 + instance.m_NormalOffset) * b.y;
		float3 world = optixTransformNormalFromObjectToWorldSpace(n);
		const float n2 = dot(world, world);
		if (n2 > 1e-30f)
		{
			world = world * rsqrtf(n2);
			shading = dot(world, geometric) < 0.0f ? world * -1.0f : world;
		}
	}

	optixSetPayload_0(__float_as_uint(optixGetRayTmax()));
	optixSetPayload_1(__float_as_uint(geometric.x));
	optixSetPayload_2(__float_as_uint(geometric.y));
	optixSetPayload_3(__float_as_uint(geometric.z));
	optixSetPayload_4(__float_as_uint(shading.x));
	optixSetPayload_5(__float_as_uint(shading.y));
	optixSetPayload_6(__float_as_uint(shading.z));
	optixSetPayload_7(instance_id);
	optixSetPayload_8(1);
}
