/*****************************************************************************
**  optxRandom.h
**
**      The hash random number generator: a PCG hash chain seeded by
**	(pixel, sample, seed), so the same launch gives the same numbers.
**	optxSampler.h hands its numbers out for the dimensions that aren't
**	low-discrepancy. Host and device (optxHostDevice.h); include it
**	guarded.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXRANDOM_H
#error optxRandom.h multiply included
#endif
#define GOX_OPTXRANDOM_H

#ifndef GOX_OPTXHOSTDEVICE_H
#include "optxHostDevice.h"
#endif

namespace gox
{
namespace optxq
{
	//	PCG hash (Jarzynski and Olano, "Hash Functions for GPU Rendering")
	GOX_HD unsigned int pcg(unsigned int v)
	{
		const unsigned int state = v * 747796405u + 2891336453u;
		const unsigned int word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	//	The top 24 bits of x as a float in [0, 1)
	GOX_HD float to_unit(unsigned int x)
	{
		return (x >> 8) * (1.0f / 16777216.0f);
	}

	struct Rng
	{
		unsigned int m_State;

		GOX_HD Rng(unsigned int i_Pixel, unsigned int i_Sample, unsigned int i_Seed)
			: m_State(pcg(i_Pixel ^ pcg(i_Sample ^ pcg(i_Seed))))
		{
		}

		//	[0, 1)
		GOX_HD float Next()
		{
			m_State = pcg(m_State);
			return to_unit(m_State);
		}
	};
}
}
