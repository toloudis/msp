/****************************************************************************\
**  g2dTextureVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g2dTextureDX11: a texture image
**	shaders read (vkdImage), with the DXGI format of its data, which the
**	texture classes report their g2dPFD from (vkdFormat may have
**	substituted the Vulkan format).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G2DTEXTUREVK_HPP
#error g2dTextureVk.hpp multiply included
#endif
#define GVK_G2DTEXTUREVK_HPP

#ifndef GVK_G2DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g2dStubsVk.hpp"
#endif

#include <cstdint>
#include <memory>

namespace gvk
{
	class vkdImage;

	class g2dTextureVk
	{
	public:
		g2dTextureVk();
		virtual ~g2dTextureVk();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format (one mip).
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, uint32_t i_DxgiFormat,
			g2dResourceCounterVk::eResourceCategory i_Category = g2dResourceCounterVk::eTexture);

		//--------------------------------------------------------------------
		// This object will take ownership of the image! i_DxgiFormat is the
		// format of its data.
		//--------------------------------------------------------------------
		void Make(std::unique_ptr<vkdImage> i_pImage, uint32_t i_DxgiFormat,
			g2dResourceCounterVk::eResourceCategory i_Category = g2dResourceCounterVk::eTexture);

		vkdImage* GetImage() const { return m_pImage.get(); }

		int GetWidth() const {return m_Width;}
		int GetHeight() const {return m_Height;}
		int GetMipLevels() const {return m_MipLevels;}
		uint32_t GetFormat() const {return m_Format;}

	private:
		g2dResourceCounterVk::eResourceCategory m_Category;
		short m_Width, m_Height;
		uint32_t m_Format;
		int m_MipLevels;
		std::unique_ptr<vkdImage> m_pImage;
	};

	//	The usage every texture image gets: sampled, written from the CPU and
	//	by copies, read back and copied from
	extern const uint32_t c_TextureUsage;

	//	The number of mips in a full chain down to 1x1 (D3D11's MipLevels 0)
	uint32_t g2dFullMipCount(uint32_t i_Width, uint32_t i_Height, uint32_t i_Depth = 1);
}
