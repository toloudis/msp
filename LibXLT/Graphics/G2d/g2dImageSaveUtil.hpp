/****************************************************************************\
**  g2dImageSaveUtil.hpp
**
**  Image file writers, for every backend: g2dImageSave (images and
**  windows) and matTextureMgr's texture saving write through them.
**
**  - PNG, BMP, JPG, TGA, HDR: stb_image_write. PNG and BMP have alpha for
**    sources with alpha; JPG at quality 90; TGA uncompressed; HDR is
**    Radiance RGBE (RLE)
**  - PPM (binary P6, or P5 for grey) and PFM (little-endian "PF", or "Pf"
**    for grey): written here (stb_image_write has neither)
**  - TIFF: libtiff, in the layout WIC wrote: 8-bit for 8-bit sources,
**    16-bit for the others; LZW
**  - DDS: the pixels as stored, under DirectXTex's header for one 2D image
**  - EXR: g2dScanlineOpenEXRUtil (OpenEXR)
**  - DIB: not written (Write throws g2dUnknownImageFileTypeX)
**
**  Pixels are read through g2dImage::Lock with the image's g2dPFD: 8-bit
**  RGBA (e_Color), BGRX (e_RColor), grey (e_Luminance8), 16-bit RGBA
**  (e_RGBA16UInt), and the float formats (e_RGBA16f, e_RGBA32f, e_Float16,
**  e_Float32); e_GR32f to DDS only. Any other format throws g2dImageSaveX.
**  The 8-bit files (PNG, BMP, JPG, TGA, PPM, 8-bit TIFF) and 16-bit TIFF
**  take float sources through the sRGB curve (alpha linear, both
**  clamped), as DirectXTex and WIC did; the float files (HDR, PFM, EXR)
**  take integer sources as value / max.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef G2D_IMAGESAVEUTIL_HPP
#error g2dImageSaveUtil.hpp multiply included
#endif
#define G2D_IMAGESAVEUTIL_HPP


//============================================================================
//============================================================================
class fsLocator;
class g2dImage;


//============================================================================
//============================================================================
namespace g2dImageSaveUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	enum FileType
	{
		e_BMP,
		e_JPG,
		e_TGA,
		e_PNG,
		e_DDS,
		e_PPM,
		e_DIB,
		e_HDR,
		e_PFM,
		e_EXR,
		e_TIFF
	};

	//--------------------------------------------------------------------
	//	The file type named by the file's extension (any case); throws
	//	g2dUnknownImageFileTypeX for an extension that isn't one of them
	//--------------------------------------------------------------------
	FileType PickFormat(const fsLocator& i_FileName);

	//--------------------------------------------------------------------
	//	Writes the image as i_Type, or as the type its extension names
	//	(see the top of the file). Throws g2dUnknownImageFileTypeX for DIB
	//	or an unknown extension, and g2dImageSaveX for a pixel format it
	//	can't read or a file it can't write
	//--------------------------------------------------------------------
	void Write(const fsLocator& i_FileName, const g2dImage* i_pImage, FileType i_Type);
	void Write(const fsLocator& i_FileName, const g2dImage* i_pImage);

	//--------------------------------------------------------------------
	//	The writers Write uses
	//--------------------------------------------------------------------
	void WritePNG(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void WriteBMP(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void WriteJPG(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void WriteTGA(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void WriteHDR(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void WritePPM(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void WritePFM(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void WriteEXR(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void WriteTIFF(const fsLocator& i_FileName, const g2dImage* i_pImage);
	void WriteDDS(const fsLocator& i_FileName, const g2dImage* i_pImage);
}
