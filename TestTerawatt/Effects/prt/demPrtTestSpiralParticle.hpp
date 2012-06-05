/*****************************************************************************
**  demPrtTestSpiralParticle.hpp
**
**	This module displays a demonstration/test of the scSpiralParticle.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_PRTTESTSPIRALPARTICLE_HPP
#error demPrtTestSpiralParticle.hpp multiply included
#endif
#define DEM_PRTTESTSPIRALPARTICLE_HPP

#ifndef DEM_PRTTESTMODE_HPP
#include "demPrtTestMode.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif
#ifndef AN_2STATEANIMATION_HPP
#include "an2StateAnimation.hpp"
#endif

class g3dDirectionalLight;
class g3dPointLight;
class g3dFragment;
class g3dViewer;
class matUVATexture;
class prtSpiralParticleGenerator;
class scScene;
class scObject;

class demPrtTestSpiralParticle 
:	public demPrtTestMode
{
	public:

		//====================================================================
		//====================================================================
		demPrtTestSpiralParticle(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demPrtTestSpiralParticle();

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
		void make_generator3();

		g3dViewer &m_Viewer;
		scScene *m_Scene;

		g3dFragment* m_RectFragment;
		matMaterial m_RectMat;

		scObject* m_Rect;
		prtSpiralParticleGenerator* m_Generator1;
		prtSpiralParticleGenerator* m_Generator2;
		prtSpiralParticleGenerator* m_Generator3;

		matTexture* m_RectTexture;
		matTexture* m_ParticleTexture;
		matUVATexture* m_UVATexture;
		std::vector<matTexture*> m_UVATextures;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
};
