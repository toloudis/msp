/*****************************************************************************
**  optxLights.h
**
**      The light table's lights at a point, for the lighting programs
**	(optxLighting.cu): Lighting.hlsli's attenuation, penumbra and
**	projector textures (light_at), area light shapes (disks and cones of
**	directions), and the lights that reach an instance (its light set).
**	See optxLightTypes.h. Device only (nvcc); include in a program after
**	its launch parameters `params` (an optxLightingParams: m_Lights,
**	m_LightSets, m_LightIndices, m_NumLights, m_Textures) are declared,
**	after optix.h, optxVector.h, the scene, material and light types and
**	optxMaterial.h.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXLIGHTS_H
#error optxLights.h multiply included
#endif
#define GOX_OPTXLIGHTS_H

namespace gox
{
namespace optxl
{
	using namespace optxv;

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
	//	colour times attenuation and penumbra (and projector texture); and
	//	the way to it (towards its centre, unnormalized; for directional
	//	lights, against its direction) and its distance (c_Infinity for
	//	directional lights). False if the light doesn't reach the point.
	//	The light's diffuse and specular switches aren't applied (diffuse
	//	and specular below).
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
				if (i_Light.m_Texture != c_optxNoTexture && factor > 0.0f)
				{
					//	The projector image (or ramp) at the point, as the raster's projSampler
					const optxTexture& texture = reinterpret_cast<const optxTexture*>(params.m_Textures)[i_Light.m_Texture];
					const float4 tint = optxm::sample(texture, u, v, 0.0f);
					o_Color = mul3(o_Color, make_float3(tint.x, tint.y, tint.z));
				}
				return factor > 0.0f;
			}
		}
	}

	//	The light's diffuse and specular switches (optxLightFlags), as 0 or 1
	__forceinline__ __device__ float diffuse(const optxLight& i_Light)
	{
		return (i_Light.m_Flags & e_optxLightDiffuse) ? 1.0f : 0.0f;
	}

	__forceinline__ __device__ float specular(const optxLight& i_Light)
	{
		return (i_Light.m_Flags & e_optxLightSpecular) ? 1.0f : 0.0f;
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

	//	A point on an area light seen from i_P: the direction to it, its
	//	distance (c_Infinity for a cone) and the pdf of the direction
	//	(solid angle)
	__forceinline__ __device__ void area_sample(const optxLight& i_Light, float3 i_P, float u1, float u2, float3& o_Wi,
												float& o_Distance, float& o_Pdf)
	{
		if (i_Light.m_Type == e_optxLightProjected)
		{
			float3 normal;
			const float3 to_y = disk_point(i_Light, i_P, u1, u2, normal) - i_P;
			const float d2 = dot(to_y, to_y);
			o_Distance = sqrtf(d2);
			o_Wi = to_y * (1.0f / o_Distance);
			const float cos_l = fmaxf(1e-6f, -dot(o_Wi, normal));
			o_Pdf = d2 / (c_Pi * i_Light.m_Radius * i_Light.m_Radius * cos_l);
		}
		else
		{
			o_Wi = cone_direction(i_Light, u1, u2);
			o_Distance = c_Infinity;
			o_Pdf = 1.0f / cone_solid_angle(i_Light);
		}
	}

	//	Whether a ray from i_Origin along i_W reaches an area light (seen
	//	from i_P, the point the ray leaves) before i_THit, and the pdf a
	//	light sample from i_P would have given that direction (solid angle)
	__forceinline__ __device__ bool area_hit(const optxLight& i_Light, float3 i_P, float3 i_Origin, float3 i_W, float i_THit,
											 float& o_Pdf)
	{
		if (i_Light.m_Type == e_optxLightProjected)
		{
			const float3 centre = load3(i_Light.m_Position);
			const float3 normal = normalize(i_P - centre);
			const float facing = dot(i_W, normal);
			if (facing >= 0.0f)
			{
				return false;
			}
			const float t = dot(centre - i_Origin, normal) / facing;
			if (!(t > 0.0f) || t >= i_THit)
			{
				return false;
			}
			const float3 y = i_Origin + i_W * t;
			const float3 off = y - centre;
			if (dot(off, off) > i_Light.m_Radius * i_Light.m_Radius)
			{
				return false;
			}
			const float3 to_y = y - i_P;
			const float d2 = dot(to_y, to_y);
			o_Pdf = d2 / (c_Pi * i_Light.m_Radius * i_Light.m_Radius * (-facing));
			return true;
		}
		if (i_THit < c_Infinity || dot(i_W, load3(i_Light.m_Direction)) > -i_Light.m_CosCone)
		{
			return false;
		}
		o_Pdf = 1.0f / cone_solid_angle(i_Light);
		return true;
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
}
}
