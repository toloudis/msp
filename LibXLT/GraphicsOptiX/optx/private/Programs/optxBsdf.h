/*****************************************************************************
**  optxBsdf.h
**
**      The one BRDF every traced surface uses: glTF metallic-roughness, with
**	KHR_materials_specular and KHR_materials_ior, in Pbr.hlsl's terms
**	(Vulkan's raster Pbr.fx):
**	- GGX distribution D, height-correlated Smith-GGX visibility V,
**	  Schlick Fresnel F from F0 to F90 at the half vector, Lambert diffuse
**	  on (1 - F) x baseColor x (1 - metallic);
**	- roughness clamped to [c_MinRoughness, 1] (Pbr.hlsl's clamp), alpha =
**	  roughness^2;
**	- F0 = mix(min(((ior - 1) / (ior + 1))^2 x specularColor, 1) x specular,
**	  baseColor, metallic), F90 = mix(specular, 1, metallic). Without the
**	  extensions (specular 1, specular colour white, IOR 1.5) that is the
**	  core model's F0 = mix(0.04, baseColor, metallic), F90 = 1.
**
**	Convention (Pbr.hlsl's PbrDirect): the BRDF is f = (1 - F) diffuse / pi
**	+ D V F, and a light of colour C delivers irradiance pi C: a light
**	sample adds pi C f cos. Evaluate returns the two lobes apart (diffuse
**	and specular), so the light's diffuse colour lights the one and its
**	specular colour the other, as in the raster.
**
**	Sampling: the specular lobe by GGX visible normals (Heitz 2018, in the
**	spherical-cap form of Dupuy and Benyoub 2023), the diffuse lobe
**	cosine-weighted; the lobe is chosen in proportion to an estimate of
**	its weight (the Fresnel at the view angle, against (1 - F) times the
**	diffuse colour, as luminance), and the pdf is the mixture of both
**	lobes, so multiple importance sampling sees the true pdf.
**
**	Directions are world-space unit vectors: wo towards the viewer, wi
**	towards the light; n the shading normal. A view below the shading
**	normal (normal maps bend it) is taken as grazing (n.wo at least
**	c_MinCosine), as Pbr.hlsl clamps NdotV.
**
**	Device only (nvcc); include after optix.h and optxVector.h.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXBSDF_H
#error optxBsdf.h multiply included
#endif
#define GOX_OPTXBSDF_H

namespace gox
{
namespace optxb
{
	using namespace optxv;

	//	Pbr.hlsl's c_MinRoughness
	constexpr float c_MinRoughness = 0.045f;
	//	Pbr.hlsl's NdotV floor
	constexpr float c_MinCosine = 1e-4f;

	enum Lobe
	{
		e_LobeNone = 0,
		e_LobeDiffuse = 1,
		e_LobeSpecular = 2
	};

	struct Bsdf
	{
		float3	m_Diffuse;		// baseColor x (1 - metallic)
		float3	m_F0;
		float3	m_F90;
		float	m_Roughness;	// perceptual, clamped
		float	m_Alpha;		// roughness^2
	};

	//	The BRDF of a material's values at a point
	__forceinline__ __device__ Bsdf make_bsdf(float3 i_BaseColor, float i_Metallic, float i_Roughness, float i_Specular,
											  float3 i_SpecularColor, float i_Ior)
	{
		Bsdf b;
		const float metallic = saturate(i_Metallic);
		const float specular = saturate(i_Specular);
		b.m_Roughness = fminf(fmaxf(i_Roughness, c_MinRoughness), 1.0f);
		b.m_Alpha = b.m_Roughness * b.m_Roughness;
		b.m_Diffuse = i_BaseColor * (1.0f - metallic);
		const float r = (i_Ior - 1.0f) / (i_Ior + 1.0f);
		const float3 dielectric = min3(i_SpecularColor * (r * r), 1.0f) * specular;
		b.m_F0 = lerp3(dielectric, i_BaseColor, metallic);
		b.m_F90 = lerp3(splat3(specular), splat3(1.0f), metallic);
		return b;
	}

	__forceinline__ __device__ float3 fresnel(const Bsdf& i_Bsdf, float i_Cos)
	{
		const float m = 1.0f - saturate(i_Cos);
		const float m2 = m * m;
		return i_Bsdf.m_F0 + (i_Bsdf.m_F90 - i_Bsdf.m_F0) * (m2 * m2 * m);
	}

	//	Pbr.hlsl's D_GGX
	__forceinline__ __device__ float d_ggx(float i_NdotH, float a)
	{
		const float a2 = a * a;
		const float d = i_NdotH * i_NdotH * (a2 - 1.0f) + 1.0f;
		return a2 / (c_Pi * d * d);
	}

	//	Pbr.hlsl's V_SmithGGX
	__forceinline__ __device__ float v_smith(float i_NdotL, float i_NdotV, float a)
	{
		const float a2 = a * a;
		const float gv = i_NdotL * sqrtf(i_NdotV * i_NdotV * (1.0f - a2) + a2);
		const float gl = i_NdotV * sqrtf(i_NdotL * i_NdotL * (1.0f - a2) + a2);
		return 0.5f / fmaxf(gv + gl, 1e-5f);
	}

	//	The view direction as the BRDF sees it: never below the shading
	//	normal (n.wo at least c_MinCosine)
	__forceinline__ __device__ float3 view_above(float3 i_N, float3 i_Wo)
	{
		const float c = dot(i_N, i_Wo);
		if (c >= c_MinCosine)
		{
			return i_Wo;
		}
		return normalize(i_Wo + i_N * (c_MinCosine - c));
	}

	//	The chance of choosing the specular lobe at a view
	__forceinline__ __device__ float specular_chance(const Bsdf& i_Bsdf, float i_NdotV)
	{
		const float3 f = fresnel(i_Bsdf, i_NdotV);
		const float s = luminance(f);
		const float d = luminance(mul3(splat3(1.0f) - f, i_Bsdf.m_Diffuse));
		if (s + d <= 0.0f)
		{
			return 0.0f;
		}
		return s / (s + d);
	}

	//	The specular lobe's sampling pdf (solid angle) of wi: the
	//	distribution of visible normals reflected, G1(wo) D(h) / (4 n.wo)
	__forceinline__ __device__ float specular_pdf(float i_NdotV, float i_NdotH, float a)
	{
		const float a2 = a * a;
		const float root = sqrtf(a2 + (1.0f - a2) * i_NdotV * i_NdotV);
		return d_ggx(i_NdotH, a) / (2.0f * (i_NdotV + root));
	}

	//	The BRDF (both lobes, no cosine) and the sampling pdf of wi
	struct Value
	{
		float3	m_Diffuse;		// (1 - F) diffuse / pi
		float3	m_Specular;		// D V F
		float	m_Pdf;			// the mixture's, solid angle
	};

	__forceinline__ __device__ Value evaluate(const Bsdf& i_Bsdf, float3 i_N, float3 i_Wo, float3 i_Wi)
	{
		Value v;
		v.m_Diffuse = v.m_Specular = splat3(0.0f);
		v.m_Pdf = 0.0f;
		const float NdotL = dot(i_N, i_Wi);
		if (NdotL <= 0.0f)
		{
			return v;
		}
		const float3 wo = view_above(i_N, i_Wo);
		//	Pbr.hlsl's PbrLightTerms: L + V is zero when the light is exactly
		//	behind the surface, seen from the front: NdotH and VdotH are 0 then
		const float3 h = i_Wi + wo;
		const float3 H = h * rsqrtf(fmaxf(dot(h, h), 1e-8f));
		const float NdotV = fmaxf(dot(i_N, wo), c_MinCosine);
		const float NdotH = saturate(dot(i_N, H));
		const float VdotH = saturate(dot(wo, H));
		const float a = i_Bsdf.m_Alpha;
		const float3 F = fresnel(i_Bsdf, VdotH);
		v.m_Specular = F * (d_ggx(NdotH, a) * v_smith(saturate(NdotL), NdotV, a));
		v.m_Diffuse = mul3(splat3(1.0f) - F, i_Bsdf.m_Diffuse) * (1.0f / c_Pi);
		const float chance = specular_chance(i_Bsdf, NdotV);
		v.m_Pdf = chance * specular_pdf(NdotV, NdotH, a) + (1.0f - chance) * (NdotL / c_Pi);
		return v;
	}

	//	A visible normal of the GGX distribution about z, for the view wo
	//	(local, wo.z > 0): Dupuy and Benyoub, "Sampling Visible GGX Normals
	//	with Spherical Caps" (2023)
	__forceinline__ __device__ float3 sample_visible_normal(float3 i_Wo, float a, float u1, float u2)
	{
		const float3 wo = normalize(make_float3(i_Wo.x * a, i_Wo.y * a, i_Wo.z));
		const float phi = 2.0f * c_Pi * u1;
		const float z = (1.0f - u2) * (1.0f + wo.z) - wo.z;
		const float sin_theta = sqrtf(saturate(1.0f - z * z));
		const float3 c = make_float3(sin_theta * __cosf(phi), sin_theta * __sinf(phi), z);
		const float3 h = c + wo;
		return normalize(make_float3(h.x * a, h.y * a, fmaxf(h.z, 0.0f)));
	}

	//	A sampled direction
	struct Sample
	{
		float3			m_Wi;
		float3			m_Weight;		// f cos / pdf, with the light white in both lobes
		Value			m_Value;		// f's lobes and the mixture pdf at m_Wi
		unsigned int	m_Lobe;			// Lobe; e_LobeNone: no direction (the path ends)
	};

	//	A direction from the BRDF (u0 picks the lobe, u1 and u2 the
	//	direction). A direction below the shading normal gives no sample;
	//	the caller ends the path at one below the geometric surface.
	__forceinline__ __device__ Sample sample(const Bsdf& i_Bsdf, float3 i_N, float3 i_Wo, float u0, float u1, float u2)
	{
		Sample s;
		s.m_Lobe = e_LobeNone;
		s.m_Weight = splat3(0.0f);
		const float3 wo = view_above(i_N, i_Wo);
		const float NdotV = fmaxf(dot(i_N, wo), c_MinCosine);
		const float chance = specular_chance(i_Bsdf, NdotV);
		if (u0 < chance)
		{
			float3 t, b;
			basis(i_N, t, b);
			const float3 local = make_float3(dot(wo, t), dot(wo, b), NdotV);
			const float3 m = sample_visible_normal(local, i_Bsdf.m_Alpha, u1, u2);
			const float3 h = t * m.x + b * m.y + i_N * m.z;
			s.m_Wi = h * (2.0f * dot(wo, h)) - wo;
			s.m_Lobe = e_LobeSpecular;
		}
		else
		{
			s.m_Wi = cosine_direction(i_N, u1, u2);
			s.m_Lobe = e_LobeDiffuse;
		}
		s.m_Value = evaluate(i_Bsdf, i_N, i_Wo, s.m_Wi);
		const float cosine = dot(i_N, s.m_Wi);
		if (cosine <= 0.0f || !(s.m_Value.m_Pdf > 0.0f))
		{
			s.m_Lobe = e_LobeNone;
			return s;
		}
		s.m_Weight = (s.m_Value.m_Diffuse + s.m_Value.m_Specular) * (cosine / s.m_Value.m_Pdf);
		return s;
	}
}
}
