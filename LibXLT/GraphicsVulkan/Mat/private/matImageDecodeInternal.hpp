/*****************************************************************************
**  matImageDecodeInternal.hpp
**
**      What the matImageDecode*.cpp decoders share: one entry point per
**	decoder (each checks every read against i_Size and says why it failed
**	in o_Error), the DXGI format numbers they make, and small helpers.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_MATIMAGEDECODEINTERNAL_HPP
#error matImageDecodeInternal.hpp multiply included
#endif
#define GVK_MATIMAGEDECODEINTERNAL_HPP

#ifndef GVK_MATIMAGEDECODE_HPP
#include "GraphicsVulkan/Mat/matImageDecode.hpp"
#endif

#include <cstring>
#include <string>

namespace gvk
{
	namespace matDecode
	{
		//	DXGI_FORMAT values (no dxgiformat.h in GraphicsVulkan)
		enum Dxgi : uint32_t
		{
			e_R32G32B32A32_FLOAT = 2,
			e_R16G16B16A16_FLOAT = 10,
			e_R16G16B16A16_UNORM = 11,
			e_R8G8B8A8_UNORM = 28,
			e_R8G8B8A8_UNORM_SRGB = 29,
			e_R32_FLOAT = 41,
			e_R16_UNORM = 56,
			e_R8_UNORM = 61,
			e_B5G6R5_UNORM = 85,
			e_B5G5R5A1_UNORM = 86,
			e_B8G8R8A8_UNORM = 87,
			e_B8G8R8X8_UNORM = 88,
			e_B8G8R8A8_UNORM_SRGB = 91,
			e_B8G8R8X8_UNORM_SRGB = 93,
		};

		bool DecodeStb(const uint8_t* i_pData, size_t i_Size, matImageFileType i_Type, matDecodedImage& o_Image, std::string& o_Error);
		bool DecodeTga(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, std::string& o_Error);
		bool DecodeTiff(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, std::string& o_Error);
		bool DecodeDds(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, std::string& o_Error);
		bool DecodeHdr(const uint8_t* i_pData, size_t i_Size, matDecodedImage& o_Image, std::string& o_Error);

		//	One 2D subresource of i_Width x i_Height, i_PixelBytes per pixel,
		//	rows tightly packed; returns its bytes
		uint8_t* Make2D(matDecodedImage& o_Image, uint32_t i_Dxgi, uint32_t i_Width, uint32_t i_Height, size_t i_PixelBytes);

		//	DirectXMath's XMConvertFloatToHalf (round to nearest even) and XMConvertHalfToFloat
		uint16_t FloatToHalf(float i_Value);
		float HalfToFloat(uint16_t i_Value);

		//	DirectXTex's MakeSRGB: the _SRGB format of i_Dxgi, or i_Dxgi if it has none
		uint32_t MakeSrgb(uint32_t i_Dxgi);

		inline uint16_t Read16(const uint8_t* i_p)	{ return (uint16_t)(i_p[0] | (i_p[1] << 8)); }
		inline uint32_t Read32(const uint8_t* i_p)	{ return (uint32_t)i_p[0] | ((uint32_t)i_p[1] << 8) | ((uint32_t)i_p[2] << 16) | ((uint32_t)i_p[3] << 24); }
		inline uint16_t Read16BE(const uint8_t* i_p) { return (uint16_t)((i_p[0] << 8) | i_p[1]); }
		inline uint32_t Read32BE(const uint8_t* i_p) { return ((uint32_t)i_p[0] << 24) | ((uint32_t)i_p[1] << 16) | ((uint32_t)i_p[2] << 8) | i_p[3]; }
	}
}
