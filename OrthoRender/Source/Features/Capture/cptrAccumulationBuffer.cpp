/*****************************************************************************
**  cptrAccumulationBuffer.cpp
**
**      see .h
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrAccumulationBuffer.hpp"

#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dImageCreate.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrAccumulationBuffer::cptrAccumulationBuffer()
:	m_Width(0), m_Height(0), m_nSamples(0),
	m_AccumulationBuffer(NULL),m_pAccumImg(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrAccumulationBuffer::cptrAccumulationBuffer(int i_w, int i_h, int i_nSamples)
:	m_Width(i_w), m_Height(i_h), m_nSamples(i_nSamples),
	m_AccumulationBuffer(NULL),m_pAccumImg(NULL)
{
	// array of pixels
	m_AccumulationBuffer = new g2dPixelR16G16B16[m_Width*m_Height];

	g2dPFD pfd(16,8,8,8,0,8,24,8,32);
	m_pAccumImg = g2dImageCreate::Make(m_Width, m_Height, pfd);

	// init to 0
	Clear();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrAccumulationBuffer::~cptrAccumulationBuffer()
{
	delete [] m_AccumulationBuffer;
	delete m_pAccumImg;
}

//--------------------------------------------------------------------
// zero out the buffer
//--------------------------------------------------------------------
void cptrAccumulationBuffer::Clear()
{
	memset(m_AccumulationBuffer, 0, m_Width*m_Height*sizeof(g2dPixelR16G16B16));
}

//--------------------------------------------------------------------
// add pixels into buffer
//--------------------------------------------------------------------
void cptrAccumulationBuffer::Add(g2dPixelR8G8B8* i_Buffer)
{
	int total = m_Width*m_Height;
	int i;
	for (i = 0; i < total; i++)
	{
		// note rgb-bgr here
		m_AccumulationBuffer[i].r += i_Buffer[i].b;
		m_AccumulationBuffer[i].g += i_Buffer[i].g;
		m_AccumulationBuffer[i].b += i_Buffer[i].r;
	}
}

//--------------------------------------------------------------------
// add pixels into buffer
//--------------------------------------------------------------------
void cptrAccumulationBuffer::Add(cptrAccumulationBuffer* i_Buffer)
{
	DBG_ASSERT0((i_Buffer != NULL), "Accumulation buffer is NULL");
	DBG_ASSERT0((m_Width == i_Buffer->m_Width), "Accumulation buffer width mismatch");
	DBG_ASSERT0((m_Height == i_Buffer->m_Height), "Accumulation buffer height mismatch");
	int total = m_Width*m_Height;
	int i;
	for (i = 0; i < total; i++)
	{
		m_AccumulationBuffer[i].r += i_Buffer->m_AccumulationBuffer[i].r;
		m_AccumulationBuffer[i].g += i_Buffer->m_AccumulationBuffer[i].g;
		m_AccumulationBuffer[i].b += i_Buffer->m_AccumulationBuffer[i].b;
	}
}

//--------------------------------------------------------------------
// IMPORTANT! to be called after Capture()
//--------------------------------------------------------------------
g2dImage* cptrAccumulationBuffer::GetAsImage()
{
	return m_pAccumImg;
}

//--------------------------------------------------------------------
// divide buffer by number of samples and store into g2dImage
//--------------------------------------------------------------------
void cptrAccumulationBuffer::Capture()
{
	// dump our accumulated pixels into a ddraw surface

	//lock the surface 
	BYTE* pbyBuffer = ( BYTE* )m_pAccumImg->Lock();

	g2dPixelR16G16B16 curPixel;
	DWORD dwColor;
	DWORD dwPosition;
	int pixelIndex = 0;
	//scan convert accum buffer 
	for( int y = 0; y < m_Height; y++ ) 
	{ 
		for( int x = 0; x < m_Width; x++ ) 
		{ 
			pixelIndex = x + ((m_Height-1-y) * m_Width);

			//grab pixel. note upside down flip here. 
			m_AccumulationBuffer[pixelIndex].r /= m_nSamples;
			m_AccumulationBuffer[pixelIndex].g /= m_nSamples;
			m_AccumulationBuffer[pixelIndex].b /= m_nSamples;

			curPixel = m_AccumulationBuffer[pixelIndex]; 

			//calculate position 
			dwPosition = (x * 4) + (y * m_pAccumImg->GetStride()); 

			//set argb color
			dwColor = (((((255)&0xff)<<24)|(((curPixel.r)&0xff)<<16)|(((curPixel.g)&0xff)<<8)|((curPixel.b)&0xff)));

			//copy pixel 
			memcpy( &pbyBuffer[ dwPosition ], &dwColor , 4 ); 
		} 
	} 

	//unlock the surface 
	m_pAccumImg->Release();
}
