/****************************************************************************\
**	rasterizerUtil.hpp
**
**	simplistic rasterizer renderer
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

// Includes

class camCamera;
class g2dRenderTarget;
class g3dScene;

namespace RasterPass
{
	void Init();
	void Render(g2dRenderTarget* i_pWindow, const camCamera* i_pCamera, const g3dScene* i_pScene);
	void CleanUp();
}
