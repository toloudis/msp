/****************************************************************************\
**  matTextureLoaderDDSWin.hpp
**
**      matTextureLoaderDDSWin.hpp contains some helper functions for loading
**	bitmap data to surfaces in windows.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTURELOADERDDSWIN_HPP
#error matTextureLoaderDDSWin.hpp multiply included
#endif
#define MAT_TEXTURELOADERDDSWIN_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif
#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif

#include <vector>

namespace matTextureLoaderDDS
{
	// The types of compressed textures that can be created by this namespace
	typedef enum
	{
		e_VolumeTexture,
		e_CubeTexture,
		e_MipTexture,

	} eTextureType;

	// The file information provided in the compressed texture file
	struct DDSInfo
	{
		fsLocator		s_Locator;	// the locator to the file itself
		int				s_eType;
		int				s_nWidth;		// the loaded width (based on s_nWidthReduce)
		int				s_nHeight;		// the loaded height (based on s_nHeightReduce)
		bool			s_bHasAlpha;
		g2dPFD			s_Format;
		int				s_nMipCount;
		int				s_nWidthReduce;
		int				s_nHeightReduce;
	};

	//------------------------------------------------------------------------
	// Loads a compressed .DDS file that will be decompressed by hardware
	// if the device supports it, otherwise in memory like any other graphic.
	//
	// This will return a FileInfo structure filled in with information about the
	// file that may be of interest to the caller.  Most specifically it tells
	// the caller exactly what type of pointer o_pTexture really is.
	//
	// Note:  This call will create the texture in DirectX, the caller is
	// responsible for cleaning up the returned pointer!!!!
	//------------------------------------------------------------------------
	ID3D11Resource* LoadDDS( struct DDSInfo& o_Info, const fsLocator& i_Locator, 
		int i_WidthReduce, int i_HeightReduce,
		bool mipmap_if_2D = true);
}
