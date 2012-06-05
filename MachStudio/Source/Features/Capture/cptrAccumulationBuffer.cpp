/*****************************************************************************
**  cptrAccumulationBuffer.cpp
**
**      see .h
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrAccumulationBuffer.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dImageCreate.hpp"

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrAccumulationBuffer::cptrAccumulationBuffer()
:	m_Width(0), m_Height(0), m_nSamples(0),
	m_AccumulationBuffer(NULL),m_pAccumImg(NULL),
	m_IncomingPixels(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrAccumulationBuffer::cptrAccumulationBuffer(int i_w, int i_h, int i_nSamples, const g2dPFD& i_CapturePFD)
:	m_Width(i_w), m_Height(i_h), m_nSamples(i_nSamples),
	m_AccumulationBuffer(NULL),m_pAccumImg(NULL),m_PFD(i_CapturePFD),
	m_IncomingPixels(NULL)
{
	// array of pixels
	m_AccumulationBuffer = new g2dPixelRGBA32F[m_Width*m_Height];
	m_IncomingPixels = new g2dPixelRGBA32F[m_Width*m_Height];

	m_pAccumImg = g2dImageCreate::Make(m_Width, m_Height, m_PFD);

	// init to 0
	Clear();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrAccumulationBuffer::~cptrAccumulationBuffer()
{
	delete [] m_AccumulationBuffer;
	delete [] m_IncomingPixels;
	delete m_pAccumImg;
}

//--------------------------------------------------------------------
// zero out the buffer
//--------------------------------------------------------------------
void cptrAccumulationBuffer::Clear()
{
	memset(m_AccumulationBuffer, 0, m_Width*m_Height*sizeof(g2dPixelRGBA32F));
}

//--------------------------------------------------------------------
// add pixels into buffer
//--------------------------------------------------------------------
void cptrAccumulationBuffer::Add(g2dImage* i_pImage, float i_Weight /*= 1*/)
{
	DBG_ASSERT((m_IncomingPixels != NULL), "Accumulation pixel buffer is NULL");
	DBG_ASSERT(i_pImage->GetWidth() == m_Width, "Accumulation buffer width mismatch");
	DBG_ASSERT(i_pImage->GetHeight() == m_Height, "Accumulation buffer height mismatch");

	memset(m_IncomingPixels, 0, m_Width*m_Height*sizeof(g2dPixelRGBA32F));

	i_pImage->GetPixels(m_IncomingPixels, m_Width, m_Height);

	int total = m_Width*m_Height;
	int i;
	for (i = 0; i < total; i++)
	{
		m_AccumulationBuffer[i].r += m_IncomingPixels[i].r * i_Weight;
		m_AccumulationBuffer[i].g += m_IncomingPixels[i].g * i_Weight;
		m_AccumulationBuffer[i].b += m_IncomingPixels[i].b * i_Weight;
		m_AccumulationBuffer[i].a += m_IncomingPixels[i].a * i_Weight;
	}
}

//--------------------------------------------------------------------
// add pixels into buffer
//--------------------------------------------------------------------
void cptrAccumulationBuffer::Add(cptrAccumulationBuffer* i_Buffer, float i_Weight /*= 1*/)
{
	DBG_ASSERT((i_Buffer != NULL), "Accumulation buffer is NULL");
	DBG_ASSERT((m_Width == i_Buffer->m_Width), "Accumulation buffer width mismatch");
	DBG_ASSERT((m_Height == i_Buffer->m_Height), "Accumulation buffer height mismatch");
	int total = m_Width*m_Height;
	int i;
	for (i = 0; i < total; i++)
	{
		m_AccumulationBuffer[i].r += i_Buffer->m_AccumulationBuffer[i].r * i_Weight;
		m_AccumulationBuffer[i].g += i_Buffer->m_AccumulationBuffer[i].g * i_Weight;
		m_AccumulationBuffer[i].b += i_Buffer->m_AccumulationBuffer[i].b * i_Weight;
		m_AccumulationBuffer[i].a += i_Buffer->m_AccumulationBuffer[i].a * i_Weight;
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

	g2dPixelRGBA32F curPixel;
	DWORD dwPosition;
	int pixelIndex = 0;

	DBG_ASSERT(m_PFD.GetNumChannels() == 4, "Unrecognized num channels in accumulation image.");
	DBG_ASSERT(m_PFD.GetBitsPerChannel() <= 32, "Unrecognized bits per channel in accumulation image.");

	//convert pixels into correct format.
	// float16, uint8, or float32.
	// assume that 8 bpc is the uint, and 16/32 bpc are float.
	envType::UInt16 pixel16f[4];
	float pixel32f[4];
	BYTE pixel8u[4];

	int bytesPerPixel = m_PFD.BitsPerPixel()/8;

	//scan convert accum buffer 
	for( int y = 0; y < m_Height; y++ ) 
	{ 
		for( int x = 0; x < m_Width; x++ ) 
		{ 
			//grab pixel. note upside down flip here. 
			pixelIndex = x + ((m_Height-1-y) * m_Width);

			curPixel = m_AccumulationBuffer[pixelIndex]; 

			//calculate position for image pixel
			dwPosition = (x * bytesPerPixel) + (y * m_pAccumImg->GetStride()); 

			//set argb color
			// and copy pixel
			switch(m_PFD.GetBitsPerChannel())
			{
			case 8:
				pixel8u[0] = (BYTE)(curPixel.r * 255.0f);
				pixel8u[1] = (BYTE)(curPixel.g * 255.0f);
				pixel8u[2] = (BYTE)(curPixel.b * 255.0f);
				pixel8u[3] = (BYTE)(curPixel.a * 255.0f);		
				DBG_ASSERT(bytesPerPixel == 4, "wrong pixel size");
				memcpy( &pbyBuffer[ dwPosition ], &pixel8u[0] , bytesPerPixel ); 
				break;
			case 16:
				pixel16f[0] = maFunctions::FloatToHalf(curPixel.r);
				pixel16f[1] = maFunctions::FloatToHalf(curPixel.g);
				pixel16f[2] = maFunctions::FloatToHalf(curPixel.b);
				pixel16f[3] = maFunctions::FloatToHalf(curPixel.a);
				DBG_ASSERT(bytesPerPixel == 8, "wrong pixel size");
				memcpy( &pbyBuffer[ dwPosition ], &pixel16f[0] , bytesPerPixel ); 
				break;
			case 32:
				pixel32f[0] = (curPixel.r);
				pixel32f[1] = (curPixel.g);
				pixel32f[2] = (curPixel.b);
				pixel32f[3] = (curPixel.a);
				DBG_ASSERT(bytesPerPixel == 16, "wrong pixel size");
				memcpy( &pbyBuffer[ dwPosition ], &pixel32f[0] , bytesPerPixel ); 
				break;
			};

		} 
	} 

	//unlock the surface 
	m_pAccumImg->Release();
}
