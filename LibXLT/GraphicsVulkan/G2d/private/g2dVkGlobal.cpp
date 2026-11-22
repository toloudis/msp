/*****************************************************************************
**  g2dVkGlobal.cpp
**
**      See g2dVkGlobal.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
namespace g2dVkGlobal
{

//----------------------------------------------------------------------------
//	From the block size vkdFormat knows (4 for BC1, as DX11's table)
//----------------------------------------------------------------------------
uint32_t BitsPerPixel(uint32_t i_DxgiFormat)
{
	const vkdFormatInfo info = vkdFormatInfoOf(vkdFormatFromDxgi(i_DxgiFormat));
	return info.m_BlockBytes * 8 / (info.m_BlockWidth * info.m_BlockHeight);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void PFDFromDxgiFormat(uint32_t i_Format, g2dPFD& o_PFD)
{
	// set up some basic guess first
	o_PFD.SetPixelFormat(g2dPFD::e_Color);
	o_PFD.SetBitsPerPixel(BitsPerPixel( i_Format ));

	// now look for formats we want to specifically recognize
	switch( i_Format )
	{
		case e_DXGI_R8G8B8A8_UNORM:
		case e_DXGI_R8G8B8A8_UNORM_SRGB:
			o_PFD.Set(	0,	8,
						8,	8,
						16,	8,
						24,	8,
						32);
		break;
		// TODO: this is a hack, the best approximation we've come up w/ for this
		// NOTE: this means that all DXT3 files are assumed to have alpha in them,
		//	whether they really do or not
		// DXT2 and DXT4 are the same as DXT3 and DXT5, except that they are
		// interpreted as having premultiplied alpha in the rgb colors.
		// We don't really support premultiplied alpha in our shading system.
		case e_DXGI_BC1_UNORM:  // fall through
		case e_DXGI_BC1_UNORM_SRGB:  // fall through
			o_PFD.Set(	16,	8,
						8,	8,
						0,	8,
						0,	0,
			// DXT compression results in 4 bpp storage.
						4);
			break;

		case e_DXGI_BC2_UNORM:  // fall through
		case e_DXGI_BC2_UNORM_SRGB:

		case e_DXGI_BC3_UNORM:
		case e_DXGI_BC3_UNORM_SRGB:
			o_PFD.Set(	16,	8,
						8,	8,
						0,	8,
						24,	8,
			// DXT compression results in 8 bpp storage.
						8);
			break;

		case e_DXGI_R16G16_SNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_BumpMapV16U16 );
			o_PFD.SetBitsPerPixel(32);
			break;

		case e_DXGI_R8_UNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_Luminance8 );
			o_PFD.SetBitsPerPixel(8);
			break;

		case e_DXGI_D16_UNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_Depth16 );
			o_PFD.SetBitsPerPixel(16);
			break;

		case e_DXGI_R32_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_Float32 );
			o_PFD.SetBitsPerPixel(32);
			break;

		case e_DXGI_R16_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_Float16 );
			o_PFD.SetBitsPerPixel(16);
			break;

		case e_DXGI_R16G16B16A16_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_RGBA16f );
			o_PFD.SetBitsPerPixel(64);
			o_PFD.Set(	0,	16,
						16,	16,
						32,	16,
						48,	16,
						64);
			break;
		case e_DXGI_R16G16B16A16_UNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_RGBA16UInt );
			o_PFD.SetBitsPerPixel(64);
			o_PFD.Set(	0,	16,
						16,	16,
						32,	16,
						48,	16,
						64);
			break;
		case e_DXGI_R32G32B32A32_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_RGBA32f );
			o_PFD.SetBitsPerPixel(128);
			break;

		case e_DXGI_R32G32_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_GR32f );
			o_PFD.SetBitsPerPixel(64);
			break;

		case e_DXGI_D24_UNORM_S8_UINT:
			o_PFD.SetPixelFormat( g2dPFD::e_Depth24Stencil8 );
			o_PFD.SetBitsPerPixel(32);
			break;

		case e_DXGI_D32_FLOAT:
			o_PFD.SetPixelFormat(g2dPFD::e_Depth32f);
			o_PFD.SetBitsPerPixel(32);
			break;

		case e_DXGI_B8G8R8X8_UNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_RColor );
			o_PFD.SetBitsPerPixel(32);
			break;

		default:
			o_PFD.SetPixelFormat( g2dPFD::e_Unknown );
			DBG_WARNING("PFDFromD3DFormat: Unsupported D3D texure format " << i_Format);
		break;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
uint32_t DxgiFormatFromPFD(const g2dPFD& i_PFD)
{
	int format = i_PFD.GetPixelFormat();

	if ( format == g2dPFD::e_Color )
	{
		switch( i_PFD.BitsPerPixel() )
		{
			case 32:
				return e_DXGI_R8G8B8A8_UNORM;
			break;
		}
	}
	if ( format == g2dPFD::e_RColor )
	{
		switch( i_PFD.BitsPerPixel() )
		{
		case 32:
			return e_DXGI_B8G8R8X8_UNORM;
			break;
		}
	}
	else if ( format == g2dPFD::e_BumpMapV8U8 )
	{
		return 	e_DXGI_R8G8_SNORM;
	}
	else if ( format == g2dPFD::e_BumpMapV16U16 )
	{
		return e_DXGI_R16G16_SNORM;
	}
	else if (format == g2dPFD::e_RGBA16f )
	{
		return e_DXGI_R16G16B16A16_FLOAT;
	}
	else if (format == g2dPFD::e_RGBA16UInt)
	{
		return e_DXGI_R16G16B16A16_UNORM;
	}
	else if (format == g2dPFD::e_RGBA32f )
	{
		return e_DXGI_R32G32B32A32_FLOAT;
	}
	else if (format == g2dPFD::e_GR32f )
	{
		return e_DXGI_R32G32_FLOAT;
	}
	else if (format == g2dPFD::e_Float16 )
	{
		return e_DXGI_R16_FLOAT;
	}
	else if (format == g2dPFD::e_Float32 )
	{
		return e_DXGI_R32_FLOAT;
	}
	else if (format == g2dPFD::e_Luminance8 )
	{
		return e_DXGI_R8_UNORM;
	}


	DBG_ERROR("Unknown g2dPFD format " << format);
	return e_DXGI_UNKNOWN;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool RestrictPow2Textures()
{
	return false;
}

//----------------------------------------------------------------------------
//	HasStencil
//----------------------------------------------------------------------------
bool HasStencil()
{
	return true;
}

//----------------------------------------------------------------------------
// choose highest quality AA format available
//----------------------------------------------------------------------------
VkSampleCountFlagBits ChooseSampleCount(VkFormat i_Format)
{
	vkdDevice& device = vkdGlobal::Device();
	VkImageFormatProperties color = {}, depth = {};
	if (vkGetPhysicalDeviceImageFormatProperties(device.Physical(), i_Format, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL,
												 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 0, &color) != VK_SUCCESS ||
		vkGetPhysicalDeviceImageFormatProperties(device.Physical(), VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL,
												 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 0, &depth) != VK_SUCCESS)
	{
		DBG_WARNING("Multisampled rendertarget requested but multisampling not supported.");
		return VK_SAMPLE_COUNT_1_BIT;
	}
	// D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT down to 1
	const VkSampleCountFlags supported = color.sampleCounts & depth.sampleCounts;
	for (uint32_t count = VK_SAMPLE_COUNT_32_BIT; count > VK_SAMPLE_COUNT_1_BIT; count >>= 1)
	{
		if (supported & count)
		{
			return (VkSampleCountFlagBits)count;
		}
	}
	DBG_WARNING("Multisampled rendertarget requested but multisampling not supported.");
	return VK_SAMPLE_COUNT_1_BIT;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SetRenderTargets(vkdImage* i_pColor, vkdImage* i_pDepth)
{
	vkdGlobal::Context().SetRenderTargets(i_pColor ? 1 : 0, &i_pColor, i_pDepth);
}

void SetColorTarget(vkdImage* i_pColor)
{
	SetRenderTargets(i_pColor, GetDepthTarget());
}

void SetDepthTarget(vkdImage* i_pDepth)
{
	SetRenderTargets(GetColorTarget(), i_pDepth);
}

vkdImage* GetColorTarget()
{
	vkdImage* colors[8] = {};
	vkdImage* depth = NULL;
	return vkdGlobal::Context().GetRenderTargets(colors, &depth) > 0 ? colors[0] : NULL;
}

vkdImage* GetDepthTarget()
{
	vkdImage* colors[8] = {};
	vkdImage* depth = NULL;
	vkdGlobal::Context().GetRenderTargets(colors, &depth);
	return depth;
}

void SetViewport(int i_Width, int i_Height)
{
	vkdViewport viewport;
	viewport.m_Width = (float)i_Width;
	viewport.m_Height = (float)i_Height;
	vkdGlobal::Context().SetViewports(1, &viewport);
}

//----------------------------------------------------------------------------
//	assume the current targets are the ones to clear
//----------------------------------------------------------------------------
void ClearCurrent(const float i_Rgba[4], bool i_ClearDepth, float i_Depth, bool i_ClearStencil, unsigned int i_Stencil)
{
	vkdContext& context = vkdGlobal::Context();
	if (vkdImage* color = GetColorTarget())
	{
		context.ClearRenderTarget(color, i_Rgba);
	}
	vkdImage* depth = GetDepthTarget();
	if (i_ClearDepth && depth)
	{
		context.ClearDepthStencil(depth, true, i_Depth, i_ClearStencil, (uint8_t)i_Stencil);
	}
}

}
}
