/****************************************************************************\
**  matTextureVk.cpp
**
**	See matTextureVk.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <vector>

namespace gvk
{
	//--------------------------------------------------------------------
	//	This constructor makes an "empty" surface.
	//--------------------------------------------------------------------
	matTextureVk::matTextureVk()
	{
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matTextureVk::~matTextureVk()
	{
	}

	//--------------------------------------------------------------------
	//	MakeTransparencyMap will build a transparency map for this texture
	//	i_Bias specifies the range (0-i_Bias) that will be considered transparent
	//--------------------------------------------------------------------
	char * matTextureVk::MakeTransparencyMap(float i_Bias, const g2dPFD& i_PFD, int i_Width, int i_Height)
	{
		DBG_ERROR("not implemented");
		return NULL;
	}

	//--------------------------------------------------------------------
	//	big assumption: initial data is packed tightly and pitch
	//	is related directly to pixel size for this format
	//--------------------------------------------------------------------
	void matTextureVk::Upload(vkdImage* io_pImage, uint32_t i_DxgiFormat, const void* i_pData, int i_Width, int i_Height,
							  int i_Depth, uint32_t i_Mip, uint32_t i_Layer)
	{
		const size_t row_pitch = (size_t)g2dVkGlobal::BitsPerPixel(i_DxgiFormat) * i_Width / 8;
		const size_t slice_pitch = row_pitch * i_Height;
		if (i_DxgiFormat == g2dVkGlobal::e_DXGI_B8G8R8X8_UNORM)
		{
			std::vector<uint8_t> bytes(static_cast<const uint8_t*>(i_pData), static_cast<const uint8_t*>(i_pData) + slice_pitch * i_Depth);
			for (size_t i = 3; i < bytes.size(); i += 4)
			{
				bytes[i] = 255;
			}
			io_pImage->Upload(vkdGlobal::Context(), i_Mip, i_Layer, bytes.data(), row_pitch, slice_pitch);
			return;
		}
		io_pImage->Upload(vkdGlobal::Context(), i_Mip, i_Layer, i_pData, row_pitch, slice_pitch);
	}
}
