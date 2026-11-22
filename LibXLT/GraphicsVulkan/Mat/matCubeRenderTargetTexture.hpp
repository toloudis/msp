/****************************************************************************\
**  matCubeRenderTargetTexture.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matCubeRenderTargetTexture: a
**	cube texture (6 faces, full mip chain) whose faces can be rendered
**	to, one at a time (SetFaceTarget), with one depth-stencil buffer.
**	Mips beyond 0 are filled by whoever renders it (the cube map
**	renderer, Plan 5); the image starts as zeros, as D3D11's does.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATCUBERENDERTARGETTEXTURE_HPP
#error matCubeRenderTargetTexture.hpp multiply included
#endif
#define GVK_MATCUBERENDERTARGETTEXTURE_HPP

#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif

#include <memory>

namespace gvk
{
	class matCubeRenderTargetTexture : public matRenderTargetTexture
	{
	public:
		//--------------------------------------------------------------------
		//	This constructor will not typically be used by the mat client,
		//	since textures are loaded and managed by the matTextureMgr.
		//--------------------------------------------------------------------
		matCubeRenderTargetTexture();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~matCubeRenderTargetTexture();

		//--------------------------------------------------------------------
		//	ReloadInfo causes the matCubeRenderTargetTexture to regenerate its
		//	parameters from the image.
		//--------------------------------------------------------------------
		void ReloadInfo();

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format.  i_bAllocDepthBuf is ignored if m_bStoreDepths is true.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, bool i_bAllocDepthBuf=true);

		//------------------------------------------------------------------------
		//	Clear fills the screen with the given color.
		//------------------------------------------------------------------------
		virtual void Clear(const g2dRGBColor& i_Color);

		//------------------------------------------------------------------------
		//	Clear fills the screen with the given color and sets depth and stencil
		//------------------------------------------------------------------------
		virtual void Clear( const maFloatRGBA& i_Color, bool i_ClearDepth = false, float i_Depth = 1,
			bool i_ClearStencil = true, unsigned int i_Stencil = 0);

		//------------------------------------------------------------------------
		// set one of the 6 faces as the current render target.
		//------------------------------------------------------------------------
		void SetFaceTarget(int i_CubeFace);

		//--------------------------------------------------------------------
		//	GetCubeTextureSurface returns the cube image.
		//--------------------------------------------------------------------
		vkdImage* GetCubeTextureSurface() { return m_pCubeImage.get(); }

		//----------------------------------------------------------------------------
		//	GetSize returns approximate amount of memory (in kbytes) being
		// used by this texture
		//----------------------------------------------------------------------------
		virtual float GetSize() const;

		//--------------------------------------------------------------------
		//	GetImage returns the cube image, usable for shaders.
		//--------------------------------------------------------------------
		virtual vkdImage* GetImage() const;

	protected:
		//--------------------------------------------------------------------
		// set up buffers for rendering
		//--------------------------------------------------------------------
		virtual void configure_device();

	private:

		//--------------------------------------------------------------------
		//	This operator = is not implemented.  It is placed here in private
		//	to prevent its usage.
		//--------------------------------------------------------------------
		matCubeRenderTargetTexture& operator = (const matCubeRenderTargetTexture& i_Image);

		//	mip 0 of face i_CubeFace (D3D11's render target views)
		vkdImage* face(int i_CubeFace) const;

		int m_CurrentFaceTarget;
		std::unique_ptr<vkdImage> m_pCubeImage;
		uint32_t m_DxgiFormat;
	};
}
