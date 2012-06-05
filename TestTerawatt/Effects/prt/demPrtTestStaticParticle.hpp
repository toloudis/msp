/*****************************************************************************
**  demPrtTestStaticParticle.hpp
**
**		This mode displays a demonstration/test of the scStaticObject
**	and scSimpleMovableObject.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_PRTTESTSTATICPARTICLE_HPP
#error demPrtTestStaticParticle.hpp multiply included
#endif
#define DEM_PRTTESTSTATICPARTICLE_HPP

#ifndef DEM_PRTTESTMODE_HPP
#include "demPrtTestMode.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "matMaterial.hpp"
#endif
#ifndef SC_PARTICLEGENERATORTEMPLATE_HPP
#include "prtParticleGeneratorTemplate.hpp"
#endif
#ifndef SC_GENERATOREVENTHANDLER_HPP
#include "prtGeneratorEventHandler.hpp"
#endif

class g3dDirectionalLight;
class g3dPointLight;
class g3dFragment;
class g3dViewer;
class prtStaticParticleGenerator;
class scScene;
class scObject;

class demPrtTestStaticParticle 
:	public demPrtTestMode,
	public prtGeneratorEventHandler
{
	public:

		//====================================================================
		//====================================================================
		demPrtTestStaticParticle(g3dViewer &i_Viewer);

		//====================================================================
		//====================================================================
		virtual ~demPrtTestStaticParticle();

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

		//====================================================================
		//====================================================================
		virtual void GeneratorDestroyed(prtParticleGenerator* i_Generator);

	private:

		void make_generator1();
		void make_generator2();

		g3dViewer &m_Viewer;
		scScene *m_Scene;

		g3dFragment* m_RectFragment;
		matMaterial m_RectMat;

		scObject* m_Rect;
		prtStaticParticleGenerator* m_Generator1;
		prtStaticParticleGenerator* m_Generator2;

		matTexture* m_RectTexture;

		g3dDirectionalLight* m_Light1;
		g3dDirectionalLight* m_Light2;
		prtParticleGeneratorTemplate m_Template1;
		prtParticleGeneratorTemplate m_Template2;
};
