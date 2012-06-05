/****************************************************************************\
**  g2dRenderTarget.hpp
**
**      g2dRenderTarget.hpp defines the base class for things that can
**	be rendered to.  This includes a window in an application or
**	an offscreen buffer or texture.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_RENDERTARGET_HPP
#error g2dRenderTarget.hpp multiply included
#endif
#define G2D_RENDERTARGET_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

//============================================================================
//============================================================================
class g2dPFD;
class g2dRGBColor;
class maFloatRGBA;
class g2dDepthStencilBuffer;

//============================================================================
//============================================================================
class g2dRenderTarget
{
protected:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g2dRenderTarget();

public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~g2dRenderTarget();

	//------------------------------------------------------------------------
	// BeginScene must be called before rendering to this window
	//------------------------------------------------------------------------
	virtual void BeginScene() = 0;

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	virtual void MakeCurrent() = 0;

	//------------------------------------------------------------------------
	// call this to make this depth buffer the current depth buffer for all
	// subsequent rendering calls.  
	//------------------------------------------------------------------------
	virtual void MakeDepthCurrent() = 0;

	//------------------------------------------------------------------------
	//	EndScene must be called when you are done with the drawing operations
	//	on the current frame.  It will cause whatever drawing you have
	//	requested to be visible on the screen.
	//------------------------------------------------------------------------
	virtual void EndScene() = 0;

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	virtual void Clear(const g2dRGBColor& i_Color) = 0;

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color and sets depth and stencil
	//------------------------------------------------------------------------
	virtual void Clear( const maFloatRGBA& i_Color, bool i_ClearDepth = false, float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0) = 0;

	//------------------------------------------------------------------------
	//	Clear fills the depthstencil buffer with the given values.
	//	If there is no depthstencil, then this does nothing.
	//------------------------------------------------------------------------
	virtual void ClearDepthStencil(float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0) = 0; 

	//------------------------------------------------------------------------
	//	GetPixelFormat returns the current pixel format of the render target.
	//------------------------------------------------------------------------
	virtual const g2dPFD& GetPixelFormat() const = 0;

	//------------------------------------------------------------------------
	//	GetDimensions returns the width and height of the render target.
	//------------------------------------------------------------------------
	virtual void GetDimensions(int& o_Width, int& o_Height) const = 0;

	//------------------------------------------------------------------------
	//	Returns true if this object has allocated a depth buffer.
	//------------------------------------------------------------------------
	virtual bool GetHasDepthBuffer() const = 0;

	//------------------------------------------------------------------------
	//	GetDepthStencilBuffer accesses the depth/stencil if the target has one.
	//------------------------------------------------------------------------
	virtual shared_ptr<g2dDepthStencilBuffer> GetDepthStencilBuffer() const = 0;
};
