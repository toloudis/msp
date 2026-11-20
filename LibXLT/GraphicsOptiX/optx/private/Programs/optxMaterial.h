/*****************************************************************************
**  optxMaterial.h
**
**      Device code shared by the programs that read materials (optxAO.cu,
**	optxLighting.cu): texture sampling, a hit's vertex attributes, the
**	ray-cone mip level, and the opacity test the any-hit programs run.
**	Device only (nvcc); include after optix.h, optxSceneTypes.h and
**	optxMaterialTypes.h.
**
**	Textures (optxMaterialTypes.h): a sample is read with the record's
**	swizzle, so it reads as Vulkan's sampler gives it.
**
**	Mip levels come from ray cones (Akenine-Moller et al., "Improved Shader
**	and Texture Level of Detail Using Ray Cones"): a hit's level is
**	0.5 log2(texel area / world area) of its triangle (with the material's
**	UV transform; HitLodBase), plus log2 of the cone's width at the hit
**	over the cosine between the ray and the surface, plus 0.5 log2 of the
**	texture's texel count.
**
**	Opacity (OpacityPasses): a material's opacity is its value times its
**	map (red, or alpha for a PBR base colour map) at the hit, at mip 0. A
**	cut-out (MASK) material lets rays through below its cutoff. A
**	transparent one lets each ray through with probability 1 - opacity,
**	from a hash of the ray and the hit: stochastic transparency, which the
**	progressive samples average (a different ray each sample).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXMATERIAL_H
#error optxMaterial.h multiply included
#endif
#define GOX_OPTXMATERIAL_H

namespace gox
{
namespace optxm
{
	__forceinline__ __device__ unsigned int hash(unsigned int v)
	{
		const unsigned int state = v * 747796405u + 2891336453u;
		const unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	//	A texture sample, swizzled (i_Level: mip level, fractional)
	__forceinline__ __device__ float4 sample(const optxTexture& i_Texture, float u, float v, float i_Level)
	{
		const cudaTextureObject_t object = i_Texture.m_Object;
		switch (i_Texture.m_Swizzle)
		{
			case e_optxSwizzleBGRA:
			{
				const float4 s = tex2DLod<float4>(object, u, v, i_Level);
				return make_float4(s.z, s.y, s.x, s.w);
			}
			case e_optxSwizzleR001:
				return make_float4(tex2DLod<float>(object, u, v, i_Level), 0.0f, 0.0f, 1.0f);
			case e_optxSwizzleRG01:
			{
				const float2 s = tex2DLod<float2>(object, u, v, i_Level);
				return make_float4(s.x, s.y, 0.0f, 1.0f);
			}
			case e_optxSwizzle000A:
				return make_float4(0.0f, 0.0f, 0.0f, tex2DLod<float>(object, u, v, i_Level));
			case e_optxSwizzleRGB1:
			{
				const float4 s = tex2DLod<float4>(object, u, v, i_Level);
				return make_float4(s.x, s.y, s.z, 1.0f);
			}
			default:
				return tex2DLod<float4>(object, u, v, i_Level);
		}
	}

	//	A texture of the table at a ray-cone level (i_LodBase: the hit's
	//	level without the texture's size)
	__forceinline__ __device__ float4 sample_lod(unsigned long long i_Textures, unsigned int i_Index, float2 i_UV, float i_LodBase)
	{
		const optxTexture& texture = reinterpret_cast<const optxTexture*>(i_Textures)[i_Index];
		const float level = fmaxf(0.0f, i_LodBase + 0.5f * __log2f(float(texture.m_Width) * float(texture.m_Height)));
		return sample(texture, i_UV.x, i_UV.y, level);
	}

	__forceinline__ __device__ float2 transform_uv(const optxMaterial& i_Material, float2 i_UV)
	{
		const float* t = i_Material.m_UV;
		return make_float2(t[0] * i_UV.x + t[1] * i_UV.y + t[2], t[3] * i_UV.x + t[4] * i_UV.y + t[5]);
	}

	//	A triangle's three vertex indices
	__forceinline__ __device__ uint3 triangle_indices(const optxGeometryInput& i_Input, unsigned int i_Triangle)
	{
		if (i_Input.m_IndexSize == 2)
		{
			const unsigned short* indices = reinterpret_cast<const unsigned short*>(i_Input.m_Indices) + 3 * i_Triangle;
			return make_uint3(indices[0], indices[1], indices[2]);
		}
		const unsigned int* indices = reinterpret_cast<const unsigned int*>(i_Input.m_Indices) + 3 * i_Triangle;
		return make_uint3(indices[0], indices[1], indices[2]);
	}

	__forceinline__ __device__ float2 read_float2(unsigned long long i_Address)
	{
		const float* p = reinterpret_cast<const float*>(i_Address);
		return make_float2(p[0], p[1]);
	}

	//	The texture coordinates of a triangle's vertices (untransformed;
	//	zeros for a format without them)
	__forceinline__ __device__ void triangle_uvs(const optxInstanceData& i_Instance, uint3 i_Indices, float2& o_0, float2& o_1,
												 float2& o_2)
	{
		if (i_Instance.m_TexCoordOffset == c_optxNoAttribute)
		{
			o_0 = o_1 = o_2 = make_float2(0.0f, 0.0f);
			return;
		}
		const unsigned long long base = i_Instance.m_Vertices + i_Instance.m_TexCoordOffset;
		o_0 = read_float2(base + (unsigned long long)i_Indices.x * i_Instance.m_VertexStride);
		o_1 = read_float2(base + (unsigned long long)i_Indices.y * i_Instance.m_VertexStride);
		o_2 = read_float2(base + (unsigned long long)i_Indices.z * i_Instance.m_VertexStride);
	}

	__forceinline__ __device__ float2 lerp2(float2 a, float2 b, float2 c, float2 i_Bary)
	{
		const float w = 1.0f - i_Bary.x - i_Bary.y;
		return make_float2(a.x * w + b.x * i_Bary.x + c.x * i_Bary.y, a.y * w + b.y * i_Bary.x + c.y * i_Bary.y);
	}

	//	Whether a ray keeps a hit on a material (any-hit): see above. False
	//	means the ray goes through.
	__forceinline__ __device__ bool opacity_passes(unsigned long long i_Instances, unsigned long long i_Materials,
												   unsigned long long i_Textures, unsigned int i_Seed)
	{
		const optxInstanceData& instance = reinterpret_cast<const optxInstanceData*>(i_Instances)[optixGetInstanceId()];
		const optxGeometryInput& input = reinterpret_cast<const optxGeometryInput*>(instance.m_Inputs)[optixGetSbtGASIndex()];
		if (input.m_Material == c_optxNoMaterial || !i_Materials)
		{
			return true;
		}
		const optxMaterial& material = reinterpret_cast<const optxMaterial*>(i_Materials)[input.m_Material];
		if (!(material.m_Flags & (e_optxMaterialCutOut | e_optxMaterialTransparent)))
		{
			return true;
		}
		float opacity = material.m_Opacity;
		if (material.m_OpacityMap != c_optxNoTexture)
		{
			const uint3 indices = triangle_indices(input, optixGetPrimitiveIndex());
			float2 uv0, uv1, uv2;
			triangle_uvs(instance, indices, uv0, uv1, uv2);
			const float2 uv = transform_uv(material, lerp2(uv0, uv1, uv2, optixGetTriangleBarycentrics()));
			const optxTexture& texture = reinterpret_cast<const optxTexture*>(i_Textures)[material.m_OpacityMap];
			const float4 s = sample(texture, uv.x, uv.y, 0.0f);
			opacity *= (material.m_Flags & e_optxMaterialOpacityAlpha) ? s.w : s.x;
		}
		if (material.m_Flags & e_optxMaterialCutOut)
		{
			return opacity >= material.m_Cutoff;
		}
		//	A value for this ray and hit: the ray's origin and direction
		//	differ from sample to sample
		const float3 o = optixGetWorldRayOrigin();
		const float3 d = optixGetWorldRayDirection();
		unsigned int h = hash(i_Seed ^ 0x9e3779b9u);
		h = hash(h ^ __float_as_uint(o.x));
		h = hash(h ^ __float_as_uint(o.y));
		h = hash(h ^ __float_as_uint(o.z));
		h = hash(h ^ __float_as_uint(d.x));
		h = hash(h ^ __float_as_uint(d.y));
		h = hash(h ^ __float_as_uint(d.z));
		h = hash(h ^ optixGetPrimitiveIndex() ^ (optixGetInstanceId() << 16));
		const float u = (h >> 8) * (1.0f / 16777216.0f);
		return u < opacity;
	}
}
}
