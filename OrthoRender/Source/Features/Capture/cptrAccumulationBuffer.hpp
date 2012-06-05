/*****************************************************************************
**  cptrAccumulationBuffer.hpp
**
**      The Capture mode
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_ACCUMULATIONBUFFER_HPP
#error cptrAccumulationBuffer.hpp multiply included
#endif
#define CPTR_ACCUMULATIONBUFFER_HPP

class g2dImage;
struct g2dPixelR16G16B16;
struct g2dPixelR8G8B8;

class cptrAccumulationBuffer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cptrAccumulationBuffer();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cptrAccumulationBuffer(int i_w, int i_h, int i_nSamples);

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
	void Add(g2dPixelR8G8B8* i_Buffer);

	//--------------------------------------------------------------------
	// add pixels into buffer
	//--------------------------------------------------------------------
	void Add(cptrAccumulationBuffer* i_Buffer);

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
	g2dPixelR16G16B16* m_AccumulationBuffer;

	// "capture" the averaged buffer to this image
	g2dImage* m_pAccumImg;
};

