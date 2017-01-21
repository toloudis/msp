/****************************************************************************\
**  g2dTiledOpenEXRUtil.cpp
**
**  OpenEXR file format support
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

//#include "Graphics/g2d/g2dTiledOpenEXRUtil.hpp"
// 
//
//#include "Core/Fs/fsLocator.hpp"
//#include "Graphics/g2d/g2dImage.hpp"
//#include "Graphics/g3d/g3dConditionalCompile.hpp"
//#include "Graphics/g2d/g2dOpenEXRDefines.hpp"
//#include "Core/fs/fsFileUtil.hpp"
//#include "Core/dbg/dbgMsg.hpp"
//#include "Graphics/g3d/g3dPrefs.hpp"
//
//// OpenEXR includes
//#ifdef USE_OPENEXR
//
//// Using openexr from a dll and not static libs.
//#define OPENEXR_DLL
//
//#include <ImfRgbaFile.h>
//#include <ImfChannelList.h>
//#include <ImfOutputFile.h>
//#include <ImfTiledOutputFile.h>
//#include <ImfTiledRgbaFile.h>
//#include <ImfStringAttribute.h>
//#include <ImfArray.h>
//
//// OpenEXR Libs
//#pragma comment(lib, "IlmImf.lib")
//#pragma comment(lib, "Half.lib") 
//#pragma comment(lib, "Iex.lib")
//#pragma comment(lib, "IlmThread.lib")
//#pragma comment(lib, "Imath.lib")
//#pragma comment(lib, "zdll.lib")
//
//namespace 
//{
//
////--------------------------------------------------------------------
//// Utility function to get filename into char array, from fsLocator
////--------------------------------------------------------------------
//char * getFileName(const fsLocator& i_Locator) 
//{
//	std::string fname;
//	fsFileUtil::LocatorToANSIFilename( i_Locator, fname );
//	int strSize = fname.size();
//	char *fileName = new char[strSize+1];
//	fileName[strSize] = 0;
//	memcpy( fileName , fname.c_str() , strSize );
//	return fileName;
//}
//
////--------------------------------------------------------------------
//// Convert regular 8-bit color buffer to Imf::Rgba, which is what 
//// OpenEXR needs to write regular color images
////--------------------------------------------------------------------
//void ColorBufferToEXR_RGBA(g2dImage* in, Imf::Rgba * out, int numBytesPerPixel) 
//{
//	// Lock surfaces
//	void* imgBits = in->Lock();
//	BYTE* pImgBits = (BYTE*)imgBits; // Format: B, G, R, A
//
//	//Loop through all the pixels
//	int i, j, dst_idx(0);
//	for (i = 0 ; i < in->GetHeight() ; i++) {
//		for (j = 0 ; j < in->GetWidth() ; j++) {
//			int idx = (j*numBytesPerPixel) + (i*in->GetStride());
//			out[dst_idx].r = pImgBits[idx+R_OFFSET_E_COLOR];
//			out[dst_idx].g = pImgBits[idx+G_OFFSET_E_COLOR];
//			out[dst_idx].b = pImgBits[idx+B_OFFSET_E_COLOR];
//			out[dst_idx].a = pImgBits[idx+A_OFFSET_E_COLOR];
//			dst_idx++;
//		}
//	}
//	// Unlock surfaces
//	in->Release();
//}
//
////--------------------------------------------------------------------
//// Convert current RGBA16f buffer to a 2D IMF array of *halfs*
////--------------------------------------------------------------------
//void g2dImageToHalfArray2D(g2dImage* in, Imf::Array2D<Imf::Rgba> &out, int numBytesPerPixel) 
//{
//	// Lock surfaces
//	void* imgBits = in->Lock();
//	BYTE* pImgBits = (BYTE*)imgBits;
//
//	//Loop through all the pixels
//	int i, j;
//	for (i = 0 ; i < in->GetHeight() ; i++) {
//		for (j = 0 ; j < in->GetWidth() ; j++) {
//			int idx = (j*numBytesPerPixel) + (i*in->GetStride());			
//			out[i][j].r = *(half*)(pImgBits + idx + R_OFFSET_RGBAxxf*sizeof(half));
//			out[i][j].g = *(half*)(pImgBits + idx + G_OFFSET_RGBAxxf*sizeof(half));
//			out[i][j].b = *(half*)(pImgBits + idx + B_OFFSET_RGBAxxf*sizeof(half));
//			out[i][j].a = *(half*)(pImgBits + idx + A_OFFSET_RGBAxxf*sizeof(half));
//		}
//	}
//	// Unlock surfaces 
//	in->Release();
//}
//
////--------------------------------------------------------------------
//// Convert current RGBA32f buffer to a 2D IMF array of *floats*
////--------------------------------------------------------------------
//void g2dImageToFloatArray2D(g2dImage* in, Imf::Array2D<floatRgba> &out, int numBytesPerPixel) 
//{
//	// Lock surfaces
//	void* imgBits = in->Lock();
//	BYTE* pImgBits = (BYTE*)imgBits;
//
//	//Loop through all the pixels
//	int i, j;
//	for (i = 0 ; i < in->GetHeight() ; i++) {
//		for (j = 0 ; j < in->GetWidth() ; j++) {
//			int idx = (j*numBytesPerPixel) + (i*in->GetStride());			
//			out[i][j].r = *(float*)(pImgBits + idx + R_OFFSET_RGBAxxf*sizeof(float));
//			out[i][j].g = *(float*)(pImgBits + idx + G_OFFSET_RGBAxxf*sizeof(float));
//			out[i][j].b = *(float*)(pImgBits + idx + B_OFFSET_RGBAxxf*sizeof(float));
//			out[i][j].a = *(float*)(pImgBits + idx + A_OFFSET_RGBAxxf*sizeof(float));
//		}
//	}
//	// Unlock surfaces 
//	in->Release();
//}
//
////--------------------------------------------------------------------
//// Write tiled EXR file with an arbitrary channel of type *float*
////
//// This code comes directly from the OpenEXR document:
//// "Reading and Writing OpenEXR Image Files with the IlmIlf Library"
////--------------------------------------------------------------------
//void WriteTiledArbitraryFloat(const char fileName[],
//						 Imf::Array2D<floatRgba> &pixels,
//						 int width, int height,
//						 int tileWidth, int tileHeight,
//						 const char comments[])
//{
//	Imf::Header header(width, height);									// 1
//	header.insert("comments",Imf::StringAttribute(comments));
//	header.channels().insert("R", Imf::Channel(Imf::FLOAT));// 2
//	header.channels().insert("G", Imf::Channel(Imf::FLOAT));// 3
//	header.channels().insert("B", Imf::Channel(Imf::FLOAT));			
//	header.channels().insert("A", Imf::Channel(Imf::FLOAT));			
//	header.setTileDescription
//		(Imf::TileDescription(tileWidth, tileHeight, Imf::ONE_LEVEL));	// 4
//	
//	Imf::TiledOutputFile out(fileName, header);							// 5
//
//	Imf::FrameBuffer frameBuffer;										// 6
//	frameBuffer.insert("R",									// name // 7
//					   Imf::Slice (Imf::FLOAT,							// type // 8
//					   (char *) &pixels[0][0].r,						// base // 9
//					   sizeof (pixels[0][0]) * 1,						// xStride // 10
//					   sizeof (pixels[0][0]) * width));					// yStride // 11
//	
//	frameBuffer.insert("G",									// name // 12
//					   Imf::Slice (Imf::FLOAT,							// type // 13
//					   (char *) &pixels[0][0].g,						// base // 14
//					   sizeof (pixels[0][0]) * 1,						// xStride // 15
//					   sizeof (pixels[0][0]) * width));					// yStride // 16
//
//	frameBuffer.insert("B",									// name // 12
//					   Imf::Slice (Imf::FLOAT,							// type // 13
//					   (char *) &pixels[0][0].b,						// base // 14
//					   sizeof (pixels[0][0]) * 1,						// xStride // 15
//					   sizeof (pixels[0][0]) * width));					// yStride // 16
//
//	frameBuffer.insert("A",									// name // 12
//					   Imf::Slice (Imf::FLOAT,							// type // 13
//					   (char *) &pixels[0][0].a,						// base // 14
//					   sizeof (pixels[0][0]) * 1,						// xStride // 15
//					   sizeof (pixels[0][0]) * width));					// yStride // 16
//			
//	out.setFrameBuffer(frameBuffer);									// 17
//	
//	for (int tileY = 0; tileY < out.numYTiles (); ++tileY)				// 18
//		for (int tileX = 0; tileX < out.numXTiles (); ++tileX)			// 19
//			out.writeTile (tileX, tileY);								// 20
//}
//
//
//
////--------------------------------------------------------------------
//// Write tiled EXR file with an arbitrary channel of type *half*
////
//// This code comes directly from the OpenEXR document:
//// "Reading and Writing OpenEXR Image Files with the IlmIlf Library"
////--------------------------------------------------------------------
//void WriteTiledArbitraryHalf(const char fileName[],
//						 Imf::Array2D<Imf::Rgba> &pixels,
//						 int width, int height,
//						 int tileWidth, int tileHeight,
//						 const char comments[])
//{
//	Imf::Header header(width, height);									// 1
//	header.insert("comments",Imf::StringAttribute(comments));
//	header.channels().insert("R", Imf::Channel(Imf::HALF));	// 2
//	header.channels().insert("G", Imf::Channel(Imf::HALF));	// 3
//	header.channels().insert("B", Imf::Channel(Imf::HALF));			
//	header.channels().insert("A", Imf::Channel(Imf::HALF));			
//	header.setTileDescription
//		(Imf::TileDescription(tileWidth, tileHeight, Imf::ONE_LEVEL));	// 4
//	
//	Imf::TiledOutputFile out(fileName, header);							// 5
//
//	Imf::FrameBuffer frameBuffer;										// 6
//	frameBuffer.insert("R",									// name // 7
//					   Imf::Slice (Imf::HALF,							// type // 8
//					   (char *) &pixels[0][0].r,						// base // 9
//					   sizeof (pixels[0][0]) * 1,						// xStride // 10
//					   sizeof (pixels[0][0]) * width));					// yStride // 11
//	
//	frameBuffer.insert("G",									// name // 12
//					   Imf::Slice (Imf::HALF,							// type // 13
//					   (char *) &pixels[0][0].g,						// base // 14
//					   sizeof (pixels[0][0]) * 1,						// xStride // 15
//					   sizeof (pixels[0][0]) * width));					// yStride // 16
//
//	frameBuffer.insert("B",									// name // 12
//					   Imf::Slice (Imf::HALF,							// type // 13
//					   (char *) &pixels[0][0].b,						// base // 14
//					   sizeof (pixels[0][0]) * 1,						// xStride // 15
//					   sizeof (pixels[0][0]) * width));					// yStride // 16
//
//	frameBuffer.insert("A",									// name // 12
//					   Imf::Slice (Imf::HALF,							// type // 13
//					   (char *) &pixels[0][0].a,						// base // 14
//					   sizeof (pixels[0][0]) * 1,						// xStride // 15
//					   sizeof (pixels[0][0]) * width));					// yStride // 16
//			
//	out.setFrameBuffer(frameBuffer);									// 17
//	
//	for (int tileY = 0; tileY < out.numYTiles (); ++tileY)				// 18
//		for (int tileX = 0; tileX < out.numXTiles (); ++tileX)			// 19
//			out.writeTile (tileX, tileY);								// 20
//}
//
//
////--------------------------------------------------------------------
//// Write a tiled EXR file with regular 8-bit color information
////
//// This code comes untouched from the OpenEXR document:
//// "Reading and Writing OpenEXR Image Files with the IlmIlf Library"
////--------------------------------------------------------------------
//void WriteTiledRGBA(const char fileName[],
//				   const Imf::Rgba *pixels,
//				   int width, int height,
//				   int tileWidth, int tileHeight)
//{
//	Imf::TiledRgbaOutputFile out (fileName,
//								 width, height,					// image size
//								 tileWidth, tileHeight,			// tile size
//							     Imf::ONE_LEVEL,				// level mode
//								 Imf::ROUND_DOWN,				// rounding mode
//								 Imf::WRITE_RGBA);				// channels in file // 1
//
//	out.setFrameBuffer(pixels, 1, width);						// 2
//
//	for (int tileY = 0; tileY < out.numYTiles (); ++tileY)		// 3
//		for (int tileX = 0; tileX < out.numXTiles (); ++tileX)	// 4
//			out.writeTile(tileX, tileY);						// 5
//}
//
//} // namespace
//
//#endif // USE_OPENEXR
//
//
////--------------------------------------------------------------------
//// Function to invoke different methods of writing tile EXR files.
//// It identifies the current buffer type and writes the 
//// appropriate type of EXR file.
////--------------------------------------------------------------------
//void g2dTiledOpenEXRUtil::WriteToTiledEXR(const fsLocator& i_Locator, const g2dImage* pImg) 
//{
//
//#ifdef USE_OPENEXR
//	// Get image dimensions & file info
//	int width = pImg->GetWidth();
//	int height = pImg->GetHeight();
//	char * fileName = getFileName(i_Locator);
//	int pixelFormat = pImg->GetPixelFormat().GetPixelFormat();
//
//	// If RGBA16f buffer is found, write out a RGBA16 FP OpenEXR file
//	if ( pixelFormat == g2dPFD::e_RGBA16f ) 
//	{
//		// Declare IMF-proprietary Array2D
//		Imf::Array2D<Imf::Rgba> pixels(height,width);
//
//		// Fill buffers with motion data from the RGBA16f D3D surface
//		g2dImageToHalfArray2D( (g2dImage*)pImg, pixels, BYTES_PER_PIX_8);
//
//		// Write out OpenEXR file with half channels
//		WriteTiledArbitraryHalf( fileName, pixels, width, height, TILE_SIZE_X, TILE_SIZE_Y ,
//								 "Tiled EXR generated by MachStudio." );
//	
//	} // If RGBA32f buffer is found, write out a RGBA32 FP OpenEXR file
//	else if ( pixelFormat == g2dPFD::e_RGBA32f ) 
//	{
//		// Declare IMF-proprietary Array2D
//		Imf::Array2D<floatRgba> pixels(height,width);
//
//		// Fill buffers with motion data from the RGBA32f D3D surface
//		g2dImageToFloatArray2D( (g2dImage*)pImg, pixels, BYTES_PER_PIX_16);
//
//		// Write out OpenEXR file with float channels
//		WriteTiledArbitraryFloat( fileName, pixels, width, height, TILE_SIZE_X, TILE_SIZE_Y ,
//								  "Tiled EXR generated by MachStudio." );
//	
//	} // If ARGB buffer is found, write out a regular RGBA OpenEXR file
//	else if ( pixelFormat == g2dPFD::e_Color ) 
//	{	
//		// Create RGBA array, fill it, and write it to EXR file
//		Imf::Rgba * rgbaPixels = new Imf::Rgba[width*height];	
//		ColorBufferToEXR_RGBA( (g2dImage*)pImg , rgbaPixels, BYTES_PER_PIX_4 );
//		WriteTiledRGBA(fileName, rgbaPixels, width, height, TILE_SIZE_X, TILE_SIZE_Y);
//		delete [] rgbaPixels;
//		rgbaPixels = NULL;
//	}
//
//	// Clean up
//	delete [] fileName;
//	fileName = NULL;
//
//#endif //USE_OPENEXR
//}
