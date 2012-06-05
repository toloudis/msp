/****************************************************************************\
**	pick3dPickBuffer.hpp
**
**	Use special renderer and the back buffer to do picking with the GPU.
**	Each fragment gets encoded with a unique color in a single pixel 
**	viewport render. Then you can tell which fragment was picked by 
**	examining the color at that pixel.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef PICK3D_PICKBUFFER_HPP
#error pick3dPickBuffer.hpp multiply included
#endif
#define PICK3D_PICKBUFFER_HPP

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif

#ifndef G3D_PICKINFO_HPP
#include "Graphics/g3d/g3dPickInfo.hpp"
#endif


//============================================================================
// forward references
//============================================================================
class g2dRenderTarget;
class g3dPickRenderer;
class g3dTargetRenderer;
class g3dViewer;
class g3dLayer;
class matTexture;


//============================================================================
//============================================================================
class pick3dPickBuffer
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pick3dPickBuffer(g3dViewer& i_Viewer);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~pick3dPickBuffer();

		//--------------------------------------------------------------------
		// Render objects at given pixel with color encodings.
		// Returns pick code that can be used to search for the picked
		// g3dSceneNode in the scene graph. A returned value of 0
		// means nothing was picked.
		//--------------------------------------------------------------------
		void DoPickRender(int i_X, int i_Y, float i_Time, envType::UInt32 i_PickMask,
							g3dPickInfo& o_PickInfo,
							const std::vector<g3dLayer*>& i_ViewerLayers);

	private:
		g3dViewer& m_Viewer;
		g3dPickRenderer* m_pPickRenderer;
		g3dTargetRenderer* m_pTargetRenderer;
		camCamera m_PickCamera;
		g3dPickInfo m_PickInfo;

		matTexture* m_pPickTexture;
		g2dRenderTarget* m_pPickTarget;
};
