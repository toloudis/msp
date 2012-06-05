/*****************************************************************************
**  demG3dTestShadow.hpp
**
**		This mode displays a demonstration/test of the different model space
**	concepts used in the Terawatt 3d engine.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_G3DTESTSHADOW_HPP
#error demG3dTestShadow.hpp multiply included
#endif
#define DEM_G3DTESTSHADOW_HPP

#ifndef DEM_G3DTESTMODE_HPP
#include "demG3dTestMode.hpp"
#endif

#ifndef G3D_FRAGMENTHANDLE_HPP
#include "g3dFragmentHandle.hpp"
#endif

#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif

class g3dDynamicModel;
class g3dStaticModel;
class g3dDirectionalLight;
class matPlainTexture;
class g3dWorldStaticModel;
class g3dPointLight;
class g3dStaticProgModel;

class demG3dTestShadow 
:	public demG3dTestMode
{
	public:

		//====================================================================
		//====================================================================
		demG3dTestShadow();

		//====================================================================
		//====================================================================
		virtual ~demG3dTestShadow();

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
		//====================================================================
		virtual void ReceiveCharEvent(appCharEvent& i_Event);

	private:

		g3dFragmentHandle m_SphereFragment;
		g3dFragmentHandle m_RectFragment;
		g3dFragmentHandle m_AntiRectFragment;
		g3dFragmentHandle m_LightVis1Fragment;
		g3dFragmentHandle m_LightVis2Fragment;

		g3dStaticModel* m_Sphere1;
		g3dStaticModel* m_Sphere2;
		g3dStaticModel* m_AntiRect;
		g3dStaticProgModel* m_Rect;

		g3dDynamicModel* m_LightVis1;
		g3dDynamicModel* m_LightVis2;

		matMaterial m_Mat1;
		matMaterial m_Mat2;
		matMaterial m_RectMat;
		matMaterial m_Light1Mat;
		matMaterial m_Light2Mat;

		matTexture* m_Texture1;
		matTexture* m_Texture2;

		g3dPointLight* m_Light1;
		g3dPointLight* m_Light2;
};
