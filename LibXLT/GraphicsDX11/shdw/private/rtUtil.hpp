/****************************************************************************\
**	rtUtil.hpp
**
**	Ray tracing with DX11 Compute Shader 
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

// Includes

class camCamera;
class g2dRenderTarget;
class g3dScene;

void RTInit();
void RT(g2dRenderTarget* i_pWindow, const camCamera* i_pCamera, const g3dScene* i_pScene);
void RTCleanUp();
