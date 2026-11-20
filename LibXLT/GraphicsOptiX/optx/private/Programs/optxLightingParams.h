/*****************************************************************************
**  optxLightingParams.h
**
**      The launch parameters of the lighting programs (optxLighting.cu),
**	shared by the device programs and the host code that launches them
**	(optxLighting). The scene is white diffuse surfaces lit by the light
**	table's lights (optxLightTypes.h). Two launches, each m_Width x
**	m_Height, laid out as the AO programs' (optxAOParams.h): the
**	accumulation buffer holds rgb sums and, from sample 0, the pixel's
**	clip-space depth (1 for a miss); __raygen__lighting adds m_Samples
**	samples (resolving too with m_Resolve), __raygen__resolve resolves
**	m_FirstSample samples.
**
**	Modes (m_Mode):
**	- e_optxLightingIllumination: the light arriving at the first hit, as
**	  the raster Illumination Only pass draws it (light colour times
**	  attenuation, penumbra and the cosine, summed over the lights that
**	  reach the surface), with traced shadows; then up to m_Bounces
**	  cosine-weighted bounces off surfaces of albedo m_Albedo, each adding
**	  the direct light where it lands (Russian roulette after the third).
**	  Bounce rays that reach an area light add its light, weighted against
**	  the light samples by the power heuristic (m_Sampling picks light
**	  samples only, bounce rays only, or both). Misses add m_Background.
**	  Without m_bUseCosine (the raster's "Illumination Uses Normals"
**	  preference off) direct light has no cosine and area lights are
**	  sampled only through light samples.
**	- e_optxLightingShadowMask: the raster Shadow Mask pass: the product,
**	  over the projected lights that reach the surface and face it, of 1 -
**	  (1 - visibility) * cosine; no bounces. Misses add m_Background
**	  (white).
**	The resolve writes the mean, clamped to [0, 1], straight into the 8-bit
**	colour buffer (no sRGB encoding: the raster passes copy their linear
**	buffer the same way).
**
**	Shadow rays (occlusion mask, terminate on first hit) are traced for
**	lights with e_optxLightShadows, from points of instances with
**	e_optxInstanceReceivesShadow, when m_bShadows (the preference to
**	enable shadows) is set.
**
**	Plain C structs: the host and nvcc must lay them out the same way.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXLIGHTINGPARAMS_H
#error optxLightingParams.h multiply included
#endif
#define GOX_OPTXLIGHTINGPARAMS_H

#include <optix_types.h>

namespace gox
{
	enum optxLightingMode
	{
		e_optxLightingIllumination = 0,
		e_optxLightingShadowMask = 1
	};

	//	How area lights are sampled (for tests of the weighting)
	enum optxLightSampling
	{
		e_optxSampleBoth = 0,		// light samples and bounce rays, by the power heuristic
		e_optxSampleLights = 1,		// light samples only
		e_optxSampleBounces = 2		// bounce rays only (delta lights still by light samples)
	};

	struct optxLightingParams
	{
		OptixTraversableHandle		m_Handle;		// the scene's IAS; 0: every ray misses
		unsigned long long			m_Instances;	// device address of the optxInstanceData table
		unsigned long long			m_Lights;		// device address of m_NumLights optxLights
		unsigned long long			m_LightSets;	// device address of the optxLightSets
		unsigned long long			m_LightIndices;	// device address of the sets' light indices
		unsigned long long			m_Accum;		// device address of 4 floats per pixel
		unsigned long long			m_Color;		// device address of a 32-bit texel per pixel
		unsigned long long			m_Depth;		// device address of a 32-bit texel per pixel
		unsigned int				m_Width;
		unsigned int				m_Height;
		float						m_InvViewProj[16];
		float						m_ViewProj[16];
		float						m_Background[3];	// linear
		float						m_Epsilon;			// secondary rays start this far off the surface
		float						m_Albedo;			// of the bounce surfaces
		unsigned int				m_NumLights;
		unsigned int				m_Mode;				// optxLightingMode
		unsigned int				m_Bounces;
		unsigned int				m_Sampling;			// optxLightSampling
		unsigned int				m_bUseCosine;
		unsigned int				m_bShadows;
		unsigned int				m_FirstSample;		// samples accumulated before this launch
		unsigned int				m_Samples;			// samples this launch adds
		unsigned int				m_ColorOrder;		// optxColorOrder (optxAOParams.h)
		unsigned int				m_DepthLayout;		// optxDepthLayout (optxAOParams.h)
		unsigned int				m_Seed;
		unsigned int				m_Resolve;			// __raygen__lighting: also resolve (non-zero)
	};
}
