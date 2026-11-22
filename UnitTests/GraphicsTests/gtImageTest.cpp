/*****************************************************************************
**  gtImageTest.cpp
**
**      The parity tests' image metrics, on hand-made images.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtImage.hpp"

#include <gtest/gtest.h>

TEST(gtImage, Metrics)
{
	// 2x2: one pixel off by 3 in green, one off by 1 in alpha
	const uint8_t a[16] = { 10, 20, 30, 255,  10, 20, 30, 255,  10, 20, 30, 255,  10, 20, 30, 255 };
	const uint8_t b[16] = { 10, 20, 30, 255,  10, 23, 30, 255,  10, 20, 30, 254,  10, 20, 30, 255 };
	gtCompareResult r = gtCompareImages(a, b, 2, 2);
	EXPECT_FLOAT_EQ(3 / 255.0f, r.m_MaxErr);
	EXPECT_FLOAT_EQ(1 / 255.0f, r.m_MeanErr);		// (3 + 1) / 4 pixels
	EXPECT_FLOAT_EQ(25.0f, r.m_PctOver);			// 1 of 4 over 1/255
	EXPECT_FALSE(r.m_bPass);						// max 3/255 > 2/255

	EXPECT_TRUE(gtCompareImages(a, b, 2, 2, 3 / 255.0f, 1 / 255.0f, 25.0f).m_bPass);
	EXPECT_FALSE(gtCompareImages(a, b, 2, 2, 3 / 255.0f, 1 / 255.0f, 24.0f).m_bPass);
	EXPECT_TRUE(gtCompareImages(a, a, 2, 2).m_bPass);
	EXPECT_EQ(0.0f, gtCompareImages(a, a, 2, 2).m_MaxErr);
}
