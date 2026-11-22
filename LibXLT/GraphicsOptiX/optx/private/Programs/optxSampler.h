/*****************************************************************************
**  optxSampler.h
**
**      The one interface the traced programs draw their numbers from. A
**	Sampler is made per pixel and sample, the pixel given by its place in
**	the whole image (not its tile, so a frame traced in tiles takes the
**	same numbers as one traced whole), and hands numbers out by what they
**	are for:
**	- camera_pixel: where in the pixel the camera ray goes (sample 0: the
**	  centre, whose depth the raster overlay tests against; sample k
**	  takes sequence point k - 1, so samples 1 to 2^m are stratified);
**	- camera_lens: the point of the lens (depth of field);
**	- light: a light sample's two numbers, and direction: a sampled
**	  bounce direction's (the BRDF's, an AO ray's); low-discrepancy at the
**	  first hit's first one, the hash generator after;
**	- next: any other number (lobe and light choices, roulette).
**	Which numbers are low-discrepancy, and how, is decided here alone
**	(m_Order, the same for a whole launch):
**	- e_optxOrderBlueNoise (the default): each pair of dimensions is the
**	  first two dimensions of the Sobol sequence (optxSobol.h) at the
**	  index optxBlueNoise.h's ordering gives the pixel's sample, Owen
**	  scrambled (optxOwen.h) with a seed per pair; pixels own aligned
**	  blocks of 2^m_Log2Samples points, so a sample number past the block
**	  takes hash numbers instead;
**	- e_optxOrderPerPixel: Owen-scrambled Sobol, each pixel its own
**	  scramble and index shuffle (no blue noise; for comparison);
**	- e_optxOrderHash: the hash generator (optxRandom.h) for everything,
**	  sample 0 still at the centre (the positions before Sobol; for
**	  comparison).
**	Host and device (optxHostDevice.h); include it guarded.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXSAMPLER_H
#error optxSampler.h multiply included
#endif
#define GOX_OPTXSAMPLER_H

#ifndef GOX_OPTXRANDOM_H
#include "optxRandom.h"
#endif
#ifndef GOX_OPTXSOBOL_H
#include "optxSobol.h"
#endif
#ifndef GOX_OPTXBLUENOISE_H
#include "optxBlueNoise.h"
#endif

namespace gox
{
	//	optxPixelSequence::m_Order
	enum optxSequenceOrder
	{
		e_optxOrderBlueNoise = 0,
		e_optxOrderPerPixel = 1,
		e_optxOrderHash = 2
	};

	//	The launch's sequence (plain C: in the launch parameters)
	struct optxPixelSequence
	{
		unsigned int	m_Order;			// optxSequenceOrder
		unsigned int	m_Log2Resolution;	// the whole image's larger side, rounded up to a power of two (at most 16)
		unsigned int	m_Log2Samples;		// a pixel's block of the blue-noise sequence: 2^m points
		unsigned int	m_Seed;
	};

namespace optxq
{
	struct Pair
	{
		float	u;
		float	v;
	};

	//	The pairs of dimensions the sequence gives
	enum SamplerPair
	{
		e_PairPixel = 0,
		e_PairLens = 1,
		e_PairLight = 2,
		e_PairDirection = 3
	};

	class Sampler
	{
		public:
			//	Pixel (x, y) of the whole image, sample i_Sample
			GOX_HD Sampler(const optxPixelSequence& i_Sequence, unsigned int x, unsigned int y, unsigned int i_Sample)
				: m_Sequence(i_Sequence)
				, m_X(x)
				, m_Y(y)
				, m_Sample(i_Sample)
				, m_Rng(morton2(x, y), i_Sample, i_Sequence.m_Seed)
			{
			}

			GOX_HD Pair camera_pixel()
			{
				if (m_Sample == 0)
				{
					return Pair{ 0.5f, 0.5f };
				}
				return pair(e_PairPixel, m_Sample - 1);
			}

			GOX_HD Pair camera_lens() { return pair(e_PairLens, m_Sample); }

			//	The i_Index-th light sample at bounce i_Bounce (0: the first hit)
			GOX_HD Pair light(unsigned int i_Bounce, unsigned int i_Index = 0)
			{
				return (i_Bounce == 0 && i_Index == 0) ? pair(e_PairLight, m_Sample) : hashed();
			}

			//	The i_Index-th sampled direction at bounce i_Bounce
			GOX_HD Pair direction(unsigned int i_Bounce, unsigned int i_Index = 0)
			{
				return (i_Bounce == 0 && i_Index == 0) ? pair(e_PairDirection, m_Sample) : hashed();
			}

			GOX_HD float next() { return m_Rng.Next(); }

			//	The sequence's point for a pair of dimensions at sequence index
			//	i_Index (for tests; the calls above use it)
			GOX_HD Pair pair(unsigned int i_Pair, unsigned int i_Index)
			{
				unsigned int index, seed;
				switch (m_Sequence.m_Order)
				{
					case e_optxOrderBlueNoise:
					{
						if (m_Sequence.m_Log2Samples < 32 && (i_Index >> m_Sequence.m_Log2Samples) != 0)
						{
							return hashed();	// past the pixel's block
						}
						seed = hash(i_Pair, m_Sequence.m_Seed);
						index = zsobol_index(m_X, m_Y, i_Index, m_Sequence.m_Log2Resolution, m_Sequence.m_Log2Samples, seed);
						break;
					}
					case e_optxOrderPerPixel:
						seed = hash(hash(morton2(m_X, m_Y), i_Pair), m_Sequence.m_Seed);
						index = nested_uniform_scramble(i_Index, seed);		// Burley's shuffle
						break;
					default:
						return hashed();
				}
				return Pair{ to_unit(nested_uniform_scramble(sobol0(index), hash(seed, 0u))),
							 to_unit(nested_uniform_scramble(sobol1(index), hash(seed, 1u))) };
			}

		private:
			GOX_HD Pair hashed()
			{
				const float u = m_Rng.Next();
				return Pair{ u, m_Rng.Next() };
			}

			optxPixelSequence	m_Sequence;
			unsigned int		m_X;
			unsigned int		m_Y;
			unsigned int		m_Sample;
			Rng					m_Rng;
	};
}
}
