/****************************************************************************\
**  g2dDX11SurfaceUtil.hpp
**
**      g2dDX11SurfaceUtil.hpp is the implementation of routines to extract data
**		from D3D surfaces
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g2d/private/g2dDX11SurfaceUtil.hpp"

#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceLocker::SurfaceLocker(g2dD3D11TexturePtr i_pSurface, bool i_NeedWriteAccess /*=false*/)
: m_pSurface(NULL), m_pIterator(NULL)
{
	D3D11_MAPPED_SUBRESOURCE lockInfo;
	HRESULT op_result = g2dDX11Global::g_pDeviceContext->Map(i_pSurface,
		D3D11CalcSubresource(0,0,1),
		D3D11_MAP_READ,
		0,
		&lockInfo
		);
	if ( !SUCCEEDED(op_result) )
	{
		DBG_ASSERT(false, "Couldn't Lock surface");
	}
	else
	{

		m_pSurface = i_pSurface;	// if successful, set pointer

		// also create iterator!
		m_pIterator = CreateIterator(lockInfo);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceLocker::~SurfaceLocker()
{
	if( m_pSurface )
	{
		delete m_pIterator;

		g2dDX11Global::g_pDeviceContext->Unmap(m_pSurface, D3D11CalcSubresource(0,0,1));
	}
}

SurfaceIterator* SurfaceLocker::CreateIterator(D3D11_MAPPED_SUBRESOURCE& i_LockInfo)
{
	D3D11_TEXTURE2D_DESC surfDesc;
	m_pSurface->GetDesc(&surfDesc);

	g2dPFD src_pfd;
	g2dDX11Global::PFDFromD3DFormat(surfDesc.Format, src_pfd);
	int w = surfDesc.Width;
	int h = surfDesc.Height;

	int nc = src_pfd.GetNumChannels();
	bool isfloat = src_pfd.IsFloat();

	DBG_ASSERT(nc <= 4, "bad num of channels");
	switch(src_pfd.GetBitsPerChannel())
	{
	case 8:
		DBG_ASSERT(nc == 4, "bad capture fmt: 8 bits per channel but not 4 channels");
		DBG_ASSERT(!isfloat, "bad capture fmt: 8 bits per channel but float");
		return new SurfaceIteratorRGBA8U(w,h,i_LockInfo.RowPitch,i_LockInfo.pData,src_pfd);
		break;
	case 16:
		{
		DBG_ASSERT(isfloat, "bad capture fmt: 16 bits per channel but not float");
		switch (nc)
		{
		case 4:
			return new SurfaceIteratorRGBA16F(w,h,i_LockInfo.RowPitch,i_LockInfo.pData,src_pfd);
			break;
		case 2:
			return new SurfaceIteratorRG16F(w,h,i_LockInfo.RowPitch,i_LockInfo.pData,src_pfd);
			break;
		case 1:
			return new SurfaceIteratorR16F(w,h,i_LockInfo.RowPitch,i_LockInfo.pData,src_pfd);
			break;
		}
		}
		break;
	case 32:
		{
		DBG_ASSERT(isfloat, "bad capture fmt: 32 bits per channel but not float");
		switch(nc)
		{
		case 4:
			return new SurfaceIteratorRGBA32F(w,h,i_LockInfo.RowPitch,i_LockInfo.pData,src_pfd);
			break;
		case 2:
			return new SurfaceIteratorRG32F(w,h,i_LockInfo.RowPitch,i_LockInfo.pData,src_pfd);
			break;
		case 1:
			return new SurfaceIteratorR32F(w,h,i_LockInfo.RowPitch,i_LockInfo.pData,src_pfd);
			break;
		}
		}
		break;
	};

	// fall thru: returns a generic surfaceiterator?
	return new SurfaceIterator(w,h,i_LockInfo.RowPitch,i_LockInfo.pData,src_pfd);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceIterator::SurfaceIterator(	int i_Width,
									int i_Height,
									int i_Pitch,
									void* i_pData,
									const g2dPFD& i_PFD)
									//RECT *i_pRect)
:	m_Width(i_Width),
	m_Height(i_Height),
	m_Pitch(i_Pitch),
	m_PFD(i_PFD),
	//m_pRect(i_pRect),
	m_pData((envType::UInt8*)i_pData),
	m_pBase((envType::UInt8*)i_pData)
{
	// calculate the number of components
	//	24-bit should be 3 (RGB) and 32-bit should be 4 (RGBA)
	//
	//m_RGBA_bytes = m_PFD.BitsPerPixel() / m_PFD.NumRedBits();
	m_RGBA_bytes = m_PFD.BitsPerPixel() / 8;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIterator::GetValue(	envType::UInt8 &o_Red,
								envType::UInt8 &o_Green,
								envType::UInt8 &o_Blue,
								envType::UInt8 &o_Alpha) const
{
	static g2dARGBColor color;
	static envType::UInt16 rgb;
	switch( m_PFD.BitsPerPixel() )
	{
		case 16:
		{
			rgb = *(envType::UInt16*) (m_pData);
			color	= m_PFD.MakeScaledColor(rgb);
			o_Blue	= color.GetBlue();
			o_Green = color.GetGreen();
			o_Red	= color.GetRed();
			o_Alpha = color.GetAlpha();
		}
		break;
		case 24:
		{
			o_Red	= *(m_pData+0);
			o_Green = *(m_pData+1);
			o_Blue	= *(m_pData+2);
			o_Alpha = 255;
		}
		break;
		case 32:
		{
			o_Red		= *(m_pData+0);
			o_Green		= *(m_pData+1);
			o_Blue		= *(m_pData+2);
			o_Alpha		= *(m_pData+3);
		}
		break;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIterator::GetValue(	envType::Float32 &o_Red,
								envType::Float32 &o_Green,
								envType::Float32 &o_Blue,
								envType::Float32 &o_Alpha) const
{
	int nc = m_PFD.GetNumChannels();
	bool isfloat = m_PFD.IsFloat();

	DBG_ASSERT(nc <= 4, "bad num of channels");
	switch(m_PFD.GetBitsPerChannel())
	{
	case 8:
		DBG_ASSERT(nc == 4, "bad capture fmt: 8 bits per channel but not 4 channels");
		DBG_ASSERT(!isfloat, "bad capture fmt: 8 bits per channel but float");
		o_Red		= (*(m_pData+0)) / 255.0f;
		o_Green		= (*(m_pData+1)) / 255.0f;
		o_Blue		= (*(m_pData+2)) / 255.0f;
		o_Alpha		= (*(m_pData+3)) / 255.0f;
		break;
	case 16:
		{
		DBG_ASSERT(isfloat, "bad capture fmt: 16 bits per channel but not float");
		envType::UInt16* pHData = (envType::UInt16*)m_pData;
//		float tmp[4];
		switch (nc)
		{
		case 4:
			//::D3DXFloat16To32Array(tmp, (HALF*)pHData, 4);
			//o_Red	 = tmp[0];
			//o_Green	 = tmp[1];
			//o_Blue	 = tmp[2];
			//o_Alpha	 = tmp[3];
			o_Red		= maFunctions::HalfToFloat(*(pHData+0));
			o_Green		= maFunctions::HalfToFloat(*(pHData+1));
			o_Blue		= maFunctions::HalfToFloat(*(pHData+2));
			o_Alpha		= maFunctions::HalfToFloat(*(pHData+3));
			break;
		case 2:
			//::D3DXFloat16To32Array(tmp, (HALF*)pHData, 2);
			//o_Red		= tmp[0];
			//o_Green		= tmp[1];
			o_Red		= maFunctions::HalfToFloat(*(pHData+0));
			o_Green		= maFunctions::HalfToFloat(*(pHData+1));
			o_Blue		= 0;
			o_Alpha		= 1;
			break;
		case 1:
			//::D3DXFloat16To32Array(&o_Red, (HALF*)pHData, 1);
			o_Red		= maFunctions::HalfToFloat(*(pHData+0));
			o_Green		= o_Red;
			o_Blue		= o_Red;
			o_Alpha		= 1;
			break;
		}
		}
		break;
	case 32:
		{
		DBG_ASSERT(isfloat, "bad capture fmt: 32 bits per channel but not float");
		float* pFData = (float*)m_pData;
		switch(nc)
		{
		case 4:
			o_Red		= (*(pFData+0));
			o_Green		= (*(pFData+1));
			o_Blue		= (*(pFData+2));
			o_Alpha		= (*(pFData+3));
			break;
		case 2:
			o_Red		= (*(pFData+0));
			o_Green		= (*(pFData+1));
			o_Blue		= 0;
			o_Alpha		= 1;
			break;
		case 1:
			o_Red		= (*(pFData+0));
			o_Green		= o_Red;
			o_Blue		= o_Red;
			o_Alpha		= 1;
			break;
		}
		}
		break;
	};
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIterator::SetValue(	float i_Red,
								float i_Green,
								float i_Blue,
								float i_Alpha) const
{
	int nc = m_PFD.GetNumChannels();
	bool isfloat = m_PFD.IsFloat();


	DBG_ASSERT(nc <= 4, "bad num of channels");
	switch(m_PFD.GetBitsPerChannel())
	{
	case 8:
		DBG_ASSERT(nc == 4, "bad capture fmt: 8 bits per channel but not 4 channels");
		DBG_ASSERT(!isfloat, "bad capture fmt: 8 bits per channel but float");
		m_pData[0] = (envType::UInt8)maFunctions::Clamp(i_Red * 255.0f, 0.f, 255.f);
		m_pData[1] = (envType::UInt8)maFunctions::Clamp(i_Green * 255.0f, 0.f, 255.f);
		m_pData[2] = (envType::UInt8)maFunctions::Clamp(i_Blue * 255.0f, 0.f, 255.f);
		m_pData[3] = (envType::UInt8)maFunctions::Clamp(i_Alpha * 255.0f, 0.f, 255.f);
		break;
	case 16:
		{
		DBG_ASSERT(isfloat, "bad capture fmt: 16 bits per channel but not float");
		envType::UInt16* pHData = (envType::UInt16*)m_pData;
		switch (nc)
		{
		case 4:
			{
			//float tmp[4] = {i_Blue,i_Green,i_Red,i_Alpha};
			//::D3DXFloat32To16Array((HALF*)pHData, tmp, 4);
			pHData[0] = maFunctions::FloatToHalf(i_Red);
			pHData[1] = maFunctions::FloatToHalf(i_Green);
			pHData[2] = maFunctions::FloatToHalf(i_Blue);
			pHData[3] = maFunctions::FloatToHalf(i_Alpha);
			}
			break;
		case 2:
			{
			//float tmp[2] = {i_Green,i_Red};
			//::D3DXFloat32To16Array((HALF*)pHData, tmp, 2);
			pHData[0] = maFunctions::FloatToHalf(i_Red);
			pHData[1] = maFunctions::FloatToHalf(i_Green);
			}
			break;
		case 1:
			//::D3DXFloat32To16Array((HALF*)pHData, &i_Red, 1);
			pHData[0] = maFunctions::FloatToHalf(i_Red);
			break;
		}
		}
		break;
	case 32:
		{
		DBG_ASSERT(isfloat, "bad capture fmt: 32 bits per channel but not float");
		float* pFData = (float*)m_pData;
		switch(nc)
		{
		case 4:
			pFData[0] = (i_Red);
			pFData[1] = (i_Green);
			pFData[2] = (i_Blue);
			pFData[3] = (i_Alpha);
			break;
		case 2:
			pFData[0] = (i_Red);
			pFData[1] = (i_Green);
			break;
		case 1:
			pFData[0] = (i_Red);
			break;
		}
		}
		break;
	};
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIterator::SetPosition(int i_Row, int i_Col)
{
	// clamp to bounds!
	if (i_Row < 0) i_Row = 0;
	if (i_Row >= m_Height) i_Row = m_Height-1;
	if (i_Col < 0) i_Col = 0;
	if (i_Col >= m_Width) i_Col = m_Width-1;

	//int RowOffset = m_pRect ? (i_Row+m_pRect->top) : i_Row;
	//int ColOffset = m_pRect ? (i_Col+m_pRect->left) : i_Col;
	int RowOffset = i_Row;
	int ColOffset = i_Col;

	m_pData =	m_pBase +
				RowOffset * m_Pitch +
				ColOffset * m_RGBA_bytes;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceIteratorRGBA32F::SurfaceIteratorRGBA32F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD)
:SurfaceIterator(i_Width,i_Height,i_Pitch,i_pData,i_PFD)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceIteratorRG32F::SurfaceIteratorRG32F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD)
:SurfaceIterator(i_Width,i_Height,i_Pitch,i_pData,i_PFD)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceIteratorR32F::SurfaceIteratorR32F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD)
:SurfaceIterator(i_Width,i_Height,i_Pitch,i_pData,i_PFD)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceIteratorRGBA16F::SurfaceIteratorRGBA16F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD)
:SurfaceIterator(i_Width,i_Height,i_Pitch,i_pData,i_PFD)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceIteratorRG16F::SurfaceIteratorRG16F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD)
:SurfaceIterator(i_Width,i_Height,i_Pitch,i_pData,i_PFD)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceIteratorR16F::SurfaceIteratorR16F(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD)
:SurfaceIterator(i_Width,i_Height,i_Pitch,i_pData,i_PFD)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SurfaceIteratorRGBA8U::SurfaceIteratorRGBA8U(int i_Width,int i_Height,int i_Pitch,void* i_pData,const g2dPFD& i_PFD)
:SurfaceIterator(i_Width,i_Height,i_Pitch,i_pData,i_PFD)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRGBA32F::GetValue(	float &o_Red,
		float &o_Green,
		float &o_Blue,
		float &o_Alpha) const
{
	float* pFData = (float*)m_pData;
	o_Red		= (*(pFData+0));
	o_Green		= (*(pFData+1));
	o_Blue		= (*(pFData+2));
	o_Alpha		= (*(pFData+3));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRGBA32F::SetValue(	float i_Red,
		float i_Green,
		float i_Blue,
		float i_Alpha) const
{
	float* pFData = (float*)m_pData;
	pFData[0] = (i_Red);
	pFData[1] = (i_Green);
	pFData[2] = (i_Blue);
	pFData[3] = (i_Alpha);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRG32F::GetValue(	float &o_Red,
		float &o_Green,
		float &o_Blue,
		float &o_Alpha) const
{
	float* pFData = (float*)m_pData;
	o_Red		= (*(pFData+0));
	o_Green		= (*(pFData+1));
	o_Blue		= 0;
	o_Alpha		= 1;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRG32F::SetValue(	float i_Red,
		float i_Green,
		float i_Blue,
		float i_Alpha) const
{
	float* pFData = (float*)m_pData;
	pFData[0] = (i_Red);
	pFData[1] = (i_Green);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorR32F::GetValue(	float &o_Red,
		float &o_Green,
		float &o_Blue,
		float &o_Alpha) const
{
	float* pFData = (float*)m_pData;
	o_Red		= (*(pFData+0));
	o_Green		= 0;
	o_Blue		= 0;
	o_Alpha		= 1;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorR32F::SetValue(	float i_Red,
		float i_Green,
		float i_Blue,
		float i_Alpha) const
{
	float* pFData = (float*)m_pData;
	pFData[0] = (i_Red);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRGBA16F::GetValue(	float &o_Red,
		float &o_Green,
		float &o_Blue,
		float &o_Alpha) const
{
	envType::UInt16* pHData = (envType::UInt16*)m_pData;

	//float tmp[4];
	//::D3DXFloat16To32Array(tmp, (HALF*)pHData, 4);
	//o_Red		= tmp[0];
	//o_Green	= tmp[1];
	//o_Blue	= tmp[2];
	//o_Alpha	= tmp[3];
	o_Red		= maFunctions::HalfToFloat(*(pHData+0));
	o_Green		= maFunctions::HalfToFloat(*(pHData+1));
	o_Blue		= maFunctions::HalfToFloat(*(pHData+2));
	o_Alpha		= maFunctions::HalfToFloat(*(pHData+3));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRGBA16F::SetValue(	float i_Red,
		float i_Green,
		float i_Blue,
		float i_Alpha) const
{
	envType::UInt16* pHData = (envType::UInt16*)m_pData;

	//float tmp[4] = {i_Blue,i_Green,i_Red,i_Alpha};
	//::D3DXFloat32To16Array((HALF*)pHData, tmp, 4);
	pHData[0] = maFunctions::FloatToHalf(i_Red);
	pHData[1] = maFunctions::FloatToHalf(i_Green);
	pHData[2] = maFunctions::FloatToHalf(i_Blue);
	pHData[3] = maFunctions::FloatToHalf(i_Alpha);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRG16F::GetValue( float &o_Red,
		float &o_Green,
		float &o_Blue,
		float &o_Alpha) const
{
	envType::UInt16* pHData = (envType::UInt16*)m_pData;

	//float tmp[2];
	//::D3DXFloat16To32Array(tmp, (HALF*)pHData, 2);
	//o_Red		= tmp[0];
	//o_Green		= tmp[1];
	o_Red		= maFunctions::HalfToFloat(*(pHData+0));
	o_Green		= maFunctions::HalfToFloat(*(pHData+1));

	o_Blue		= 0;
	o_Alpha		= 1;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRG16F::SetValue(	float i_Red,
		float i_Green,
		float i_Blue,
		float i_Alpha) const
{
	envType::UInt16* pHData = (envType::UInt16*)m_pData;
	//float tmp[2] = {i_Green,i_Red};
	//::D3DXFloat32To16Array((HALF*)pHData, tmp, 2);
	pHData[0] = maFunctions::FloatToHalf(i_Red);
	pHData[1] = maFunctions::FloatToHalf(i_Green);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorR16F::GetValue(	float &o_Red,
		float &o_Green,
		float &o_Blue,
		float &o_Alpha) const
{
	envType::UInt16* pHData = (envType::UInt16*)m_pData;
	//::D3DXFloat16To32Array(&o_Red, (HALF*)pHData, 1);
	o_Red		= maFunctions::HalfToFloat(*(pHData+0));
	o_Green		= 0;
	o_Blue		= 0;
	o_Alpha		= 1;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorR16F::SetValue(	float i_Red,
		float i_Green,
		float i_Blue,
		float i_Alpha) const
{
	envType::UInt16* pHData = (envType::UInt16*)m_pData;
	//::D3DXFloat32To16Array((HALF*)pHData, &i_Red, 1);
	pHData[0] = maFunctions::FloatToHalf(i_Red);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRGBA8U::GetValue(	float &o_Red,
		float &o_Green,
		float &o_Blue,
		float &o_Alpha) const
{
	o_Red		= (*(m_pData+0)) / 255.0f;
	o_Green		= (*(m_pData+1)) / 255.0f;
	o_Blue		= (*(m_pData+2)) / 255.0f;
	o_Alpha		= (*(m_pData+3)) / 255.0f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SurfaceIteratorRGBA8U::SetValue(	float i_Red,
		float i_Green,
		float i_Blue,
		float i_Alpha) const
{
	m_pData[0] = (envType::UInt8)maFunctions::Clamp(i_Red * 255.0f, 0.f, 255.f);
	m_pData[1] = (envType::UInt8)maFunctions::Clamp(i_Green * 255.0f, 0.f, 255.f);
	m_pData[2] = (envType::UInt8)maFunctions::Clamp(i_Blue * 255.0f, 0.f, 255.f);
	m_pData[3] = (envType::UInt8)maFunctions::Clamp(i_Alpha * 255.0f, 0.f, 255.f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_data_greyscale_sampled(envType::UInt8 *i_pBuffer,
							SurfaceIterator &i_SurfIt,
							int i_Width,
							int i_Height,
							int i_Sampling)
{
	int BufferWidth = i_Width / i_Sampling;
	envType::UInt8 *ptr = i_pBuffer;
	envType::UInt8 red,blue,green,alpha;
	int average;
	int num_samples = i_Sampling * i_Sampling;
	int FinalWidth = (i_Width-i_Sampling);

	for (int r=(i_Height-i_Sampling); r>=0; r-=i_Sampling)
	{
		for (int c=0; c<FinalWidth; c+=i_Sampling)
		{
			average = 0;
			for (int h=0; h<i_Sampling; h++)
			{
				i_SurfIt.SetPosition(r+h,c);

				for (int w=0; w<i_Sampling; w++)
				{
					i_SurfIt.GetValue(red,green,blue,alpha);
					average += (red + blue + green) / 3;
					i_SurfIt.IncCol();
				}
			}

			*ptr = envType::UInt8(average / num_samples);
			ptr++;
		}
	}

}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_data_24bit_sampled(envType::UInt8 *i_pBuffer,
							SurfaceIterator &i_SurfIt,
							int i_Width,
							int i_Height,
							int i_Sampling)
{
	int BufferWidth = 3 * (i_Width / i_Sampling);
	envType::UInt8 *ptr = i_pBuffer;
	envType::UInt8 red,blue,green,alpha;
	int red_avg, green_avg, blue_avg;
	int num_samples = i_Sampling * i_Sampling;
	int FinalWidth = (i_Width-i_Sampling+1);

	//	go through all the source pixels
	for (int r=(i_Height-i_Sampling); r>=0; r-=i_Sampling)
	{
		for (int c=0; c<FinalWidth; c+=i_Sampling)
		{
			//	gather the pixels around and get an average color
			red_avg = green_avg = blue_avg = 0;
			for (int h=0; h<i_Sampling; h++)
			{
				i_SurfIt.SetPosition(r+h,c);

				for (int w=0; w<i_Sampling; w++)
				{
					i_SurfIt.GetValue(red,green,blue,alpha);
					red_avg		+= red;
					green_avg	+= green;
					blue_avg	+= blue;
					i_SurfIt.IncCol();
				}
			}

			//	output the information
			*ptr++ = envType::UInt8(blue_avg / num_samples);
			*ptr++ = envType::UInt8(green_avg / num_samples);
			*ptr++ = envType::UInt8(red_avg / num_samples);
		}
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_data_greyscale(envType::UInt8 *i_pBuffer,
							SurfaceIterator &i_SurfIt,
							int i_Width,
							int i_Height)
{
	int BufferWidth = i_Width;
	envType::UInt8 *ptr = i_pBuffer;
	envType::UInt8 red,blue,green,alpha;

	for (int r=i_Height-1; r>=0; r--)
	{
		i_SurfIt.SetPosition(r,0);
		for (int c=0; c<i_Width; c++)
		{
			i_SurfIt.GetValue(red,green,blue,alpha);
			*ptr = (red + blue + green) / 3;
			ptr++;
			i_SurfIt.IncCol();
		}
	}

}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_bmp_data_24bit(	envType::UInt8 *i_pBuffer,
							SurfaceIterator &i_SurfIt,
							int i_Width,
							int i_Height)
{
	int BufferWidth = i_Width * 3;
	envType::UInt8 *ptr = i_pBuffer;
	envType::UInt8 alpha;

	for (int r=i_Height-1; r>=0; r--)
	{
		i_SurfIt.SetPosition(r,0);
		for (int c=0; c<i_Width; c++)
		{
			i_SurfIt.GetValue(*(ptr+2), *(ptr+1), *(ptr+0), alpha);
			ptr += 3;
			i_SurfIt.IncCol();
		}
	}

}

void copyPixels(g2dPixelR16G16B16A16* i_pBuffer,
							SurfaceIterator& i_SurfIt,
							int i_Width,
							int i_Height)
{
	g2dPixelR16G16B16A16 *ptr = i_pBuffer;
	envType::UInt8 pr,pg,pb,pa;
	for (int r=i_Height-1; r>=0; r--)
	{
		i_SurfIt.SetPosition(r,0);
		for (int c=0; c<i_Width; c++)
		{
			i_SurfIt.GetValue(pr,pg,pb,pa);
			ptr->r = pr;
			ptr->g = pg;
			ptr->b = pb;
			ptr->a = pa;
			ptr++;
			i_SurfIt.IncCol();
		}
	}

}
void copyPixels(g2dPixelR8G8B8A8* i_pBuffer,
							SurfaceIterator& i_SurfIt,
							int i_Width,
							int i_Height)
{
	g2dPixelR8G8B8A8 *ptr = i_pBuffer;
	for (int r=i_Height-1; r>=0; r--)
	{
		i_SurfIt.SetPosition(r,0);
		for (int c=0; c<i_Width; c++)
		{
			i_SurfIt.GetValue((ptr->r), (ptr->g), (ptr->b), (ptr->a));
			ptr++;
			i_SurfIt.IncCol();
		}
	}
}
void copyPixels(g2dPixelRGBA32F* i_pBuffer,
							SurfaceIterator& i_SurfIt,
							int i_Width,
							int i_Height)
{
	g2dPixelRGBA32F *ptr = i_pBuffer;
	float pr,pg,pb,pa;
	for (int r=i_Height-1; r>=0; r--)
	{
		i_SurfIt.SetPosition(r,0);
		for (int c=0; c<i_Width; c++)
		{
			i_SurfIt.GetValue(pr,pg,pb,pa);
			ptr->r = pr;
			ptr->g = pg;
			ptr->b = pb;
			ptr->a = pa;
			ptr++;
			i_SurfIt.IncCol();
		}
	}

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int write_packet_RLE(	envType::UInt8 **io_pBuffer, 
						envType::UInt8 i_R,
						envType::UInt8 i_G,
						envType::UInt8 i_B,
						envType::UInt8 i_Count )
{
	if ( i_Count > 0 )
		*(*io_pBuffer+0) = (envType::UInt8)(128 + i_Count);
	else
		*(*io_pBuffer+0) = (envType::UInt8)(i_Count);
    *(*io_pBuffer+1) = i_B;
    *(*io_pBuffer+2) = i_G;
    *(*io_pBuffer+3) = i_R;
	*io_pBuffer += 4;

	//DBG_LOG5( "RLE packet count=%d (%x) (%2x,%2x,%2x)", i_Count, (128 + i_Count), i_R, i_G, i_B );
	return 4;
}
//----------------------------------------------------------------------------
//	VerifyWriteOK - verify that the amount of data to be written will fit
//	in the allocated buffer
//
//	throws: cptrWriteBufferOverrunX
//----------------------------------------------------------------------------
void VerifyWriteOK( int i_BufferSize, int i_BufferSizeUsed, int i_BytesToWrite )
{
	if ( i_BufferSize < (i_BufferSizeUsed + i_BytesToWrite) )
	{
		DBG_WARNING( "RLE will overrun size of uncompressed image -- saving as uncompressed" );

		//	throw an exception so something can catch it and either output 
		//	an uncompressed format or make the buffer larger
		//
		throw g2dWriteBufferOverrunX();
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int write_TGA_data_24bit_sampledRLE(	envType::UInt8 *io_pBuffer,
										SurfaceIterator &i_SurfIt,
										int i_Width,
										int i_Height,
										int i_Sampling,
										int i_BufferSize  )
{
	int BufferSizeUsed = 0;
	envType::UInt8 *ptr = io_pBuffer;
	envType::UInt8 red,blue,green,alpha;
	int red_avg, green_avg, blue_avg;
	int num_samples = i_Sampling * i_Sampling;
	int FinalWidth = (i_Width-i_Sampling+1);

	int buffSize = sizeof(*io_pBuffer);
	DBG_LOG("Buffer size = " << buffSize);

	//	go through all the source pixels
	//for (int r=(i_Height-i_Sampling); r>=0; r-=i_Sampling)
	for (int row=0; row<=(i_Height-i_Sampling); row+=i_Sampling)
	{
		envType::UInt8 matchcount = 255;// (-1)
		envType::UInt8 rl, gl, bl;		// last

		for (int col=0; col<FinalWidth; col+=i_Sampling)
		{
			//	gather the pixels around and get an average color
			red_avg = green_avg = blue_avg = 0;
			for (int h=0; h<i_Sampling; h++)
			{
				i_SurfIt.SetPosition(row+h,col);

				for (int w=0; w<i_Sampling; w++)
				{
					i_SurfIt.GetValue(red,green,blue,alpha);
					red_avg		+= red;
					green_avg	+= green;
					blue_avg	+= blue;
					i_SurfIt.IncCol();
				}
			}

			red_avg		= (envType::UInt8)(red_avg / num_samples);
			green_avg	= (envType::UInt8)(green_avg / num_samples);
			blue_avg	= (envType::UInt8)(blue_avg / num_samples);

			//	first time in row, set "previous" values to current
			if ( matchcount == 255 )
			{
				rl = red_avg;	gl = green_avg;	bl = blue_avg;
			}

			//	if there isn't an exact match, write out what was stored already
			//
			if ((rl != red_avg) || (gl != green_avg) || (bl != blue_avg))
			{
				VerifyWriteOK( i_BufferSize, BufferSizeUsed, 4 );
				BufferSizeUsed += write_packet_RLE( &ptr, rl, gl, bl, matchcount );
				rl = red_avg;	gl = green_avg;	bl = blue_avg;
				matchcount = 0;
			}
			else
			{
				matchcount++;
			}
		}

		// write out the last pixels
		VerifyWriteOK( i_BufferSize, BufferSizeUsed, 4 );
		BufferSizeUsed += write_packet_RLE( &ptr, rl, gl, bl, matchcount );
	}

	return BufferSizeUsed;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_TGA_data_24bit_sampled(	envType::UInt8 *io_pBuffer,
									SurfaceIterator &i_SurfIt,
									int i_Width,
									int i_Height,
									int i_Sampling)
{
	int BufferWidth = 3 * (i_Width / i_Sampling);
	envType::UInt8 *ptr = io_pBuffer;
	envType::UInt8 red,blue,green,alpha;
	int red_avg, green_avg, blue_avg;
	int num_samples = i_Sampling * i_Sampling;
	int FinalWidth = (i_Width-i_Sampling+1);

	//	go through all the source pixels
	//for (int r=(i_Height-i_Sampling); r>=0; r-=i_Sampling)
	for (int r=0; r<=(i_Height-i_Sampling); r+=i_Sampling)
	{
		for (int c=0; c<FinalWidth; c+=i_Sampling)
		{
			//	gather the pixels around and get an average color
			red_avg = green_avg = blue_avg = 0;
			for (int h=0; h<i_Sampling; h++)
			{
				i_SurfIt.SetPosition(r+h,c);

				for (int w=0; w<i_Sampling; w++)
				{
					i_SurfIt.GetValue(red,green,blue,alpha);
					red_avg		+= red;
					green_avg	+= green;
					blue_avg	+= blue;
					i_SurfIt.IncCol();
				}
			}

			//	output the information
			*ptr++ = envType::UInt8(blue_avg / num_samples);
			*ptr++ = envType::UInt8(green_avg / num_samples);
			*ptr++ = envType::UInt8(red_avg / num_samples);
		}
	}
}
//----------------------------------------------------------------------------
//	return the *actual* size of the buffer used.
//----------------------------------------------------------------------------
int write_TGA_data_24bitRLE(	envType::UInt8 *io_pBuffer,
								SurfaceIterator &i_SurfIt,
								int i_Width,
								int i_Height,
								int i_BufferSize )
{
	int BufferSizeUsed = 0;
	envType::UInt8 *ptr = io_pBuffer;

	//for (int r=i_Height-1; r>=0; r--)
	for (int row=0; row<(i_Height-1); row++)
	{
		envType::UInt8 matchcount = 255;// (-1)
		envType::UInt8 r, g, b, a;			// current
		envType::UInt8 rl, gl, bl, al;		// last

		i_SurfIt.SetPosition(row,0);
		for (int col=0; col<i_Width; col++)
		{
			i_SurfIt.GetValue(r,g,b,a);

			//	first time in row, set "previous" values to current
			if ( matchcount == 255 )
			{
				rl = r;	gl = g;	bl = b; al = a;
			}

			//	if there isn't an exact match, write out what was stored already
			//
			if ((rl != r) || (gl != g) || (bl != b))
			{
				VerifyWriteOK( i_BufferSize, BufferSizeUsed, 4 );
				BufferSizeUsed += write_packet_RLE( &ptr, rl, gl, bl, matchcount );

				rl = r;	gl = g;	bl = b; al = a;
				matchcount = 0;
			}
			else
			{
				matchcount++;
			}

			i_SurfIt.IncCol();
		}

		// write out the last pixels
		VerifyWriteOK( i_BufferSize, BufferSizeUsed, 4 );
		BufferSizeUsed += write_packet_RLE( &ptr, rl, gl, bl, matchcount );
	}

	return BufferSizeUsed;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void write_TGA_data_24bit(	envType::UInt8 *io_pBuffer,
							SurfaceIterator &i_SurfIt,
							int i_Width,
							int i_Height)
{
	int BufferWidth = i_Width * 3;
	envType::UInt8 *ptr = io_pBuffer;
	envType::UInt8 alpha;

	int buffSize = sizeof(io_pBuffer);

	//for (int r=i_Height-1; r>=0; r--)
	for (int r=0; r<(i_Height-1); r++)
	{
		i_SurfIt.SetPosition(r,0);
		for (int c=0; c<i_Width; c++)
		{
			i_SurfIt.GetValue(*(ptr+2), *(ptr+1), *(ptr+0), alpha);
			ptr += 3;
			i_SurfIt.IncCol();
		}
	}
}

namespace g2dDX11SurfaceUtil
{
//----------------------------------------------------------------------------
// this call allocates a big block of mem!!!!!
// caller must delete [] o_pBuffer when done using it!!!!
//----------------------------------------------------------------------------
void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
									 envType::UInt8** o_pBuffer,
									 int* o_bufSize,
									 int* o_width,
									 int* o_height,
									 int* o_bpp,
									 int i_sampling /*= 1*/,
									 bool i_bGreyscale /*= false*/,
									 RECT* i_pRect /*= NULL*/)
{
	SurfaceLocker surf_locker(i_pSurface);

	if (surf_locker.IsLocked())
	{
		SurfaceIterator& surf_it = *(surf_locker.GetIterator());

		int width	= surf_it.GetWidth();
		int height	= surf_it.GetHeight();

		int bytes_per_pixel = (i_bGreyscale) ? 1 : 3;
		int bufferSize = width * height * bytes_per_pixel;
		if (i_sampling > 1)
		{
			bufferSize /= i_sampling;
			bufferSize /= i_sampling;
		}
		envType::UInt8* pBuffer =  new envType::UInt8[bufferSize];

		if (i_sampling > 1)
		{
			if (i_bGreyscale)
				write_bmp_data_greyscale_sampled(pBuffer, surf_it, width, height, i_sampling);
			else
				write_bmp_data_24bit_sampled(pBuffer, surf_it, width, height, i_sampling);
		}
		else
		{
			if (i_bGreyscale)
				write_bmp_data_greyscale(pBuffer, surf_it, width, height);
			else
				write_bmp_data_24bit(pBuffer, surf_it, width, height);
		}

		*o_pBuffer = pBuffer;
		*o_bufSize = bufferSize;
		*o_width = width;
		*o_height = height;
		*o_bpp = bytes_per_pixel*8;
	}
	else
	{
		DBG_WARNING("Error locking surface buffer.");
	}
}
void GetSurfacePixelsTGA(g2dD3D11TexturePtr i_pSurface,
	envType::UInt8 **o_pBuffer,
	int *o_bufSize,
	int *o_width,
	int *o_height,
	int *o_bpp,
	int	i_Sampling /*= 1*/,
	const std::string& i_CompressionCode /*= std::string("None")*/,
	const RECT* i_pRect /*= NULL*/)
{
	SurfaceLocker surf_locker(i_pSurface);

	if (surf_locker.IsLocked())
	{
		SurfaceIterator& surf_it = *(surf_locker.GetIterator());

		int width	= surf_it.GetWidth();
		int height	= surf_it.GetHeight();

		int bytes_per_pixel = 3;
		int bufferSize = width * height * bytes_per_pixel;
		if (i_Sampling > 1)
		{
			bufferSize /= i_Sampling;
			bufferSize /= i_Sampling;
		}
		envType::UInt8 *pBuffer =  new envType::UInt8[bufferSize];

		if (i_Sampling > 1)
		{
			if ( strcmp(i_CompressionCode.c_str(),"RLE")==0 )
			{
				try
				{
					//	write out the frame...if the buffer is too small (i.e. really poor compression)
					//	it throws an error, so this will fall back and output an uncompressed frame.
					bufferSize = write_TGA_data_24bit_sampledRLE(pBuffer, surf_it, width, height, i_Sampling, bufferSize);
				}
				catch( const g2dWriteBufferOverrunX& /*i_Ex*/ )
				{
					write_TGA_data_24bit_sampled(pBuffer, surf_it, width, height, i_Sampling);
				}
			}
			else
			{
				write_TGA_data_24bit_sampled(pBuffer, surf_it, width, height, i_Sampling);
			}
		}
		else
		{
			if ( strcmp(i_CompressionCode.c_str(),"RLE")==0 )
			{
				try
				{
					//	write out the frame...if the buffer is too small (i.e. really poor compression)
					//	it throws an error, so this will fall back and output an uncompressed frame.
					bufferSize = write_TGA_data_24bitRLE(pBuffer, surf_it, width, height, bufferSize);
				}
				catch( const g2dWriteBufferOverrunX& /*i_Ex*/ )
				{
					write_TGA_data_24bit(pBuffer, surf_it, width, height);
				}
			}
			else
			{
				write_TGA_data_24bit(pBuffer, surf_it, width, height);
			}
		}

		*o_pBuffer = pBuffer;
		*o_bufSize = bufferSize;
		*o_width = width;
		*o_height = height;
		*o_bpp = bytes_per_pixel*8;
	}
	else
	{
		DBG_WARNING("Error locking buffer when writing to bitmap.");
	}
}

//----------------------------------------------------------------------------
// this call allocates a big block of mem!!!!!
// caller must delete [] o_pBuffer when done using it!!!!
//----------------------------------------------------------------------------
void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
									 g2dPixelR16G16B16A16** o_pBuffer,
									 int* o_width,
									 int* o_height)
{
	SurfaceLocker surf_locker(i_pSurface);

	if (surf_locker.IsLocked())
	{
		SurfaceIterator& surf_it = *(surf_locker.GetIterator());

		int width	= surf_it.GetWidth();
		int height	= surf_it.GetHeight();
		int bufferSize = width * height;
		g2dPixelR16G16B16A16* pBuffer =  new g2dPixelR16G16B16A16[bufferSize];
		copyPixels(pBuffer, surf_it, width, height);

		*o_pBuffer = pBuffer;
		*o_width = width;
		*o_height = height;
	}
	else
	{
		DBG_WARNING("Error locking surface buffer.");
	}
}
//----------------------------------------------------------------------------
// this call allocates a big block of mem!!!!!
// caller must delete [] o_pBuffer when done using it!!!!
//----------------------------------------------------------------------------
void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
									 g2dPixelR8G8B8A8** o_pBuffer,
									 int* o_width,
									 int* o_height)
{
	SurfaceLocker surf_locker(i_pSurface);

	if (surf_locker.IsLocked())
	{
		SurfaceIterator& surf_it = *(surf_locker.GetIterator());

		int width	= surf_it.GetWidth();
		int height	= surf_it.GetHeight();
		int bufferSize = width * height;
		g2dPixelR8G8B8A8* pBuffer =  new g2dPixelR8G8B8A8[bufferSize];
		copyPixels(pBuffer, surf_it, width, height);

		*o_pBuffer = pBuffer;
		*o_width = width;
		*o_height = height;
	}
	else
	{
		DBG_WARNING("Error locking surface buffer.");
	}
}

//----------------------------------------------------------------------------
// this call allocates a big block of mem!!!!!
// caller must delete [] o_pBuffer when done using it!!!!
//----------------------------------------------------------------------------
void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
					  g2dPixelRGBA32F** o_pBuffer,
					  int* o_width,
					  int* o_height)
{
	SurfaceLocker surf_locker(i_pSurface);

	if (surf_locker.IsLocked())
	{
		SurfaceIterator& surf_it = *(surf_locker.GetIterator());

		int width	= surf_it.GetWidth();
		int height	= surf_it.GetHeight();
		int bufferSize = width * height;
		g2dPixelRGBA32F* pBuffer =  new g2dPixelRGBA32F[bufferSize];
		copyPixels(pBuffer, surf_it, width, height);

		*o_pBuffer = pBuffer;
		*o_width = width;
		*o_height = height;
	}
	else
	{
		DBG_WARNING("Error locking surface buffer.");
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void GetSurfacePixels(g2dD3D11TexturePtr i_pSurface,
					  g2dPixelRGBA32F* o_pBuffer,
					  int i_Width,
					  int i_Height)
{
	SurfaceLocker surf_locker(i_pSurface);

	if (surf_locker.IsLocked())
	{
		SurfaceIterator& surf_it = *(surf_locker.GetIterator());
		copyPixels(o_pBuffer, surf_it, i_Width, i_Height);
	}
	else
	{
		DBG_WARNING("Error locking surface buffer.");
	}
}

//----------------------------------------------------------------------------
// Get the color of the pixel at an individual position on the surface
//----------------------------------------------------------------------------
void GetPixelColor(g2dD3D11TexturePtr i_pSurface,
						int i_X,
						int i_Y,
						envType::UInt8 &o_Red,
						envType::UInt8 &o_Green,
						envType::UInt8 &o_Blue,
						envType::UInt8 &o_Alpha)
{
	SurfaceLocker surf_locker(i_pSurface);

	if (surf_locker.IsLocked())
	{
		SurfaceIterator& surf_it = *(surf_locker.GetIterator());

		//int width	= surf_it.GetWidth();
		//int height	= surf_it.GetHeight();

		surf_it.SetPosition(i_Y, i_X);

		surf_it.GetValue(o_Red, o_Green, o_Blue, o_Alpha);
	}
}

//----------------------------------------------------------------------------
// Get the color of the pixel at an individual position on the surface
//----------------------------------------------------------------------------
void GetPixelColor(g2dD3D11TexturePtr i_pSurface,
						int i_X,
						int i_Y,
						envType::Float32 &o_Red,
						envType::Float32 &o_Green,
						envType::Float32 &o_Blue,
						envType::Float32 &o_Alpha)
{
	SurfaceLocker surf_locker(i_pSurface);

	if (surf_locker.IsLocked())
	{
		SurfaceIterator& surf_it = *(surf_locker.GetIterator());

		surf_it.SetPosition(i_Y, i_X);

		surf_it.GetValue(o_Red, o_Green, o_Blue, o_Alpha);
	}
}

} // namespace g2dDX11SurfaceUtil
