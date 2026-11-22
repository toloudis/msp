/*****************************************************************************
**  matImageDecode.hpp
**
**      Decodes a texture file into a CPU image the way GraphicsDX11's
**	loaders (DirectXTex, and WIC through it) do, without either: the same
**	DXGI format wherever it changes what a shader samples (grey stays one
**	channel, sRGB-tagged files get the _SRGB format, 16-bit stays 16-bit,
**	palettes and 24-bit RGB become RGBA8 with alpha 1), the same size,
**	mips, faces and slices, and the same texels. Four-channel 8-bit images
**	may come back as RGBA where DirectXTex gives BGRA.
**
**	| Files                   | Decoder                                            |
**	|-------------------------|----------------------------------------------------|
**	| PNG, JPEG, BMP, GIF     | stb_image (24/32-bit BMPs read directly), plus the  |
**	|                         | chunk / EXIF / header reading                      |
**	|                         | that decides the format as WIC does                 |
**	| TIFF                    | libtiff                                            |
**	| TGA                     | a port of DirectXTex's TGA reader                  |
**	| DDS                     | a port of DirectXTex's DDS reader, legacy (D3D9)   |
**	|                         | formats converted as DirectXTex converts them, and |
**	|                         | GraphicsDX11's own CxV8U8 conversion               |
**	| HDR (Radiance RGBE)     | GraphicsDX11's RGBE reader, on memory; halves      |
**	| WMP (JPEG XR)           | not supported: the decode fails                    |
**
**	A truncated or corrupt file fails the decode (with the reason in
**	o_pError); no decoder reads outside the bytes it is given.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_MATIMAGEDECODE_HPP
#error matImageDecode.hpp multiply included
#endif
#define GVK_MATIMAGEDECODE_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>

class fsLocator;

namespace gvk
{
	struct matDecodedImage
	{
		uint32_t	m_DxgiFormat = 0;		// what DirectXTex would report (DXGI_FORMAT value), or its RGBA counterpart
		uint32_t	m_Width = 0;
		uint32_t	m_Height = 0;
		uint32_t	m_Depth = 1;
		uint32_t	m_Mips = 1;
		uint32_t	m_Layers = 1;			// cube faces count as layers
		bool		m_bCube = false;

		//	Where each subresource is in m_Bytes: layer-major, then mip
		//	(m_Subs[layer * m_Mips + mip]); a 3D mip's slices follow each
		//	other m_SlicePitch apart. Rows are blocks for compressed formats.
		struct Sub
		{
			size_t	m_Offset;
			size_t	m_RowPitch;
			size_t	m_SlicePitch;
		};
		std::vector<Sub>		m_Subs;
		std::vector<uint8_t>	m_Bytes;
	};

	//	GraphicsDX11's pick_format kinds (matTextureLoaderWin), plus HDR and GIF
	enum class matImageFileType { e_DDS, e_TGA, e_HDR, e_BMP, e_JPG, e_PNG, e_TIFF, e_GIF, e_WMP, e_Unknown };

	bool matDecodeImage(const fsLocator& i_File, matImageFileType i_Type, matDecodedImage& o_Image, std::string* o_pError);
	//	LoadFrom...Memory's counterpart
	bool matDecodeImageMemory(const void* i_pData, size_t i_Size, matImageFileType i_Type, matDecodedImage& o_Image,
							  std::string* o_pError);
}
