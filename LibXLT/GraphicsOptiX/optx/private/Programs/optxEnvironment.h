/*****************************************************************************
**  optxEnvironment.h
**
**      The beauty pass's environment light (optxLightingParams.h's
**	optxEnvironmentLight) for the lighting programs (optxLighting.cu): its
**	radiance in a direction, read from its lat-long map as the raster
**	reads one (Support.hlsli's rotateAboutY and CartesianToPolar), its
**	mean radiance (how often the light sample chooses it), and its
**	sampling: from its table (a piecewise-constant distribution over the
**	map's u and v, in proportion to luminance x sin theta per cell) or,
**	without one, cosine-weighted about the shading normal; the pdf of a
**	direction in solid angle, for multiple importance sampling.
**
**	Device only (nvcc); include in a program after its launch parameters
**	`params` (an optxLightingParams) are declared, after optxVector.h,
**	optxMaterial.h and optxSurface.h.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXENVIRONMENT_H
#error optxEnvironment.h multiply included
#endif
#define GOX_OPTXENVIRONMENT_H

namespace gox
{
namespace optxe
{
	using namespace optxv;

	__forceinline__ __device__ const optxEnvironmentLight& env()
	{
		return params.m_Environment;
	}

	__forceinline__ __device__ bool on()
	{
		return (env().m_Flags & e_optxEnvironmentOn) != 0;
	}

	//	Whether it lights surfaces (either lobe)
	__forceinline__ __device__ bool lights()
	{
		return on() && (env().m_Flags & (e_optxEnvironmentDiffuse | e_optxEnvironmentSpecular)) != 0;
	}

	__forceinline__ __device__ bool background()
	{
		return on() && (env().m_Flags & e_optxEnvironmentBackground) != 0;
	}

	__forceinline__ __device__ float diffuse()
	{
		return (env().m_Flags & e_optxEnvironmentDiffuse) ? 1.0f : 0.0f;
	}

	__forceinline__ __device__ float specular()
	{
		return (env().m_Flags & e_optxEnvironmentSpecular) ? 1.0f : 0.0f;
	}

	//	Support.hlsli's rotateAboutY
	__forceinline__ __device__ float3 rotate_y(float3 v, float i_Angle)
	{
		if (i_Angle == 0.0f)
		{
			return v;
		}
		float s, c;
		sincosf(i_Angle, &s, &c);
		return make_float3(v.z * s + v.x * c, v.y, v.z * c - v.x * s);
	}

	//	Support.hlsli's CartesianToPolar: the map coordinates the raster
	//	reads a direction at (with its 0.001 in the denominator)
	__forceinline__ __device__ float2 polar(float3 v)
	{
		const float3 n = normalize(v);
		return make_float2((atan2f(n.z, n.x + 0.001f) + c_Pi) * (0.5f / c_Pi), acosf(fminf(fmaxf(n.y, -1.0f), 1.0f)) / c_Pi);
	}

	__forceinline__ __device__ const optxTexture* texture()
	{
		if (env().m_Texture == c_optxNoTexture || !params.m_Textures)
		{
			return NULL;
		}
		return reinterpret_cast<const optxTexture*>(params.m_Textures) + env().m_Texture;
	}

	//	The map's level for a cone of a spread angle (a texel spans
	//	pi / height radians of latitude)
	__forceinline__ __device__ float level_for(float i_Spread)
	{
		const optxTexture* map = texture();
		if (!map || !(i_Spread > 0.0f))
		{
			return 0.0f;
		}
		return fmaxf(0.0f, __log2f(i_Spread * float(map->m_Height) / c_Pi));
	}

	//	The level rays leaving a surface read the map at (light samples and
	//	BRDF rays alike, so both strategies see the same radiance): the
	//	cone's, but never blurrier than the table's cells
	__forceinline__ __device__ float surface_level(float i_Spread)
	{
		const float level = level_for(i_Spread);
		return env().m_Table ? fminf(level, env().m_TableLevel) : level;
	}

	//	The radiance arriving from direction d (world, unit)
	__forceinline__ __device__ float3 radiance(float3 d, float i_Level)
	{
		const float3 color = load3(env().m_Color);
		const optxTexture* map = texture();
		if (!map)
		{
			return color;
		}
		const float2 uv = polar(rotate_y(d, env().m_Angle));
		const float4 s = optxm::sample(*map, uv.x, uv.y, i_Level);
		return mul3(color, make_float3(s.x, s.y, s.z));
	}

	//------------------------------------------------------------------------
	//	The sampling table
	//------------------------------------------------------------------------

	__forceinline__ __device__ const float* table()
	{
		return reinterpret_cast<const float*>(env().m_Table);
	}

	__forceinline__ __device__ float total()
	{
		return table()[optxEnvTotalAt(env().m_TableWidth, env().m_TableHeight)];
	}

	//	The table's coordinates of a direction in the map's frame (atan2
	//	exactly, the inverse of direction_of)
	__forceinline__ __device__ float2 table_uv(float3 m)
	{
		const float u = (atan2f(m.z, m.x) + c_Pi) * (0.5f / c_Pi);
		const float v = acosf(fminf(fmaxf(m.y, -1.0f), 1.0f)) / c_Pi;
		return make_float2(fminf(fmaxf(u, 0.0f), 0.99999994f), fminf(fmaxf(v, 0.0f), 0.99999994f));
	}

	//	The direction (map's frame) of table coordinates
	__forceinline__ __device__ float3 direction_of(float u, float v)
	{
		float st, ct, sp, cp;
		sincosf(v * c_Pi, &st, &ct);
		sincosf(u * 2.0f * c_Pi - c_Pi, &sp, &cp);
		return make_float3(st * cp, ct, st * sp);
	}

	//	The solid-angle density of a cell's value f at sin theta
	__forceinline__ __device__ float cell_pdf(float f, float i_SinTheta)
	{
		const unsigned int W = env().m_TableWidth, H = env().m_TableHeight;
		const float t = total();
		if (!(t > 0.0f) || !(i_SinTheta > 1e-7f))
		{
			return 0.0f;
		}
		return f * float(W) * float(H) / (t * 2.0f * c_Pi * c_Pi * i_SinTheta);
	}

	//	The first k in [0, n) with cdf[k] <= u < cdf[k + 1] (cdf has n + 1
	//	entries from 0 to 1, u in [0, 1))
	__forceinline__ __device__ unsigned int find_interval(const float* cdf, unsigned int n, float u)
	{
		unsigned int lo = 0, hi = n;	// cdf[lo] <= u, and the answer is below hi
		while (hi - lo > 1)
		{
			const unsigned int mid = (lo + hi) / 2;
			if (cdf[mid] <= u)
			{
				lo = mid;
			}
			else
			{
				hi = mid;
			}
		}
		return lo;
	}

	//------------------------------------------------------------------------
	//	Sampling
	//------------------------------------------------------------------------

	//	The solid-angle pdf of the light sample choosing direction d (world,
	//	unit) at a surface, once the environment is chosen
	__forceinline__ __device__ float pdf(float3 d, const optxs::Surface& i_Surface)
	{
		if (!env().m_Table)
		{
			return fmaxf(dot(d, i_Surface.m_Shading), 0.0f) / c_Pi;
		}
		const float3 m = rotate_y(d, env().m_Angle);
		const float2 uv = table_uv(m);
		const unsigned int W = env().m_TableWidth, H = env().m_TableHeight;
		const unsigned int i = min(unsigned(uv.x * W), W - 1), j = min(unsigned(uv.y * H), H - 1);
		return cell_pdf(table()[j * W + i], sqrtf(fmaxf(0.0f, 1.0f - m.y * m.y)));
	}

	//	A light sample's direction and its pdf; false if there is none
	__forceinline__ __device__ bool sample(const optxs::Surface& i_Surface, float u1, float u2, float3& o_Wi, float& o_Pdf)
	{
		if (!env().m_Table)
		{
			o_Wi = cosine_direction(i_Surface.m_Shading, u1, u2);
			o_Pdf = fmaxf(dot(o_Wi, i_Surface.m_Shading), 0.0f) / c_Pi;
			return o_Pdf > 0.0f;
		}
		if (!(total() > 0.0f))
		{
			return false;
		}
		const unsigned int W = env().m_TableWidth, H = env().m_TableHeight;
		const float* marginal = table() + optxEnvMarginalAt(W, H);
		const unsigned int j = find_interval(marginal, H, u1);
		const float dv = (u1 - marginal[j]) / fmaxf(marginal[j + 1] - marginal[j], 1e-30f);
		const float* row = table() + optxEnvRowCdfAt(W, H) + j * (W + 1);
		const unsigned int i = find_interval(row, W, u2);
		const float du = (u2 - row[i]) / fmaxf(row[i + 1] - row[i], 1e-30f);
		const float u = (float(i) + fminf(du, 0.99999994f)) / float(W);
		const float v = (float(j) + fminf(dv, 0.99999994f)) / float(H);
		const float3 m = direction_of(u, v);
		o_Wi = rotate_y(m, -env().m_Angle);
		o_Pdf = cell_pdf(table()[j * W + i], sqrtf(fmaxf(0.0f, 1.0f - m.y * m.y)));
		return o_Pdf > 0.0f;
	}

	//	How strongly it lights surfaces, for choosing it against the lights
	//	(their light at a point, which delivers pi times it as irradiance):
	//	its mean radiance's luminance, 0 when it lights nothing
	__forceinline__ __device__ float importance()
	{
		if (!lights())
		{
			return 0.0f;
		}
		if (!env().m_Table)
		{
			return luminance(load3(env().m_Color));
		}
		//	the integral over the sphere is total x (2 pi / W)(pi / H); the
		//	mean is that over 4 pi
		return total() * c_Pi / (2.0f * float(env().m_TableWidth) * float(env().m_TableHeight));
	}

	//	What it gives through the BRDF from direction wi: L (f_diffuse D +
	//	f_specular S) cos, D and S its switches
	__forceinline__ __device__ float3 lit(float3 i_Radiance, const optxb::Value& i_F, float i_Cosine)
	{
		return mul3(i_Radiance, (i_F.m_Diffuse * diffuse() + i_F.m_Specular * specular()) * i_Cosine);
	}
}
}
