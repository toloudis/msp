/*****************************************************************************
**  gtImage.hpp
**
**      Image comparison for the backend parity tests, with the metrics and
**      default tolerance of Tools/gfxcompare (the GraphicsVulkan design,
**      "Proving parity"): a pixel's error is its largest channel
**      difference; an image passes when the largest error is at most
**      i_MaxErr and at most i_MaxPctOver percent of pixels exceed
**      i_PixelErr. Images are 8-bit RGBA, rows tightly packed.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct gtCompareResult
{
	float	m_MaxErr = 0;		// 0..1
	float	m_MeanErr = 0;		// 0..1
	float	m_PctOver = 0;		// percent of pixels
	bool	m_bPass = false;

	//	"max 1.00/255, mean 0.012/255, 0.024% over"
	std::string Describe() const;
};

gtCompareResult gtCompareImages(const uint8_t* i_pA, const uint8_t* i_pB, int i_Width, int i_Height,
								float i_MaxErr = 2 / 255.0f, float i_PixelErr = 1 / 255.0f, float i_MaxPctOver = 0.1f);

//	Writes dx11.png, vulkan.png and diff.png (the per-pixel error, scaled up)
//	to <test exe dir>/GraphicsTestsImages/<i_Test>/; returns that directory.
std::string gtSaveComparison(const std::string& i_Test, const uint8_t* i_pDX11, const uint8_t* i_pVulkan, int i_Width, int i_Height);
