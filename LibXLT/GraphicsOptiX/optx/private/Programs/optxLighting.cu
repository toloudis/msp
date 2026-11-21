/*****************************************************************************
**  optxLighting.cu
**
**      The lighting programs: the beauty pass (a path tracer), Illumination
**	Only, Shadow Mask and the tracer debug view, traced. See
**	optxLightingParams.h (what each launch and mode does), optxLightTypes.h
**	(the lights), optxMaterialTypes.h and optxMaterial.h (materials,
**	textures, ray cones, opacity), optxBsdf.h (the BRDF). The device code
**	the modes share is in optxRays.h (rays), optxLights.h (lights),
**	optxSurface.h (shading points) and optxEnvironment.h (the beauty
**	pass's environment light).
**
**	Compiled to OptiX IR by nvcc at build time and embedded as
**	"optxLighting" (optxEmbeddedPrograms). Entry points:
**	__raygen__lighting and __raygen__resolve, __raygen__env_cells,
**	__raygen__env_rows and __raygen__env_marginal (the environment's
**	sampling table; they trace nothing), __miss__camera (miss
**	record 0), __miss__occlusion (miss record 1), __closesthit__camera
**	and __anyhit__opacity (every hit group record: one per build input of
**	the biggest GAS; opaque build inputs disable the any-hit program).
**	Every ray is traced from a ray generation program (trace depth 1).
**	Occlusion rays run no closest hit program.
**
**	Extra Large Technology
\****************************************************************************/
#include <optix.h>
#include <cuda_fp16.h>

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
#ifndef GOX_OPTXMATERIALTYPES_H
#include "optxMaterialTypes.h"
#endif
#ifndef GOX_OPTXMATERIAL_H
#include "optxMaterial.h"
#endif
#ifndef GOX_OPTXVECTOR_H
#include "optxVector.h"
#endif
#ifndef GOX_OPTXBSDF_H
#include "optxBsdf.h"
#endif

using namespace gox;

extern "C"
{
	__constant__ optxLightingParams params;
}

#ifndef GOX_OPTXRAYS_H
#include "optxRays.h"
#endif
#ifndef GOX_OPTXLIGHTS_H
#include "optxLights.h"
#endif
#ifndef GOX_OPTXSURFACE_H
#include "optxSurface.h"
#endif
#ifndef GOX_OPTXENVIRONMENT_H
#include "optxEnvironment.h"
#endif

using namespace gox::optxv;
using namespace gox::optxr;
using namespace gox::optxl;
using namespace gox::optxs;

namespace
{
	//------------------------------------------------------------------------
	//	Output
	//------------------------------------------------------------------------

	//	The 8-bit resolve: the mean, clamped, as linear bytes (the raster
	//	copies its linear buffer into the 8-bit target the same way)
	__forceinline__ __device__ unsigned int to_byte(float c)
	{
		return static_cast<unsigned int>(saturate(c) * 255.0f + 0.5f);
	}

	__forceinline__ __device__ void write_depth(unsigned int i_Pixel, float i_Depth)
	{
		unsigned int* depth = reinterpret_cast<unsigned int*>(params.m_Depth);
		depth[i_Pixel] = params.m_DepthLayout == e_optxDepthFloat ? __float_as_uint(i_Depth)
																  : min(__float2uint_rn(i_Depth * 16777215.0f), 16777215u);
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
		write_depth(i_Pixel, i_Depth);
	}

	//	A half, finite (NaN reads as 0; beyond the largest half, the largest)
	__forceinline__ __device__ unsigned short to_half(float c)
	{
		const float v = (c == c) ? fminf(fmaxf(c, -65504.0f), 65504.0f) : 0.0f;
		return __half_as_ushort(__float2half_rn(v));
	}

	__forceinline__ __device__ uint2 half4(float x, float y, float z, float w)
	{
		return make_uint2(to_half(x) | (unsigned(to_half(y)) << 16), to_half(z) | (unsigned(to_half(w)) << 16));
	}

	//	The beauty resolve: the linear mean (unclamped) and coverage as
	//	RGBA16F, or with m_DenoiseInput the denoiser's input planes (the
	//	means of colour and coverage, albedo, normal); and depth
	__forceinline__ __device__ void write_hdr_pixel(unsigned int i_Pixel, const float4* i_Planes, unsigned int i_nSamples)
	{
		const unsigned int count = params.m_Width * params.m_Height;
		const float4 color = i_Planes[0];
		const float4 guide = i_Planes[count];
		const float scale = 1.0f / i_nSamples;
		if (params.m_DenoiseInput)
		{
			const float4 normal = i_Planes[2 * count];
			float4* input = reinterpret_cast<float4*>(params.m_DenoiseInput) + i_Pixel;
			input[0] = make_float4(color.x * scale, color.y * scale, color.z * scale, guide.w * scale);
			input[count] = make_float4(guide.x * scale, guide.y * scale, guide.z * scale, 1.0f);
			input[2 * count] = make_float4(normal.x * scale, normal.y * scale, normal.z * scale, 0.0f);
		}
		else
		{
			reinterpret_cast<uint2*>(params.m_Color)[i_Pixel] = half4(color.x * scale, color.y * scale, color.z * scale, guide.w * scale);
		}
		write_depth(i_Pixel, color.w);
	}

	//	The thin lens (optxLightingParams.h): a camera ray from a point of
	//	the aperture towards where the pinhole ray (i_Origin on the near
	//	plane, i_Direction) meets the plane in focus; its length to the far
	//	plane becomes io_Length
	__forceinline__ __device__ void lens_ray(float3& io_Origin, float3& io_Direction, float& io_Length, float u1, float u2)
	{
		const float3 eye = load3(params.m_Eye);
		const float3 forward = load3(params.m_Forward);
		const float along = dot(io_Direction, forward);
		if (!(along > 1e-6f))
		{
			return;
		}
		const float near_depth = dot(io_Origin - eye, forward);
		const float far_depth = near_depth + io_Length * along;
		const float3 focus = io_Origin + io_Direction * ((params.m_FocusDistance - near_depth) / along);
		const float r = params.m_LensRadius * sqrtf(u1);
		const float phi = 2.0f * c_Pi * u2;
		const float3 lens = eye + load3(params.m_LensRight) * (r * __cosf(phi)) + load3(params.m_LensUp) * (r * __sinf(phi));
		const float3 d = normalize(focus - lens);
		const float d_along = fmaxf(dot(d, forward), 1e-6f);
		io_Origin = lens + d * (near_depth / d_along);
		io_Direction = d;
		io_Length = (far_depth - near_depth) / d_along;
	}

	//------------------------------------------------------------------------
	//	Illumination Only and the Shadow Mask
	//------------------------------------------------------------------------

	__forceinline__ __device__ bool shadowed_pair(const Surface& i_Surface, const optxLight& i_Light)
	{
		return params.m_bShadows && i_Surface.m_bReceivesShadow && (i_Light.m_Flags & e_optxLightShadows);
	}

	//	Whether the launch runs the beauty path: the beauty pass, or
	//	Reflection Only (the beauty path after its first hit)
	__forceinline__ __device__ bool beauty()
	{
		return params.m_Mode == e_optxLightingBeauty || params.m_Mode == e_optxLightingReflection;
	}

	__forceinline__ __device__ bool reflection_only()
	{
		return params.m_Mode == e_optxLightingReflection;
	}

	//	Whether a light is sampled by both light samples and bounce rays
	//	(an area light whose shadows are traced; Illumination: with the
	//	cosine on)
	__forceinline__ __device__ bool weighted(const Surface& i_Surface, const optxLight& i_Light)
	{
		return is_area(i_Light) && shadowed_pair(i_Surface, i_Light) && (beauty() || params.m_bUseCosine) &&
			   params.m_Sampling != e_optxSampleLights;
	}

	__forceinline__ __device__ float cosine_term(const Surface& i_Surface, float3 i_Direction)
	{
		return params.m_bUseCosine ? saturate(dot(i_Direction, i_Surface.m_Shading)) : 1.0f;
	}

	//	Whether a light sample's direction is blocked
	__forceinline__ __device__ bool blocked(const Surface& i_Surface, float3 i_Wi, float i_Distance)
	{
		return dot(i_Wi, i_Surface.m_Geometric) <= 0.0f || occluded(i_Surface.m_Origin, i_Wi, shadow_tmax(i_Distance));
	}

	//	Illumination: the direct light at a surface by light samples (one
	//	per light; the lights' diffuse colour), and whether any light there
	//	is also reached by bounce rays
	__forceinline__ __device__ float3 direct_light(const Surface& i_Surface, Rng& io_Rng, bool& o_bWeighted)
	{
		float3 sum = splat3(0.0f);
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
			color = color * diffuse(light);
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
				area_sample(light, i_Surface.m_P, u1, u2, wi, tmax, pdf_light);
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
			if (shadows && blocked(i_Surface, wi, tmax))
			{
				continue;
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

	//	Illumination: the light a bounce ray from a surface brings from the
	//	area lights it reaches before i_THit (the nearest geometry,
	//	c_Infinity for none), weighted against their light samples. With the
	//	cosine-weighted bounce pdf (cos / pi), a hit's estimate is the
	//	light's radiance: pi times the light at its centre over its solid
	//	angle density (a disk: d^2 / (area cos); a cone: 1 / solid angle),
	//	so that a light sample's estimate (colour times cosine) has the same
	//	expectation
	__forceinline__ __device__ float3 bounce_light(const Surface& i_Surface, float3 i_W, float i_THit)
	{
		float3 sum = splat3(0.0f);
		const float pdf_bounce = saturate(dot(i_W, i_Surface.m_Shading)) / c_Pi;
		for (unsigned int i = 0; i < i_Surface.m_Count; ++i)
		{
			const optxLight& light = light_of(i_Surface.m_First, i, i_Surface.m_bAll);
			if (!weighted(i_Surface, light))
			{
				continue;
			}
			float3 color, to_light;
			float distance, pdf_light;
			if (!light_at(light, i_Surface.m_P, color, to_light, distance) ||
				!area_hit(light, i_Surface.m_P, i_Surface.m_Origin, i_W, i_THit, pdf_light))
			{
				continue;
			}
			const float weight = params.m_Sampling == e_optxSampleBounces ? 1.0f : power_heuristic(pdf_bounce, pdf_light);
			sum = sum + color * (diffuse(light) * c_Pi * pdf_light * weight);
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
				float pdf;
				area_sample(light, p, u1, u2, wi, tmax, pdf);
			}
			if (blocked(i_Surface, wi, tmax))
			{
				mask *= 1.0f - saturate(cosine);
			}
		}
		return mask;
	}

	//	Illumination Only: the light arriving at the first hit, then up to
	//	m_Bounces diffuse bounces carrying the surfaces' albedo
	__forceinline__ __device__ float3 illumination(Surface surface, Rng& rng, float width, float spread)
	{
		float3 radiance = splat3(0.0f);
		float3 throughput = splat3(1.0f);
		for (unsigned int bounce = 0;; ++bounce)
		{
			bool any_weighted = false;
			radiance = radiance + mul3(direct_light(surface, rng, any_weighted), throughput);
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
				radiance = radiance + mul3(bounce_light(surface, wi, next.m_bHit ? next.m_T : c_Infinity), throughput);
			}
			if (bounce >= params.m_Bounces || !next.m_bHit)
			{
				break;
			}
			//	The path leaves with this surface's diffuse albedo
			throughput = mul3(throughput, surface.m_Albedo);
			if (bounce + 1 >= 3)
			{
				const float survive = fminf(max3(throughput), 0.95f);
				if (rng.Next() >= survive)
				{
					break;
				}
				throughput = throughput * (1.0f / survive);
			}
			spread += c_optxBounceSpread;
			width += spread * next.m_T;
			surface = make_surface(surface.m_Origin + wi * next.m_T, next, wi, width, false);
		}
		return radiance;
	}

	//------------------------------------------------------------------------
	//	The beauty pass
	//------------------------------------------------------------------------

	//	What a light gives through the BRDF towards wo from direction wi:
	//	pi (f_diffuse Cd + f_specular Cs) cos, Cd and Cs the light's colour
	//	times its diffuse and specular switches
	__forceinline__ __device__ float3 lit(const optxLight& i_Light, float3 i_Color, const optxb::Value& i_F, float i_Cosine)
	{
		return mul3(i_Color, (i_F.m_Diffuse * diffuse(i_Light) + i_F.m_Specular * specular(i_Light)) * (c_Pi * i_Cosine));
	}

	//	How much a light might give a point, for choosing one (step 26):
	//	its unshadowed light there (attenuation, penumbra, projector
	//	texture) times the cosine to its centre, as luminance; 0 if it can't
	//	light the point at all
	__forceinline__ __device__ float importance(const optxLight& i_Light, const Surface& i_Surface, float3& o_Color,
												float3& o_ToLight, float& o_Distance)
	{
		if ((i_Light.m_Flags & (e_optxLightDiffuse | e_optxLightSpecular)) == 0 ||
			!light_at(i_Light, i_Surface.m_P, o_Color, o_ToLight, o_Distance))
		{
			return 0.0f;
		}
		const float cosine = dot(normalize(o_ToLight), i_Surface.m_Shading);
		return luminance(o_Color) * fmaxf(cosine, 0.0f);
	}

	//	The light chosen at a surface: one in proportion to its importance
	//	(one random number, kept uniform as it is reused down the list; the
	//	environment, if it lights surfaces, is the last choice, index
	//	m_Count), or every light (e_optxSelectAll)
	struct Chosen
	{
		unsigned int	m_Index;		// in the surface's light range; m_Count: the environment
		float3			m_Color;
		float3			m_ToLight;
		float			m_Distance;
		float			m_Importance;
		float			m_Total;		// the importances' sum: the light's chance is m_Importance / m_Total
	};

	__forceinline__ __device__ bool choose_light(const Surface& i_Surface, float u, Chosen& o_Chosen)
	{
		o_Chosen.m_Total = 0.0f;
		bool found = false;
		for (unsigned int i = 0; i <= i_Surface.m_Count; ++i)
		{
			float3 color = splat3(0.0f), to_light = splat3(0.0f);
			float distance = 0.0f;
			float w;
			if (i < i_Surface.m_Count)
			{
				w = importance(light_of(i_Surface.m_First, i, i_Surface.m_bAll), i_Surface, color, to_light, distance);
			}
			else
			{
				w = optxe::importance();
			}
			if (!(w > 0.0f))
			{
				continue;
			}
			o_Chosen.m_Total += w;
			const float chance = w / o_Chosen.m_Total;
			if (u < chance)
			{
				u = u / chance;
				o_Chosen.m_Index = i;
				o_Chosen.m_Color = color;
				o_Chosen.m_ToLight = to_light;
				o_Chosen.m_Distance = distance;
				o_Chosen.m_Importance = w;
				found = true;
			}
			else
			{
				u = (u - chance) / (1.0f - chance);
			}
		}
		return found;
	}

	//	The chance the environment had of being chosen at a surface
	__forceinline__ __device__ float environment_chance(float i_Total)
	{
		if (params.m_LightSelection == e_optxSelectAll)
		{
			return 1.0f;
		}
		return i_Total > 0.0f ? optxe::importance() / i_Total : 0.0f;
	}

	//	The environment's light sample at a surface through the BRDF (its map
	//	read at i_Level), over i_Chance (its chance of being chosen), weighted
	//	against the BRDF's pdf. Every surface the camera sees occludes it, as
	//	it stops BRDF rays (casting shadows or not), so both strategies see
	//	the same sky.
	__forceinline__ __device__ float3 environment_sample(const Surface& i_Surface, float3 i_Wo, float i_Chance, float i_Level,
														 Rng& io_Rng)
	{
		const float u1 = io_Rng.Next(), u2 = io_Rng.Next();
		if (params.m_Sampling == e_optxSampleBounces || !(i_Chance > 0.0f))
		{
			return splat3(0.0f);	// BRDF rays alone
		}
		float3 wi;
		float pdf;
		if (!optxe::sample(i_Surface, u1, u2, wi, pdf))
		{
			return splat3(0.0f);
		}
		const float cosine = dot(wi, i_Surface.m_Shading);
		if (cosine <= 0.0f || dot(wi, i_Surface.m_Geometric) <= 0.0f)
		{
			return splat3(0.0f);
		}
		const optxb::Value f = optxb::evaluate(i_Surface.m_Bsdf, i_Surface.m_Shading, i_Wo, wi);
		const float3 value = optxe::lit(optxe::radiance(wi, i_Level), f, cosine);
		if (max3(value) <= 0.0f || occluded(i_Surface.m_Origin, wi, c_Infinity, e_optxMaskCamera))
		{
			return splat3(0.0f);
		}
		float weight = 1.0f / (i_Chance * pdf);
		if (params.m_Sampling == e_optxSampleBoth)
		{
			weight *= power_heuristic(i_Chance * pdf, f.m_Pdf);
		}
		return value * weight;
	}

	//	The chance a light had of being chosen at a surface (1 with every
	//	light selected)
	__forceinline__ __device__ float select_chance(const optxLight& i_Light, const Surface& i_Surface, float i_Total)
	{
		if (params.m_LightSelection == e_optxSelectAll)
		{
			return 1.0f;
		}
		float3 color, to_light;
		float distance;
		const float w = importance(i_Light, i_Surface, color, to_light, distance);
		return i_Total > 0.0f ? w / i_Total : 0.0f;
	}

	//	One light's sample at a surface through the BRDF: shadowed, and
	//	(area lights also reached by BRDF rays) weighted against the BRDF's
	//	pdf, the light's pdf times i_Chance (its chance of being chosen)
	__forceinline__ __device__ float3 light_sample(const Surface& i_Surface, float3 i_Wo, const optxLight& i_Light, float3 i_Color,
												   float3 i_ToLight, float i_Distance, float i_Chance, Rng& io_Rng)
	{
		const bool shadows = shadowed_pair(i_Surface, i_Light);
		const bool mis = weighted(i_Surface, i_Light);
		const float u1 = io_Rng.Next(), u2 = io_Rng.Next();
		if (mis && params.m_Sampling == e_optxSampleBounces)
		{
			return splat3(0.0f);	// BRDF rays alone
		}
		float3 wi;
		float pdf_light = 0.0f;
		float tmax = i_Distance;
		if (shadows && is_area(i_Light))
		{
			area_sample(i_Light, i_Surface.m_P, u1, u2, wi, tmax, pdf_light);
		}
		else
		{
			wi = normalize(i_ToLight);
		}
		const float cosine = dot(wi, i_Surface.m_Shading);
		if (cosine <= 0.0f)
		{
			return splat3(0.0f);
		}
		const optxb::Value f = optxb::evaluate(i_Surface.m_Bsdf, i_Surface.m_Shading, i_Wo, wi);
		float3 value = lit(i_Light, i_Color, f, cosine);
		if (max3(value) <= 0.0f || (shadows && blocked(i_Surface, wi, tmax)))
		{
			return splat3(0.0f);
		}
		float weight = 1.0f / i_Chance;
		if (mis)
		{
			weight *= power_heuristic(i_Chance * pdf_light, f.m_Pdf);
		}
		return value * weight;
	}

	//	The beauty pass's direct light at a surface (next-event estimation):
	//	one chosen light's sample over its chance, or with e_optxSelectAll
	//	every light's (the environment's map read at i_EnvLevel); o_Total:
	//	the importances' sum (for select_chance), and whether any light
	//	there is also reached by BRDF rays (an area light, the environment)
	__forceinline__ __device__ float3 beauty_direct(const Surface& i_Surface, float3 i_Wo, float i_EnvLevel, Rng& io_Rng,
													float& o_Total, bool& o_bWeighted)
	{
		o_Total = 0.0f;
		o_bWeighted = optxe::lights();
		for (unsigned int i = 0; i < i_Surface.m_Count; ++i)
		{
			o_bWeighted |= weighted(i_Surface, light_of(i_Surface.m_First, i, i_Surface.m_bAll));
		}
		if (params.m_LightSelection == e_optxSelectAll)
		{
			float3 sum = splat3(0.0f);
			for (unsigned int i = 0; i < i_Surface.m_Count; ++i)
			{
				const optxLight& light = light_of(i_Surface.m_First, i, i_Surface.m_bAll);
				float3 color, to_light;
				float distance;
				if (light_at(light, i_Surface.m_P, color, to_light, distance))
				{
					sum = sum + light_sample(i_Surface, i_Wo, light, color, to_light, distance, 1.0f, io_Rng);
				}
			}
			if (optxe::lights())
			{
				sum = sum + environment_sample(i_Surface, i_Wo, 1.0f, i_EnvLevel, io_Rng);
			}
			return sum;
		}
		Chosen chosen;
		if (!choose_light(i_Surface, io_Rng.Next(), chosen))
		{
			return splat3(0.0f);
		}
		o_Total = chosen.m_Total;
		if (chosen.m_Index == i_Surface.m_Count)
		{
			return environment_sample(i_Surface, i_Wo, chosen.m_Importance / chosen.m_Total, i_EnvLevel, io_Rng);
		}
		const optxLight& light = light_of(i_Surface.m_First, chosen.m_Index, i_Surface.m_bAll);
		return light_sample(i_Surface, i_Wo, light, chosen.m_Color, chosen.m_ToLight, chosen.m_Distance,
							chosen.m_Importance / chosen.m_Total, io_Rng);
	}

	//	The light a BRDF ray from a surface brings from the area lights it
	//	reaches before i_THit, weighted against their light samples (each
	//	light's pdf times its chance of being chosen; with i_bAlone, no light
	//	was sampled there, so the ray takes it all). An area light's
	//	radiance is pi times its light at the surface over its solid angle
	//	density (see optxLightTypes.h)
	__forceinline__ __device__ float3 beauty_bounce_light(const Surface& i_Surface, const optxb::Sample& i_Sample, float i_THit,
														  float i_Total, bool i_bAlone)
	{
		float3 sum = splat3(0.0f);
		const float cosine = dot(i_Sample.m_Wi, i_Surface.m_Shading);
		for (unsigned int i = 0; i < i_Surface.m_Count; ++i)
		{
			const optxLight& light = light_of(i_Surface.m_First, i, i_Surface.m_bAll);
			//	Alone, every area light whose shadows are traced is reached
			//	(whatever m_Sampling says: nothing else can reach it here)
			if (i_bAlone ? !(is_area(light) && shadowed_pair(i_Surface, light)) : !weighted(i_Surface, light))
			{
				continue;
			}
			float3 color, to_light;
			float distance, pdf_light;
			if (!light_at(light, i_Surface.m_P, color, to_light, distance) ||
				!area_hit(light, i_Surface.m_P, i_Surface.m_Origin, i_Sample.m_Wi, i_THit, pdf_light))
			{
				continue;
			}
			float weight = 1.0f;
			if (!i_bAlone && params.m_Sampling != e_optxSampleBounces)
			{
				weight = power_heuristic(i_Sample.m_Value.m_Pdf, select_chance(light, i_Surface, i_Total) * pdf_light);
			}
			//	L f cos / pdf, L = pi pdf_light C
			sum = sum + lit(light, color, i_Sample.m_Value, cosine) * (pdf_light * weight / i_Sample.m_Value.m_Pdf);
		}
		return sum;
	}

	//	Scaled down so no component exceeds m_MaxRadiance (0: as it is)
	__forceinline__ __device__ float3 clamp_radiance(float3 c)
	{
		if (params.m_MaxRadiance > 0.0f)
		{
			const float m = max3(c);
			if (m > params.m_MaxRadiance)
			{
				return c * (params.m_MaxRadiance / m);
			}
		}
		return c;
	}

	//	The beauty pass's radiance along a camera ray from its first hit: a
	//	unidirectional path tracer with next-event estimation and multiple
	//	importance sampling (optxLightingParams.h). Reflection Only: at the
	//	first hit, no emission, no light sample and no diffuse lobe, only one
	//	sample of the specular lobe (weighted f_spec cos / pdf_spec, so the
	//	BRDF's Fresnel weights the reflection) whose ray brings all the light
	//	it finds; from the surface it reaches, the beauty path, with
	//	m_Bounces bounces after that surface.
	__forceinline__ __device__ float3 beauty_path(Surface surface, float3 direction, Rng& rng, float width, float spread)
	{
		//	Reflection Only's reflection vertex comes before the beauty path's
		//	first hit: it doesn't count as a bounce
		const unsigned int first = reflection_only() ? 1u : 0u;
		float3 radiance = first ? splat3(0.0f) : surface.m_Emission;
		float3 throughput = splat3(1.0f);
		for (unsigned int bounce = 0;; ++bounce)
		{
			const float3 wo = direction * -1.0f;
			const bool reflect = bounce < first;
			//	Light from here on is limited past the beauty path's first hit
			const bool limited = bounce > first;
			//	One environment level for every ray leaving this surface
			const float env_level = optxe::surface_level(spread + c_optxBounceSpread * surface.m_Bsdf.m_Roughness);
			float total = 0.0f;
			bool any_weighted = true;	// Reflection Only: the reflection ray brings all its light
			if (!reflect)
			{
				const float3 direct = mul3(beauty_direct(surface, wo, env_level, rng, total, any_weighted), throughput);
				radiance = radiance + (limited ? clamp_radiance(direct) : direct);
				if (bounce >= params.m_Bounces + first && !any_weighted)
				{
					break;
				}
			}
			const float u0 = rng.Next(), u1 = rng.Next(), u2 = rng.Next();
			const optxb::Sample sample = reflect ? optxb::sample_specular(surface.m_Bsdf, surface.m_Shading, wo, u1, u2)
												 : optxb::sample(surface.m_Bsdf, surface.m_Shading, wo, u0, u1, u2);
			if (sample.m_Lobe == optxb::e_LobeNone || dot(sample.m_Wi, surface.m_Geometric) <= 0.0f)
			{
				break;	// no direction, or one below the surface
			}
			const Hit next = trace_camera(surface.m_Origin, sample.m_Wi, c_Infinity);
			if (any_weighted)
			{
				float3 light = beauty_bounce_light(surface, sample, next.m_bHit ? next.m_T : c_Infinity, total, reflect);
				if (!next.m_bHit && optxe::lights() && (reflect || params.m_Sampling != e_optxSampleLights))
				{
					//	Into the environment, weighted against its light samples
					//	(none at the reflection vertex)
					float weight = 1.0f;
					if (!reflect && params.m_Sampling == e_optxSampleBoth)
					{
						weight = power_heuristic(sample.m_Value.m_Pdf, environment_chance(total) * optxe::pdf(sample.m_Wi, surface));
					}
					const float cosine = dot(sample.m_Wi, surface.m_Shading);
					light = light + optxe::lit(optxe::radiance(sample.m_Wi, env_level), sample.m_Value, cosine) *
										(weight / sample.m_Value.m_Pdf);
				}
				light = mul3(light, throughput);
				radiance = radiance + (limited ? clamp_radiance(light) : light);
			}
			if (bounce >= params.m_Bounces + first || !next.m_bHit)
			{
				break;
			}
			throughput = mul3(throughput, sample.m_Weight);
			if (bounce + 1 >= 3 + first)
			{
				const float survive = fminf(max3(throughput), 0.95f);
				if (rng.Next() >= survive)
				{
					break;
				}
				throughput = throughput * (1.0f / survive);
			}
			//	The cone widens by the lobe's spread: a mirror keeps it sharp
			spread += sample.m_Lobe == optxb::e_LobeDiffuse ? c_optxBounceSpread : c_optxBounceSpread * surface.m_Bsdf.m_Roughness;
			width += spread * next.m_T;
			direction = sample.m_Wi;
			surface = make_surface(surface.m_Origin + direction * next.m_T, next, direction, width, true);
			//	(Reflection Only: the surface the reflection shows glows as
			//	the beauty pass's first hit does, unlimited)
			const float3 emitted = mul3(surface.m_Emission, throughput);
			radiance = radiance + (reflect ? emitted : clamp_radiance(emitted));
		}
		return radiance;
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
	const float3 background = load3(params.m_Background);
	const bool hdr = beauty();

	float3 sum = splat3(0.0f);
	float3 albedo = splat3(0.0f);		// beauty: first-hit guides
	float3 normal = splat3(0.0f);
	float coverage = 0.0f;
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
		float3 direction = step * rsqrtf(dot(step, step));
		float length = dot(far_point - near_point, direction);
		float3 origin = near_point;
		if (hdr && params.m_LensRadius > 0.0f && sample != 0)
		{
			//	sample 0 stays the pinhole's: it gives the depth the raster overlay tests
			const float u1 = rng.Next(), u2 = rng.Next();
			lens_ray(origin, direction, length, u1, u2);
		}

		const Hit first = trace_camera(origin, direction, length);
		float depth = 1.0f;
		if (!first.m_bHit)
		{
			//	The beauty pass shows the environment behind the scene if asked
			//	(Reflection Only never: it shows reflections alone)
			const bool show_environment = hdr && !reflection_only() && optxe::background();
			sum = sum + (show_environment ? optxe::radiance(direction, optxe::level_for(params.m_ConeSpread)) : background);
		}
		else
		{
			const float3 position = origin + direction * first.m_T;
			depth = project(params.m_ViewProj, position.x, position.y, position.z, w).z;
			const float width = params.m_ConeWidth + params.m_ConeSpread * first.m_T;
			const Surface surface = make_surface(position, first, direction, width, hdr);

			if (params.m_Mode == e_optxLightingDebug)
			{
				sum = sum + debug_value(first, surface);
			}
			else if (params.m_Mode == e_optxLightingShadowMask)
			{
				sum = sum + splat3(shadow_mask(surface, rng));
			}
			else if (hdr)
			{
				sum = sum + beauty_path(surface, direction, rng, width, params.m_ConeSpread);
				//	The denoiser's albedo guide: what the image is the light
				//	times. Reflection Only shows the specular lobe alone,
				//	weighted by the BRDF's Fresnel at the view.
				albedo = albedo + (reflection_only() ? optxb::fresnel(surface.m_Bsdf, dot(surface.m_Shading, direction * -1.0f))
													 : surface.m_BaseColor);
				normal = normal + surface.m_Shading;
				coverage += 1.0f;
			}
			else
			{
				sum = sum + illumination(surface, rng, width, params.m_ConeSpread);
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
	if (hdr)
	{
		//	Two more planes: albedo and coverage; normal
		const unsigned int count = params.m_Width * params.m_Height;
		if (params.m_FirstSample != 0)
		{
			const float4 a = accum[count];
			const float4 n = accum[2 * count];
			albedo = albedo + make_float3(a.x, a.y, a.z);
			coverage += a.w;
			normal = normal + make_float3(n.x, n.y, n.z);
		}
		accum[count] = make_float4(albedo.x, albedo.y, albedo.z, coverage);
		accum[2 * count] = make_float4(normal.x, normal.y, normal.z, 0.0f);
		if (params.m_Resolve)
		{
			write_hdr_pixel(pixel, accum, params.m_FirstSample + params.m_Samples);
		}
	}
	else if (params.m_Resolve)
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
	const float4* accum = reinterpret_cast<const float4*>(params.m_Accum) + pixel;
	if (beauty())
	{
		write_hdr_pixel(pixel, accum, params.m_FirstSample);
		return;
	}
	write_pixel(pixel, make_float3(accum->x, accum->y, accum->z), params.m_FirstSample, accum->w);
}

//	The environment's sampling table (optxLightingParams.h): each cell's
//	luminance of colour x map (m_Taps x m_Taps samples at m_TapLevel)
//	times sin theta at its centre
extern "C" __global__ void __raygen__env_cells()
{
	const uint3 index = optixGetLaunchIndex();
	const optxEnvironmentLight& env = params.m_Environment;
	const unsigned int W = env.m_TableWidth, H = env.m_TableHeight;
	const optxTexture* map = optxe::texture();
	if (index.x >= W || index.y >= H || !map || !env.m_Table)
	{
		return;
	}
	const float3 color = load3(env.m_Color);
	const unsigned int taps = max(env.m_Taps, 1u);
	float sum = 0.0f;
	for (unsigned int b = 0; b < taps; ++b)
	{
		const float v = (index.y + (b + 0.5f) / taps) / H;
		for (unsigned int a = 0; a < taps; ++a)
		{
			const float u = (index.x + (a + 0.5f) / taps) / W;
			const float4 s = optxm::sample(*map, u, v, env.m_TapLevel);
			sum += fmaxf(luminance(mul3(color, make_float3(s.x, s.y, s.z))), 0.0f);
		}
	}
	const float sin_theta = __sinf((index.y + 0.5f) / H * c_Pi);
	float* table = reinterpret_cast<float*>(env.m_Table);
	table[index.y * W + index.x] = sum / float(taps * taps) * sin_theta;
}

//	Each row's sum and CDF (from 0 to 1; even steps for a row of zeros)
extern "C" __global__ void __raygen__env_rows()
{
	const uint3 index = optixGetLaunchIndex();
	const optxEnvironmentLight& env = params.m_Environment;
	const unsigned int W = env.m_TableWidth, H = env.m_TableHeight;
	if (index.x >= H || !env.m_Table)
	{
		return;
	}
	float* table = reinterpret_cast<float*>(env.m_Table);
	const float* cells = table + index.x * W;
	float* cdf = table + optxEnvRowCdfAt(W, H) + index.x * (W + 1);
	float sum = 0.0f;
	cdf[0] = 0.0f;
	for (unsigned int i = 0; i < W; ++i)
	{
		sum += cells[i];
		cdf[i + 1] = sum;
	}
	for (unsigned int i = 1; i <= W; ++i)
	{
		cdf[i] = sum > 0.0f ? cdf[i] / sum : float(i) / W;
	}
	cdf[W] = 1.0f;
	table[optxEnvRowSumsAt(W, H) + index.x] = sum;
}

//	The rows' CDF and the cells' total
extern "C" __global__ void __raygen__env_marginal()
{
	const uint3 index = optixGetLaunchIndex();
	const optxEnvironmentLight& env = params.m_Environment;
	const unsigned int W = env.m_TableWidth, H = env.m_TableHeight;
	if (index.x != 0 || index.y != 0 || !env.m_Table)
	{
		return;
	}
	float* table = reinterpret_cast<float*>(env.m_Table);
	const float* sums = table + optxEnvRowSumsAt(W, H);
	float* cdf = table + optxEnvMarginalAt(W, H);
	float total = 0.0f;
	cdf[0] = 0.0f;
	for (unsigned int j = 0; j < H; ++j)
	{
		total += sums[j];
		cdf[j + 1] = total;
	}
	for (unsigned int j = 1; j <= H; ++j)
	{
		cdf[j] = total > 0.0f ? cdf[j] / total : float(j) / H;
	}
	cdf[H] = 1.0f;
	table[optxEnvTotalAt(W, H)] = total;
}

extern "C" __global__ void __miss__camera()
{
	optixSetPayload_8(0);
}

extern "C" __global__ void __miss__occlusion()
{
	optixSetPayload_0(0);
}

//	The hit (see Hit): its distance, world-space geometric normal (facing
//	the ray), interpolated vertex normal and tangents S and T (flipped
//	together onto the geometric normal's side, as the raster flips the
//	bumped normal of a back face), instance, material, transformed texture
//	coordinates and ray-cone level base
extern "C" __global__ void __closesthit__camera()
{
	const unsigned int instance_id = optixGetInstanceId();
	const optxInstanceData& instance = reinterpret_cast<const optxInstanceData*>(params.m_Instances)[instance_id];
	const optxGeometryInput& input = reinterpret_cast<const optxGeometryInput*>(instance.m_Inputs)[optixGetSbtGASIndex()];
	const uint3 idx = optxm::triangle_indices(input, optixGetPrimitiveIndex());
	const unsigned long long v0 = instance.m_Vertices + (unsigned long long)idx.x * instance.m_VertexStride;
	const unsigned long long v1 = instance.m_Vertices + (unsigned long long)idx.y * instance.m_VertexStride;
	const unsigned long long v2 = instance.m_Vertices + (unsigned long long)idx.z * instance.m_VertexStride;
	const float3 p0 = read_float3(v0);
	const float3 p1 = read_float3(v1);
	const float3 p2 = read_float3(v2);
	const float2 b = optixGetTriangleBarycentrics();
	const float b0 = 1.0f - b.x - b.y;

	const float3 direction = optixGetWorldRayDirection();
	const float3 e1 = optixTransformVectorFromObjectToWorldSpace(p1 - p0);
	const float3 e2 = optixTransformVectorFromObjectToWorldSpace(p2 - p0);
	const float world_area = sqrtf(dot(cross(e1, e2), cross(e1, e2)));
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
	float flip = 1.0f;
	if (instance.m_NormalOffset != c_optxNoAttribute)
	{
		const float3 n = read_float3(v0 + instance.m_NormalOffset) * b0 + read_float3(v1 + instance.m_NormalOffset) * b.x +
						 read_float3(v2 + instance.m_NormalOffset) * b.y;
		float3 world = optixTransformNormalFromObjectToWorldSpace(n);
		const float n2 = dot(world, world);
		if (n2 > 1e-30f)
		{
			world = world * rsqrtf(n2);
			flip = dot(world, geometric) < 0.0f ? -1.0f : 1.0f;
			shading = world * flip;
		}
	}

	float3 s = splat3(0.0f), t = splat3(0.0f);
	if (instance.m_TangentOffset != c_optxNoAttribute && instance.m_BitangentOffset != c_optxNoAttribute)
	{
		s = optixTransformVectorFromObjectToWorldSpace(read_float3(v0 + instance.m_TangentOffset) * b0 +
													   read_float3(v1 + instance.m_TangentOffset) * b.x +
													   read_float3(v2 + instance.m_TangentOffset) * b.y) * flip;
		t = optixTransformVectorFromObjectToWorldSpace(read_float3(v0 + instance.m_BitangentOffset) * b0 +
													   read_float3(v1 + instance.m_BitangentOffset) * b.x +
													   read_float3(v2 + instance.m_BitangentOffset) * b.y) * flip;
	}

	float2 uv = make_float2(0.0f, 0.0f);
	float lod_base = -64.0f;
	if (input.m_Material != c_optxNoMaterial && params.m_Materials)
	{
		const optxMaterial& material = reinterpret_cast<const optxMaterial*>(params.m_Materials)[input.m_Material];
		float2 uv0, uv1, uv2;
		optxm::triangle_uvs(instance, idx, uv0, uv1, uv2);
		uv0 = optxm::transform_uv(material, uv0);
		uv1 = optxm::transform_uv(material, uv1);
		uv2 = optxm::transform_uv(material, uv2);
		uv = optxm::lerp2(uv0, uv1, uv2, b);
		const float uv_area = fabsf((uv1.x - uv0.x) * (uv2.y - uv0.y) - (uv2.x - uv0.x) * (uv1.y - uv0.y));
		if (uv_area > 0.0f && world_area > 0.0f)
		{
			lod_base = 0.5f * __log2f(uv_area / world_area);
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
	optixSetPayload_9(input.m_Material);
	optixSetPayload_10(__float_as_uint(uv.x));
	optixSetPayload_11(__float_as_uint(uv.y));
	optixSetPayload_12(__float_as_uint(s.x));
	optixSetPayload_13(__float_as_uint(s.y));
	optixSetPayload_14(__float_as_uint(s.z));
	optixSetPayload_15(__float_as_uint(t.x));
	optixSetPayload_16(__float_as_uint(t.y));
	optixSetPayload_17(__float_as_uint(t.z));
	optixSetPayload_18(__float_as_uint(lod_base));
}

//	Cut-out and transparent materials let rays through (optxMaterial.h)
extern "C" __global__ void __anyhit__opacity()
{
	if (!optxm::opacity_passes(params.m_Instances, params.m_Materials, params.m_Textures, params.m_Seed))
	{
		optixIgnoreIntersection();
	}
}
