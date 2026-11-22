/*****************************************************************************
**  optxOwen.h
**
**      Hash-based Owen scrambling (Burley 2020, "Practical Hash-based Owen
**	Scrambling", JCGT 9(4)) and the hashes the sampling headers use:
**	- nested_uniform_scramble: a 32-bit fixed-point coordinate in [0, 1)
**	  scrambled as Owen's nested uniform scrambling does (each bit flipped
**	  by a hash of the bits above it), so a (t, m, s)-net stays one;
**	- laine_karras_permutation: the hash at its core; applied to a sample
**	  index (unreversed) it shuffles the sequence's order while keeping
**	  every power-of-two prefix a net (Burley's index shuffle);
**	- mix_bits: a 64-bit finalizer (pbrt-v4's MixBits), and hash, two
**	  32-bit values to one.
**	Host and device (optxHostDevice.h); include it guarded.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXOWEN_H
#error optxOwen.h multiply included
#endif
#define GOX_OPTXOWEN_H

#ifndef GOX_OPTXHOSTDEVICE_H
#include "optxHostDevice.h"
#endif

namespace gox
{
namespace optxq
{
	GOX_HD unsigned int reverse_bits(unsigned int x)
	{
#if defined(__CUDA_ARCH__)
		return __brev(x);
#else
		x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
		x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
		x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
		x = ((x >> 8) & 0x00ff00ffu) | ((x & 0x00ff00ffu) << 8);
		return (x >> 16) | (x << 16);
#endif
	}

	//	Burley's improved Laine-Karras hash: each bit of the result depends
	//	only on that bit and the bits below it
	GOX_HD unsigned int laine_karras_permutation(unsigned int x, unsigned int seed)
	{
		x ^= x * 0x3d20adeau;
		x += seed;
		x *= (seed >> 16) | 1u;
		x ^= x * 0x05526c56u;
		x ^= x * 0x53a22864u;
		return x;
	}

	//	Owen scrambling of a coordinate (bits from the most significant):
	//	the Laine-Karras hash on the reversed bits, so each bit depends on
	//	the bits above it
	GOX_HD unsigned int nested_uniform_scramble(unsigned int x, unsigned int seed)
	{
		return reverse_bits(laine_karras_permutation(reverse_bits(x), seed));
	}

	GOX_HD unsigned long long mix_bits(unsigned long long v)
	{
		v ^= v >> 31;
		v *= 0x7fb5d329728ea185ull;
		v ^= v >> 27;
		v *= 0x81dadef4bc2dd44dull;
		v ^= v >> 33;
		return v;
	}

	GOX_HD unsigned int hash(unsigned int a, unsigned int b)
	{
		return static_cast<unsigned int>(mix_bits((static_cast<unsigned long long>(a) << 32) | b));
	}
}
}
