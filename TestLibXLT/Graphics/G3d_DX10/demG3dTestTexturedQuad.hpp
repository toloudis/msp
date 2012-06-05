/*****************************************************************************
**  demG3dTestTexturedQuad.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTTEXTUREDQUAD_HPP
#error demG3dTestTexturedQuad.hpp multiply included
#endif
#define DEM_G3DTESTTEXTUREDQUAD_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

class g3dFragment;
class g3dScene;
class g3dSceneNode;
class g3dSceneRenderer;
class g3dViewer;
class matMaterial;
class matTexture;

class demG3dTestTexturedQuad 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestTexturedQuad(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestTexturedQuad();

		//====================================================================
		//	Think
		//====================================================================
		virtual void Think();

		//====================================================================
		//====================================================================
		virtual void Initialize();

		//====================================================================
		//====================================================================
		virtual void DeInitialize();

		//====================================================================
		//	Override this function to get appCharEvents.
		//====================================================================
		virtual void ReceiveCharEvent(appCharEvent& i_Event);

	private:
		g3dViewer& m_Viewer;

		std::vector<matTexture*> m_Textures;
		std::vector<matMaterial*> m_Materials;
		g3dSceneRenderer* m_Renderer;
		g3dSceneRenderer* m_OldRenderer;

		g3dScene *m_Scene;
		g3dSceneNode *m_Root;

		g3dFragment* m_RectFragment;
		matMaterial* m_RectMat;
		g3dSceneNode* m_RectNode;
};
