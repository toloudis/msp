/*****************************************************************************
**  cptrAccumulationBuffer.hpp
**
**      The Capture mode
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_ACCUMULATIONBUFFER_HPP
#error cptrAccumulationBuffer.hpp multiply included
#endif
#define CPTR_ACCUMULATIONBUFFER_HPP

#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif

class g2dImage;

class cptrAccumulationBuffer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cptrAccumulationBuffer();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cptrAccumulationBuffer(int i_w, int i_h, int i_nSamples, const g2dPFD& i_CapturePFD);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cptrAccumulationBuffer();

	//--------------------------------------------------------------------
	// zero out the buffer
	//--------------------------------------------------------------------
	void Clear();

	//--------------------------------------------------------------------
	// add pixels into buffer
	//--------------------------------------------------------------------
	void Add(g2dImage* i_pImage, float i_Weight = 1);

	//--------------------------------------------------------------------
	// add pixels into buffer
	//--------------------------------------------------------------------
	void Add(cptrAccumulationBuffer* i_Buffer, float i_Weight = 1);

	//--------------------------------------------------------------------
	// divide buffer by number of samples and store into g2dImage
	//--------------------------------------------------------------------
	void Capture();

	//--------------------------------------------------------------------
	// IMPORTANT! to be called after Capture()
	//--------------------------------------------------------------------
	g2dImage* GetAsImage();

private:
	int m_nSamples;

	int m_Width, m_Height;

	// combine several images together in this buffer
	g2dPixelRGBA32F* m_AccumulationBuffer;

	// "temp" storage for incoming image pixels
	g2dPixelRGBA32F* m_IncomingPixels;

	// "capture" the averaged buffer to this image
	g2dPFD m_PFD;
	g2dImage* m_pAccumImg;
};

