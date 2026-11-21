/*****************************************************************************
**  optxSobol.h
**
**      The first two dimensions of the Sobol sequence as 32-bit fixed-point
**	coordinates in [0, 1): dimension 0 is the van der Corput sequence (the
**	index's bits reversed), dimension 1 the one from the primitive
**	polynomial x + 1 (Joe and Kuo's first; direction numbers v1 = 2^31,
**	v(k+1) = v(k) ^ (v(k) >> 1)). Together they are a (0, 2)-sequence in
**	base 2: every aligned block of 2^m points is a (0, m, 2)-net. Indices
**	are 32 bits. Host and device (optxHostDevice.h); include it guarded.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXSOBOL_H
#error optxSobol.h multiply included
#endif
#define GOX_OPTXSOBOL_H

#ifndef GOX_OPTXOWEN_H
#include "optxOwen.h"
#endif

namespace gox
{
namespace optxq
{
	GOX_HD unsigned int sobol0(unsigned int i_Index)
	{
		return reverse_bits(i_Index);
	}

	GOX_HD unsigned int sobol1(unsigned int i_Index)
	{
		unsigned int result = 0;
		for (unsigned int v = 1u << 31; i_Index; i_Index >>= 1, v ^= v >> 1)
		{
			if (i_Index & 1u)
			{
				result ^= v;
			}
		}
		return result;
	}
}
}
