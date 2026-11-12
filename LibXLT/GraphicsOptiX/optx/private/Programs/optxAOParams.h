/*****************************************************************************
**  optxAOParams.h
**
**      The launch parameters of the ambient occlusion programs (optxAO.cu),
**	shared by the device programs and the host code that launches them
**	(optxAO). Two launches, each m_Width x m_Height:
**
**	- __raygen__ao adds m_Samples samples per pixel to the accumulation
**	  buffer (the first, at m_FirstSample 0, replaces what it holds). Each
**	  sample is a camera ray through a point of the pixel (its centre for
**	  sample 0, else a random point) from the inverse view-projection; on a
**	  hit, m_AORays cosine-weighted occlusion rays up to m_Radius from the
**	  hit point add the fraction that escaped; a miss adds the background.
**	  Sample 0 also writes the depth buffer: the hit's clip-space z/w from
**	  m_ViewProj (the raster's), or 1 for a miss.
**	- __raygen__resolve divides the sums by m_FirstSample (the samples
**	  accumulated so far), encodes them as sRGB and writes the 8-bit colour
**	  buffer in the target's byte order.
**
**	Matrices multiply row vectors (p * M), as maMatrix4x4 does; row-major.
**	Random numbers are a hash of (pixel, sample, m_Seed): the same
**	parameters give the same image.
**
**	Plain C structs: the host and nvcc must lay them out the same way.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXAOPARAMS_H
#error optxAOParams.h multiply included
#endif
#define GOX_OPTXAOPARAMS_H

#include <optix_types.h>

namespace gox
{
	//	m_ColorOrder: the bytes of a colour buffer texel, first to last
	enum optxColorOrder
	{
		e_optxColorRGBA = 0,
		e_optxColorBGRA = 1
	};

	//	m_DepthLayout: a depth buffer texel (32 bits), as a copy into the
	//	target's depth aspect wants it
	enum optxDepthLayout
	{
		e_optxDepthD24 = 0,		// unsigned 24-bit depth in the low bits (D24_UNORM_S8_UINT)
		e_optxDepthFloat = 1	// float (D32_SFLOAT)
	};

	struct optxAOParams
	{
		OptixTraversableHandle		m_Handle;		// the scene's IAS; 0: every ray misses
		unsigned long long			m_Instances;	// device address of the optxInstanceData table
		unsigned long long			m_Accum;		// device address of 4 floats per pixel (rgb sums)
		unsigned long long			m_Color;		// device address of a 32-bit texel per pixel
		unsigned long long			m_Depth;		// device address of a 32-bit texel per pixel
		unsigned int				m_Width;
		unsigned int				m_Height;
		float						m_InvViewProj[16];
		float						m_ViewProj[16];
		float						m_Background[3];	// linear
		float						m_Radius;			// occlusion ray length
		float						m_Epsilon;			// occlusion rays start this far off the surface
		unsigned int				m_FirstSample;		// samples accumulated before this launch
		unsigned int				m_Samples;			// samples this launch adds
		unsigned int				m_AORays;			// occlusion rays per sample
		unsigned int				m_ColorOrder;		// optxColorOrder
		unsigned int				m_DepthLayout;		// optxDepthLayout
		unsigned int				m_Seed;
	};
}
