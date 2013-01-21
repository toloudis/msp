#pragma once

#include "Area18/ogl/oglBufferHandle.h"
#include "Area18/ogl/oglTypes.hpp"

#include "Core/env/envBoost.hpp"
#include <vector>

class oglContext;
class shdrShaders;

class oglDevice
{
public:
	oglDevice(oglContext* contect);
	virtual ~oglDevice(void);

	// should share objects with other contexts created on this device
	oglContext* mContext;

	oglBufferHandle CreateBuffer(GLenum i_Target, UINT i_Size,
            void* i_pInitialData);
//	g2dDX11Texture1D* CreateTexture1D(const D3D11_TEXTURE1D_DESC *pDesc,
//		const D3D11_SUBRESOURCE_DATA *i_pInitialData);
//	g2dDX11Texture2D* CreateTexture2D(const D3D11_TEXTURE2D_DESC *pDesc,
//		const D3D11_SUBRESOURCE_DATA *i_pInitialData);
//	g2dDX11Texture3D* CreateTexture3D(const D3D11_TEXTURE3D_DESC *pDesc,
//		const D3D11_SUBRESOURCE_DATA *i_pInitialData);

//	void FullscreenQuad(boost::shared_ptr<shdrPS> i_pPS, int i_Width, int i_Height,
//		int i_OffsetPixelsX = 0, int i_OffsetPixelsY = 0);

//	g2dFullscreenQuad* m_FullscreenQuad;

	// These are the "current" depth and color render targets.
//	g2dD3D11RenderTargetPtr m_CurRenderTarget;
//	g2dD3D11DepthStencilPtr m_CurDepthStencil;

//	ID3D11DepthStencilState* m_pDepthStencilState;
//	ID3D11RasterizerState* m_pRasterizerState;

};
