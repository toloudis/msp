/*****************************************************************************
**  optxSamplerTest.cpp
**
**      Part 3e of the OptiX plan (docs/plans/2026-11-21-optix-capture-
**	sampling.md), on the CPU: the sampling headers the traced programs use
**	(optx/private/Programs: optxSobol.h, optxOwen.h, optxBlueNoise.h,
**	optxSampler.h, optxPixelFilter.h) are plain C++, so these tests run the
**	code the GPU runs:
**	- the first two Sobol dimensions are a (0, 2)-sequence: every aligned
**	  block of 2^m points is a (0, m, 2)-net;
**	- Owen scrambling keeps nets nets, and the index shuffle keeps
**	  power-of-two prefixes nets;
**	- permute4 gives all 24 permutations;
**	- the blue-noise ordering gives each pixel an aligned block of the
**	  sequence (distinct indices over a whole image), so a pixel's
**	  power-of-four prefixes are nets; different pixels get different
**	  points;
**	- the sampler's sample 0 is the pixel centre, samples past the block
**	  fall back to the hash generator, and the hash order is the old one;
**	- each filter's weight is maFilter::Evaluate's.
**
**	Extra Large Technology
\****************************************************************************/
#include <gtest/gtest.h>

#ifndef GOX_OPTXSAMPLER_H
#include "GraphicsOptiX/optx/private/Programs/optxSampler.h"
#endif
#ifndef GOX_OPTXPIXELFILTER_H
#include "GraphicsOptiX/optx/private/Programs/optxPixelFilter.h"
#endif
#ifndef MA_SAMPLING_HPP
#include "Core/Ma/maSampling.hpp"
#endif

#include <memory>
#include <set>
#include <vector>

using namespace gox;
using namespace gox::optxq;

namespace
{
	struct Point
	{
		unsigned	x;		// 32-bit fixed point in [0, 1)
		unsigned	y;
	};

	//	Whether 2^m points are a (0, m, 2)-net: each elementary interval of
	//	area 2^-m (2^-a by 2^-(m - a)) holds exactly one
	::testing::AssertionResult is_net(const std::vector<Point>& i_Points, unsigned m)
	{
		if (i_Points.size() != (size_t(1) << m))
		{
			return ::testing::AssertionFailure() << i_Points.size() << " points for m = " << m;
		}
		for (unsigned a = 0; a <= m; ++a)
		{
			std::vector<int> count(size_t(1) << m, 0);
			for (const Point& p : i_Points)
			{
				const unsigned cx = a ? (p.x >> (32 - a)) : 0;
				const unsigned cy = (m - a) ? (p.y >> (32 - (m - a))) : 0;
				++count[(size_t(cx) << (m - a)) | cy];
			}
			for (size_t i = 0; i < count.size(); ++i)
			{
				if (count[i] != 1)
				{
					return ::testing::AssertionFailure() << "interval " << i << " of shape 2^-" << a << " x 2^-" << (m - a)
														 << " holds " << count[i] << " points (m = " << m << ")";
				}
			}
		}
		return ::testing::AssertionSuccess();
	}

	optxPixelSequence sequence(unsigned i_Order, unsigned i_Log2Res, unsigned i_Log2Spp, unsigned i_Seed = 7)
	{
		optxPixelSequence s;
		s.m_Order = i_Order;
		s.m_Log2Resolution = i_Log2Res;
		s.m_Log2Samples = i_Log2Spp;
		s.m_Seed = i_Seed;
		return s;
	}

	Point to_fixed(const Pair& i_Pair)
	{
		//	to_unit keeps the top 24 bits exactly
		return Point{ unsigned(double(i_Pair.u) * 4294967296.0), unsigned(double(i_Pair.v) * 4294967296.0) };
	}
}

TEST(optxSampler, SobolStartsAsExpected)
{
	//	(0, 0), (1/2, 1/2), (1/4, 3/4), (3/4, 1/4), (1/8, 5/8)
	const double expected[5][2] = { { 0, 0 }, { 0.5, 0.5 }, { 0.25, 0.75 }, { 0.75, 0.25 }, { 0.125, 0.625 } };
	for (unsigned i = 0; i < 5; ++i)
	{
		EXPECT_EQ(expected[i][0], sobol0(i) / 4294967296.0) << "point " << i;
		EXPECT_EQ(expected[i][1], sobol1(i) / 4294967296.0) << "point " << i;
	}
}

TEST(optxSampler, SobolBlocksAreNets)
{
	for (unsigned m = 1; m <= 10; ++m)
	{
		for (unsigned block = 0; block < 3; ++block)
		{
			std::vector<Point> points;
			for (unsigned i = 0; i < (1u << m); ++i)
			{
				const unsigned index = (block << m) | i;
				points.push_back(Point{ sobol0(index), sobol1(index) });
			}
			EXPECT_TRUE(is_net(points, m)) << "block " << block;
		}
	}
}

TEST(optxSampler, OwenScramblingAndShuffleKeepNets)
{
	for (unsigned seed : { 1u, 12345u, 0xdeadbeefu })
	{
		for (unsigned m = 1; m <= 10; ++m)
		{
			std::vector<Point> scrambled, shuffled;
			for (unsigned i = 0; i < (1u << m); ++i)
			{
				scrambled.push_back(Point{ nested_uniform_scramble(sobol0(i), seed), nested_uniform_scramble(sobol1(i), seed * 3) });
				const unsigned index = nested_uniform_scramble(i, seed);
				shuffled.push_back(Point{ sobol0(index), sobol1(index) });
			}
			EXPECT_TRUE(is_net(scrambled, m)) << "scrambled, seed " << seed;
			EXPECT_TRUE(is_net(shuffled, m)) << "shuffled prefix, seed " << seed;
		}
	}
	//	and the scramble changes the points
	EXPECT_NE(sobol0(1), nested_uniform_scramble(sobol0(1), 12345u));
}

TEST(optxSampler, Permute4GivesEveryPermutation)
{
	std::set<unsigned> seen;
	for (unsigned p = 0; p < 24; ++p)
	{
		unsigned code = 0, used = 0;
		for (unsigned d = 0; d < 4; ++d)
		{
			const unsigned v = permute4(p, d);
			ASSERT_LT(v, 4u);
			used |= 1u << v;
			code = code * 4 + v;
		}
		EXPECT_EQ(15u, used) << "permutation " << p << " isn't one";
		seen.insert(code);
	}
	EXPECT_EQ(24u, seen.size());
}

TEST(optxSampler, BlueNoiseOrderGivesEachPixelABlock)
{
	//	A 16 x 16 image, 2^m samples a pixel: the indices of all pixels and
	//	samples are distinct, and each pixel's are an aligned block
	for (unsigned m : { 0u, 1u, 2u, 3u, 4u })
	{
		std::set<unsigned> all;
		for (unsigned y = 0; y < 16; ++y)
		{
			for (unsigned x = 0; x < 16; ++x)
			{
				std::set<unsigned> blocks;
				for (unsigned j = 0; j < (1u << m); ++j)
				{
					const unsigned index = zsobol_index(x, y, j, 4, m, 99);
					all.insert(index);
					blocks.insert(index >> m);
				}
				EXPECT_EQ(1u, blocks.size()) << "pixel " << x << ", " << y << " (m = " << m << ")";
			}
		}
		EXPECT_EQ(size_t(256) << m, all.size()) << "m = " << m;
	}
}

TEST(optxSampler, PixelsPowerOfFourPrefixesAreNets)
{
	//	Blue noise: every pixel's first 4^k samples (pixel positions) are a
	//	net; and different pixels' first points differ
	const optxPixelSequence seq = sequence(e_optxOrderBlueNoise, 6, 8);
	std::set<unsigned> firsts;
	for (unsigned y = 0; y < 8; ++y)
	{
		for (unsigned x = 0; x < 8; ++x)
		{
			Sampler sampler(seq, x, y, 0);
			for (unsigned m : { 2u, 4u, 6u, 8u })
			{
				std::vector<Point> points;
				for (unsigned j = 0; j < (1u << m); ++j)
				{
					points.push_back(to_fixed(sampler.pair(e_PairPixel, j)));
				}
				//	the sampler's floats keep 24 bits: enough for m <= 24
				EXPECT_TRUE(is_net(points, m)) << "pixel " << x << ", " << y;
			}
			firsts.insert(to_fixed(sampler.pair(e_PairPixel, 0)).x);
		}
	}
	EXPECT_GT(firsts.size(), 60u) << "pixels share their first points";

	//	per-pixel scrambling: every power-of-two prefix is a net
	const optxPixelSequence per_pixel = sequence(e_optxOrderPerPixel, 6, 8);
	Sampler sampler(per_pixel, 3, 5, 0);
	for (unsigned m = 1; m <= 8; ++m)
	{
		std::vector<Point> points;
		for (unsigned j = 0; j < (1u << m); ++j)
		{
			points.push_back(to_fixed(sampler.pair(e_PairLens, j)));
		}
		EXPECT_TRUE(is_net(points, m)) << "per pixel, m = " << m;
	}
}

TEST(optxSampler, PairsAreIndependentlyOrdered)
{
	//	The pixel and lens pairs of one sample aren't the same point
	const optxPixelSequence seq = sequence(e_optxOrderBlueNoise, 6, 8);
	int same = 0;
	for (unsigned j = 0; j < 64; ++j)
	{
		Sampler sampler(seq, 10, 20, j + 1);
		const Pair a = sampler.pair(e_PairPixel, j), b = sampler.pair(e_PairLens, j);
		same += (a.u == b.u && a.v == b.v) ? 1 : 0;
	}
	EXPECT_EQ(0, same);
}

TEST(optxSampler, SamplerCallsAndFallbacks)
{
	const optxPixelSequence seq = sequence(e_optxOrderBlueNoise, 6, 4);
	{
		Sampler sampler(seq, 5, 6, 0);
		const Pair centre = sampler.camera_pixel();
		EXPECT_EQ(0.5f, centre.u);
		EXPECT_EQ(0.5f, centre.v);
	}
	{
		//	sample k's pixel position is sequence point k - 1
		Sampler a(seq, 5, 6, 3), b(seq, 5, 6, 0);
		const Pair p = a.camera_pixel(), q = b.pair(e_PairPixel, 2);
		EXPECT_EQ(q.u, p.u);
		EXPECT_EQ(q.v, p.v);
	}
	{
		//	past the pixel's block (2^4): the hash generator, as for the
		//	second light sample
		Sampler a(seq, 5, 6, 40), b(seq, 5, 6, 40);
		const Pair past = a.camera_lens();
		const Pair hashed = b.light(1);
		EXPECT_EQ(hashed.u, past.u);
		EXPECT_EQ(hashed.v, past.v);
	}
	{
		//	the hash order is the generator alone, sample 0 still at the centre
		const optxPixelSequence hash = sequence(e_optxOrderHash, 6, 4);
		Sampler sampler(hash, 5, 6, 9);
		Rng rng(morton2(5, 6), 9, hash.m_Seed);
		const Pair p = sampler.camera_pixel();
		EXPECT_EQ(rng.Next(), p.u);
		EXPECT_EQ(rng.Next(), p.v);
		Sampler zero(hash, 5, 6, 0);
		EXPECT_EQ(0.5f, zero.camera_pixel().u);
	}
}

TEST(optxSampler, FilterWeightsAreMaFilters)
{
	std::unique_ptr<maFilter> filters[e_optxFilterCount];
	filters[e_optxFilterBox].reset(new maBoxFilter);
	filters[e_optxFilterGaussian].reset(new maGaussianFilter);
	filters[e_optxFilterMitchell].reset(new maMitchellFilter);
	filters[e_optxFilterTriangle].reset(new maTriangleFilter);
	filters[e_optxFilterSinc].reset(new maSincFilter);
	filters[e_optxFilterLanczos].reset(new maLanczosFilter);
	filters[e_optxFilterBlackmanHarris].reset(new maBlackmanHarrisFilter);
	filters[e_optxFilterCatmullRom].reset(new maCatmullRomFilter);
	for (unsigned kind = 0; kind < e_optxFilterCount; ++kind)
	{
		for (float width : { 1.0f, 2.0f, 3.5f })
		{
			const optxPixelFilter filter = { kind, width };
			for (int i = 0; i < 11; ++i)
			{
				for (int j = 0; j < 11; ++j)
				{
					//	over the width the samples are spread across
					const float dx = optxf::offset(filter, (i + 0.5f) / 11.0f);
					const float dy = optxf::offset(filter, (j + 0.37f) / 11.0f);
					const float expected = filters[kind]->Evaluate(dx, dy, width, width);
					EXPECT_NEAR(expected, optxf::weight(filter, dx, dy), 1e-5f)
						<< "filter " << kind << " width " << width << " at " << dx << ", " << dy;
				}
			}
		}
	}
	//	a box of width 1 weighs the whole pixel 1
	const optxPixelFilter box = { e_optxFilterBox, 1.0f };
	EXPECT_EQ(1.0f, optxf::weight(box, optxf::offset(box, 0.0f), optxf::offset(box, 0.999f)));
}
