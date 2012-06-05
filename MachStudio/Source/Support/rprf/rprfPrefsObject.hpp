/*****************************************************************************
**	rprfPrefsObject.cpp
**
**	NOTE: There are 2 children classes that handle viewport and capture
**	preferences.  You need to add an "update" function to each of them.
**
**	FIX - These extra classes should be handled in a different way and
**	the capture data should not be in g3d.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef RPRFPREFSOBJECT_HPP
#error rprfPrefsObject.hpp multiply included
#endif
#define RPRFPREFSOBJECT_HPP

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENERENDERENGINECREATE_HPP
#include "Graphics/g3d/g3dSceneRenderEngineCreate.hpp"
#endif
#ifndef RPRFPREFSDATA_HPP
#include "Support/rprf/rprfPrefsData.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif


//============================================================================
//============================================================================
#define		AO_LOW_STEPS	10
#define		AO_LOW_DIRS		5
#define		AO_MED_STEPS	50
#define		AO_MED_DIRS		20
#define		AO_HIGH_STEPS	100
#define		AO_HIGH_DIRS	32

#define		GI_LOW_STEPS	10
#define		GI_LOW_DIRS		5
#define		GI_MED_STEPS	50
#define		GI_MED_DIRS		20
#define		GI_HIGH_STEPS	100
#define		GI_HIGH_DIRS	32

//============================================================================
//	Forward References
//============================================================================
class gpxRenderPrefs;
class prtyProperty;
class prtyListBoxUIInfo;
class prtyComboBoxUIInfo;
class prtyPropertyUIInfo;
class prtyFileChooserUIInfo;
class prtyButtonUIInfo;
class prtyCheckBoxUIInfo;
class prtyFloatEditUIInfo;

//============================================================================
//  Prefs object for full render prefs used by the render layers
//============================================================================
class rprfPrefsObject : public prtyObject
{
	public:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		rprfPrefsObject();
		
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		rprfPrefsObject(const rprfPrefsData& i_Data, const g3dPrefs::g3dRenderPrefs& i_RenderPrefs);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		rprfPrefsObject(const g3dPrefs::g3dRenderPrefs& i_RenderPrefs);

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void Apply();

		//--------------------------------------------------------------------
		// Convert UI specific combo box index to a known enum type
		//--------------------------------------------------------------------
		static g3dSceneRendererTypes::RendererType GetRendererType(int i_ComboBoxIndex);

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Init();	

	private:
		void AddCallbacks();
		
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		virtual void UpdateShadows(prtyProperty *i_pProperty, bool i_bDirty);
		//virtual void UpdateHeadlight(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateDOF(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateGlow(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateMatte(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateOutline(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateMotionBlur(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdatePasses(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateRenderType(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateRenderEngine(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateHDR(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateEnvironment(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateResolution(prtyProperty *i_pProperty, bool i_bDirty);
		//virtual void UpdateAO(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateProjLtFrustumCull(prtyProperty *i_pProperty, bool i_bDirty);
		
		virtual void UpdateSSAO(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateSSAOSampling(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateSSAOQuality(prtyProperty *i_pProperty, bool i_bDirty);

		virtual void UpdateSSGI(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateSSGISampling(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateSSGIQuality(prtyProperty *i_pProperty, bool i_bDirty);
		//virtual void RegisterSSAOSampling();
		virtual int MatchSSAOStateToPreset();
		virtual int MatchSSGIStateToPreset();
		virtual void UpdateHardwareTessellation(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateRenderWireframe( prtyProperty *o_pProperty, bool i_bDirty );
		virtual void UpdateTransparency(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateIlluminationRender(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateHair(prtyProperty *i_pProperty, bool i_bDirty);
		virtual void UpdateRenderManOptions(prtyProperty *i_pProperty, bool i_bDirty);

	protected:
		//--------------------------------------------------------------------
		// Convert UI specific combo box index to a known enum type
		//--------------------------------------------------------------------
		g3dSceneRenderEngineCreate::RenderEngine GetRenderEngine(int i_ComboBoxIndex);
		
	public:
		rprfPrefsData m_Data;
		g3dPrefs::g3dRenderPrefs m_ActualPrefs;
		shared_ptr<gpxRenderPrefs> m_PrefsProxy;

	private:
		std::string m_PrefsFilename;
		std::string m_PrefsDefaultFilename;
};
