/*****************************************************************************
**  optxSurface.h
**
**      A shading point for the lighting programs (optxLighting.cu): a
**	camera-ray hit (optxRays.h's Hit) made into a Surface with its
**	material read (optxMaterialTypes.h; textures at ray-cone levels,
**	optxMaterial.h), the normal map applied (never past the horizon), its
**	light set, and in full (beauty) its BRDF (optxBsdf.h) and emission;
**	and the tracer debug view's quantities. Device only (nvcc); include in
**	a program after its launch parameters `params` (an optxLightingParams:
**	m_Instances, m_Materials, m_Textures, m_Epsilon, m_bAlbedoOverride,
**	m_Albedo, m_DebugView) are declared, after optxVector.h, optxBsdf.h,
**	optxRays.h and optxLights.h.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXSURFACE_H
#error optxSurface.h multiply included
#endif
#define GOX_OPTXSURFACE_H

namespace gox
{
namespace optxs
{
	using namespace optxv;
	using optxr::Hit;

	//	A point being shaded
	struct Surface
	{
		float3			m_P;
		float3			m_Geometric;	// facing the incoming ray
		float3			m_Shading;		// normal-mapped, never past the horizon
		float3			m_Origin;		// off the surface, for secondary rays
		float3			m_Albedo;		// what diffuse bounces leave with (Illumination)
		float			m_LodBase;		// the hit's ray-cone level, without a texture's size
		bool			m_bReceivesShadow;
		unsigned int	m_First;
		unsigned int	m_Count;
		bool			m_bAll;
		//	Full surfaces only (make_surface's i_bFull)
		optxb::Bsdf		m_Bsdf;
		float3			m_BaseColor;	// base colour x map (the denoiser's albedo guide)
		float3			m_Emission;
	};

	__forceinline__ __device__ const optxMaterial* material_of(const Hit& i_Hit)
	{
		if (i_Hit.m_Material == c_optxNoMaterial || !params.m_Materials)
		{
			return NULL;
		}
		return reinterpret_cast<const optxMaterial*>(params.m_Materials) + i_Hit.m_Material;
	}

	__forceinline__ __device__ float4 material_sample(unsigned int i_Map, const Surface& i_Surface, const Hit& i_Hit)
	{
		return optxm::sample_lod(params.m_Textures, i_Map, i_Hit.m_UV, i_Surface.m_LodBase);
	}

	__forceinline__ __device__ float3 base_color(const optxMaterial& i_Material, const Surface& i_Surface, const Hit& i_Hit)
	{
		float3 base = load3(i_Material.m_BaseColor);
		if (i_Material.m_BaseColorMap != c_optxNoTexture)
		{
			const float4 s = material_sample(i_Material.m_BaseColorMap, i_Surface, i_Hit);
			base = mul3(base, make_float3(s.x, s.y, s.z));
		}
		return base;
	}

	__forceinline__ __device__ float metallic_of(const optxMaterial& i_Material, const Surface& i_Surface, const Hit& i_Hit)
	{
		float metallic = i_Material.m_Metallic;
		if (i_Material.m_MetallicRoughnessMap != c_optxNoTexture)
		{
			metallic *= material_sample(i_Material.m_MetallicRoughnessMap, i_Surface, i_Hit).z;
		}
		return metallic;
	}

	__forceinline__ __device__ float roughness_of(const optxMaterial& i_Material, const Surface& i_Surface, const Hit& i_Hit)
	{
		float roughness = i_Material.m_Roughness;
		if (i_Material.m_GlossMap != c_optxNoTexture && (i_Material.m_Flags & (e_optxMaterialGlossBlinn | e_optxMaterialGlossPhong)))
		{
			//	effTraceMaterial::RoughnessForGloss
			const float gloss = material_sample(i_Material.m_GlossMap, i_Surface, i_Hit).x;
			if (i_Material.m_Flags & e_optxMaterialGlossBlinn)
			{
				roughness = 1.0f - saturate(i_Material.m_Shininess * gloss);
			}
			else
			{
				const float alpha = sqrtf(2.0f / (fmaxf(i_Material.m_Shininess * gloss, 0.0f) + 2.0f));
				roughness = saturate(sqrtf(alpha));
			}
			roughness = fminf(roughness, i_Material.m_ReflectionRoughness);
		}
		if (i_Material.m_MetallicRoughnessMap != c_optxNoTexture)
		{
			roughness *= material_sample(i_Material.m_MetallicRoughnessMap, i_Surface, i_Hit).y;
		}
		return roughness;
	}

	__forceinline__ __device__ float3 specular_color_of(const optxMaterial& i_Material, const Surface& i_Surface, const Hit& i_Hit)
	{
		float3 color = load3(i_Material.m_SpecularColor);
		if (i_Material.m_SpecularColorMap != c_optxNoTexture)
		{
			const float4 s = material_sample(i_Material.m_SpecularColorMap, i_Surface, i_Hit);
			color = mul3(color, make_float3(s.x, s.y, s.z));
		}
		return color;
	}

	__forceinline__ __device__ float3 emission_of(const optxMaterial& i_Material, const Surface& i_Surface, const Hit& i_Hit)
	{
		float3 e = load3(i_Material.m_Emission);
		if (i_Material.m_EmissionMap != c_optxNoTexture && max3(e) > 0.0f)
		{
			const float4 s = material_sample(i_Material.m_EmissionMap, i_Surface, i_Hit);
			e = mul3(e, make_float3(s.x, s.y, s.z));
		}
		return e;
	}

	//	A hit made a shading point: lights, the normal map (as the raster's
	//	GetBumpNormal: (map * 2 - 1) scaled by (s, s, 1) in the S, T, N
	//	frame), bent back above the horizon, and the albedo diffuse bounces
	//	take; with i_bFull, the BRDF and emission too (a surface without a
	//	material is white and rough; with m_bAlbedoOverride, a Lambert
	//	surface of that albedo).
	//	i_Width: the ray cone's width at the hit.
	__forceinline__ __device__ Surface make_surface(float3 i_P, const Hit& i_Hit, float3 i_Direction, float i_Width, bool i_bFull)
	{
		Surface s;
		s.m_P = i_P;
		s.m_Geometric = i_Hit.m_Geometric;
		s.m_Shading = i_Hit.m_Shading;
		s.m_Origin = i_P + i_Hit.m_Geometric * params.m_Epsilon;
		const float cosine = fmaxf(fabsf(dot(i_Direction, i_Hit.m_Geometric)), 1e-4f);
		s.m_LodBase = i_Hit.m_LodBase + __log2f(fmaxf(i_Width, 1e-30f) / cosine);
		const optxInstanceData& instance = reinterpret_cast<const optxInstanceData*>(params.m_Instances)[i_Hit.m_Instance];
		s.m_bReceivesShadow = (instance.m_Flags & e_optxInstanceReceivesShadow) != 0;
		optxl::light_range(instance, s.m_First, s.m_Count, s.m_bAll);

		const optxMaterial* material = material_of(i_Hit);
		s.m_Albedo = splat3(1.0f);
		s.m_BaseColor = splat3(1.0f);
		s.m_Emission = splat3(0.0f);
		float metallic = 0.0f, roughness = 1.0f, specular = 1.0f, ior = 1.5f;
		float3 specular_color = splat3(1.0f);
		if (material)
		{
			if (material->m_NormalMap != c_optxNoTexture && material->m_NormalScale != 0.0f)
			{
				const float4 m = material_sample(material->m_NormalMap, s, i_Hit);
				const float nx = (m.x * 2.0f - 1.0f) * material->m_NormalScale;
				const float ny = (m.y * 2.0f - 1.0f) * material->m_NormalScale;
				const float nz = m.z * 2.0f - 1.0f;
				const float3 n = i_Hit.m_S * nx + i_Hit.m_T3 * ny + i_Hit.m_Shading * nz;
				const float n2 = dot(n, n);
				if (n2 > 1e-20f)
				{
					s.m_Shading = n * rsqrtf(n2);
				}
			}
			s.m_BaseColor = base_color(*material, s, i_Hit);
			metallic = saturate(metallic_of(*material, s, i_Hit));
			s.m_Albedo = s.m_BaseColor * (1.0f - metallic);
			if (i_bFull)
			{
				roughness = roughness_of(*material, s, i_Hit);
				specular = material->m_Specular;
				specular_color = specular_color_of(*material, s, i_Hit);
				ior = material->m_Ior;
				s.m_Emission = emission_of(*material, s, i_Hit);
			}
		}
		//	Never past the horizon: a normal map can't light a surface's back
		const float above = dot(s.m_Shading, s.m_Geometric);
		if (above < 1e-3f)
		{
			s.m_Shading = normalize(s.m_Shading + s.m_Geometric * (1e-3f - above));
		}
		if (params.m_bAlbedoOverride)
		{
			s.m_Albedo = s.m_BaseColor = splat3(params.m_Albedo);
			metallic = 0.0f;
			specular = 0.0f;
		}
		if (i_bFull)
		{
			s.m_Bsdf = optxb::make_bsdf(s.m_BaseColor, metallic, roughness, specular, specular_color, ior);
		}
		return s;
	}

	//	The debug view's quantity at the first hit (optxDebugView)
	__forceinline__ __device__ float3 debug_value(const Hit& i_Hit, const Surface& i_Surface)
	{
		const optxMaterial* material = material_of(i_Hit);
		switch (params.m_DebugView)
		{
			case e_optxDebugNormal:
				return i_Surface.m_Shading * 0.5f + splat3(0.5f);
			case e_optxDebugBaseColor:
				return material ? base_color(*material, i_Surface, i_Hit) : splat3(1.0f);
			case e_optxDebugRoughness:
				return splat3(material ? roughness_of(*material, i_Surface, i_Hit) : 1.0f);
			case e_optxDebugMetallic:
				return splat3(material ? metallic_of(*material, i_Surface, i_Hit) : 0.0f);
			case e_optxDebugEmission:
				return material ? emission_of(*material, i_Surface, i_Hit) : splat3(0.0f);
			case e_optxDebugOpacity:
			{
				float o = 1.0f;
				if (material && (material->m_Flags & (e_optxMaterialCutOut | e_optxMaterialTransparent)))
				{
					o = material->m_Opacity;
					if (material->m_OpacityMap != c_optxNoTexture)
					{
						const float4 s = material_sample(material->m_OpacityMap, i_Surface, i_Hit);
						o *= (material->m_Flags & e_optxMaterialOpacityAlpha) ? s.w : s.x;
					}
				}
				return splat3(o);
			}
			default:
				return splat3(0.0f);
		}
	}
}
}
