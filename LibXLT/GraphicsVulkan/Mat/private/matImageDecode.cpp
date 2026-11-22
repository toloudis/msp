/*****************************************************************************
**  matImageDecode.cpp
**
**      See matImageDecode.hpp. The dispatch by file type and the helpers
**	the decoders share.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATIMAGEDECODEINTERNAL_HPP
#include "GraphicsVulkan/Mat/private/matImageDecodeInternal.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/Fs/fsFileUtil.hpp"
#endif
#include <string>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace gvk
{
	namespace matDecode
	{
		uint8_t* Make2D(matDecodedImage& o_Image, uint32_t i_Dxgi, uint32_t i_Width, uint32_t i_Height, size_t i_PixelBytes)
		{
			o_Image = matDecodedImage();
			o_Image.m_DxgiFormat = i_Dxgi;
			o_Image.m_Width = i_Width;
			o_Image.m_Height = i_Height;
			const size_t row = i_PixelBytes * i_Width;
			o_Image.m_Subs.push_back({ 0, row, row * i_Height });
			o_Image.m_Bytes.assign(row * i_Height, 0);
			return o_Image.m_Bytes.data();
		}

		//	DirectXMath's XMConvertFloatToHalf, no-intrinsics path
		uint16_t FloatToHalf(float i_Value)
		{
			uint32_t value;
			memcpy(&value, &i_Value, 4);
			const uint32_t sign = (value & 0x80000000U) >> 16U;
			value = value & 0x7FFFFFFFU;
			uint32_t result;
			if (value >= 0x47800000)
			{
				//	too large for a half: infinity, or NaN
				result = 0x7C00U | ((value > 0x7F800000) ? (0x200 | ((value >> 13U) & 0x3FFU)) : 0U);
			}
			else if (value <= 0x33000000U)
			{
				result = 0;
			}
			else if (value < 0x38800000U)
			{
				//	too small for a normalized half: denormalized
				const uint32_t shift = 125U - (value >> 23U);
				value = 0x800000U | (value & 0x7FFFFFU);
				result = value >> (shift + 1);
				const uint32_t s = (value & ((1U << shift) - 1)) != 0;
				result += (result | s) & ((value >> shift) & 1U);
			}
			else
			{
				//	rebias the exponent
				value += 0xC8000000U;
				result = ((value + 0x0FFFU + ((value >> 13U) & 1U)) >> 13U) & 0x7FFFU;
			}
			return (uint16_t)(result | sign);
		}

		//	DirectXMath's XMConvertHalfToFloat, no-intrinsics path
		float HalfToFloat(uint16_t i_Value)
		{
			uint32_t mantissa = (uint32_t)(i_Value & 0x03FF);
			uint32_t exponent = (i_Value & 0x7C00);
			if (exponent == 0x7C00)
			{
				exponent = 0x8f;					// INF/NAN
			}
			else if (exponent != 0)
			{
				exponent = (uint32_t)((i_Value >> 10) & 0x1F);
			}
			else if (mantissa != 0)
			{
				//	denormalized: normalize it
				exponent = 1;
				do
				{
					exponent--;
					mantissa <<= 1;
				} while ((mantissa & 0x0400) == 0);
				mantissa &= 0x03FF;
			}
			else
			{
				exponent = (uint32_t)-112;
			}
			const uint32_t result = (((uint32_t)i_Value & 0x8000) << 16) | ((exponent + 112) << 23) | (mantissa << 13);
			float f;
			memcpy(&f, &result, 4);
			return f;
		}

		uint32_t MakeSrgb(uint32_t i_Dxgi)
		{
			switch (i_Dxgi)
			{
				case 28: return 29;		// R8G8B8A8
				case 71: return 72;		// BC1
				case 74: return 75;		// BC2
				case 77: return 78;		// BC3
				case 87: return 91;		// B8G8R8A8
				case 88: return 93;		// B8G8R8X8
				case 98: return 99;		// BC7
				default: return i_Dxgi;
			}
		}
	}

	bool matDecodeImageMemory(const void* i_pData, size_t i_Size, matImageFileType i_Type, matDecodedImage& o_Image,
							  std::string* o_pError)
	{
		using namespace matDecode;
		std::string error;
		bool ok = false;
		o_Image = matDecodedImage();
		const uint8_t* data = static_cast<const uint8_t*>(i_pData);
		if (!data || i_Size == 0)
		{
			error = "the file is empty";
		}
		else
		{
			switch (i_Type)
			{
				case matImageFileType::e_PNG:
				case matImageFileType::e_JPG:
				case matImageFileType::e_BMP:
				case matImageFileType::e_GIF:	ok = DecodeStb(data, i_Size, i_Type, o_Image, error); break;
				case matImageFileType::e_TGA:	ok = DecodeTga(data, i_Size, o_Image, error); break;
				case matImageFileType::e_TIFF:	ok = DecodeTiff(data, i_Size, o_Image, error); break;
				case matImageFileType::e_DDS:	ok = DecodeDds(data, i_Size, o_Image, error); break;
				case matImageFileType::e_HDR:	ok = DecodeHdr(data, i_Size, o_Image, error); break;
				case matImageFileType::e_WMP:
					error = "JPEG XR (.wdp, .jxr, .hdp) images are not supported by the Vulkan backend";
					break;
				default:
					error = "unknown image file type";
					break;
			}
		}
		if (!ok)
		{
			o_Image = matDecodedImage();
			if (error.empty())
			{
				error = "the image could not be decoded";
			}
			if (o_pError)
			{
				*o_pError = error;
			}
		}
		return ok;
	}

	bool matDecodeImage(const fsLocator& i_File, matImageFileType i_Type, matDecodedImage& o_Image, std::string* o_pError)
	{
		std::string name;
		fsFileUtil::LocatorToUnicodeString(i_File, name);
		std::ifstream in(std::filesystem::u8path(name), std::ios::binary);
		if (!in)
		{
			o_Image = matDecodedImage();
			if (o_pError)
			{
				*o_pError = std::string("the file could not be opened");
			}
			return false;
		}
		const std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return matDecodeImageMemory(bytes.data(), bytes.size(), i_Type, o_Image, o_pError);
	}
}
