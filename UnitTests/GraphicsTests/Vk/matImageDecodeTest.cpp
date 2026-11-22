/*****************************************************************************
**  matImageDecodeTest.cpp
**
**      matImageDecode (stb_image, libtiff, DirectXTex's DDS reading ported,
**      GraphicsDX11's RGBE reader) against what GraphicsDX11 gets from
**      DirectXTex and WIC, for every file of gtImageFiles: the same format
**      (or RGBA where DirectXTex gives BGRA, or a named substitute), size,
**      mips, faces and bytes. JPEG decoders differ slightly (WIC's and
**      stb's IDCT and chroma upsampling), so JPEG bytes may differ by the
**      file's tolerance; the measured error is printed. CPU only.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtImageFiles.hpp"
#include "gtTempDir.hpp"

#ifndef GVK_MATIMAGEDECODE_HPP
#include "GraphicsVulkan/Mat/matImageDecode.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/Fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif
#include <string>

#include <gtest/gtest.h>
#include <crtdbg.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>

using namespace gvk;

namespace
{
	matImageFileType file_type(gtImageFile::Kind i_Kind)
	{
		switch (i_Kind)
		{
			case gtImageFile::e_DDS:	return matImageFileType::e_DDS;
			case gtImageFile::e_TGA:	return matImageFileType::e_TGA;
			case gtImageFile::e_HDR:	return matImageFileType::e_HDR;
			case gtImageFile::e_BMP:	return matImageFileType::e_BMP;
			case gtImageFile::e_JPG:	return matImageFileType::e_JPG;
			case gtImageFile::e_PNG:	return matImageFileType::e_PNG;
			case gtImageFile::e_TIFF:	return matImageFileType::e_TIFF;
			case gtImageFile::e_GIF:	return matImageFileType::e_GIF;
		}
		return matImageFileType::e_Unknown;
	}

	std::vector<uint8_t> read_file(const std::filesystem::path& i_Path)
	{
		std::ifstream in(i_Path, std::ios::binary);
		return std::vector<uint8_t>((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	}

	//	One image in a common shape: subresources layer-major then mip, each
	//	rows tightly packed (volume slices one after another)
	struct Flat
	{
		uint32_t m_Format = 0, m_W = 0, m_H = 0, m_D = 1, m_Mips = 1, m_Layers = 1;
		bool m_bCube = false;
		std::vector<std::vector<uint8_t>> m_Subs;
	};

	size_t row_bytes(uint32_t i_Format, uint32_t i_W)
	{
		const vkdFormatInfo info = vkdFormatInfoOf(vkdFormatFromDxgi(i_Format));
		return (i_W + info.m_BlockWidth - 1) / info.m_BlockWidth * info.m_BlockBytes;
	}

	uint32_t row_count(uint32_t i_Format, uint32_t i_H)
	{
		const vkdFormatInfo info = vkdFormatInfoOf(vkdFormatFromDxgi(i_Format));
		return (i_H + info.m_BlockHeight - 1) / info.m_BlockHeight;
	}

	Flat flatten(const DirectX::ScratchImage& i_Image)
	{
		const DirectX::TexMetadata& m = i_Image.GetMetadata();
		Flat f;
		f.m_Format = (uint32_t)m.format;
		f.m_W = (uint32_t)m.width;
		f.m_H = (uint32_t)m.height;
		f.m_D = (uint32_t)m.depth;
		f.m_Mips = (uint32_t)m.mipLevels;
		f.m_Layers = (uint32_t)m.arraySize;
		f.m_bCube = m.IsCubemap();
		for (size_t layer = 0; layer < m.arraySize; layer++)
		{
			for (size_t mip = 0; mip < m.mipLevels; mip++)
			{
				std::vector<uint8_t> sub;
				const size_t depth = std::max<size_t>(1, m.depth >> mip);
				for (size_t slice = 0; slice < depth; slice++)
				{
					const DirectX::Image* img = i_Image.GetImage(mip, layer, slice);
					const size_t row = std::min(img->rowPitch, row_bytes(f.m_Format, (uint32_t)img->width));
					const uint32_t rows = row_count(f.m_Format, (uint32_t)img->height);
					for (uint32_t y = 0; y < rows; y++)
					{
						sub.insert(sub.end(), img->pixels + y * img->rowPitch, img->pixels + y * img->rowPitch + row);
					}
				}
				f.m_Subs.push_back(sub);
			}
		}
		return f;
	}

	Flat flatten(const matDecodedImage& i_Image)
	{
		Flat f;
		f.m_Format = i_Image.m_DxgiFormat;
		f.m_W = i_Image.m_Width;
		f.m_H = i_Image.m_Height;
		f.m_D = i_Image.m_Depth;
		f.m_Mips = i_Image.m_Mips;
		f.m_Layers = i_Image.m_Layers;
		f.m_bCube = i_Image.m_bCube;
		for (uint32_t layer = 0; layer < i_Image.m_Layers; layer++)
		{
			for (uint32_t mip = 0; mip < i_Image.m_Mips; mip++)
			{
				const matDecodedImage::Sub& s = i_Image.m_Subs[layer * i_Image.m_Mips + mip];
				const uint32_t w = std::max(1u, i_Image.m_Width >> mip), h = std::max(1u, i_Image.m_Height >> mip);
				const uint32_t d = std::max(1u, i_Image.m_Depth >> mip);
				const size_t row = row_bytes(f.m_Format, w);
				const uint32_t rows = row_count(f.m_Format, h);
				std::vector<uint8_t> sub;
				for (uint32_t slice = 0; slice < d; slice++)
				{
					for (uint32_t y = 0; y < rows; y++)
					{
						const size_t at = s.m_Offset + slice * s.m_SlicePitch + y * s.m_RowPitch;
						if (at + row > i_Image.m_Bytes.size())
						{
							ADD_FAILURE() << "subresource " << layer << "/" << mip << " runs past the image bytes";
							return f;
						}
						sub.insert(sub.end(), i_Image.m_Bytes.begin() + at, i_Image.m_Bytes.begin() + at + row);
					}
				}
				f.m_Subs.push_back(sub);
			}
		}
		return f;
	}

	//	The BGRA/RGBA allowance: 8-bit four-channel images are compared in
	//	RGBA order; X8 formats (DirectXTex keeps them, Vulkan has none) read
	//	alpha as 1, as D3D samples them
	void to_rgba(Flat& io_Image)
	{
		bool swap = false, opaque = false;
		switch (io_Image.m_Format)
		{
			case 87: io_Image.m_Format = 28; swap = true; break;					// B8G8R8A8_UNORM
			case 91: io_Image.m_Format = 29; swap = true; break;					// B8G8R8A8_UNORM_SRGB
			case 88: io_Image.m_Format = 28; swap = true; opaque = true; break;		// B8G8R8X8_UNORM
			case 93: io_Image.m_Format = 29; swap = true; opaque = true; break;		// B8G8R8X8_UNORM_SRGB
			default: return;
		}
		for (std::vector<uint8_t>& sub : io_Image.m_Subs)
		{
			for (size_t i = 0; i + 4 <= sub.size(); i += 4)
			{
				if (swap)
				{
					std::swap(sub[i], sub[i + 2]);
				}
				if (opaque)
				{
					sub[i + 3] = 255;
				}
			}
		}
	}

	//	Largest byte difference (8-bit formats) or -1 when sizes differ
	int max_difference(const std::vector<uint8_t>& i_A, const std::vector<uint8_t>& i_B)
	{
		if (i_A.size() != i_B.size())
		{
			return -1;
		}
		int worst = 0;
		for (size_t i = 0; i < i_A.size(); i++)
		{
			worst = std::max(worst, std::abs((int)i_A[i] - (int)i_B[i]));
		}
		return worst;
	}

	void expect_same(const gtImageFile& i_File, const DirectX::ScratchImage& i_Reference, const matDecodedImage& i_Decoded)
	{
		Flat ref = flatten(i_Reference);
		Flat got = flatten(i_Decoded);
		to_rgba(ref);
		to_rgba(got);
		const char* substitution = nullptr;
		vkdFormatFromDxgi(i_Decoded.m_DxgiFormat, &substitution);
		EXPECT_EQ(ref.m_Format, got.m_Format) << i_File.m_Name << ": DXGI format";
		EXPECT_TRUE(vkdFormatFromDxgi(got.m_Format) != VK_FORMAT_UNDEFINED) << i_File.m_Name << ": no Vulkan format for " << got.m_Format;
		EXPECT_EQ(ref.m_W, got.m_W) << i_File.m_Name;
		EXPECT_EQ(ref.m_H, got.m_H) << i_File.m_Name;
		EXPECT_EQ(ref.m_D, got.m_D) << i_File.m_Name;
		EXPECT_EQ(ref.m_Mips, got.m_Mips) << i_File.m_Name;
		EXPECT_EQ(ref.m_Layers, got.m_Layers) << i_File.m_Name;
		EXPECT_EQ(ref.m_bCube, got.m_bCube) << i_File.m_Name;
		if (ref.m_Format != got.m_Format || ref.m_Subs.size() != got.m_Subs.size())
		{
			return;
		}
		int worst = 0;
		for (size_t s = 0; s < ref.m_Subs.size(); s++)
		{
			const int diff = (i_File.m_Tolerance > 0) ? max_difference(ref.m_Subs[s], got.m_Subs[s])
													  : (ref.m_Subs[s] == got.m_Subs[s] ? 0 : (ref.m_Subs[s].size() == got.m_Subs[s].size() ? 256 : -1));
			if (diff < 0 || diff > i_File.m_Tolerance)
			{
				ADD_FAILURE() << i_File.m_Name << ": subresource " << s << (diff < 0 ? " has a different size" : " differs")
							  << (i_File.m_Tolerance > 0 ? " (max " + std::to_string(diff) + "/255)" : "");
				return;
			}
			worst = std::max(worst, diff);
		}
		if (i_File.m_Tolerance > 0)
		{
			std::printf("%s: max difference %d/255 (WIC against stb_image)\n", i_File.m_Name.c_str(), worst);
		}
	}

	class matImageDecodeTest : public ::testing::Test
	{
		protected:
			void SetUp() override
			{
				m_Files = gtWriteImageFiles(m_Dir.GetPath());
			}

			gtTempDir				m_Dir;
			std::vector<gtImageFile> m_Files;
	};
}

TEST_F(matImageDecodeTest, DecodeMatchesDirectXTex)
{
	int compared = 0;
	for (const gtImageFile& file : m_Files)
	{
		if (file.m_bLegacyDds)
		{
			continue;		// LegacyDdsConverted
		}
		DirectX::ScratchImage reference;
		std::string error;
		ASSERT_TRUE(gtLoadReference(file, reference, error)) << error;
		matDecodedImage decoded;
		std::string decode_error;
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(file.m_Path.u8string(), locator);
		ASSERT_TRUE(matDecodeImage(locator, file_type(file.m_Kind), decoded, &decode_error))
			<< file.m_Name << ": " << decode_error;
		expect_same(file, reference, decoded);
		//	From memory, the same
		const std::vector<uint8_t> bytes = read_file(file.m_Path);
		matDecodedImage from_memory;
		ASSERT_TRUE(matDecodeImageMemory(bytes.data(), bytes.size(), file_type(file.m_Kind), from_memory, nullptr)) << file.m_Name;
		EXPECT_TRUE(from_memory.m_Bytes == decoded.m_Bytes) << file.m_Name << ": file and memory decodes differ";
		compared++;
	}
	EXPECT_GT(compared, 60);
}

TEST_F(matImageDecodeTest, LegacyDdsConverted)
{
	int compared = 0;
	for (const gtImageFile& file : m_Files)
	{
		if (!file.m_bLegacyDds)
		{
			continue;
		}
		DirectX::ScratchImage reference;
		std::string error;
		ASSERT_TRUE(gtLoadReference(file, reference, error)) << error;
		const std::vector<uint8_t> bytes = read_file(file.m_Path);
		matDecodedImage decoded;
		std::string decode_error;
		ASSERT_TRUE(matDecodeImageMemory(bytes.data(), bytes.size(), matImageFileType::e_DDS, decoded, &decode_error))
			<< file.m_Name << ": " << decode_error;
		expect_same(file, reference, decoded);
		compared++;
	}
	EXPECT_GT(compared, 40);
}

TEST(matImageDecode, WmpUnsupported)
{
	const uint8_t c_JpegXr[] = { 0x49, 0x49, 0xBC, 0x01, 0x08, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	matDecodedImage decoded;
	std::string error;
	EXPECT_FALSE(matDecodeImageMemory(c_JpegXr, sizeof(c_JpegXr), matImageFileType::e_WMP, decoded, &error));
	const std::string message = error;
	EXPECT_NE(std::string::npos, message.find("JPEG XR")) << message;
}

TEST_F(matImageDecodeTest, TruncatedFilesFail)
{
	for (const gtImageFile& file : m_Files)
	{
		const std::vector<uint8_t> whole = read_file(file.m_Path);
		const matImageFileType type = file_type(file.m_Kind);
		matDecodedImage full;
		ASSERT_TRUE(matDecodeImageMemory(whole.data(), whole.size(), type, full, nullptr)) << file.m_Name;
		const size_t cuts[] = { 0, 16, whole.size() / 2, whole.size() - 1 };
		for (size_t cut : cuts)
		{
			//	An exactly sized heap copy, so a read past the end hits the
			//	debug heap's guard bytes rather than the rest of the file
			std::unique_ptr<uint8_t[]> part(new uint8_t[std::max<size_t>(cut, 1)]);
			memcpy(part.get(), whole.data(), cut);
			matDecodedImage decoded;
			std::string error;
			const bool ok = matDecodeImageMemory(part.get(), cut, type, decoded, &error);
			if (cut + 1 == whole.size() && ok)
			{
				//	Some formats end with bytes no decoder needs (a TGA footer,
				//	PNG's IEND CRC, BMP row padding): then the image must be whole
				EXPECT_TRUE(decoded.m_Bytes == full.m_Bytes) << file.m_Name << " cut by one byte decoded differently";
			}
			else
			{
				EXPECT_FALSE(ok) << file.m_Name << " cut at " << cut << " of " << whole.size() << " decoded";
				EXPECT_FALSE(error.empty()) << file.m_Name << " cut at " << cut << ": no error";
			}
			EXPECT_TRUE(_CrtCheckMemory()) << file.m_Name << " cut at " << cut;
		}
		//	A corrupted header: the first bytes of a wrong kind of file
		std::vector<uint8_t> corrupt = whole;
		for (size_t i = 0; i < std::min<size_t>(12, corrupt.size()); i++)
		{
			corrupt[i] ^= 0x5A;
		}
		matDecodedImage decoded;
		EXPECT_FALSE(matDecodeImageMemory(corrupt.data(), corrupt.size(), type, decoded, nullptr)) << file.m_Name << " with a corrupted header";
		EXPECT_TRUE(_CrtCheckMemory()) << file.m_Name;
	}
}
