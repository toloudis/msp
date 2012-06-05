/*****************************************************************************
**  demPrtTestConeParticle.hpp
**
**		This mode displays a demonstration/test of the scStaticObject
**	and scSimpleMovableObject.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_PRTTESTCONEPARTICLE_HPP
#error demPrtTestConeParticle.hpp multiply included
#endif
#define DEM_PRTTESTCONEPARTICLE_HPP

#ifndef DEM_PRTTESTMODE_HPP
#include "demPrtTestMode.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif

class g3dDirectionalLight;
class g3dPointLight;
class g3dFragment;
class g3dViewer;
class matUVATexture;
class prtConeParticleGenerator;
class scScene;
class scObject;

class demPrtTestConeParticle 
:	public demPrtTestMode
{
	public:

		//====================================================================
		//====================================================================
		demPrtTestConeParticle(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demPrtTestConeParticle();

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

		void make_generator1();
		void make_generator2();

		g3dViewer &m_Viewer;
		scScene *m_Scene;

		g3dFragment* m_RectFragment;
		matMaterial m_RectMat;

		scObject* m_Rect;
		prtConeParticleGenerator* m_Generator1;
		prtConeParticleGenerator* m_Generator2;

		matTexture* m_RectTexture;
		matTexture* m_ParticleTexture;
		matUVATexture* m_UVATexture;
		std::vector<matTexture*> m_UVATextures;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
