/*****************************************************************************
**	dcutViewer.hpp
**
**	Viewer for multiple camera views in Cue Form
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef DCUT_VIEWER_HPP
#error dcutViewer.hpp multiply included
#endif
#define DCUT_VIEWER_HPP

#ifndef G3D_VIEWER_HPP
#include "Graphics/g3d/g3dViewer.hpp"
#endif

//============================================================================
//============================================================================
class g2dSystem;
class g2dWindow;
class g3dSceneRenderer;

//============================================================================
//============================================================================
class dcutViewer : public g3dViewer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dcutViewer(g2dSystem &i_System, void* i_Hwnd);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~dcutViewer();

	//--------------------------------------------------------------------
	// Set which camera to use for this view
	//--------------------------------------------------------------------
	void SetCameraIndex(int i_Index);

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//--------------------------------------------------------------------
	virtual void Render( float i_fSimTime );

private:
	g2dSystem &m_System;
	g2dWindow* m_pWindow;
	g3dSceneRenderer* m_pRenderer;
	int m_CameraIndex;
};
