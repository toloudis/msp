/*****************************************************************************
**  demPrtTestCone3DParticle.hpp
**
**		This mode displays a demonstration/test of the 3D particles.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_PRTTESTCONE3DPARTICLE_HPP
#error demPrtTestCone3DParticle.hpp multiply included
#endif
#define DEM_PRTTESTCON3DEPARTICLE_HPP

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
class prtConeParticleGenerator;
class prtCone3DParticleGenerator;
class scScene;
class scObject;
class scRayIntersectionCheck;


class demPrtTestCone3DParticle 
:	public demPrtTestMode
{
	public:

		//====================================================================
		//====================================================================
		demPrtTestCone3DParticle(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demPrtTestCone3DParticle();

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
		matTexture* m_RectTexture;

		g3dFragment* m_SphereFragment;
		matMaterial* m_pSphereMat;
		matTexture* m_pSphereTexture;

		scObject* m_Rect;
		prtConeParticleGenerator* m_Generator1;
		prtCone3DParticleGenerator* m_Generator2;

		matTexture* m_ParticleTexture;
		matUVATexture* m_UVATexture;
		std::vector<matTexture*> m_UVATextures;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;

		scRayIntersectionCheck* m_pRayIntersectionCheck;
};
