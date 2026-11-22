/****************************************************************************\
**  g2dRenderTargetVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g2dRenderTargetDX11: a render
**	target (colour, multisampled if asked, and a depth-stencil buffer)
**	that isn't a texture; Resolve copies it into a render target texture.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G2DRENDERTARGETVK_HPP
#error g2dRenderTargetVk.hpp multiply included
#endif
#define GVK_G2DRENDERTARGETVK_HPP

#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif

#include <memory>

namespace gvk
{
	class matRenderTargetTexture;
	class g2dDepthStencilBufferVk;
	class vkdImage;

	class g2dRenderTargetVk : public g2dRenderTarget
	{
	public:
		g2dRenderTargetVk();
		virtual ~g2dRenderTargetVk();

		//------------------------------------------------------------------------
		// BeginScene must be called before rendering to this window
		//------------------------------------------------------------------------
		virtual void BeginScene();

		//------------------------------------------------------------------------
		// similar to BeginScene, call this to make this the current target of all
		// subsequent rendering calls.  Need not be followed by EndScene.
		//------------------------------------------------------------------------
		virtual void MakeCurrent();

		//------------------------------------------------------------------------
		// call this to make this depth buffer the current depth buffer for all
		// subsequent rendering calls.
		//------------------------------------------------------------------------
		virtual void MakeDepthCurrent();

		//------------------------------------------------------------------------
		//	EndScene must be called when you are done with the drawing operations
		//	on the current frame.  It will cause whatever drawing you have
		//	requested to be visible on the screen.
		//------------------------------------------------------------------------
		virtual void EndScene();

		//------------------------------------------------------------------------
		//	Clear fills the screen with the given color.
		//------------------------------------------------------------------------
		virtual void Clear(const g2dRGBColor& i_Color);

		//------------------------------------------------------------------------
		//	Clear fills the screen with the given color and sets depth and stencil
		//------------------------------------------------------------------------
		virtual void Clear( const maFloatRGBA& i_Color, bool i_ClearDepth = false, float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0);

		//------------------------------------------------------------------------
		//	Clear fills the depthstencil buffer with the given values.
		//	If there is no depthstencil, then this does nothing.
		//------------------------------------------------------------------------
		virtual void ClearDepthStencil(float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0);

		//------------------------------------------------------------------------
		//	GetPixelFormat returns the current pixel format of the render target.
		//------------------------------------------------------------------------
		virtual const g2dPFD& GetPixelFormat() const;

		//------------------------------------------------------------------------
		//	GetDimensions returns the width and height of the render target.
		//------------------------------------------------------------------------
		virtual void GetDimensions(int& o_Width, int& o_Height) const;

		//--------------------------------------------------------------------
		//	Make makes the surface into one with the given dimensions and
		//	pixel format.
		//--------------------------------------------------------------------
		void Make(int i_Width, int i_Height, const g2dPFD& i_PFD, bool i_AntiAlias = false);

		//------------------------------------------------------------------------
		//	Resolve will copy the contents to the given texture, resolving
		//	the multisample surface to a single sampled one.
		//------------------------------------------------------------------------
		void Resolve(matRenderTargetTexture* o_pTexture);

		vkdImage* GetColorView() const { return m_pTarget.get(); }

		//------------------------------------------------------------------------
		//	Returns true if this object has allocated a depth buffer.
		//------------------------------------------------------------------------
		virtual bool GetHasDepthBuffer() const;

		//------------------------------------------------------------------------
		//	GetDepthStencilBuffer accesses the depth/stencil buffer
		//------------------------------------------------------------------------
		virtual shared_ptr<g2dDepthStencilBuffer> GetDepthStencilBuffer() const;

	private:
		short m_Width, m_Height;
		g2dPFD m_PFD;
		std::unique_ptr<vkdImage> m_pTarget;
		shared_ptr<g2dDepthStencilBufferVk> m_DepthStencil;

		//	The KB counted (g2dRenderTargetDX11::GetSize)
		float GetSize();

		//------------------------------------------------------------------------
		// set up device to render to our window
		//------------------------------------------------------------------------
		void configure_device();
	};
}
