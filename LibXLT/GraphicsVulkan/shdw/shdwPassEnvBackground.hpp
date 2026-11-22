/****************************************************************************\
**	shdwPassEnvBackground.hpp
**
**	Pre processing to render background
**
**  Riva Chang
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_SHDW_PASSENVBACKGROUND_HPP
#error shdwPassEnvBackground.hpp multiply included
#endif
#define GVK_SHDW_PASSENVBACKGROUND_HPP

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

namespace gvk
{

class shdwPassTraversal;

class shdwPassEnvBackground : public g3dRenderPass
{
public:
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	shdwPassEnvBackground(g2dRenderTarget* i_pRenderTarget, 
						const camCamera* i_pCamera,
						const g3dAmbientEnvState& i_AmbientState);

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	~shdwPassEnvBackground();

	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	virtual int Render(float i_time);

	static void InitStates();
	static void CleanupStates();

protected:
	const g3dAmbientEnvState m_AmbientState;
	const camCamera* m_pCamera;
};

}
