/****************************************************************************\
**  cptrWriteAVI.hpp
**
**      Supplies routines for writing the AVI format
**
**	StudioGPU
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_WRITEAVI_HPP
#error cptrWriteAVI.hpp multiply included
#endif
#define CPTR_WRITEAVI_HPP


//============================================================================
//	forward references
//============================================================================
class fsLocator;
class itString;
class g2dImage;
class g2dWindow;
class captRenderOutputData;


//============================================================================
//============================================================================
namespace cptrWriteAVI
{
	//------------------------------------------------------------------------
	//	Write writes screen in 24-bit BMP format to given filename.
	//
	//	the sample rate is how many input pixels per output pixel
	//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
	//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
	//------------------------------------------------------------------------
	void Write( g2dWindow* i_pWindow, const fsLocator& i_Locator, const int i_Sampling = 1 );

	//------------------------------------------------------------------------
	//	Write writes screen in 24-bit BMP format to given filename.
	//
	//	the sample rate is how many input pixels per output pixel
	//	(a sampling of 2 on a 640x480 image will produce a 320x240 image)
	//	(a sampling of 4 on a 640x480 image will produce a 160x120 image)
	//------------------------------------------------------------------------
	void Write( g2dImage* i_pSurface, const fsLocator& i_Locator, const int i_Sampling = 1 );

	//------------------------------------------------------------------------
	//	BeginCapture/EndCapture() - notify start and stop capturing so that
	//		AVI files can be begun and ended
	//------------------------------------------------------------------------
	void BeginCapture(captRenderOutputData& i_Data);
	void EndCapture();
	
	//------------------------------------------------------------------------
	//	WriteAudio() - write the audio file to the AVI.  This must get
	//	called after the frames are written right before EndCapture().
	//------------------------------------------------------------------------
	void WriteAudio(const fsLocator& i_AudioFile);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void AppendAVI(const itString i_origmovie, const itString i_countdown, const itString i_tempPath);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void set_to_fusion(bool i_isFusionApp);
}
