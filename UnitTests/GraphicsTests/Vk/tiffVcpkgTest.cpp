/*****************************************************************************
**  tiffVcpkgTest.cpp
**
**      libtiff comes from vcpkg (GraphicsDX11's and GraphicsVulkan's
**      manifests, pinned to one version so MachStudio loads one tiff.dll):
**      8-bit RGBA and 32-bit float images survive a write and read back,
**      and the library is the pinned version.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtTempDir.hpp"

#include <gtest/gtest.h>
#include <tiffio.h>

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace
{
	//	Writes a contiguous RGBA image, one scanline at a time.
	bool WriteTiff(const std::string& i_Path, uint32_t i_Width, uint32_t i_Height,
				   uint16_t i_BitsPerSample, uint16_t i_SampleFormat, const std::vector<uint8_t>& i_Bytes)
	{
		TIFF* tif = TIFFOpen(i_Path.c_str(), "w");
		if (!tif)
			return false;
		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, i_Width);
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, i_Height);
		TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 4);
		TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, i_BitsPerSample);
		TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, i_SampleFormat);
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
		uint16_t extra = EXTRASAMPLE_UNASSALPHA;
		TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &extra);
		TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_ADOBE_DEFLATE);
		const size_t row = size_t(i_Width) * 4 * (i_BitsPerSample / 8);
		bool ok = true;
		for (uint32_t y = 0; y < i_Height && ok; ++y)
			ok = TIFFWriteScanline(tif, const_cast<uint8_t*>(i_Bytes.data() + y * row), y, 0) == 1;
		TIFFClose(tif);
		return ok;
	}

	std::vector<uint8_t> ReadTiff(const std::string& i_Path, uint32_t& o_Width, uint32_t& o_Height,
								  uint16_t& o_BitsPerSample, uint16_t& o_SampleFormat)
	{
		std::vector<uint8_t> bytes;
		TIFF* tif = TIFFOpen(i_Path.c_str(), "r");
		if (!tif)
			return bytes;
		o_SampleFormat = SAMPLEFORMAT_UINT;
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &o_Width);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &o_Height);
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &o_BitsPerSample);
		TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &o_SampleFormat);
		const tmsize_t row = TIFFScanlineSize(tif);
		bytes.resize(size_t(row) * o_Height);
		for (uint32_t y = 0; y < o_Height; ++y)
			if (TIFFReadScanline(tif, bytes.data() + y * row, y, 0) != 1)
			{
				bytes.clear();
				break;
			}
		TIFFClose(tif);
		return bytes;
	}
}

TEST(tiffVcpkg, TiffRoundTrip)
{
	gtTempDir dir;
	const uint32_t w = 13, h = 7;

	std::vector<uint8_t> rgba8(w * h * 4);
	for (size_t i = 0; i < rgba8.size(); ++i)
		rgba8[i] = uint8_t(i * 37 + 11);
	std::vector<float> rgba32f(w * h * 4);
	for (size_t i = 0; i < rgba32f.size(); ++i)
		rgba32f[i] = float(i) * 0.37f - 5.0f;
	std::vector<uint8_t> floatBytes(rgba32f.size() * sizeof(float));
	std::memcpy(floatBytes.data(), rgba32f.data(), floatBytes.size());

	struct Case { const char* m_Name; uint16_t m_Bits; uint16_t m_Format; const std::vector<uint8_t>* m_pBytes; };
	const Case cases[] = {
		{ "rgba8.tif", 8, SAMPLEFORMAT_UINT, &rgba8 },
		{ "rgba32f.tif", 32, SAMPLEFORMAT_IEEEFP, &floatBytes },
	};
	for (const Case& c : cases)
	{
		const std::string path = (dir.GetPath() / c.m_Name).string();
		ASSERT_TRUE(WriteTiff(path, w, h, c.m_Bits, c.m_Format, *c.m_pBytes)) << c.m_Name;
		uint32_t rw = 0, rh = 0;
		uint16_t bits = 0, format = 0;
		std::vector<uint8_t> back = ReadTiff(path, rw, rh, bits, format);
		EXPECT_EQ(w, rw) << c.m_Name;
		EXPECT_EQ(h, rh) << c.m_Name;
		EXPECT_EQ(c.m_Bits, bits) << c.m_Name;
		EXPECT_EQ(c.m_Format, format) << c.m_Name;
		EXPECT_TRUE(back == *c.m_pBytes) << c.m_Name << ": bytes differ after the round trip";
	}
}

TEST(tiffVcpkg, TiffVersionPinned)
{
	//	"LIBTIFF, Version 4.7.1\nCopyright ..."; keep in step with the
	//	overrides in the vcpkg manifests.
	const std::string version = TIFFGetVersion();
	EXPECT_NE(std::string::npos, version.find("Version 4.7.1")) << version;
}
