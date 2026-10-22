/****************************************************************************\
**  g2dImageSaveUtil.hpp
**
**  Image file writers shared by the graphics backends. g2dImageSave's
**  implementations (GraphicsDX11's, GraphicsVulkan's) pick the format with
**  PickFormat and write it with Write; only TIFF is left to them.
**
**  - PNG, BMP, JPG, TGA, HDR: stb_image_write. PNG and BMP have alpha for
**    sources with alpha; JPG at quality 90; TGA uncompressed; HDR is
**    Radiance RGBE (RLE)
**  - PPM (binary P6) and PFM (RGB "PF", little-endian): written here
**    (stb_image_write has neither)
**  - EXR: g2dScanlineOpenEXRUtil (OpenEXR)
**  - TIFF, DDS, DIB: not written here (Write throws
**    g2dUnknownImageFileTypeX); TIFF is the backend's
**
**  Pixels are read through g2dImage::Lock with the image's g2dPFD: 8-bit
**  RGBA (e_Color), BGRX (e_RColor), grey (e_Luminance8), 16-bit RGBA
**  (e_RGBA16UInt), and the float formats (e_RGBA16f, e_RGBA32f, e_Float16,
**  e_Float32); any other format throws g2dImageSaveX. The 8-bit files
**  (PNG, BMP, JPG, TGA, PPM) take float sources through the sRGB curve
**  (alpha linear, both clamped), as DirectXTex and WIC did; the float files
**  (HDR, PFM, EXR) take integer sources as value / max.
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
	//	Writes the image as i_Type (see the top of the file). Throws
	//	g2dUnknownImageFileTypeX for TIFF, DDS and DIB, and g2dImageSaveX
	//	for a pixel format it can't read or a file it can't write
	//--------------------------------------------------------------------
	void Write(const fsLocator& i_FileName, const g2dImage* i_pImage, FileType i_Type);

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
}
