/*****************************************************************************
**  tstModeBumpMaterialTest.hpp
**
**  tstModeBumpMaterialTest is a test mode for testing bump mapping
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TST_MODEBUMPMATERIALTEST_HPP
#error tstModeBumpMaterialTest.hpp multiply included
#endif
#define TST_MODEBUMPMATERIALTEST_HPP

#ifndef TST_MODE_HPP
#include "tstMode.hpp"
#endif

#ifndef APP_CHAREVENTHANDLER_HPP
#include "appCharEventHandler.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif

class g3dDirectionalLight;
class g3dFragment;
class g3dPointLight;
class scObject;

class tstModeBumpMaterialTest : public tstMode,
							 public appCharEventHandler
{
	public:

		//====================================================================
		// Construction
		//====================================================================
		tstModeBumpMaterialTest();

		//====================================================================
		// Destruction
		//====================================================================
		virtual ~tstModeBumpMaterialTest();

		//====================================================================
		//	The mode should do it's per frame "work" in the Think function.
		//====================================================================
		virtual void Think();

		//====================================================================
		//	Initialize will be called before the first call of Think after
		//	the object is first created or DeInitialized.  During the
		//	lifetime of a mode, Initialize and DeInitialize may be called
		//	several times.  Children of tstModeBumpMaterialTest should remember to call
		//	tstModeBumpMaterialTest::Initialize() at the beginning of their Initialize
		//	function.
		//====================================================================
		virtual void Initialize();

		//====================================================================
		//	The object should clean up things that are not needed while the
		//	mode is not running in the DeInitialize function.
		//====================================================================
		virtual void DeInitialize();

	// Keyboard events
		//====================================================================
		//	Override this function to get appCharEvents.
		//====================================================================
		virtual void ReceiveCharEvent(appCharEvent& i_Event);

	private:

		scObject* m_ExtrObj;
		g3dFragment *m_ExtrFrag;
		matMaterial m_Material;
		matTexture* m_BumpTexture;
		matTexture* m_Texture;
		matTexture* m_GoldTexture;
		matTexture* m_GradientTexture;

		g3dDirectionalLight* m_Light1, *m_Light2;
		g3dPointLight* m_RedPoint;

		g3dFragment *m_RectFragment;
		matMaterial m_RectMat;
		scObject* m_BaseRect;
};
