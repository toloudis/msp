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
**	Three more launches build m_Environment's sampling table (see
**	optxEnvTableFloats), in order on the stream: __raygen__env_cells over
**	the table's W x H cells (each cell's luminance of colour x map, read
**	as m_Taps x m_Taps samples at m_TapLevel, times sin theta at the
**	cell's centre), __raygen__env_rows over its H rows (each row's sum and
**	CDF), __raygen__env_marginal once (the rows' CDF and the total).
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
**	  never limited.
**	  The environment (m_Environment, optxEnvironmentLight, when
**	  e_optxEnvironmentOn) is an infinitely distant sphere of radiance
**	  colour x map(direction) (the map read as the raster reads a lat-long
**	  map: CartesianToPolar of the direction turned about y by the angle;
**	  no map: the colour alone). It is one of the choices of the light
**	  sample, weighted by its mean radiance's luminance, and is sampled
**	  from its sampling table (in proportion to luminance x sin theta per
**	  cell) or, without one, cosine-weighted about the shading normal; its
**	  shadow rays go to infinity, and every surface the camera sees
**	  occludes it (casting shadows or not, as for BRDF rays). BRDF rays
**	  that miss add it, weighted against its light samples by the power
**	  heuristic. It lights the BRDF's diffuse and specular lobes as its
**	  switches say. Rays leaving a surface read the map at one level per
**	  surface (its ray cone's, but never blurrier than the sampling
**	  table), so both strategies see the same radiance. Camera misses add
**	  the environment (at the pixel's cone level) with
**	  e_optxEnvironmentBackground, else m_Background; with the
**	  environment off, other misses add nothing.
**	  Depth of field is the camera's lens: with m_LensRadius above 0, every
**	  sample but sample 0 (which gives the depth) starts its camera ray at
**	  a uniformly chosen point of a disk of that radius around m_Eye (in
**	  the plane of m_LensRight and m_LensUp) and aims it at the point the
**	  pinhole ray reaches at depth m_FocusDistance along m_Forward (a thin
**	  lens); the ray starts at the near plane's depth.
**	  The accumulation buffer has three planes of m_Width x m_Height float4
**	  (each plane after the last): rgb sums and depth; first-hit base
**	  colour sums and coverage (the samples that hit); first-hit shading
**	  normal sums (w unused). The resolve writes the linear mean
**	  (unclamped) and the coverage as RGBA16F into m_Color.
**	- e_optxLightingReflection: Reflection Only, traced: the beauty pass
**	  (everything above, its output included) with its first hit changed.
**	  There, nothing is emitted, no light is sampled and the diffuse lobe
**	  is skipped: one direction is sampled from the specular lobe alone
**	  (GGX visible normals) and weighted by f_spec cos / pdf_spec, so the
**	  BRDF's Fresnel (Schlick from F0 to F90) is the reflection's weight: a
**	  metal tints it, it grows at grazing angles, and a surface with no
**	  specular (F0 and F90 black) reflects nothing. That ray takes all the
**	  light it finds (an area light it hits, the environment if it misses:
**	  no light sample there to weigh it against); the surface it reaches is
**	  lit as the beauty pass's first hit (its emission, direct light, then
**	  m_Bounces bounces). Camera misses add m_Background, never the
**	  environment.
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
		e_optxLightingBeauty = 3,
		e_optxLightingReflection = 4
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

	//	optxEnvironmentLight::m_Flags
	enum optxEnvironmentFlags
	{
		e_optxEnvironmentOn = 1,			// the beauty pass has an environment light
		e_optxEnvironmentDiffuse = 2,		// it lights the diffuse lobe
		e_optxEnvironmentSpecular = 4,		// it lights the specular lobe
		e_optxEnvironmentBackground = 8		// camera misses show it
	};

	//	The sampling table (a float array at m_Table, m_TableWidth = W by
	//	m_TableHeight = H cells over the map's u and v), laid out as these
	//	offsets give: the cells' values (luminance x sin theta at the cell,
	//	H rows of W), each row's CDF (H rows of W + 1, from 0 to 1), the
	//	rows' CDF (H + 1), each row's sum (H), and the cells' total (1)
#if defined(__CUDACC__)
#define GOX_ENV_HD __host__ __device__
#else
#define GOX_ENV_HD
#endif
	GOX_ENV_HD inline unsigned int optxEnvRowCdfAt(unsigned int W, unsigned int H) { return W * H; }
	GOX_ENV_HD inline unsigned int optxEnvMarginalAt(unsigned int W, unsigned int H) { return W * H + (W + 1) * H; }
	GOX_ENV_HD inline unsigned int optxEnvRowSumsAt(unsigned int W, unsigned int H) { return W * H + (W + 1) * H + H + 1; }
	GOX_ENV_HD inline unsigned int optxEnvTotalAt(unsigned int W, unsigned int H) { return W * H + (W + 1) * H + 2 * H + 1; }
	GOX_ENV_HD inline unsigned int optxEnvTableFloats(unsigned int W, unsigned int H) { return optxEnvTotalAt(W, H) + 1; }
#undef GOX_ENV_HD

	//	The beauty pass's environment light (see above)
	struct optxEnvironmentLight
	{
		unsigned long long		m_Table;		// device address of the sampling table; 0: cosine-weighted samples
		unsigned int			m_Texture;		// index into the texture table; c_optxNoTexture: the colour alone
		unsigned int			m_Flags;		// optxEnvironmentFlags
		float					m_Color[3];		// linear: the map's colour x factor
		float					m_Angle;		// radians: a direction d reads the map at rotateAboutY(d, m_Angle)
		unsigned int			m_TableWidth;
		unsigned int			m_TableHeight;
		float					m_TableLevel;	// the map's level the table's cells match (rays never read blurrier)
		unsigned int			m_Taps;			// building the table: samples per cell along each axis
		float					m_TapLevel;		// and the level they are read at
		float					m_Pad;
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
		unsigned int				m_Width;
		unsigned int				m_Height;
		float						m_InvViewProj[16];
		float						m_ViewProj[16];
		float						m_Background[3];	// linear
		float						m_Epsilon;			// secondary rays start this far off the surface
		float						m_Albedo;			// of the bounce surfaces, with m_bAlbedoOverride
		float						m_ConeWidth;		// a camera ray's cone width at its origin (orthographic: the pixel's)
		float						m_ConeSpread;		// its spread angle (perspective: a pixel's angle)
		float						m_Eye[3];			// beauty: the lens's centre (the camera's position)
		float						m_LensRadius;		// beauty: the aperture's radius (world units; 0: a pinhole)
		float						m_Forward[3];		// beauty: the view direction (unit)
		float						m_FocusDistance;	// beauty: the depth in focus, along m_Forward
		float						m_LensRight[3];		// beauty: the lens plane's axes (unit)
		float						m_Pad0;
		float						m_LensUp[3];
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
		unsigned int				m_Pad1;
		optxEnvironmentLight		m_Environment;		// beauty (and the table's build launches)
	};
}
