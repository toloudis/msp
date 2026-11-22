/*****************************************************************************
**  optxPixelFilter.h
**
**      The pixel filter the tracer's samples are weighted by: the capture
**	filters (Core's maFilter kinds, in captRenderOutputData::eFilterFunc's
**	order, which g3dProgressive::FilterKind mirrors) and their width in
**	pixels. A sample lands at offset (dx, dy) from its pixel's centre, a
**	point of [0, 1)^2 spread over the filter's width (offset: (u - 0.5)
**	width, as maSampling::GetSamples2D_Repeatable spreads the raster
**	capture's samples), and is weighted by weight(dx, dy) (maFilter::
**	Evaluate's formulas). The pixel resolves to the weighted sum over the
**	weights' sum, so filters with negative lobes work as the raster
**	capture's do. A box of width 1 weighs every sample 1: plain
**	averaging over the pixel (the viewport's filter).
**	Plain C struct and functions for the launch parameters; host and
**	device (optxHostDevice.h); include it guarded.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXPIXELFILTER_H
#error optxPixelFilter.h multiply included
#endif
#define GOX_OPTXPIXELFILTER_H

#ifndef GOX_OPTXHOSTDEVICE_H
#include "optxHostDevice.h"
#endif

#if !defined(__CUDACC__)
#include <math.h>
#endif

namespace gox
{
	enum optxPixelFilterKind
	{
		e_optxFilterBox = 0,
		e_optxFilterGaussian = 1,
		e_optxFilterMitchell = 2,
		e_optxFilterTriangle = 3,
		e_optxFilterSinc = 4,
		e_optxFilterLanczos = 5,
		e_optxFilterBlackmanHarris = 6,
		e_optxFilterCatmullRom = 7,
		e_optxFilterCount = 8
	};

	struct optxPixelFilter
	{
		unsigned int	m_Kind;		// optxPixelFilterKind
		float			m_Width;	// pixels, the same along x and y
	};

namespace optxf
{
	constexpr float c_Pi = 3.14159265358979f;

	//	A point in [0, 1) to an offset from the pixel's centre
	GOX_HD float offset(const optxPixelFilter& i_Filter, float u)
	{
		return (u - 0.5f) * i_Filter.m_Width;
	}

	GOX_HD float mitchell_1d(float x)
	{
		const float B = 1.0f / 3.0f;
		const float C = 1.0f / 3.0f;
		x = fabsf(2.0f * x);
		if (x > 1.0f)
		{
			return ((-B - 6 * C) * x * x * x + (6 * B + 30 * C) * x * x + (-12 * B - 48 * C) * x + (8 * B + 24 * C)) * (1.0f / 6.0f);
		}
		return ((12 - 9 * B - 6 * C) * x * x * x + (-18 + 12 * B + 6 * C) * x * x + (6 - 2 * B)) * (1.0f / 6.0f);
	}

	//	The raster's windowed sinc (a -pi to pi cosine window)
	GOX_HD float sinc_1d(float x, float width)
	{
		if (x == 0.0f)
		{
			return 1.0f;
		}
		x *= c_Pi;
		return cosf(0.5f * x / width) * sinf(x) / x;
	}

	GOX_HD float lanczos_1d(float x, float width)
	{
		x = fabsf(x);
		return x < width ? sinc_1d(x, width) * sinc_1d(x / width, width) : 0.0f;
	}

	//	The filter's weight at offset (dx, dy) pixels (maFilter::Evaluate
	//	with both widths the filter's)
	GOX_HD float weight(const optxPixelFilter& i_Filter, float dx, float dy)
	{
		const float w = i_Filter.m_Width;
		switch (i_Filter.m_Kind)
		{
			default:
			case e_optxFilterBox:
				return (fabsf(dx) <= 0.5f * w && fabsf(dy) <= 0.5f * w) ? 1.0f : 0.0f;
			case e_optxFilterGaussian:
			{
				const float x = dx / w, y = dy / w;
				return expf(-8.0f * (x * x + y * y));
			}
			case e_optxFilterMitchell:
				return mitchell_1d(dx / w) * mitchell_1d(dy / w);
			case e_optxFilterTriangle:
			{
				const float h = 0.5f * w;
				const float ax = fabsf(dx), ay = fabsf(dy);
				return fminf(ax <= h ? (h - ax) / h : 0.0f, ay <= h ? (h - ay) / h : 0.0f);
			}
			case e_optxFilterSinc:
				return sinc_1d(dx, w) * sinc_1d(dy, w);
			case e_optxFilterLanczos:
				return lanczos_1d(dx, w) * lanczos_1d(dy, w);
			case e_optxFilterBlackmanHarris:
			{
				const float x = dx / w, y = dy / w;
				const float r = 0.5f - sqrtf(x * x + y * y);
				if (r > 0.5f)
				{
					return 0.0f;
				}
				return 0.35875f - 0.48829f * cosf(2.0f * c_Pi * r) + 0.14128f * cosf(4.0f * c_Pi * r) -
					   0.01168f * cosf(6.0f * c_Pi * r);
			}
			case e_optxFilterCatmullRom:
			{
				//	in pixels, whatever the width (as the raster's)
				const float r2 = dx * dx + dy * dy;
				const float r = sqrtf(r2);
				if (r < 1.0f)
				{
					return 1.5f * r * r2 - 2.5f * r2 + 1.0f;
				}
				if (r < 2.0f)
				{
					return -0.5f * r * r2 + 2.5f * r2 - 4.0f * r + 2.0f;
				}
				return 0.0f;
			}
		}
	}
}
}
