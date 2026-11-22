/*****************************************************************************
**  effOpacityTest.cpp
**
**      effOpacity::AlphaTestRef: a pass's alpha test reference, raised to
**      the cutoff for a MASK material only. effOpacity::Of is tested on
**      materials in Vk/effPbrVkTest.cpp (the parameters need an effect).
**
**	Extra Large Technology
\****************************************************************************/
#ifndef EFF_OPACITY_HPP
#include "Graphics/Eff/effOpacity.hpp"
#endif

#include <gtest/gtest.h>

#include <cmath>

TEST(effOpacity, AlphaTestRefRaisedOnlyForMask)
{
	const effOpacity none;
	EXPECT_EQ(1.0f / 255.0f, none.AlphaTestRef(1.0f / 255.0f));
	EXPECT_EQ(0.9f, none.AlphaTestRef(0.9f));

	effOpacity mask;
	mask.m_bCutOut = true;
	mask.m_Cutoff = 0.5f;
	const float ref = mask.AlphaTestRef(1.0f / 255.0f);
	EXPECT_EQ(std::nextafter(0.5f, 0.0f), ref);
	// both pass shader forms keep alpha == cutoff: `ref >= alpha` discards,
	// `clip(alpha - ref)` discards below ref
	EXPECT_FALSE(ref >= 0.5f);
	EXPECT_TRUE(ref >= std::nextafter(0.5f, 0.0f));
	// a pass's own higher reference stays
	EXPECT_EQ(0.9f, mask.AlphaTestRef(0.9f));
}
