/*****************************************************************************
**  optxAOParams.h
**
**      The launch parameters of the ambient occlusion programs (optxAO.cu),
**	shared by the device programs and the host code that launches them
**	(optxAO). Two launches, each m_Width x m_Height:
**
**	- __raygen__ao adds m_Samples samples per pixel to the accumulation
**	  buffer (the first, at m_FirstSample 0, replaces what it holds). Each
**	  sample is a camera ray through a point of the pixel from the inverse
**	  view-projection: the centre for sample 0, else the point the
**	  sampler (optxSampler.h, m_Sequence) gives, spread over the pixel
**	  filter's width (optxPixelFilter.h, m_Filter). On a hit, m_AORays
**	  cosine-weighted occlusion rays up to m_Radius from the hit point give
**	  the fraction that escaped; a miss gives the background. The sample
**	  adds its value times the filter's weight at its offset to the first
**	  plane's rgb, and the weight to the second plane's x. Sample 0 also
**	  keeps the pixel's depth in the first plane's w: the hit's clip-space
**	  z/w from m_ViewProj (the raster's), or 1 for a miss. With m_Resolve,
**	  it then resolves the pixel as __raygen__resolve does, so a frame that
**	  adds samples and shows them is one launch.
**	- __raygen__resolve divides the sums by the weights' sum, encodes them
**	  as sRGB and writes the 8-bit colour buffer in the target's byte
**	  order, and writes the kept depth to the depth buffer.
**
**	The accumulation buffer is two planes of m_Width x m_Height float4,
**	the second after the first. Pixels are identified by their place in
**	the whole image (m_ImageX, m_ImageY plus the launch index) for the
**	sampler.
**
**	Cut-out and transparent materials (m_Materials, m_Textures:
**	optxMaterialTypes.h) let camera and occlusion rays through, as
**	optxMaterial.h's OpacityPasses says (the any-hit program).
**
**	Matrices multiply row vectors (p * M), as maMatrix4x4 does; row-major.
**	The same parameters give the same image (m_Seed seeds the sampler
**	and the any-hit program's stochastic transparency).
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

#ifndef GOX_OPTXSAMPLER_H
#include "optxSampler.h"
#endif
#ifndef GOX_OPTXPIXELFILTER_H
#include "optxPixelFilter.h"
#endif

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
		unsigned long long			m_Materials;	// device address of the optxMaterial table (0: all opaque)
		unsigned long long			m_Textures;		// device address of the optxTexture table
		unsigned long long			m_Accum;		// device address of 2 planes of 4 floats per pixel (rgb sums and depth; the weights' sum)
		unsigned long long			m_Color;		// device address of a 32-bit texel per pixel
		unsigned long long			m_Depth;		// device address of a 32-bit texel per pixel
		unsigned int				m_Width;
		unsigned int				m_Height;
		unsigned int				m_ImageX;		// the frame's first pixel in the whole image
		unsigned int				m_ImageY;
		optxPixelSequence			m_Sequence;		// where samples go in their pixels, and the low-discrepancy numbers
		optxPixelFilter				m_Filter;		// what the samples are weighted by
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
		unsigned int				m_Resolve;			// __raygen__ao: also resolve (non-zero)
	};
}
