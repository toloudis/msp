/*****************************************************************************
**  optxBlueNoise.h
**
**      The screen-space ordering of the pixel sequence: which point of one
**	global sequence a pixel's sample takes, so that the errors of pixels
**	close on screen are blue noise (Ahmed and Wonka 2020, "Screen-Space
**	Blue-Noise Diffusion of Monte Carlo Sampling Error via Hierarchical
**	Ordering of Pixels"), in the randomized form of pbrt-v4's
**	ZSobolSampler:
**	- the pixels are ordered along the Morton (Z) curve of the whole image
**	  (its larger side rounded up to a power of two: 2^log2Res), and pixel
**	  p's sample j takes global index (morton(p) << log2Spp) | j, so each
**	  pixel owns an aligned block of 2^log2Spp points (a net: its samples
**	  are stratified) and neighbouring pixels own neighbouring blocks;
**	- the index's base-4 digits, from the most significant, are each
**	  permuted by one of the 24 permutations of {0, 1, 2, 3}, picked by a
**	  hash of the digits above it and the dimension (a random tree over
**	  the hierarchy of pixels and samples), so neighbours' samples are
**	  stratified against each other at every sample count and each
**	  dimension pair is ordered independently.
**	Only the index's low 32 bits are kept (optxSobol.h's indices): the
**	bits above only change a point below 2^-32.
**	zsobol_index is the one function the sampler calls; another ordering
**	(Heitz and Belcour's tables, or none) could stand behind it instead.
**	Host and device (optxHostDevice.h); include it guarded.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXBLUENOISE_H
#error optxBlueNoise.h multiply included
#endif
#define GOX_OPTXBLUENOISE_H

#ifndef GOX_OPTXOWEN_H
#include "optxOwen.h"
#endif

namespace gox
{
namespace optxq
{
	//	The bits of x (16 of them) moved to the even positions
	GOX_HD unsigned int spread_bits(unsigned int x)
	{
		x &= 0x0000ffffu;
		x = (x | (x << 8)) & 0x00ff00ffu;
		x = (x | (x << 4)) & 0x0f0f0f0fu;
		x = (x | (x << 2)) & 0x33333333u;
		x = (x | (x << 1)) & 0x55555555u;
		return x;
	}

	//	Morton (Z) order of a pixel: x in the even bits, y in the odd ones
	GOX_HD unsigned int morton2(unsigned int x, unsigned int y)
	{
		return spread_bits(x) | (spread_bits(y) << 1);
	}

	//	Element i_Digit of permutation i_Which (0 to 23) of {0, 1, 2, 3}:
	//	i_Which read as a Lehmer code (factorial base 3! 2! 1!)
	GOX_HD unsigned int permute4(unsigned int i_Which, unsigned int i_Digit)
	{
		unsigned int left = 0x3210u;		// the elements not yet placed, a nibble each
		unsigned int order = 0;
		unsigned int radix = 6;
		for (unsigned int k = 0; k < 3; ++k)
		{
			const unsigned int c = i_Which / radix;
			i_Which %= radix;
			radix = (k == 0) ? 2 : 1;
			const unsigned int shift = 4 * c;
			order |= ((left >> shift) & 0xfu) << (4 * k);
			left = (left & ((1u << shift) - 1u)) | ((left >> (shift + 4)) << shift);
		}
		order |= (left & 0xfu) << 12;
		return (order >> (4 * i_Digit)) & 0xfu;
	}

	//	The global sequence index of pixel (x, y)'s sample i_Sample (below
	//	2^i_Log2Spp; x and y below 2^i_Log2Res, at most 16) for one
	//	dimension pair (i_Dimension: any value; it seeds the permutations)
	GOX_HD unsigned int zsobol_index(unsigned int x, unsigned int y, unsigned int i_Sample, unsigned int i_Log2Res,
									 unsigned int i_Log2Spp, unsigned int i_Dimension)
	{
		const unsigned long long morton = (static_cast<unsigned long long>(morton2(x, y)) << i_Log2Spp) | i_Sample;
		const unsigned long long salt = 0x55555555ull * i_Dimension;
		const unsigned int odd = i_Log2Spp & 1u;
		const int digits = int(i_Log2Res + (i_Log2Spp + 1) / 2);
		unsigned long long index = 0;
		for (int i = digits - 1; i >= int(odd); --i)
		{
			const int shift = 2 * i - int(odd);
			const unsigned int digit = static_cast<unsigned int>(morton >> shift) & 3u;
			const unsigned long long higher = morton >> (shift + 2);
			const unsigned int which = static_cast<unsigned int>((mix_bits(higher ^ salt) >> 24) % 24);
			index |= static_cast<unsigned long long>(permute4(which, digit)) << shift;
		}
		if (odd)
		{
			//	the lowest digit is one bit: flipped or not
			index |= (morton & 1ull) ^ (mix_bits((morton >> 1) ^ salt) & 1ull);
		}
		return static_cast<unsigned int>(index);
	}
}
}
