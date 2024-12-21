/****************************************************************************\
**  g2dScanlineOpenEXRUtil.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g2d/g2dScanlineOpenEXRUtil.hpp"


#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g2d/g2dOpenEXRDefines.hpp"
#include "Graphics/g2d/g2dOpenEXROutStream.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"

// OpenEXR includes
#ifdef USE_OPENEXR

// Using openexr from a dll and not static libs.
#define OPENEXR_DLL

#undef min
#undef max
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfStringAttribute.h>

// OpenEXR Libs
//#pragma comment(lib, "IlmImf-2_2.lib")
//#pragma comment(lib, "Half.lib") 
//#pragma comment(lib, "Iex-2_2.lib")
//#pragma comment(lib, "IlmThread-2_2.lib")
//#pragma comment(lib, "Imath-2_2.lib")
//#pragma comment(lib, "zdll.lib")

#include <stdio.h>

#include <fstream>
#include <iostream>
#include <ostream>

//============================================================================
//============================================================================
namespace 
{

//--------------------------------------------------------------------
// Utility function to get filename into char array, from fsLocator
//--------------------------------------------------------------------
std::string getFileName(const fsLocator& i_Locator) 
{
	itString itFileName;
	fsFileUtil::LocatorToUnicodeString(i_Locator,itFileName);
	const itString::CharType* UTF_8_str = itFileName.GetString();

	std::string ReturnString;
	for (int i = 0; i < itFileName.GetLength(); ++i)
	{
		ReturnString.push_back((char)UTF_8_str[i]);
	}

	/*
	int strSize = UTF_8_str.size();
	char *fileName = new char[strSize+1];
	fileName[strSize] = 0;
	memcpy( fileName , UTF_8_str.c_str() , strSize );
	*/

	return ReturnString;
}

//--------------------------------------------------------------------
// Convert regular 8-bit color buffer to Imf::Rgba, which is what 
// OpenEXR needs to write regular color images
//--------------------------------------------------------------------
void ColorBufferToEXR_RGBA(g2dImage* in, Imf::Rgba * out, int numBytesPerPixel) 
{
	// Lock surfaces
	void* imgBits = in->Lock();
	BYTE* pImgBits = (BYTE*)imgBits; // Format: B, G, R, A

	//Loop through all the pixels
	int i, j, dst_idx(0);
	for (i = 0 ; i < in->GetHeight() ; i++) 
	{
		for (j = 0 ; j < in->GetWidth() ; j++) 
		{
			int idx = (j*numBytesPerPixel) + (i*in->GetStride());
			out[dst_idx].r = pImgBits[idx+R_OFFSET_E_COLOR] / 255.0f;
			out[dst_idx].g = pImgBits[idx+G_OFFSET_E_COLOR] / 255.0f;
			out[dst_idx].b = pImgBits[idx+B_OFFSET_E_COLOR] / 255.0f;
			out[dst_idx].a = pImgBits[idx+A_OFFSET_E_COLOR] / 255.0f;
			dst_idx++;
		}
	}
	// Unlock surfaces
	in->Release();
}

//--------------------------------------------------------------------
// Convert current RGBA16f buffer to an array of *halfs*
//--------------------------------------------------------------------
void g2dImageToHalfArray(g2dImage* in, half * out, int numBytesPerPixel, int curr_channel) 
{
	// Lock surfaces
	void* imgBits = in->Lock();
	BYTE* pImgBits = (BYTE*)imgBits;

	//Loop through all the pixels
	int i, j, dst_idx(0);
	for (i = 0 ; i < in->GetHeight() ; i++) 
	{
		for (j = 0 ; j < in->GetWidth() ; j++) 
		{
			int idx = (j*numBytesPerPixel) + (i*in->GetStride());
			//out[dst_idx] = *(half*)(&(pImgBits[idx + curr_channel*sizeof(half)]));
			out[dst_idx] = *(half*)(pImgBits + idx + curr_channel*sizeof(half));
			dst_idx++;
		}
	}
	// Unlock surfaces
	in->Release();
}

//--------------------------------------------------------------------
// Convert current RGBA32f buffer to an array of *floats*
//--------------------------------------------------------------------
void g2dImageToFloatArray(g2dImage* in, float * out, int numBytesPerPixel, int curr_channel) 
{
	// Lock surfaces
	void* imgBits = in->Lock();
	BYTE* pImgBits = (BYTE*)imgBits;

	//Loop through all the pixels
	int i, j, dst_idx(0);
	for (i = 0 ; i < in->GetHeight() ; i++) 
	{
		for (j = 0 ; j < in->GetWidth() ; j++) 
		{
			int idx = (j*numBytesPerPixel) + (i*in->GetStride());
			//out[dst_idx] = *(float*)(&(pImgBits[idx + curr_channel*sizeof(float)]));
			out[dst_idx] = *(float*)(pImgBits + idx + curr_channel*sizeof(float));
			dst_idx++;
		}
	}
	// Unlock surfaces
	in->Release();
}


//--------------------------------------------------------------------
// Write scanline EXR file with an arbitrary channel of type *float*
//
// This code comes directly from the OpenEXR document:
// "Reading and Writing OpenEXR Image Files with the IlmIlf Library"
//--------------------------------------------------------------------
void WriteScanlineArbitraryFloat(std::string fileName,
								const float *r_arb_channel,
								const float *g_arb_channel,
								const float *b_arb_channel,
								const float *a_arb_channel,
								int width, int height, 
								const char comments[],
								const fsLocator& i_Locator)
{
	Imf::Header header(width, height);									// 1
	header.insert("comments",Imf::StringAttribute(comments));
	header.channels().insert("R", Imf::Channel(Imf::FLOAT));// 2
	header.channels().insert("G", Imf::Channel(Imf::FLOAT));// 3
	header.channels().insert("B", Imf::Channel(Imf::FLOAT));	
	header.channels().insert("A", Imf::Channel(Imf::FLOAT));	
	
	g2dOpenEXROutStream ostr(fileName.c_str(),i_Locator);

	Imf::OutputFile file(ostr, header);							// 4
	//Imf::OutputFile file(fileName.c_str(), header);							// 4
	Imf::FrameBuffer frameBuffer;									// 5
	
	frameBuffer.insert("R",								// name // 6
						Imf::Slice(Imf::FLOAT,						// type // 7
						(char *)r_arb_channel,						// base // 8
						sizeof(*r_arb_channel) * 1,					// xStride// 9
						sizeof(*r_arb_channel) * width));			// yStride// 10
	
	frameBuffer.insert("G",								// name // 11
						Imf::Slice(Imf::FLOAT,						// type // 12
						(char *)g_arb_channel,						// base // 13
						sizeof(*g_arb_channel) * 1,					// xStride// 14
						sizeof(*g_arb_channel) * width));			// yStride// 15

	frameBuffer.insert("B",								// name // 11
						Imf::Slice(Imf::FLOAT,						// type // 12
						(char *)b_arb_channel,						// base // 13
						sizeof(*b_arb_channel) * 1,					// xStride// 14
						sizeof(*b_arb_channel) * width));			// yStride// 15

	frameBuffer.insert("A",								// name // 11
						Imf::Slice(Imf::FLOAT,						// type // 12
						(char *)a_arb_channel,						// base // 13
						sizeof(*a_arb_channel) * 1,					// xStride// 14
						sizeof(*a_arb_channel) * width));			// yStride// 15

	file.setFrameBuffer(frameBuffer);								// 16
	file.writePixels(height);										// 17
	
}


//--------------------------------------------------------------------
// Write scanline EXR file with an arbitrary channel of type *half*
//
// This code comes directly from the OpenEXR document:
// "Reading and Writing OpenEXR Image Files with the IlmIlf Library"
//--------------------------------------------------------------------
void WriteScanlineArbitraryHalf(std::string fileName,
							const half *r_arb_channel,
							const half *g_arb_channel,
							const half *b_arb_channel,
							const half *a_arb_channel,
							int width, int height, 
							const char comments[],
							const fsLocator& i_Locator)
{
	Imf::Header header(width, height);									// 1
	header.insert("comments",Imf::StringAttribute(comments));
	header.channels().insert("R", Imf::Channel(Imf::HALF));	// 2
	header.channels().insert("G", Imf::Channel(Imf::HALF));	// 3
	header.channels().insert("B", Imf::Channel(Imf::HALF));	
	header.channels().insert("A", Imf::Channel(Imf::HALF));	
	
	g2dOpenEXROutStream ostr(fileName.c_str(),i_Locator);

	Imf::OutputFile file(ostr, header);							// 4
	//Imf::OutputFile file(fileName.c_str(), header);							// 4
	Imf::FrameBuffer frameBuffer;									// 5
	
	frameBuffer.insert("R",								// name // 6
						Imf::Slice(Imf::HALF,						// type // 7
						(char *)r_arb_channel,						// base // 8
						sizeof(*r_arb_channel) * 1,					// xStride// 9
						sizeof(*r_arb_channel) * width));			// yStride// 10
	
	frameBuffer.insert("G",								// name // 11
						Imf::Slice(Imf::HALF,						// type // 12
						(char *)g_arb_channel,						// base // 13
						sizeof(*g_arb_channel) * 1,					// xStride// 14
						sizeof(*g_arb_channel) * width));			// yStride// 15

	frameBuffer.insert("B",								// name // 11
						Imf::Slice(Imf::HALF,						// type // 12
						(char *)b_arb_channel,						// base // 13
						sizeof(*b_arb_channel) * 1,					// xStride// 14
						sizeof(*b_arb_channel) * width));			// yStride// 15

	frameBuffer.insert("A",								// name // 11
						Imf::Slice(Imf::HALF,						// type // 12
						(char *)a_arb_channel,						// base // 13
						sizeof(*a_arb_channel) * 1,					// xStride// 14
						sizeof(*a_arb_channel) * width));			// yStride// 15

	file.setFrameBuffer(frameBuffer);								// 16
	file.writePixels(height);										// 17

}

//--------------------------------------------------------------------
// Write a scanline EXR file with regular 8-bit color information
//
// This code comes untouched from the OpenEXR document:
// "Reading and Writing OpenEXR Image Files with the IlmIlf Library"
//--------------------------------------------------------------------
void WriteScanlineRGBA(std::string fileName, const Imf::Rgba *pixels, 
					    int width, int height,const fsLocator& i_Locator)	
{
	g2dOpenEXROutStream ostr(fileName.c_str(),i_Locator);
	Imf::Header header(width, height);
	Imf::RgbaOutputFile file(ostr, header);

	//Imf::RgbaOutputFile file(fileName.c_str(), width, height, Imf::WRITE_RGBA);

	file.setFrameBuffer(pixels, 1, width);
	file.writePixels(height);
}

} // namespace

#endif // USE_OPENEXR


//--------------------------------------------------------------------
// Function to invoke different methods of writing scanline EXR files.
// It identifies the current buffer type and writes the 
// appropriate type of EXR file.
//--------------------------------------------------------------------
void g2dScanlineOpenEXRUtil::WriteToScanlineEXR(const fsLocator& i_Locator, const g2dImage* pImg) 
{
#ifdef USE_OPENEXR
	// Get image dimensions & file info
	int width = pImg->GetWidth();
	int height = pImg->GetHeight();

	std::string fileName = getFileName(i_Locator);
	int pixelFormat = pImg->GetPixelFormat().GetPixelFormat();

	// If RGBA16f buffer is found, write out a RGBA16 FP OpenEXR file
	if ( pixelFormat == g2dPFD::e_RGBA16f || pixelFormat == g2dPFD::e_Float16 ) 
	{

		// Create buffer to hold arbitrary RGBA channels
		half * r_arb_channel = new half[width*height];
		half * g_arb_channel = new half[width*height];
		half * b_arb_channel = new half[width*height];
		half * a_arb_channel = new half[width*height];

		// Fill buffers with motion data from the RGBA16f D3D surface
		if ( pixelFormat == g2dPFD::e_RGBA16f ) 
		{
			g2dImageToHalfArray( (g2dImage*)pImg, r_arb_channel, BYTES_PER_PIX_8, R_OFFSET_RGBAxxf );
			g2dImageToHalfArray( (g2dImage*)pImg, g_arb_channel, BYTES_PER_PIX_8, G_OFFSET_RGBAxxf );
			g2dImageToHalfArray( (g2dImage*)pImg, b_arb_channel, BYTES_PER_PIX_8, B_OFFSET_RGBAxxf );
			g2dImageToHalfArray( (g2dImage*)pImg, a_arb_channel, BYTES_PER_PIX_8, A_OFFSET_RGBAxxf );
		} 
		else if ( pixelFormat == g2dPFD::e_Float16 ) 
		{
			g2dImageToHalfArray( (g2dImage*)pImg, r_arb_channel, BYTES_PER_PIX_2, R_OFFSET_RGBAxxf );
		}

		// Write out OpenEXR file with half channels
		WriteScanlineArbitraryHalf(fileName, 
								   r_arb_channel, 
								   g_arb_channel, 
								   b_arb_channel, 
								   a_arb_channel, 
								   width, height,
								   "Scanline EXR generated by MachStudio.",
								   i_Locator);

		// Clean up
		delete [] r_arb_channel;
		delete [] g_arb_channel;
		delete [] b_arb_channel;
		delete [] a_arb_channel;
		r_arb_channel = NULL;
		g_arb_channel = NULL;
		b_arb_channel = NULL;
		a_arb_channel = NULL; 
	
	} // If RGBA32f buffer is found, write out a RGBA32 FP OpenEXR file
	else if ( pixelFormat == g2dPFD::e_RGBA32f || pixelFormat == g2dPFD::e_Float32 ) 
	{
		// Create buffer to hold arbitrary RGBA channels
		float * r_arb_channel = new float[width*height];
		float * g_arb_channel = new float[width*height];
		float * b_arb_channel = new float[width*height];
		float * a_arb_channel = new float[width*height];

		// Fill buffers with motion data from the 32f D3D surface
		if ( pixelFormat == g2dPFD::e_RGBA32f ) 
		{
			g2dImageToFloatArray( (g2dImage*)pImg, r_arb_channel, BYTES_PER_PIX_16, R_OFFSET_RGBAxxf );
			g2dImageToFloatArray( (g2dImage*)pImg, g_arb_channel, BYTES_PER_PIX_16, G_OFFSET_RGBAxxf );
			g2dImageToFloatArray( (g2dImage*)pImg, b_arb_channel, BYTES_PER_PIX_16, B_OFFSET_RGBAxxf );
			g2dImageToFloatArray( (g2dImage*)pImg, a_arb_channel, BYTES_PER_PIX_16, A_OFFSET_RGBAxxf );
		} 
		else if ( pixelFormat == g2dPFD::e_Float32 ) 
		{
			g2dImageToFloatArray( (g2dImage*)pImg, r_arb_channel, BYTES_PER_PIX_4, R_OFFSET_RGBAxxf );
		}

		// Write out OpenEXR file with float channels
		WriteScanlineArbitraryFloat(fileName, 
									r_arb_channel, 
									g_arb_channel, 
									b_arb_channel, 
									a_arb_channel, 
									width, height,
									"Scanline EXR generated by MachStudio.",
									i_Locator);

		// Clean up
		delete [] r_arb_channel;
		delete [] g_arb_channel;
		delete [] b_arb_channel;
		delete [] a_arb_channel;
		r_arb_channel = NULL;
		g_arb_channel = NULL;
		b_arb_channel = NULL;
		a_arb_channel = NULL;
	
	} // If ARGB buffer is found, write out a regular RGBA OpenEXR file
	else if ( pixelFormat == g2dPFD::e_Color ) 
	{
		// Create RGBA array, fill it, and write it to EXR file
		Imf::Rgba * rgbaPixels = new Imf::Rgba[width*height];	
		ColorBufferToEXR_RGBA( (g2dImage*)pImg , rgbaPixels, BYTES_PER_PIX_4 );
		WriteScanlineRGBA(fileName, rgbaPixels, width, height, i_Locator);
		delete [] rgbaPixels;
		rgbaPixels = NULL;
	}

	// Clean up
	//delete [] fileName;
	//fileName = NULL;

#endif //USE_OPENEXR
}

