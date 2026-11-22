/*****************************************************************************
**  gtImage.cpp
**
**      see gtImage.hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "gtImage.hpp"
#include "gtPlatform.hpp"

//	static: ImportExport (glTF, gltfMaterialImportTest) links its own stb_image_write
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include <stb_image_write.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

std::string gtCompareResult::Describe() const
{
	char text[128];
	snprintf(text, sizeof(text), "max %.2f/255, mean %.3f/255, %.3f%% over", m_MaxErr * 255, m_MeanErr * 255, m_PctOver);
	return text;
}

gtCompareResult gtCompareImages(const uint8_t* i_pA, const uint8_t* i_pB, int i_Width, int i_Height,
								float i_MaxErr, float i_PixelErr, float i_MaxPctOver)
{
	gtCompareResult result;
	const int pixels = i_Width * i_Height;
	double sum = 0;
	int over = 0;
	for (int p = 0; p < pixels; p++)
	{
		int err = 0;
		for (int c = 0; c < 4; c++)
		{
			err = std::max(err, abs((int)i_pA[p * 4 + c] - (int)i_pB[p * 4 + c]));
		}
		const float e = err / 255.0f;
		result.m_MaxErr = std::max(result.m_MaxErr, e);
		sum += e;
		over += (e > i_PixelErr + 1e-6f) ? 1 : 0;
	}
	result.m_MeanErr = pixels ? (float)(sum / pixels) : 0;
	result.m_PctOver = pixels ? 100.0f * over / pixels : 0;
	result.m_bPass = result.m_MaxErr <= i_MaxErr + 1e-6f && result.m_PctOver <= i_MaxPctOver;
	return result;
}

std::string gtSaveComparison(const std::string& i_Test, const uint8_t* i_pDX11, const uint8_t* i_pVulkan, int i_Width, int i_Height)
{
	std::string name = i_Test;
	for (char& c : name)
	{
		if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|')
		{
			c = '_';
		}
	}
	const std::filesystem::path dir = std::filesystem::path(gtPlatform::ExeDir()) / "GraphicsTestsImages" / name;
	std::error_code ec;
	std::filesystem::create_directories(dir, ec);

	//	the error, x16 (an error of 1/255 shows as 16/255), opaque
	std::vector<uint8_t> diff((size_t)i_Width * i_Height * 4);
	for (int p = 0; p < i_Width * i_Height; p++)
	{
		int err = 0;
		for (int c = 0; c < 4; c++)
		{
			err = std::max(err, abs((int)i_pDX11[p * 4 + c] - (int)i_pVulkan[p * 4 + c]));
		}
		const uint8_t v = (uint8_t)std::min(255, err * 16);
		diff[p * 4 + 0] = diff[p * 4 + 1] = diff[p * 4 + 2] = v;
		diff[p * 4 + 3] = 255;
	}
	stbi_write_png((dir / "dx11.png").u8string().c_str(), i_Width, i_Height, 4, i_pDX11, i_Width * 4);
	stbi_write_png((dir / "vulkan.png").u8string().c_str(), i_Width, i_Height, 4, i_pVulkan, i_Width * 4);
	stbi_write_png((dir / "diff.png").u8string().c_str(), i_Width, i_Height, 4, diff.data(), i_Width * 4);
	return dir.u8string();
}
