/****************************************************************************\
**	shdwPassEnvBackground.hpp
**
**	Pre processing to render background
**
**  Riva Chang
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef SHDW_PASSENVBACKGROUND_HPP
#error shdwPassEnvBackground.hpp multiply included
#endif
#define SHDW_PASSENVBACKGROUND_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

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
