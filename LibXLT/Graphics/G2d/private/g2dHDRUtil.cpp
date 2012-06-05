/****************************************************************************\
**  g2dHDRUtil.cpp
**
**  HDR file format support
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "Graphics/g2d/g2dHDRUtil.hpp"


#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g2d/g2dOpenEXRDefines.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g2d/private/rgbe.h"

//#include "half.h"

//============================================================================
//============================================================================
namespace 
{
//--------------------------------------------------------------------
// Convert current RGBA32f buffer to an array of RGB *floats*
//--------------------------------------------------------------------
void g2dImageToRGBFloatArray(g2dImage* in, float * out) 
{
	int w, h;
	g2dPixelRGBA32F* buf;
	in->GetPixels(&buf, &w, &h);

	//Loop through all the pixels
	int i, j, dst_idx(0);
	for (i = in->GetHeight() - 1 ; i >=0 ; i--) 
	{
		for (j = 0 ; j < in->GetWidth() ; j++) 
		{
			out[dst_idx] = buf[i * w + j].r;
			out[dst_idx+1] = buf[i * w + j].g;
			out[dst_idx+2] = buf[i * w + j].b;
			dst_idx+=3;
		}
	}
	delete [] buf;
}
}


namespace g2dHDRUtil
{

//--------------------------------------------------------------------
// Function to write HDR files
//--------------------------------------------------------------------
void WriteToHDR( const fsLocator& i_Locator, const g2dImage* pImg )
{
	// Get image dimensions & file info
	int width = pImg->GetWidth();
	int height = pImg->GetHeight();

	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_Locator, filename);

	try
	{
		FILE* fp = ::_wfopen(filename.GetString(), L"wb");
		RGBE_WriteHeader(fp, width, height, NULL);
		
		float* buffer = new float[width * height * 3];
		memset(buffer, 0, sizeof(float) * width * height * 3);

		g2dImageToRGBFloatArray(const_cast<g2dImage*>(pImg), buffer);
		
		RGBE_WritePixels(fp, buffer,width * height);
		::fclose(fp);

		delete [] buffer;
	}catch(...)
	{
		DBG_ERROR("Errors occurrs while writing HDR files");
	}
	
}

}