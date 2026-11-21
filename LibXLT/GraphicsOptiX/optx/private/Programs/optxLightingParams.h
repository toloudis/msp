/*****************************************************************************
**  optxLightingParams.h
**
**      The launch parameters of the lighting programs (optxLighting.cu),
**	shared by the device programs and the host code that launches them
**	(optxLighting). Surfaces are lit by the light table's lights
**	(optxLightTypes.h). Two launches, each m_Width x m_Height, laid out as
**	the AO programs' (optxAOParams.h): the accumulation buffer holds rgb
**	sums and, from sample 0, the pixel's clip-space depth (1 for a miss);
**	__raygen__lighting adds m_Samples samples (resolving too with
**	m_Resolve), __raygen__resolve resolves m_FirstSample samples.
**
**	Modes (m_Mode):
**	- e_optxLightingBeauty: the path-traced beauty pass. Every surface
**	  reflects light through one BRDF (optxBsdf.h: glTF metallic-
**	  roughness with KHR specular and IOR) and glows with its emission.
**	  A unidirectional path tracer with next-event estimation: at each hit,
**	  one light (chosen in proportion to its unshadowed light at the point
**	  times the cosine to its centre, as luminance; or with
**	  e_optxSelectAll, every light) is sampled with a shadow ray and adds
**	  pi C f cos over its chance of being chosen (the light's colour C
**	  through the BRDF's diffuse and specular lobes as the light's switches
**	  say); then the BRDF gives the next ray (throughput times f cos / pdf),
**	  up to m_Bounces bounces after the first hit (Russian roulette from
**	  the third). Area lights are reached both ways and weighted by the
**	  power heuristic over the BRDF's pdf and the light's pdf times its
**	  chance (m_Sampling as in Illumination); point lights only by light
**	  samples; emissive surfaces only by BRDF rays (and camera rays).
**	  Contributions after the first bounce are scaled down so no component
**	  exceeds m_MaxRadiance (0: no limit); direct light at the first hit is
**	  never limited. Camera misses add m_Background; other misses nothing.
**	  The accumulation buffer has three planes of m_Width x m_Height float4
**	  (each plane after the last): rgb sums and depth; first-hit base
**	  colour sums and coverage (the samples that hit); first-hit shading
**	  normal sums and sample 0's view-space depth (m_ViewZ; c_Infinity for
**	  a miss). The resolve writes the linear mean (unclamped) and the
**	  coverage as RGBA16F into m_Color, and, when m_Aux is set, the mean
**	  with the depth of field blurriness in alpha (the DOF prep pass's
**	  ComputeDepthBlur of the view depth with m_Dof and m_DofCutoff; 0.5,
**	  no blur, for misses) into m_Aux.
**	- e_optxLightingIllumination: the light arriving at the first hit, as
**	  the raster Illumination Only pass draws it (light colour times
**	  attenuation, penumbra and the cosine, summed over the lights that
**	  reach the surface and light diffuse surfaces), with traced shadows;
**	  then up to m_Bounces cosine-weighted bounces, each leaving its
**	  surface with the surface's diffuse albedo (base colour x (1 -
**	  metallic); or m_Albedo with m_bAlbedoOverride, for tests) and adding
**	  the direct light where it lands (Russian roulette after the third).
**	  Direct light isn't tinted by the first surface: the pass shows the
**	  light arriving. Projected lights' textures colour their light
**	  (optxLightTypes.h).
**	  Bounce rays that reach an area light add its light, weighted against
**	  the light samples by the power heuristic (m_Sampling picks light
**	  samples only, bounce rays only, or both). Misses add m_Background.
**	  Without m_bUseCosine (the raster's "Illumination Uses Normals"
**	  preference off) direct light has no cosine and area lights are
**	  sampled only through light samples.
**	- e_optxLightingDebug: the tracer debug view: m_DebugView's quantity
**	  (optxDebugView) of the first surface hit, unlit; misses black.
**	- e_optxLightingShadowMask: the raster Shadow Mask pass: the product,
**	  over the projected lights that reach the surface and face it, of 1 -
**	  (1 - visibility) * cosine; no bounces. Misses add m_Background
**	  (white).
**	Except in beauty mode, the resolve writes the mean, clamped to [0, 1],
**	straight into the 8-bit colour buffer (no sRGB encoding: the raster
**	passes copy their linear buffer the same way).
**
**	Surfaces have their materials (optxMaterialTypes.h): textures sampled
**	at ray-cone levels (optxMaterial.h; camera rays start m_ConeWidth wide
**	and widen by m_ConeSpread per unit of distance; each bounce adds
**	c_optxBounceSpread), normal maps bend the shading normal (never past
**	the horizon), and cut-out and transparent materials let rays through.
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
		e_optxLightingShadowMask = 1,
		e_optxLightingDebug = 2,
		e_optxLightingBeauty = 3
	};

	//	How the beauty pass picks the light it samples at a hit
	enum optxLightSelection
	{
		e_optxSelectOne = 0,		// one, in proportion to its light at the point
		e_optxSelectAll = 1			// every light (for tests: converges to the same image)
	};

	//	m_DebugView (g3dRenderPrefs::m_TraceDebugView's values)
	enum optxDebugView
	{
		e_optxDebugOff = 0,
		e_optxDebugBaseColor = 1,		// base colour x map
		e_optxDebugNormal = 2,			// the shading normal (world), as 0.5 + 0.5 n
		e_optxDebugRoughness = 3,
		e_optxDebugMetallic = 4,
		e_optxDebugEmission = 5,		// emission x strength x map
		e_optxDebugOpacity = 6			// opacity value x map
	};

	//	The angle (radians) a diffuse bounce adds to a ray cone's spread
	constexpr float c_optxBounceSpread = 0.25f;

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
		unsigned long long			m_Materials;	// device address of the optxMaterial table
		unsigned long long			m_Textures;		// device address of the optxTexture table
		unsigned long long			m_Lights;		// device address of m_NumLights optxLights
		unsigned long long			m_LightSets;	// device address of the optxLightSets
		unsigned long long			m_LightIndices;	// device address of the sets' light indices
		unsigned long long			m_Accum;		// device address of 4 floats per pixel (beauty: 3 planes of them)
		unsigned long long			m_Color;		// device address of a 32-bit texel per pixel (beauty: 64-bit, RGBA16F)
		unsigned long long			m_Depth;		// device address of a 32-bit texel per pixel
		unsigned long long			m_Aux;			// beauty: device address of an RGBA16F texel per pixel, or 0
		unsigned int				m_Width;
		unsigned int				m_Height;
		float						m_InvViewProj[16];
		float						m_ViewProj[16];
		float						m_Background[3];	// linear
		float						m_Epsilon;			// secondary rays start this far off the surface
		float						m_Albedo;			// of the bounce surfaces, with m_bAlbedoOverride
		float						m_ConeWidth;		// a camera ray's cone width at its origin (orthographic: the pixel's)
		float						m_ConeSpread;		// its spread angle (perspective: a pixel's angle)
		float						m_ViewZ[4];			// view-space depth = dot(p, xyz) + w (the camera matrix's third column)
		float						m_Dof[4];			// near blur, near focal, far focal, far blur distances
		float						m_DofCutoff;		// the far blur's limit
		float						m_MaxRadiance;		// beauty: the limit after the first bounce (0: none)
		unsigned int				m_bAlbedoOverride;
		unsigned int				m_DebugView;		// optxDebugView, e_optxLightingDebug
		unsigned int				m_NumLights;
		unsigned int				m_Mode;				// optxLightingMode
		unsigned int				m_Bounces;
		unsigned int				m_Sampling;			// optxLightSampling
		unsigned int				m_LightSelection;	// optxLightSelection (beauty)
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
