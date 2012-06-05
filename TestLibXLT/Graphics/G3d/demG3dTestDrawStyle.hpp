/*****************************************************************************
**  demG3dTestDrawStyle.hpp
**
**		This mode displays a demonstration/test of the draw style enumeration
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTDRAWSTYLE_HPP
#error demG3dTestDrawStyle.hpp multiply included
#endif
#define DEM_G3DTESTDRAWSTYLE_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif

class g3dViewer;
class g3dScene;
class g3dSceneNode;
class g3dDirectionalLight;
class g3dPointLight;

class demG3dTestDrawStyle 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestDrawStyle(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demG3dTestDrawStyle();

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
		g3dViewer &m_Viewer;
		g3dScene *m_Scene;
		g3dSceneNode *m_Root;

		g3dFragment* m_ShipFragment;

		g3dSceneNode* m_Ship;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;

		std::vector<matMaterial*> m_Materials;
		std::vector<matTexture*> m_Textures;
};
