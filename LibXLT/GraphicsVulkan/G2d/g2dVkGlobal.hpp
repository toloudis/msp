/*****************************************************************************
**  g2dVkGlobal.hpp
**
**      The pixel format helpers of GraphicsDX11's g2dDX11Global, on DXGI
**	format numbers (plain uint32_t: GraphicsVulkan doesn't include
**	dxgiformat.h): g2dPFD to and from a DXGI format, bits per pixel, the
**	power-of-two rule and the multisample choice. Textures keep the DXGI
**	format of their data (vkdFormat maps it to Vulkan), so a texture's
**	g2dPFD comes out as DX11's does.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DVKGLOBAL_HPP
#error g2dVkGlobal.hpp multiply included
#endif
#define GVK_G2DVKGLOBAL_HPP

#include <volk.h>

#include <cstdint>

class g2dPFD;

namespace gvk
{
	class vkdImage;

	namespace g2dVkGlobal
	{
		//	The DXGI formats this code names (DXGI_FORMAT values)
		enum DxgiFormat : uint32_t
		{
			e_DXGI_UNKNOWN				= 0,
			e_DXGI_R32G32B32A32_FLOAT	= 2,
			e_DXGI_R16G16B16A16_FLOAT	= 10,
			e_DXGI_R16G16B16A16_UNORM	= 11,
			e_DXGI_R32G32_FLOAT			= 16,
			e_DXGI_R8G8B8A8_UNORM		= 28,
			e_DXGI_R8G8B8A8_UNORM_SRGB	= 29,
			e_DXGI_R16G16_SNORM			= 37,
			e_DXGI_D32_FLOAT			= 40,
			e_DXGI_R32_FLOAT			= 41,
			e_DXGI_D24_UNORM_S8_UINT	= 45,
			e_DXGI_R8G8_SNORM			= 51,
			e_DXGI_R16_FLOAT			= 54,
			e_DXGI_D16_UNORM			= 55,
			e_DXGI_R8_UNORM				= 61,
			e_DXGI_BC1_UNORM			= 71,
			e_DXGI_BC1_UNORM_SRGB		= 72,
			e_DXGI_BC2_UNORM			= 74,
			e_DXGI_BC2_UNORM_SRGB		= 75,
			e_DXGI_BC3_UNORM			= 77,
			e_DXGI_BC3_UNORM_SRGB		= 78,
			e_DXGI_B8G8R8X8_UNORM		= 88,
		};

		//	g2dDX11Global::BitsPerPixel: 0 for a format vkdFormat doesn't know
		uint32_t BitsPerPixel(uint32_t i_DxgiFormat);

		//	g2dDX11Global::PFDFromD3DFormat
		void PFDFromDxgiFormat(uint32_t i_DxgiFormat, g2dPFD& o_PFD);

		//	g2dDX11Global::D3DFormatFromPFD (e_DXGI_UNKNOWN, with an error
		//	logged, for a pixel format it doesn't know)
		uint32_t DxgiFormatFromPFD(const g2dPFD& i_PFD);

		//	g2dDX11Global::RestrictPow2Textures (false, as in DX11)
		bool RestrictPow2Textures();

		//	g2dDX11Global::g_bHasStencil (true once DX11's primary window has
		//	its D24S8 depth buffer): every Vulkan depth buffer is D24S8
		bool HasStencil();

		//	g2dDX11Global::ChooseMultisampleQuality: the most samples the
		//	device supports for colour images of i_Format (and D24S8 depth),
		//	up to D3D11's largest count (32); 1 if none
		VkSampleCountFlagBits ChooseSampleCount(VkFormat i_Format);

		//	g2dDX11Global's current targets, on vkdContext: one colour target
		//	and a depth target (NULL = none). SetColorTarget keeps the depth
		//	target, SetDepthTarget the colour target.
		void SetRenderTargets(vkdImage* i_pColor, vkdImage* i_pDepth);
		void SetColorTarget(vkdImage* i_pColor);
		void SetDepthTarget(vkdImage* i_pDepth);
		vkdImage* GetColorTarget();
		vkdImage* GetDepthTarget();

		//	A viewport over (0, 0, i_Width, i_Height), depth 0..1
		void SetViewport(int i_Width, int i_Height);

		//	g2dWindowDrawUtilDX11::Clear: clears the current colour target
		//	and, if asked, the current depth target (and its stencil)
		void ClearCurrent(const float i_Rgba[4], bool i_ClearDepth = false, float i_Depth = 1,
						  bool i_ClearStencil = true, unsigned int i_Stencil = 0);
	}
}
