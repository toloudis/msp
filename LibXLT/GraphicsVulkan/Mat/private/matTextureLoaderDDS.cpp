/****************************************************************************\
**  matTextureLoaderDDS.cpp
**
**	See matTextureLoader.hpp: the port of matTextureLoaderDDSWin. The file
**	is decoded by matImageDecode (DirectXTex's DDS reader and legacy
**	conversions, and DX11's own CxV8U8 conversion) and becomes an image as
**	it is.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATTEXTURELOADER_HPP
#include "GraphicsVulkan/Mat/private/matTextureLoader.hpp"
#endif
#ifndef GVK_MATIMAGEDECODE_HPP
#include "GraphicsVulkan/Mat/matImageDecode.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_RESOURCETRACKER_HPP
#include "Core/fs/fsResourceTracker.hpp"
#endif
#include <string>

namespace gvk
{
namespace
{
	//------------------------------------------------------------------------
	// load_dds_data from the file.
	//------------------------------------------------------------------------
	std::unique_ptr<vkdImage> load_dds_data( struct matTextureLoaderDDS::DDSInfo& io_Info, uint32_t& o_DxgiFormat )
	{
		fsResourceTracker::MarkBegin(io_Info.s_Locator);

		std::string txfile;
		fsFileUtil::LocatorToANSIFilename(io_Info.s_Locator, txfile);

		matDecodedImage decoded;
		std::string error;
		if (!matDecodeImage(io_Info.s_Locator, matImageFileType::e_DDS, decoded, &error))
		{
			DBG_WARNING("A non-DDS file made it into load_dds_data: " << txfile << " (" << error << ")");
			fsResourceTracker::Remove(io_Info.s_Locator);
			return NULL;
		}

		const bool cm = decoded.m_bCube;
		const bool vol = decoded.m_Depth > 1;

		std::unique_ptr<vkdImage> pTexture = matTextureLoader::CreateImage(decoded, txfile.c_str());
		//	check for errors loading
		if ( !pTexture )
		{
			fsResourceTracker::Remove(io_Info.s_Locator);
			if (cm)
				DBG_ERROR("error creating compressed cubemap texture: " << txfile);
			else if (vol)
				DBG_ERROR("error creating compressed volume texture: " << txfile);
			else
				DBG_ERROR("error creating compressed texture: " << txfile);
		}

		if (vol)
			io_Info.s_eType = matTextureLoaderDDS::e_VolumeTexture;
		else if (cm)
			io_Info.s_eType = matTextureLoaderDDS::e_CubeTexture;
		else
			io_Info.s_eType = matTextureLoaderDDS::e_MipTexture;

		io_Info.s_nWidth = decoded.m_Width;
		io_Info.s_nHeight = decoded.m_Height;
		io_Info.s_nMipCount = decoded.m_Mips;
		g2dVkGlobal::PFDFromDxgiFormat(decoded.m_DxgiFormat, io_Info.s_Format);
		io_Info.s_bHasAlpha = io_Info.s_Format.NumAlphaBits() != 0;
		o_DxgiFormat = decoded.m_DxgiFormat;

		if (pTexture != NULL)
			fsResourceTracker::MarkEnd(io_Info.s_Locator);

		return pTexture;
	}
}

namespace matTextureLoaderDDS
{
	//------------------------------------------------------------------------
	// Loads a .DDS file as it is.
	//------------------------------------------------------------------------
	std::unique_ptr<vkdImage> LoadDDS( struct DDSInfo& o_Info, const fsLocator& i_Locator, int i_WidthReduce, int i_HeightReduce,
		uint32_t& o_DxgiFormat, bool mipmap_if_2D /*=true*/)
	{
		//	fill-in part of the info structure
		//
		o_Info.s_Locator		= i_Locator;
		o_Info.s_nWidthReduce	= i_WidthReduce;
		o_Info.s_nHeightReduce	= i_HeightReduce;

		return load_dds_data( o_Info, o_DxgiFormat );
	}
}
}
