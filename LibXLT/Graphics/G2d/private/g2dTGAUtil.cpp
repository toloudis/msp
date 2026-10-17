/****************************************************************************\
**  g2dTGAUtil.cpp
**
**  TGA file format support
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "Graphics/g2d/g2dTGAUtil.hpp"


#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g2d/g2dOpenEXRDefines.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g2d/private/tga.h"

#include <filesystem>
#include <fstream>


//============================================================================
//============================================================================
namespace 
{
}


namespace g2dTGAUtil
{

//--------------------------------------------------------------------
// Function to write HDR files
//--------------------------------------------------------------------
void WriteToTGA( const fsLocator& i_Locator, const g2dImage* pImg )
{
	// Get image dimensions & file info
	int width;
	int height;

	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_Locator, filename);

	try
	{
		std::ofstream file(std::filesystem::path(filename.GetString()), std::ios::binary);
		if (!file)
		{
			DBG_ERROR("Unable to open TGA file for writing");
			return;
		}
		
		g2dPixelR8G8B8A8* buffer = NULL;
		(const_cast<g2dImage*>(pImg))->GetPixels(&buffer, &width, &height);
		TGA_Write_RGBA(file, width, height, (uint32_t**)(&buffer));

		delete [] buffer;
	}catch(...)
	{
		DBG_ERROR("Errors occurrs while writing TGA files");
	}
	
}

}