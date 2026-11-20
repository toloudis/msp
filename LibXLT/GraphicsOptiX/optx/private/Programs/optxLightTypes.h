/*****************************************************************************
**  optxLightTypes.h
**
**      The light table the lighting programs read (g3dLightTableOptiX
**	builds it from GraphicsVulkan's light manager): one optxLight per
**	enabled light, and the light sets instances name (optxSceneTypes.h's
**	m_LightSet), each a run of indices into the lights.
**
**	A light's values are what the raster lighting (Lighting.hlsli) uses,
**	so a traced point gets the same linear light as a rastered one where
**	nothing shadows it:
**	- colour: the light's intensity colour times its intensity factor
**	  (black when its diffuse light is off, or diffuse lighting is off in
**	  the preferences);
**	- attenuation (point lights and spot projected lights): 1 / (f0 +
**	  f1 d + f2 d^2 + f3 d^3), d the distance past the falloff start (1/f0
**	  before it);
**	- projected lights: the light's world-to-texture matrix (row vectors)
**	  gives (u, v, z, w); the light reaches a point with z in [0, m_Far),
**	  scaled by the elliptical penumbra of its inner and outer angles (or,
**	  if the inner angle is the bigger, the square u, v in [0, 1]), and
**	  by its texture (m_Texture: the projector image, or its ramp) sampled
**	  at (u, v): bilinear, mip 0, black outside, as the raster's projSampler.
**	  For an area light the texture is read at the shaded point's (u, v),
**	  not per light sample, as the raster does. The Shadow Mask ignores it.
**	- the direction to the light: to its position, or against its
**	  direction (directional lights, and directional projected lights).
**
**	Lights with a radius are area lights, for soft shadows: a disk of that
**	radius at the light's position facing the shaded point (spot projected
**	lights), or a cone of directions of that half angle (directional
**	projected lights, m_CosCone). Light without one are delta lights.
**	An area light's radiance is defined so that, unshadowed, it gives the
**	same light as the delta light at its centre; shadow rays go to points
**	on it, and bounce rays that hit it add its light, weighted against
**	the light sample by multiple importance sampling.
**
**	Plain C structs: the host and nvcc must lay them out the same way.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXLIGHTTYPES_H
#error optxLightTypes.h multiply included
#endif
#define GOX_OPTXLIGHTTYPES_H

namespace gox
{
	enum optxLightType
	{
		e_optxLightPoint = 0,			// position, attenuation (g3dPointLight, g3dSpotLight: as the raster, no cone)
		e_optxLightDirectional = 1,		// direction (g3dDirectionalLight, the headlight)
		e_optxLightProjected = 2,		// spot projected light: position, attenuation, matrix
		e_optxLightProjectedDirectional = 3	// directional projected light: direction, matrix
	};

	enum optxLightFlags
	{
		e_optxLightShadows = 1u << 0	// shadow rays are traced towards it
	};

	struct optxLight
	{
		float			m_Position[3];
		unsigned int	m_Type;			// optxLightType
		float			m_Direction[3];	// the way the light travels (normalized)
		unsigned int	m_Flags;		// optxLightFlags
		float			m_Color[3];		// linear
		float			m_Radius;		// area light: disk radius; 0: a delta light
		float			m_Falloff[4];	// f0 .. f3
		float			m_FalloffStart;
		float			m_CosCone;		// directional projected area light: cos of the cone's half angle
		float			m_Far;			// projected: texture z limit
		float			m_InnerAngle;	// projected: radians
		float			m_OuterAngle;
		float			m_Aspect;
		unsigned int	m_Texture;		// projected: texture table index, or c_optxNoTexture
		float			m_Pad;
		float			m_Matrix[16];	// projected: world to texture, row vectors, row-major
	};

	//	A light set: m_Count indices into the lights from m_First in the
	//	index array
	struct optxLightSet
	{
		unsigned int	m_First;
		unsigned int	m_Count;
	};
}
