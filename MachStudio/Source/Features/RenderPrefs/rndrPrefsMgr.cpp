/*****************************************************************************
**	ndrPrefsMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006-7 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"

#include "Features/RenderPrefs/rndrPrefsDialogUtil.hpp"
#include "Features/RenderPrefs/rndrPrefsUtil.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/pyth/pythProperty.hpp"
#include "Support/rlyr/rlyrPassesObject.hpp"
#include "Support/rprf/rprfPrefsObject.hpp" // for AO preset defines
#include "Support/rprf/rprfPrefsUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxRenderPrefs.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"

#include <string>
#include <vector>


//============================================================================
//============================================================================
namespace rndrPrefsMgr
{
	//========================================================================
	//
	//	Prefs Mgr functions
	//
	//========================================================================

	//===========================================================================
	//	general namespace
	//===========================================================================
	namespace
	{
		std::vector<rndrPrefsDataInterest*>	l_PrefsInterestList;

		bool	l_bDataReadIn = false;
		bool	l_bReadingData = false;

		const char* lc_Reg_Folder_Render	= "RenderPrefs";
		const char* lc_Key_Renderer			= "Renderer";
		const char* lc_Key_ShadowsOn		= "MultipassLighting";
		const char* lc_Key_EnableDOF		= "EnableDOF";
		const char* lc_Key_EnableGlow		= "EnableGlow";
		const char* lc_Key_MatteMode		= "MatteMode";
//		const char* lc_Key_HeadlightOn		= "HeadlightOn";
		const char* lc_Key_EnableOutline	= "EnableOutline";
		const char* lc_Key_EnableTransparency	= "EnableTransparency";
		const char* lc_Key_EnableReflection	= "EnableReflection";
		const char* lc_Key_EnableEnvironment	= "EnableEnvironment";
//		const char* lc_Key_EnableAmbient	= "EnableAmbient";
		const char* lc_Key_EnableLitPass	= "EnableLitPass";
		const char* lc_Key_EnableDiffuseLighting = "EnableDiffuseLighting";
		const char* lc_Key_EnableSpecularLighting = "EnableSpecularLighting";
		const char* lc_Key_EnableShadows	= "EnableShadows";
		const char* lc_Key_EnableInvisibleCastShadows	= "EnableInvisibleCastShadows";
		const char* lc_Key_EnableInvisibleMaskBlack	= "EnableInvisibleMaskBlack";
		const char* lc_Key_EnableInvisibleInReflections = "EnableInvisibleInReflections";
//		const char* lc_Key_BlueShift		= "BlueShift";
		const char* lc_Key_ToneMap			= "ToneMap";
		const char* lc_Key_LowResolution	= "LowResolution";
//		const char* lc_Key_EnableAO			= "AmbientOcclusion";
		//const char* lc_Key_ProjLtFrustumCull	= "ProjLtFrustumCull";
		const char* lc_Key_HDRAA			= "HDR_AA";
		const char* lc_Key_SSAO				= "AO";
		const char* lc_Key_SSAOSteps		= "AOSteps";
		const char* lc_Key_SSAODirections	= "AODirections";
		const char* lc_Key_SSAOBlur			= "AOBlur";
		const char* lc_Key_SSAODepthPeel    = "AODepthPeeling";
		const char* lc_Key_SSAODepthLayers	= "AODepthLayers";

		const char* lc_Key_SSGI				= "GI";
		const char* lc_Key_SSGISteps		= "GISteps";
		const char* lc_Key_SSGIDirections	= "GIDirections";
		const char* lc_Key_SSGIBlur			= "GIBlur";
		const char* lc_Key_SSGIDepthPeel    = "GIDepthPeeling";
		const char* lc_Key_SSGIDepthLayers	= "GIDepthLayers";

		const char * lc_ViewportPrefsFileName_Orig			= "RenderPrefs.cfg";
		const char * lc_ViewportPrefsFileName				= "RenderViewportPrefs.cfg";
		const char * lc_ViewportPrefsFileName_Default		= "RenderViewportPrefs-Default.cfg";
		const char * lc_RenderFullPrefsFileName				= "RenderFullPrefs.cfg";
		const char * lc_RenderFullPrefsFileName_Default		= "RenderFullPrefs-Default.cfg";
		const char * lc_RenderQuickPrefsFileName			= "RenderQuickPrefs.cfg";
		const char * lc_RenderQuickPrefsFileName_Default	= "RenderQuickPrefs-Default.cfg";

		const char * lc_Key_HardwareTessellation = "EnableHardwareTessellation";
//		const char * lc_Key_HardwareTessellationValue = "HardwareTessellationValue";

		const char * lc_Key_RenderWireframe = "RenderWireframe";

		const char* lc_Key_MotionBlurEnable	= "EnableMotionBlur";
		const char* lc_Key_MotionBlurSamples = "MotionBlurSamples";
		const char* lc_Key_MotionBlurPercent = "MotionBlurPercent";

		const char* lc_Key_TransparencyMode = "TransparencyMode";
		const char* lc_Key_DepthPeel		= "TransparencyDepthEnable";
		const char* lc_Key_DepthPeelLayers	= "TransparencyDepthLayers";

		const char* lc_Key_EnableHair		 = "EnableHair";
		const char* lc_Key_HairLines		 = "HairLines";
		const char* lc_Key_HairTransparencyMode = "HairTransparencyMode";
		const char* lc_Key_HairShadowType    = "HairShadowType";
		const char* lc_Key_HairShadowRes     = "HairShadowRes";
		const char* lc_Key_HairTessellation  = "HairTessellation";
		const char* lc_Key_HairVertexLimit   = "HairVertexLimit";
		const char* lc_Key_HairStrandSkip    = "HairSkipStrand";
		const char* lc_Key_HairSubPixelPower = "HairSubPixelPower";
		const char* lc_Key_HairDepthPeel	= "HairDepthEnable";
		const char* lc_Key_HairDepthPeelLayers	= "HairDepthLayers";

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_statusbar_renderprefs()
		{
			//	display the render prefs data in the status bar
			std::string rpstring = rndrPrefsMgr::GetPrefsString(rndrPrefsMgr::e_ViewportPrefs);
			guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Render_Flags, rpstring.c_str() );
			std::string tool_tip = rndrPrefsMgr::GetPrefsStringLong(rndrPrefsMgr::e_ViewportPrefs);
			guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Render_Flags, tool_tip.c_str() );
		}

		//==========================================================================
		//
		//	Render Prefs base object
		//
		//	NOTE: There are 2 children classes that handle viewport and capture
		//	preferences.  You need to add an "update" function to each of them.
		//
		//	FIX - These extra classes should be handled in a different way and
		//	the capture data should not be in g3d.
		//
		//==========================================================================
		class rndrPrefsObject : public prtyObject
		{
			public:
				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				rndrPrefsObject();

				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				rndrPrefsObject(const char* i_ConfigFile, const char*i_DefaultConfigFile);

				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				rndrPrefsObject(g3dPrefs::g3dRenderPrefs i_RenderPrefs);

				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				void ReadPreferences();

				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				void WritePreferences();

				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				void Apply();

			protected:
				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void SetConfigFilename(const char* i_ConfigFile, const char*i_DefaultConfigFile);

				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void ReadPrefsFromConfigFile(fsLocator& i_ConfigFile);
				void WritePrefsToConfigFile();

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
				virtual void UpdateIlluminationRender(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateRenderer(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateHDR(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateEnvironment(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateResolution(prtyProperty *i_pProperty, bool i_bDirty);
				//virtual void UpdateAO(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateProjLtFrustumCull(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateSSAO(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateSSAOSampling(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateSSAOQuality(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void RegisterSSAOSampling();
				virtual int MatchSSAOStateToPreset();

				virtual void UpdateSSGI(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateSSGISampling(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateSSGIQuality(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void RegisterSSGISampling();
				virtual int MatchSSGIStateToPreset();

				virtual void UpdateHardwareTessellation(prtyProperty *i_pProperty, bool i_bDirty);
//				virtual void UpdateHardwareTessellationValue(prtyProperty *i_pProperty, bool i_bDirty);

				virtual void UpdateRenderWireframe( prtyProperty *o_pProperty, bool i_bDirty );
				virtual void UpdateTransparency(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateHair(prtyProperty *i_pProperty, bool i_bDirty);

			protected:
				//--------------------------------------------------------------------
				// Convert UI specific combo box index to a known enum type
				//--------------------------------------------------------------------
				g3dSceneRendererTypes::RendererType GetRendererType(int i_ComboBoxIndex);
				rlyrPassesObject::ePassType GetRenderPassType(int i_ComboBoxIndex);
				
			public:
				rprfPrefsData m_Data;
				g3dPrefs::g3dRenderPrefs m_ActualPrefs;
				shared_ptr<gpxRenderPrefs> m_PrefsProxy;

			private:
				std::string m_PrefsFilename;
				std::string m_PrefsDefaultFilename;
		};


		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		rndrPrefsObject::rndrPrefsObject()
		{
			// create proxy for buffering changes from GUI thread
			m_PrefsProxy.reset(new gpxRenderPrefs(m_ActualPrefs));
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		rndrPrefsObject::rndrPrefsObject(g3dPrefs::g3dRenderPrefs i_RenderPrefs)
			: m_ActualPrefs(i_RenderPrefs)
		{
			// create proxy for buffering changes from GUI thread
			m_PrefsProxy.reset(new gpxRenderPrefs(m_ActualPrefs));

#if(SGPU_APP == MS_CORE)
			rprfPrefsUtil::RegisterPrefProperties(this, m_Data, false);
#else
			rprfPrefsUtil::RegisterPrefProperties(this, m_Data, true);
#endif
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		rndrPrefsObject::rndrPrefsObject(const char* i_ConfigFile, const char*i_DefaultConfigFile)
		{
			// create proxy for buffering changes from GUI thread
			m_PrefsProxy.reset(new gpxRenderPrefs(m_ActualPrefs));

			SetConfigFilename( i_ConfigFile, i_DefaultConfigFile );
			Init();
		}

		void rndrPrefsObject::Init() 
		{
#if(SGPU_APP == MS_CORE)
			rprfPrefsUtil::RegisterPrefProperties(this, m_Data, false);
#else
			rprfPrefsUtil::RegisterPrefProperties(this, m_Data, true);
#endif
			rprfPrefsUtil::RegisterSSAOEnable(this, m_Data);
			RegisterSSAOSampling();
			rprfPrefsUtil::RegisterSSAOgui(this, m_Data);

			rprfPrefsUtil::RegisterSSGIEnable(this, m_Data);
			RegisterSSGISampling();
			rprfPrefsUtil::RegisterSSGIgui(this, m_Data);

			AddCallbacks();
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void rndrPrefsObject::Apply()
		{
			// stop any render threads
			gpxRenderControl::ConfirmSingleThread();

			g3dPrefs::SetPrefs(&m_ActualPrefs);
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void rndrPrefsObject::ReadPreferences()
		{
			l_bReadingData = true;

			std::string cfgpath;
			fsLocator cfgdir;
			cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
			cfgdir.Push( m_PrefsFilename.c_str() );

			if (fsFileUtil::FileExists(cfgdir))
			{
				ReadPrefsFromConfigFile(cfgdir);
			}
			else
			{
				// no hot key config, so check for default
				cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
				cfgdir.Push( m_PrefsDefaultFilename.c_str() );
				if (fsFileUtil::FileExists(cfgdir))
				{
					ReadPrefsFromConfigFile(cfgdir);
				}
			}
			l_bReadingData = false;
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void rndrPrefsObject::WritePreferences()
		{
			WritePrefsToConfigFile();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void rndrPrefsObject::ReadPrefsFromConfigFile(fsLocator& i_ConfigFile)
		{
			std::string cfgpath;
			fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

			try
			{
				guiXMLTextReader::Open(cfgpath.c_str());

				//	read in the preferences
				//
				guiXMLTextReader::gui_Node_Type node_type;
				std::string keyname, strvalue;
				while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
				{
					if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
					{
						if (keyname == lc_Key_Renderer)
						{
							int value; guiXMLTextReader::Convert(strvalue, value); 
							// prtyenum has a maximum set by the number of tags. index must be less than this.
							if (value < m_Data.m_RendererTypeVP.GetNumTags())
								m_Data.m_RendererTypeVP.SetValue(value);
						};
//						if (keyname == lc_Key_ShadowsOn)
//							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMultipassOn.SetValue(value);};
//						if (keyname == lc_Key_EnableDOF)
//							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableDOF.SetValue(value);};
						if (keyname == lc_Key_EnableGlow)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableGlow.SetValue(value);};
						if (keyname == lc_Key_EnableOutline)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableOutline.SetValue(value);};
//						if (keyname == lc_Key_MatteMode)
//							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMatteMode.SetValue(value);};
						//if (keyname == lc_Key_HeadlightOn)
						//	{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bHeadlightOn.SetValue(value);};
//						if (keyname == lc_Key_EnableTransparency)
//							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableTransparent.SetValue(value);};
						if (keyname == lc_Key_EnableReflection)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableReflection.SetValue(value);};
						if (keyname == lc_Key_EnableEnvironment)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableEnvironment.SetValue(value);};
						//if (keyname == lc_Key_EnableAmbient)
						//	{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableAmbientPass.SetValue(value);};
//						if (keyname == lc_Key_EnableLitPass)
//							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableLitPass.SetValue(value);};
//						if (keyname == lc_Key_EnableDiffuseLighting)
//							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableDiffuseLighting.SetValue(value);};
//						if (keyname == lc_Key_EnableSpecularLighting)
//							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableSpecularLighting.SetValue(value);};
						if (keyname == lc_Key_EnableShadows)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableShadows.SetValue(value);};
						if (keyname == lc_Key_EnableInvisibleCastShadows)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableInvisibleCastShadows.SetValue(value);};
						if (keyname == lc_Key_EnableInvisibleMaskBlack)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableInvisibleMaskBlack.SetValue(value);};
						if (keyname == lc_Key_EnableInvisibleInReflections)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableInvisibleInReflections.SetValue(value);};
						//if (keyname == lc_Key_BlueShift)
						//	{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bBlueShift.SetValue(value);};
						//if (keyname == lc_Key_ToneMap)
						//	{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bToneMap.SetValue(value);};
//						if (keyname == lc_Key_LowResolution)
//							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bLowResolution.SetValue(value);};
						//if (keyname == lc_Key_EnableAO)
						//	{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableAO.SetValue(value);};
						//if (keyname == lc_Key_ProjLtFrustumCull)
						//	{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bProjLightFrustumCull.SetValue(value);};
						if (keyname == lc_Key_HDRAA)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bHDRAA.SetValue(value);};

						if (keyname == lc_Key_SSAO)
						{
							bool value; 
							guiXMLTextReader::Convert(strvalue, value); 
							m_Data.m_bEnableSSAO.SetValue(value);
							m_Data.m_bEnableBeautySSAO.SetValue(value);
						};

						if (keyname == lc_Key_SSAOSteps)
							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSAONumSteps.SetValue(value);};
						if (keyname == lc_Key_SSAODirections)
							{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSAONumDirs.SetValue(value);};
						if (keyname == lc_Key_SSAOBlur)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSAOEnableBlur.SetValue(value);};
						if (keyname == lc_Key_SSAODepthPeel)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableSSAODepthPeeling.SetValue(value);};
						if (keyname == lc_Key_SSAODepthLayers)
							{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSAONumLayers.SetValue(value);};

						if (keyname == lc_Key_SSGI)
						{
							bool value; 
							guiXMLTextReader::Convert(strvalue, value); 
							m_Data.m_bEnableSSGI.SetValue(value);
							m_Data.m_bEnableBeautySSGI.SetValue(value);
						};

						if (keyname == lc_Key_SSGISteps)
							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSGINumSteps.SetValue(value);};
						if (keyname == lc_Key_SSGIDirections)
							{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSGINumDirs.SetValue(value);};
						if (keyname == lc_Key_SSGIBlur)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSGIEnableBlur.SetValue(value);};
						if (keyname == lc_Key_SSGIDepthPeel)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableSSGIDepthPeeling.SetValue(value);};
						if (keyname == lc_Key_SSGIDepthLayers)
							{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSGINumLayers.SetValue(value);};

						if (keyname == lc_Key_HardwareTessellation)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bUseHardwareTessellation.SetValue(value);}
//						if (keyname == lc_Key_HardwareTessellationValue)
//							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_HardwareTessellationValue.SetValue(value);};
						if (keyname == lc_Key_RenderWireframe)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bRenderWireframe.SetValue(value);};
						if (keyname == lc_Key_MotionBlurEnable)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMotionBlurEnable.SetValue(value);};
						if (keyname == lc_Key_MotionBlurSamples)
							{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MotionBlurSamples.SetValue(value);};
						if (keyname == lc_Key_MotionBlurPercent)
							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MotionBlurPercent.SetValue(value);};
						if (keyname == lc_Key_TransparencyMode)
						{
							int value; guiXMLTextReader::Convert(strvalue, value); 
							// prtyenum has a maximum set by the number of tags. index must be less than this.
							if (value < m_Data.m_TransparencyMode.GetNumTags())
							{
								m_Data.m_TransparencyMode.SetValue(value);
							}
						};
						if (keyname == lc_Key_DepthPeel )
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bDebugDepthPeel.SetValue(value);};
						if (keyname == lc_Key_DepthPeelLayers )
							{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_nDebugDepthPeelLayers.SetValue(value);};
						if (keyname == lc_Key_EnableHair)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableHair.SetValue(value);};
						if (keyname == lc_Key_HairLines)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bHairLines.SetValue(value);};
						if (keyname == lc_Key_HairTransparencyMode)
						{
							int value; guiXMLTextReader::Convert(strvalue, value); 
							// prtyenum has a maximum set by the number of tags. index must be less than this.
							if (value < m_Data.m_HairTransparencyMode.GetNumTags())
								m_Data.m_HairTransparencyMode.SetValue(value);
						};
/*						if (keyname == lc_Key_HairShadowType)
						{
							int value; guiXMLTextReader::Convert(strvalue, value); 
							// prtyenum has a maximum set by the number of tags. index must be less than this.
							if (value < m_Data.m_HairShadowType.GetNumTags())
								m_Data.m_HairShadowType.SetValue(value);
						};
						if (keyname == lc_Key_HairShadowRes)
						{
							int value; guiXMLTextReader::Convert(strvalue, value); 
							// prtyenum has a maximum set by the number of tags. index must be less than this.
							if (value < m_Data.m_HairShadowRes.GetNumTags())
								m_Data.m_HairShadowRes.SetValue(value);
						};*/
						if (keyname == lc_Key_HairTessellation)
							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_HairTessellation.SetValue(value);};
						if (keyname == lc_Key_HairVertexLimit)
							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_HairVertexLimit.SetValue(value);};
						if (keyname == lc_Key_HairStrandSkip)
							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_HairStrandSkip.SetValue(value);};
						if (keyname == lc_Key_HairSubPixelPower )
							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_HairSubPixelPower.SetValue(value);};
						if (keyname == lc_Key_HairDepthPeel )
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bHairDepthPeel.SetValue(value);};
						if (keyname == lc_Key_HairDepthPeelLayers )
							{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_nHairDepthPeelLayers.SetValue(value);};
					}
				}

				guiXMLTextReader::Close();
			}
			catch (fsXMLErrorX& /*filename*/)
			{
				//char msg[512];
				//sprintf(msg, "There is a problem reading the file (%s)", cfgpath.c_str());
				std::ostringstream oss;
				oss.setf(0, std::ios::floatfield);
				oss << "There is a problem reading the file (" << cfgpath.c_str() <<")";
				std::string msg(oss.str());
				guiMessageBox::Show(msg.c_str(), "Error reading XML file", guiMessageBox::e_OKOnly);
			}
		}


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void rndrPrefsObject::WritePrefsToConfigFile()
		{
			//	if the code is in the middle of reading the file, don't allow writing.
			if (l_bReadingData)
				return;

			fsLocator cfgdir;
			cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
			cfgdir.Push(this->m_PrefsFilename.c_str());
			std::string cfgpath;
			fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);

			//	quick and dirty XML Writer
			guiXMLTextWriter::Open(cfgpath.c_str());
			guiXMLTextWriter::WriteStartElement("Preferences");

			//	write the actual values
			guiXMLTextWriter::WriteElement(lc_Key_Renderer, m_Data.m_RendererTypeVP.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_ShadowsOn, m_Data.m_bMultipassOn.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableDOF, m_Data.m_bEnableDOF.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableGlow, m_Data.m_bEnableGlow.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_MatteMode, m_Data.m_bMatteMode.GetValue());
//			guiXMLTextWriter::WriteElement(lc_Key_HeadlightOn, m_Data.m_bHeadlightOn.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableOutline, m_Data.m_bEnableOutline.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableTransparency, m_Data.m_bEnableTransparent.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableReflection, m_Data.m_bEnableReflection.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableEnvironment, m_Data.m_bEnableEnvironment.GetValue());
//			guiXMLTextWriter::WriteElement(lc_Key_EnableAmbient, m_Data.m_bEnableAmbientPass.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableLitPass, m_Data.m_bEnableLitPass.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableDiffuseLighting, m_Data.m_bEnableDiffuseLighting.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableSpecularLighting, m_Data.m_bEnableSpecularLighting.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableShadows, m_Data.m_bEnableShadows.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableInvisibleCastShadows, m_Data.m_bEnableInvisibleCastShadows.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableInvisibleMaskBlack, m_Data.m_bEnableInvisibleMaskBlack.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableInvisibleInReflections, m_Data.m_bEnableInvisibleInReflections.GetValue());
//			guiXMLTextWriter::WriteElement(lc_Key_BlueShift, m_Data.m_bBlueShift.GetValue());
//			guiXMLTextWriter::WriteElement(lc_Key_ToneMap, m_Data.m_bToneMap.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_LowResolution, m_Data.m_bLowResolution.GetValue());
//			guiXMLTextWriter::WriteElement(lc_Key_EnableAO, m_Data.m_bEnableAO.GetValue());
//			guiXMLTextWriter::WriteElement(lc_Key_ProjLtFrustumCull, m_Data.m_bProjLightFrustumCull.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HDRAA, m_Data.m_bHDRAA.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSAO, m_Data.m_bEnableSSAO.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSAOSteps, m_Data.m_SSAONumSteps.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSAODirections, m_Data.m_SSAONumDirs.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSAOBlur, m_Data.m_SSAOEnableBlur.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSAODepthPeel, m_Data.m_bEnableSSAODepthPeeling.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSAODepthLayers, m_Data.m_SSAONumLayers.GetValue());

			guiXMLTextWriter::WriteElement(lc_Key_SSGI, m_Data.m_bEnableSSGI.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSGISteps, m_Data.m_SSGINumSteps.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSGIDirections, m_Data.m_SSGINumDirs.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSGIBlur, m_Data.m_SSGIEnableBlur.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSGIDepthPeel, m_Data.m_bEnableSSGIDepthPeeling.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSGIDepthLayers, m_Data.m_SSGINumLayers.GetValue());

			guiXMLTextWriter::WriteElement(lc_Key_HardwareTessellation, m_Data.m_bUseHardwareTessellation.GetValue());
//			guiXMLTextWriter::WriteElement(lc_Key_HardwareTessellationValue, m_Data.m_HardwareTessellationValue.GetValue());

			guiXMLTextWriter::WriteElement(lc_Key_RenderWireframe, m_Data.m_bRenderWireframe.GetValue());

			guiXMLTextWriter::WriteElement(lc_Key_MotionBlurEnable, m_Data.m_bMotionBlurEnable.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_MotionBlurSamples, m_Data.m_MotionBlurSamples.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_MotionBlurPercent, m_Data.m_MotionBlurPercent.GetValue());

			guiXMLTextWriter::WriteElement(lc_Key_TransparencyMode, m_Data.m_TransparencyMode.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_DepthPeel, m_Data.m_bDebugDepthPeel.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_DepthPeelLayers, m_Data.m_nDebugDepthPeelLayers.GetValue());

			guiXMLTextWriter::WriteElement(lc_Key_EnableHair, m_Data.m_bEnableHair.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairLines, m_Data.m_bHairLines.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairShadowType, m_Data.m_HairShadowType.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairShadowRes, m_Data.m_HairShadowRes.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairTessellation, m_Data.m_HairTessellation.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairVertexLimit, m_Data.m_HairVertexLimit.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairStrandSkip, m_Data.m_HairStrandSkip.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairTransparencyMode, m_Data.m_HairTransparencyMode.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairSubPixelPower, m_Data.m_HairSubPixelPower.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairDepthPeel, m_Data.m_bHairDepthPeel.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairDepthPeelLayers, m_Data.m_nHairDepthPeelLayers.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HairLines, m_Data.m_bHairLines.GetValue());

			//	finish it up
			guiXMLTextWriter::WriteEndElement();
			guiXMLTextWriter::Close();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void rndrPrefsObject::SetConfigFilename(const char* i_ConfigFile, const char* i_DefaultConfigFile)
		{
			m_PrefsFilename = i_ConfigFile;
			m_PrefsDefaultFilename = i_DefaultConfigFile;
		}

		void rndrPrefsObject::AddCallbacks() {

			// TODO : split these into their own atomic update functions, and have them update the "real" application data!

			m_Data.m_bMultipassOn.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateShadows));
//			m_Data.m_bEnableDOF.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateDOF));
			m_Data.m_bEnableGlow.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateGlow));
			m_Data.m_bEnableOutline.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateOutline));
//			m_Data.m_bEnableAmbientPass.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableLitPass.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableDiffuseLighting.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableSpecularLighting.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableShadows.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableInvisibleCastShadows.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableInvisibleMaskBlack.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableInvisibleInReflections.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableTransparent.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bMatteMode.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateMatte));
//			m_Data.m_bHeadlightOn.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHeadlight));
//			m_Data.m_RendererTypeVP.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateRenderer));
			m_Data.m_RenderPassVP.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateRenderer));

#ifdef _DEBUG
			m_Data.m_bToneMap.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHDR));
#endif
//			m_Data.m_bBlueShift.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHDR));
			m_Data.m_HDRDebugMode.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHDR));
			m_Data.m_bHDRAA.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHDR));

			m_Data.m_bEnableReflection.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateReflection));
			m_Data.m_bEnableEnvironment.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateEnvironment));
			m_Data.m_bLowResolution.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateResolution));

//			m_Data.m_bEnableAO.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateAO));
//			m_Data.m_bRecalcAOPerFrame.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateAO));

#ifdef _DEBUG
			m_Data.m_bProjLightFrustumCull.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateProjLtFrustumCull));
			m_Data.m_bProjLightsOn.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateProjLtFrustumCull));
			m_Data.m_bPtLightsOn.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateProjLtFrustumCull));
			m_Data.m_bDoShadowMapGen.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateProjLtFrustumCull));
//			m_Data.m_bEnableDeferredTransparency.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
#endif
			m_Data.m_bUseAOVolumes.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAO));
			m_Data.m_bEnableSSAO.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAO));
			m_Data.m_SSAONumSteps.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAOSampling));
			m_Data.m_SSAONumDirs.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAOSampling));
			m_Data.m_SSAOEnableBlur.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAO));
			m_Data.m_bEnableSSAODepthPeeling.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAO));
			m_Data.m_SSAONumLayers.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAOSampling));
			
			m_Data.m_bUseLPVGI.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSGI));
			m_Data.m_bEnableSSGI.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSGI));
			m_Data.m_SSGINumSteps.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSGISampling));
			m_Data.m_SSGINumDirs.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSGISampling));
			m_Data.m_SSGIEnableBlur.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSGI));
			m_Data.m_bEnableSSGIDepthPeeling.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSGI));
			m_Data.m_SSGINumLayers.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSGISampling));

			m_Data.m_bUseHardwareTessellation.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHardwareTessellation));	
			m_Data.m_PixelSubdivLimit.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHardwareTessellation));	

			m_Data.m_bRenderWireframe.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateRenderWireframe));	

			//motion blur
#ifdef ENABLE_MOTIONBLUR
			m_Data.m_bMotionBlurEnable.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateMotionBlur));
			m_Data.m_MotionBlurSamples.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateMotionBlur));
			m_Data.m_MotionBlurPercent.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateMotionBlur));
#endif

			m_Data.m_TransparencyMode.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateTransparency));
			m_Data.m_bDebugDepthPeel.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateTransparency));
			m_Data.m_nDebugDepthPeelLayers.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateTransparency));
#ifdef _DEBUG
			m_Data.m_bDebugSinglePeel.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateTransparency));
#endif

			m_Data.m_bIlluminationUsesNormals.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateIlluminationRender));

#ifdef HAIR_SUPPORTED
			m_Data.m_bEnableHair.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
			m_Data.m_bHairLines.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
			m_Data.m_HairTransparencyMode.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
//			m_Data.m_HairShadowRes.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
//			m_Data.m_HairShadowType.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
			m_Data.m_HairTessellation.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
//			m_Data.m_HairVertexLimit.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
//			m_Data.m_HairStrandSkip.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
			m_Data.m_HairSubPixelPower.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
			m_Data.m_bHairDepthPeel.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
			m_Data.m_nHairDepthPeelLayers.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
			m_Data.m_HairInterpolationCount.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
			m_Data.m_HairClumpRadius.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHair));
#endif
		}

		//--------------------------------------------------------------------
		// Convert UI specific combo box index to a known enum type
		//--------------------------------------------------------------------
		g3dSceneRendererTypes::RendererType rndrPrefsObject::GetRendererType(int i_ComboBoxIndex)
		{
			switch(i_ComboBoxIndex)
			{
			case 0:
				return g3dSceneRendererTypes::e_Default;
			case 1:
				return g3dSceneRendererTypes::e_AmbientOcclusion;
			case 2:
				return g3dSceneRendererTypes::e_Depth;
			case 3:
				return g3dSceneRendererTypes::e_ShadowMask;
			case 4:
				return g3dSceneRendererTypes::e_IlluminationOnly;
			case 5:
				return g3dSceneRendererTypes::e_Normals;			
			case 6:
				return g3dSceneRendererTypes::e_DirtyMatte;
			case 7:
				return g3dSceneRendererTypes::e_Wireframe;
			case 8:
				return g3dSceneRendererTypes::e_Materials;
			case 9:
				return g3dSceneRendererTypes::e_ReflectionOnly;
			case 10:
				return g3dSceneRendererTypes::e_GlobalIllumination;
			case 11:
				return g3dSceneRendererTypes::e_VelocityMap;
			case 12:
				return g3dSceneRendererTypes::e_Glow;
			default:
				return g3dSceneRendererTypes::e_Default;
			}
		}

		//--------------------------------------------------------------------
		// Convert UI specific combo box index to a known enum type
		//--------------------------------------------------------------------
		rlyrPassesObject::ePassType rndrPrefsObject::GetRenderPassType(int i_ComboBoxIndex)
		{
			switch(i_ComboBoxIndex)
			{
			case rprfPrefsData::e_PassBeauty:
				return rlyrPassesObject::e_Beauty;
			case rprfPrefsData::e_PassDiffuse:
				return rlyrPassesObject::e_Diffuse;
			case rprfPrefsData::e_PassSpecular:
				return rlyrPassesObject::e_Specular;
			case rprfPrefsData::e_PassAO:
				return rlyrPassesObject::e_AOOnly;
			case rprfPrefsData::e_PassGI:
				return rlyrPassesObject::e_GlobalIllumination;
			case rprfPrefsData::e_PassDepth:
				return rlyrPassesObject::e_Depth;
			case rprfPrefsData::e_PassShadowMask:
				return rlyrPassesObject::e_ShadowMask;			
			case rprfPrefsData::e_PassIllumination:
				return rlyrPassesObject::e_IlluminationOnly;
			case rprfPrefsData::e_PassNormals:
				return rlyrPassesObject::e_Normals;
			case rprfPrefsData::e_PassDirtyMatte:
				return rlyrPassesObject::e_DirtyMatte;
			case rprfPrefsData::e_PassWireframe:
				return rlyrPassesObject::e_Wireframe;
			case rprfPrefsData::e_PassMaterials:
				return rlyrPassesObject::e_Materials;
			case rprfPrefsData::e_PassReflections:
				return rlyrPassesObject::e_ReflectionsOnly;
			case rprfPrefsData::e_PassBloom:
				return rlyrPassesObject::e_Bloom;
			case rprfPrefsData::e_PassStar:
				return rlyrPassesObject::e_Star;
			case rprfPrefsData::e_PassCameraDOF:
				return rlyrPassesObject::e_CameraDOF;
			case rprfPrefsData::e_PassPreview:
				return rlyrPassesObject::e_Preview;
			case rprfPrefsData::e_PassEmissive:
				return rlyrPassesObject::e_Emissive;
			case rprfPrefsData::e_PassSpecEnv:
				return rlyrPassesObject::e_SpecEnv;
			case rprfPrefsData::e_PassSpecLit:
				return rlyrPassesObject::e_SpecLit;
			case rprfPrefsData::e_PassDiffEnv:
				return rlyrPassesObject::e_DiffEnv;
			case rprfPrefsData::e_PassDiffLit:
				return rlyrPassesObject::e_DiffLit;
			case rprfPrefsData::e_PassGlow:
				return rlyrPassesObject::e_Glow;
			default:
				return rlyrPassesObject::e_Beauty;
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void rndrPrefsObject::UpdateShadows(prtyProperty *i_pProperty, bool i_bDirty)
		{
			// stop any render threads when changing settings in the graphics system
			// that aren´t proxied (rndrPrefsUtil::SetMultipassRendering)
			gpxRenderControl::ConfirmSingleThread();

			if( m_Data.m_bRenderWireframe.GetValue() )
			{
				m_PrefsProxy->SetMultipassOn( false );
				m_PrefsProxy->SetHeadlightOn( true );
				rndrPrefsUtil::SetMultipassRendering( false );
			}
			else
			{
				m_PrefsProxy->SetMultipassOn( m_Data.m_bMultipassOn.GetValue() );
				m_PrefsProxy->SetHeadlightOn( !m_Data.m_bMultipassOn.GetValue() );
				rndrPrefsUtil::SetMultipassRendering( m_Data.m_bMultipassOn.GetValue() );
				WritePrefsToConfigFile();
				update_statusbar_renderprefs();
			}
		}
		//void rndrPrefsObject::UpdateHeadlight(prtyProperty *i_pProperty, bool i_bDirty)
		//{	
		//	this->m_PrefsProxy->SetHeadlightOn(  m_Data.m_bHeadlightOn.GetValue();
		//	WritePrefsToConfigFile();
		//	update_statusbar_renderprefs();
		//}
		void rndrPrefsObject::UpdateDOF(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetEnableDOF( m_Data.m_bEnableDOF.GetValue() );
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateGlow(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetEnableGlow(  m_Data.m_bEnableGlow.GetValue() );
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateMatte(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetRenderMatte(  m_Data.m_bMatteMode.GetValue() );
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateOutline(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			m_PrefsProxy->SetEnableOutline(  m_Data.m_bEnableOutline.GetValue() );
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateMotionBlur(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			m_PrefsProxy->SetMotionBlurEnable(  m_Data.m_bMotionBlurEnable.GetValue() );
			m_PrefsProxy->SetMotionBlurSamples(  m_Data.m_MotionBlurSamples.GetValue() );
			float motion_blur_percent = m_Data.m_MotionBlurPercent.GetValue() * .01f; //convert to 0-1
			m_PrefsProxy->SetMotionBlurPercent( motion_blur_percent );	

			// stop any render threads when changing settings in the graphics system
			// that aren´t proxied (api3dScene::SetMotionBlur)
			gpxRenderControl::ConfirmSingleThread();

			MotionBlurParams MBParams;
			MBParams.m_nSamples =  m_Data.m_MotionBlurSamples.GetValue();
			MBParams.m_fPercent =  motion_blur_percent;
			api3dScene::SetMotionBlur(MBParams);

			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdatePasses(prtyProperty *i_pProperty, bool i_bDirty)
		{	
//			this->m_PrefsProxy->SetEnableAmbientPass(  m_Data.m_bEnableAmbientPass.GetValue() );
			this->m_PrefsProxy->SetEnableLitPass(  m_Data.m_bEnableLitPass.GetValue() );
			this->m_PrefsProxy->SetEnableDiffuseLighting(  m_Data.m_bEnableDiffuseLighting.GetValue() );
			this->m_PrefsProxy->SetEnableSpecularLighting(  m_Data.m_bEnableSpecularLighting.GetValue() );
			this->m_PrefsProxy->SetEnableShadows(  m_Data.m_bEnableShadows.GetValue() );
			this->m_PrefsProxy->SetEnableInvisibleCastShadows(  m_Data.m_bEnableInvisibleCastShadows.GetValue() );
			this->m_PrefsProxy->SetEnableInvisibleMaskBlack(  m_Data.m_bEnableInvisibleMaskBlack.GetValue() );
			this->m_PrefsProxy->SetEnableInvisibleInReflections(  m_Data.m_bEnableInvisibleInReflections.GetValue() );
			this->m_PrefsProxy->SetEnableTransparent(  m_Data.m_bEnableTransparent.GetValue() );
//			this->m_PrefsProxy->SetEnableDeferredTransparency(  m_Data.m_bEnableDeferredTransparency.GetValue() );
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateIlluminationRender(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetIlluminationUsesNormals( m_Data.m_bIlluminationUsesNormals.GetValue() );
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateRenderer(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			if(!rprfPrefsUtil::RenderTypeEnabled( m_Data.m_RendererTypeVP.GetValue() ))
			{
				m_Data.m_RendererTypeVP.SetValue(g3dSceneRendererTypes::e_Default);
				return;
			}
			// stop any render threads when changing settings in the graphics system
			// that aren´t proxied (mnmApp::SetRenderer)
			gpxRenderControl::ConfirmSingleThread();

			rlyrPassesObject::ePassType pass_type = GetRenderPassType(m_Data.m_RenderPassVP.GetValue());

			// set a couple of defaults that apply to most passes,
			// just to save typing
			m_PrefsProxy->SetHeadlightOn( false );
			m_PrefsProxy->SetHDRDebugMode(0);

			switch (pass_type)
			{
			case rlyrPassesObject::e_Beauty:
				m_Data.m_bEnableGlow.SetValue(true);
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableSSAO.SetValue( m_Data.m_bEnableBeautySSAO.GetValue() );
				m_Data.m_bEnableSSGI.SetValue( m_Data.m_bEnableBeautySSGI.GetValue() );
				//m_Data.m_bEnableReflection.SetValue(true);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_Diffuse:
				m_Data.m_bEnableGlow.SetValue(false);
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableSpecularLighting.SetValue(false);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_Data.m_bEnableReflection.SetValue(false);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_DiffEnv:
				m_Data.m_bEnableGlow.SetValue(false);
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableSpecularLighting.SetValue(false);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(false);
				m_Data.m_bEnableReflection.SetValue(false);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_DiffLit:
				m_Data.m_bEnableGlow.SetValue(false);
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableSpecularLighting.SetValue(false);
				m_Data.m_bEnableEnvironment.SetValue(false);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_Data.m_bEnableReflection.SetValue(false);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_Specular:
				m_Data.m_bEnableGlow.SetValue(false);
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(false);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_Data.m_bEnableReflection.SetValue(false);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_SpecEnv:
				m_Data.m_bEnableGlow.SetValue(false);
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(false);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(false);
				m_Data.m_bEnableReflection.SetValue(false);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_SpecLit:
				m_Data.m_bEnableGlow.SetValue(false);
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(false);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableEnvironment.SetValue(false);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_Data.m_bEnableReflection.SetValue(false);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_Emissive:
				m_Data.m_bEnableGlow.SetValue(false);
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(false);
				m_Data.m_bEnableSpecularLighting.SetValue(false);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(false);
				m_Data.m_bEnableReflection.SetValue(false);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_AOOnly:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableSSAO.SetValue(true);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_AmbientOcclusion);
				break;
			case rlyrPassesObject::e_GlobalIllumination:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_Data.m_bEnableSSGI.SetValue(true);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_GlobalIllumination);
				break;
			case rlyrPassesObject::e_Depth:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_Depth);
				break;
			case rlyrPassesObject::e_ShadowMask:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_Data.m_bEnableShadows.SetValue(true);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_ShadowMask);
				break;
			case rlyrPassesObject::e_IlluminationOnly:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableShadows.SetValue(true);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_IlluminationOnly);
				break;
			case rlyrPassesObject::e_Normals:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_Normals);
				break;
			case rlyrPassesObject::e_DirtyMatte:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bMatteMode.SetValue(true);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_DirtyMatte);
				break;
			case rlyrPassesObject::e_Wireframe:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(true);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_Wireframe);
				break;
			case rlyrPassesObject::e_Materials:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_Materials);
				break;
			case rlyrPassesObject::e_ReflectionsOnly:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableSSAO.SetValue(true);
				m_Data.m_bEnableSSGI.SetValue(true);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_Data.m_bEnableReflection.SetValue(true);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_ReflectionOnly);
				break;
			case rlyrPassesObject::e_Velocity:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_VelocityMap);
				break;
			case rlyrPassesObject::e_Bloom:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableSSAO.SetValue(true);
				m_Data.m_bEnableSSGI.SetValue(true);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_PrefsProxy->SetHDRDebugMode(8);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_Star:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableSSAO.SetValue(true);
				m_Data.m_bEnableSSGI.SetValue(true);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_PrefsProxy->SetHDRDebugMode(9);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_CameraDOF:
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_PrefsProxy->SetHDRDebugMode(4);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_Preview:
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bMultipassOn.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(true);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_Data.m_bEnableEnvironment.SetValue(true);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_PrefsProxy->SetHeadlightOn( true );
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_HDR);
				break;
			case rlyrPassesObject::e_Glow:
				m_Data.m_bEnableGlow.SetValue(true);
				m_Data.m_bMultipassOn.SetValue(true);
				m_Data.m_bMatteMode.SetValue(false);
				m_Data.m_bRenderWireframe.SetValue(false);
				m_Data.m_bEnableDiffuseLighting.SetValue(false);
				m_Data.m_bEnableSpecularLighting.SetValue(true);
				m_Data.m_bEnableEnvironment.SetValue(false);
				m_Data.m_bEnableLitPass.SetValue(true);
				m_Data.m_bEnableReflection.SetValue(false);
				m_Data.m_bEnableSSAO.SetValue(false);
				m_Data.m_bEnableSSGI.SetValue(false);
				m_PrefsProxy->SetRendererType(g3dSceneRendererTypes::e_Glow);
				break;

			default:
				break;
			}

			//set viewport to show the render pass
			std::string enum_tag;
			enum_tag = m_Data.m_RenderPassVP.GetEnumTag( m_Data.m_RenderPassVP.GetValue() );
			mnmApp::SetRenderPass(pass_type, enum_tag);

			mtrlScriptObject::RecreateRenderTargets();
			update_statusbar_renderprefs();

			//rprfPrefsUtil::UpdateGUICategories(this->m_Data, true);
		}
		void rndrPrefsObject::UpdateHDR(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetHDRToneMap( m_Data.m_bToneMap.GetValue() );
//			this->m_PrefsProxy->SetHDRBlueShift( m_Data.m_bBlueShift.GetValue() );
			if(rprfPrefsUtil::HDRLayerEnabled( m_Data.m_HDRDebugMode.GetValue() ))
				this->m_PrefsProxy->SetHDRDebugMode( m_Data.m_HDRDebugMode.GetValue() );
			this->m_PrefsProxy->SetHDRAA( m_Data.m_bHDRAA.GetValue() );
			WritePrefsToConfigFile();
		}
		void rndrPrefsObject::UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetEnableReflection( m_Data.m_bEnableReflection.GetValue() );
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateEnvironment(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetEnableEnvironment( m_Data.m_bEnableEnvironment.GetValue() );
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateResolution(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetLowResolution( m_Data.m_bLowResolution.GetValue() );
			WritePrefsToConfigFile();
		}
//		void rndrPrefsObject::UpdateAO(prtyProperty *i_pProperty, bool i_bDirty)
//		{	
//			this->m_PrefsProxy->SetEnableAO(  m_Data.m_bEnableAO.GetValue() );
//			this->m_PrefsProxy->SetRecalcAOPerFrame(  m_Data.m_bRecalcAOPerFrame.GetValue() );
//		}
		void rndrPrefsObject::UpdateProjLtFrustumCull(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetProjLightFrustumCull( m_Data.m_bProjLightFrustumCull.GetValue() );
			this->m_PrefsProxy->SetProjLightsOn( m_Data.m_bProjLightsOn.GetValue() );
			this->m_PrefsProxy->SetPtLightsOn( m_Data.m_bPtLightsOn.GetValue() );
			this->m_PrefsProxy->SetDoShadowMapGen( m_Data.m_bDoShadowMapGen.GetValue() );
			WritePrefsToConfigFile();
		}

		void rndrPrefsObject::UpdateSSAO(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_PrefsProxy->SetUseAOVolumes( m_Data.m_bUseAOVolumes.GetValue() );
			this->m_PrefsProxy->SetEnableSSAO( m_Data.m_bEnableSSAO.GetValue() );
			this->m_PrefsProxy->SetEnableSSAOBlur( m_Data.m_SSAOEnableBlur.GetValue() );
			m_PrefsProxy->SetEnableSSAODepthPeeling( m_Data.m_bEnableSSAODepthPeeling.GetValue() );

			if ( (m_Data.m_RenderPassVP == rprfPrefsData::e_PassBeauty && i_bDirty) )
			{
				m_Data.m_bEnableBeautySSAO.SetValue( m_Data.m_bEnableSSAO.GetValue() );
			}

			WritePrefsToConfigFile();
		}
		void rndrPrefsObject::UpdateSSAOSampling(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			// custom.
			// if (i_bDirty)			
			int preset = MatchSSAOStateToPreset();
			m_Data.m_SSAOQualityVP.SetValue(preset);
			this->m_PrefsProxy->SetSSAONumSteps( m_Data.m_SSAONumSteps.GetValue() );
			this->m_PrefsProxy->SetSSAONumDirs( m_Data.m_SSAONumDirs.GetValue() );
			this->m_PrefsProxy->SetSSAONumLayers( m_Data.m_SSAONumLayers.GetValue() );
			//}

			WritePrefsToConfigFile();

		}
		void rndrPrefsObject::UpdateSSAOQuality(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			// We only want to do something if the property changed because of
			//	the user interface combo box. 
			switch(m_Data.m_SSAOQualityVP.GetValue())
				{
				case rprfPrefsData::e_SSAOLowVP:
					m_Data.m_SSAONumSteps.SetValue(AO_LOW_STEPS);
					m_Data.m_SSAONumDirs.SetValue(AO_LOW_DIRS);
					this->m_PrefsProxy->SetAOResolutionReduce(2);
					break;
				case rprfPrefsData::e_SSAOMedVP:
					m_Data.m_SSAONumSteps.SetValue(AO_MED_STEPS);
					m_Data.m_SSAONumDirs.SetValue(AO_MED_DIRS);
					this->m_PrefsProxy->SetAOResolutionReduce(1);
					break;
				case rprfPrefsData::e_SSAOHighVP:
					m_Data.m_SSAONumSteps.SetValue(AO_MED_STEPS);
					m_Data.m_SSAONumDirs.SetValue(AO_MED_DIRS);
					this->m_PrefsProxy->SetAOResolutionReduce(0);
					break;
			}
		}

		void rndrPrefsObject::UpdateSSGI(prtyProperty *i_pProperty, bool i_bDirty)
		{
			this->m_PrefsProxy->SetUseLPVGI( m_Data.m_bUseLPVGI.GetValue() );
			this->m_PrefsProxy->SetEnableSSGI( m_Data.m_bEnableSSGI.GetValue() );
			this->m_PrefsProxy->SetEnableSSGIBlur( m_Data.m_SSGIEnableBlur.GetValue() );
			m_PrefsProxy->SetEnableSSGIDepthPeeling( m_Data.m_bEnableSSGIDepthPeeling.GetValue() );

			if ( (m_Data.m_RenderPassVP == rprfPrefsData::e_PassBeauty && i_bDirty) )
			{
				m_Data.m_bEnableBeautySSGI.SetValue( m_Data.m_bEnableSSGI.GetValue() );
			}

			WritePrefsToConfigFile();
		}
		void rndrPrefsObject::UpdateSSGISampling(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			// custom.
			// if (i_bDirty)			
			int preset = MatchSSGIStateToPreset();
			m_Data.m_SSGIQualityVP.SetValue(preset);
			this->m_PrefsProxy->SetSSGINumSteps( m_Data.m_SSGINumSteps.GetValue() );
			this->m_PrefsProxy->SetSSGINumDirs( m_Data.m_SSGINumDirs.GetValue() );
			this->m_PrefsProxy->SetSSGINumLayers( m_Data.m_SSGINumLayers.GetValue() );
			//}

			WritePrefsToConfigFile();

		}
		void rndrPrefsObject::UpdateSSGIQuality(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			// We only want to do something if the property changed because of
			//	the user interface combo box. 
			switch(m_Data.m_SSGIQualityVP.GetValue())
				{
				case rprfPrefsData::e_SSGILow:
					m_Data.m_SSGINumSteps.SetValue(GI_LOW_STEPS);
					m_Data.m_SSGINumDirs.SetValue(GI_LOW_DIRS);
					this->m_PrefsProxy->SetGIResolutionReduce(2);
					break;
				case rprfPrefsData::e_SSGIMed:
					m_Data.m_SSGINumSteps.SetValue(GI_MED_STEPS);
					m_Data.m_SSGINumDirs.SetValue(GI_MED_DIRS);
					this->m_PrefsProxy->SetGIResolutionReduce(1);
					break;
				case rprfPrefsData::e_SSGIHigh:
					m_Data.m_SSGINumSteps.SetValue(GI_MED_STEPS);
					m_Data.m_SSGINumDirs.SetValue(GI_MED_DIRS);
					this->m_PrefsProxy->SetGIResolutionReduce(0);
					break;
			}
		}

		void rndrPrefsObject::UpdateHardwareTessellation(prtyProperty *i_pProperty, bool i_bDirty)
		{
			// stop any render threads when changing settings in the graphics system
			gpxRenderControl::ConfirmSingleThread();

			// This probably should be in g3d somewhere so that multiple
			// classes throughout the library can use the flag.
			m_PrefsProxy->SetUseHardwareTessellation( m_Data.m_bUseHardwareTessellation.GetValue() );
			m_PrefsProxy->SetPixelSubdivLimit( m_Data.m_PixelSubdivLimit.GetValue() );
		}

//		void rndrPrefsObject::UpdateHardwareTessellationValue(prtyProperty *i_pProperty, bool i_bDirty)
//		{
//			smdlTessellatorBase::SetTessellateValue( (m_Data.m_HardwareTessellationValue.GetValue()*2)+1 );
//		}

		void rndrPrefsObject::UpdateRenderWireframe(prtyProperty *i_pProperty, bool i_bDirty)
		{
			m_PrefsProxy->SetRenderWireframe( m_Data.m_bRenderWireframe.GetValue() );
			UpdateShadows( i_pProperty, i_bDirty );
		}

		void rndrPrefsObject::UpdateTransparency(prtyProperty *i_pProperty, bool i_bDirty)
		{
			m_PrefsProxy->SetTransparencyMode( m_Data.m_TransparencyMode.GetValue() );
			m_PrefsProxy->SetDebugDepthPeel( m_Data.m_bDebugDepthPeel.GetValue() );
			m_PrefsProxy->SetDebugSinglePeel( m_Data.m_bDebugSinglePeel.GetValue() );
			m_PrefsProxy->SetDebugDepthPeelLayers( m_Data.m_nDebugDepthPeelLayers.GetValue() );
			WritePrefsToConfigFile();
		}

		void rndrPrefsObject::UpdateHair(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			m_PrefsProxy->SetEnableHair( m_Data.m_bEnableHair.GetValue() );
			m_PrefsProxy->SetHairLines( m_Data.m_bHairLines.GetValue() );
			m_PrefsProxy->SetHairTransparencyMode( m_Data.m_HairTransparencyMode.GetValue() );
//			m_PrefsProxy->SetHairShadowType( m_Data.m_HairShadowType.GetValue() );
//			m_PrefsProxy->SetHairShadowRes( m_Data.m_HairShadowRes.GetValue() );
			m_PrefsProxy->SetHairTessellation( m_Data.m_HairTessellation.GetValue() );
//			m_PrefsProxy->SetHairVertexLimit( (unsigned int)m_Data.m_HairVertexLimit.GetValue() );
//			m_PrefsProxy->SetHairSkipStrand( (unsigned int)m_Data.m_HairStrandSkip.GetValue() );
			m_PrefsProxy->SetHairSubPixelPower( m_Data.m_HairSubPixelPower.GetValue() );
			m_PrefsProxy->SetHairDepthPeel( m_Data.m_bHairDepthPeel.GetValue() );
			m_PrefsProxy->SetHairDepthPeelLayer( m_Data.m_nHairDepthPeelLayers.GetValue() );
			m_PrefsProxy->SetHairInterpolationCount( (unsigned int)m_Data.m_HairInterpolationCount.GetValue() );
			m_PrefsProxy->SetHairClumpRadius( m_Data.m_HairClumpRadius.GetValue() );
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}

		// note this must match its values with the values in UpdateSSAOQuality
		int rndrPrefsObject::MatchSSAOStateToPreset()
		{	
			// simple round off, only works for positive ints.
			int steps = (int)(m_Data.m_SSAONumSteps.GetValue() + 0.5);
			int dirs = (int)(m_Data.m_SSAONumDirs.GetValue() + 0.5);
			if (steps == AO_LOW_STEPS && dirs == AO_LOW_DIRS)
				return 0;
			else if (steps == AO_MED_STEPS && dirs == AO_MED_DIRS)
				return 1;
			else
				return 3;
		}

		// note this must match its values with the values in UpdateSSAOQuality
		int rndrPrefsObject::MatchSSGIStateToPreset()
		{	
			// simple round off, only works for positive ints.
			int steps = (int)(m_Data.m_SSGINumSteps.GetValue() + 0.5);
			int dirs = (int)(m_Data.m_SSGINumDirs.GetValue() + 0.5);
			if (steps == GI_LOW_STEPS && dirs == GI_LOW_DIRS)
				return 0;
			else if (steps == GI_MED_STEPS && dirs == GI_MED_DIRS)
				return 1;
			else
				return 3;
		}

		void rndrPrefsObject::RegisterSSAOSampling() 
		{
			prtyComboBoxUIInfo* pCBUII;
			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_SSAOQualityVP), "Ambient Occlusion", "Sampling quality presets");
			AddProperty( pCBUII );
			m_Data.m_SSAOQualityVP.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAOQuality));
		}

		void rndrPrefsObject::RegisterSSGISampling() 
		{
			prtyComboBoxUIInfo* pCBUII;
			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_SSGIQualityVP), "Global Illumination", "Sampling quality presets");
			AddProperty( pCBUII );
			m_Data.m_SSGIQualityVP.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSGIQuality));
		}


		////==========================================================================
		////
		////	Preferences for RenderFull
		////
		////==========================================================================
		//class rndrPrefsRenderFullObject : public rndrPrefsObject
		//{
		//	public:
		//		//------------------------------------------------------------------------
		//		//------------------------------------------------------------------------
		//		rndrPrefsRenderFullObject(const char* i_ConfigFile, const char*i_DefaultConfigFile);

		//	private:
		//		//--------------------------------------------------------------------
		//		// Callbacks for when properties change, updates member data
		//		//--------------------------------------------------------------------
		//		virtual void UpdateShadows(prtyProperty *i_pProperty, bool i_bDirty);
		//		virtual void UpdateRenderer(prtyProperty *i_pProperty, bool i_bDirty);
		//		virtual void RegisterSSAOSampling();
		//		virtual void UpdateSSAOQuality(prtyProperty *i_pProperty, bool i_bDirty);
		//		virtual int MatchSSAOStateToPreset();
		//};

		////------------------------------------------------------------------------
		////------------------------------------------------------------------------
		//rndrPrefsRenderFullObject::rndrPrefsRenderFullObject(const char* i_ConfigFile, const char*i_DefaultConfigFile)
		//: rndrPrefsObject()
		//{
		//	SetConfigFilename( i_ConfigFile, i_DefaultConfigFile );
		//	Init();
		//}
		//
		//void rndrPrefsRenderFullObject::UpdateShadows(prtyProperty *i_pProperty, bool i_bDirty)
		//{
		//	this->m_PrefsProxy->SetMultipassOn = m_Data.m_bMultipassOn.GetValue();
		//	this->m_PrefsProxy->SetHeadlightOn = (!m_Data.m_bMultipassOn.GetValue()); 
		//	WritePrefsToConfigFile();
		//}
		//
		//void rndrPrefsRenderFullObject::UpdateRenderer(prtyProperty *i_pProperty, bool i_bDirty)
		//{	
		//	this->m_PrefsProxy->SetRendererType = GetRendererType(m_Data.m_RendererType.GetValue());
		//}
		//
		//void rndrPrefsRenderFullObject::RegisterSSAOSampling()
		//{	
		//	prtyComboBoxUIInfo* pCBUII;		
		//	pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_SSAOQuality), "Ambient Occlusion", "Sampling quality presets");
		//	AddProperty( pCBUII );
		//	m_Data.m_SSAOQuality.AddCallback(new prtyCallbackWrapper<rndrPrefsRenderFullObject>(this, &rndrPrefsRenderFullObject::UpdateSSAOQuality));
		//}

		//void rndrPrefsRenderFullObject::UpdateSSAOQuality(prtyProperty *i_pProperty, bool i_bDirty) 
		//{
		//	switch(m_Data.m_SSAOQuality.GetValue())
		//	{
		//		case rprfPrefsData::e_SSAOLow:
		//			m_Data.m_SSAONumSteps.SetValue(AO_LOW_STEPS);
		//			m_Data.m_SSAONumDirs.SetValue(AO_LOW_DIRS);
		//			break;
		//		case rprfPrefsData::e_SSAOMed:
		//			m_Data.m_SSAONumSteps.SetValue(AO_MED_STEPS);
		//			m_Data.m_SSAONumDirs.SetValue(AO_MED_DIRS);
		//			break;
		//		case rprfPrefsData::e_SSAOHigh:
		//			m_Data.m_SSAONumSteps.SetValue(AO_HIGH_STEPS);
		//			m_Data.m_SSAONumDirs.SetValue(AO_HIGH_DIRS);
		//			break;
		//	}
		//}

		//int rndrPrefsRenderFullObject::MatchSSAOStateToPreset() {
		//	// simple round off, only works for positive ints.
		//	int steps = (int)(m_Data.m_SSAONumSteps.GetValue() + 0.5);
		//	int dirs = (int)(m_Data.m_SSAONumDirs.GetValue() + 0.5);
		//	if (steps == AO_LOW_STEPS && dirs == AO_LOW_DIRS)
		//		return 0;
		//	else if (steps == AO_MED_STEPS && dirs == AO_MED_DIRS)
		//		return 1;
		//	else if (steps == AO_HIGH_STEPS && dirs == AO_HIGH_DIRS)
		//		return 2;
		//	else
		//		return 3;
		//}


		//==========================================================================
		//
		//	render objects
		//
		//==========================================================================
		static std::vector<rndrPrefsObject*> l_PrefsObjects;

		
		//====================================================================
		//====================================================================
		class RenderPrefsNameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve strings into our render pref property objects
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("ViewportRenderPrefs"))
				{
					return l_PrefsObjects[e_ViewportPrefs];
				}
				else if (i_PropertyObjectName == std::string("RenderFullPrefs"))
				{
					return l_PrefsObjects[e_RenderFullPrefs];
				}
				else if (i_PropertyObjectName == std::string("RenderQuickPrefs"))
				{
					return l_PrefsObjects[e_RenderQuickPrefs];
				}
				return NULL;
			}
		};

		shared_ptr<RenderPrefsNameResolver> l_NameResolver(new RenderPrefsNameResolver);
	}


	//========================================================================
	//
	//	rndrPrefsMgr functions
	//
	//========================================================================

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp()
	{
		if (!l_PrefsObjects.empty())
		{
			// Remove name resolver 
			pythProperty::RemoveNameResolver( l_NameResolver );

			delete l_PrefsObjects[e_ViewportPrefs];
			l_PrefsObjects.clear();
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ReadPrefs(rndr_Object i_ObjectID)
	{
		//	create the prefs objects if not already created
		//
		if (l_PrefsObjects.size() == 0)
		{
			l_PrefsObjects.resize(e_PrefsNum);
			l_PrefsObjects[e_ViewportPrefs]	= new rndrPrefsObject(lc_ViewportPrefsFileName, lc_ViewportPrefsFileName_Default);

			// Add name resolver so python can access the render preferences
			pythProperty::AddNameResolver( l_NameResolver );
		}

		//
		//	graphics/rendering prefs
		//
		l_PrefsObjects[i_ObjectID]->ReadPreferences();
		l_bDataReadIn = true;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void ApplyPrefs(rndr_Object i_ObjectID)
	{
		l_PrefsObjects[i_ObjectID]->Apply();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void WritePrefs(rndr_Object i_ObjectID)
	{
		//
		//	graphics/rendering prefs
		//
		l_PrefsObjects[i_ObjectID]->WritePreferences();
	}

	//------------------------------------------------------------------------
	//  AddToMenu() - add Prefs actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Import
		pCmd = new cmaCommandSimple("Viewport Render Preferences", 
									"View", 
									"View Render Preferences",
									&rndrPrefsDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "View", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "View", "Viewport Render Preferences", pCmd );
	}

	//------------------------------------------------------------------------
	// Data can be altered in the GUI thread, its changes are proxied
	//------------------------------------------------------------------------
	rprfPrefsData& Data(rndr_Object i_ObjectID)
	{
		if ( !l_bDataReadIn )
		{
			ReadPrefs(i_ObjectID);
			l_bDataReadIn = true;
		}

		return l_PrefsObjects[i_ObjectID]->m_Data;
	}
	//------------------------------------------------------------------------
	// ActualData should only be used in the render thread or when capturing
	// single threaded.
	//------------------------------------------------------------------------
	const g3dPrefs::g3dRenderPrefs& ActualData(rndr_Object i_ObjectID)
	{
		if ( !l_bDataReadIn )
		{
			ReadPrefs(i_ObjectID);
			l_bDataReadIn = true;
		}
		return l_PrefsObjects[i_ObjectID]->m_ActualPrefs;
	}


	//--------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//--------------------------------------------------------------------
	void RegisterInterest( rndrPrefsDataInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		l_PrefsInterestList.push_back( i_pInterest );
	}

	//--------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterInterest( rndrPrefsDataInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		envSTLHelpers::RemoveOneValue( l_PrefsInterestList, i_pInterest );
	}

	//--------------------------------------------------------------------
	//	Clear() - clear the interest list
	//--------------------------------------------------------------------
	void ClearInterests()
	{
		l_PrefsInterestList.clear();
	}

	//------------------------------------------------------------------------
	//	This gets called to let the interests know that the data has changed
	//------------------------------------------------------------------------
	void DataUpdated(rndr_Object i_ObjectID)
	{
		for (int i = 0; i < l_PrefsInterestList.size(); ++i)
		{
			l_PrefsInterestList[i]->PrefsDataUpdated(l_PrefsObjects[i_ObjectID]->m_Data);
		}

		//	display the render prefs data in the status bar
		std::string rpstring = rndrPrefsMgr::GetPrefsString(i_ObjectID);
		guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Render_Flags, rpstring.c_str() );
		std::string tool_tip = rndrPrefsMgr::GetPrefsStringLong(rndrPrefsMgr::e_ViewportPrefs);
		guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Render_Flags, tool_tip.c_str() );
	}

	prtyObject* CreatePrefsObject(g3dPrefs::g3dRenderPrefs* i_RenderPrefs)
	{
		return (new rndrPrefsObject(*i_RenderPrefs));
	}
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject(rndr_Object i_ObjectID)
	{
		return l_PrefsObjects[i_ObjectID];
	}

	//------------------------------------------------------------------------
	//	Build a string based on the render flags, one char per flag
	//	S - Shadows
	//	M - Render Matte
	//	D - DOF
	//	G - Glow
	//	A - Ambient
	//	L - Lit
	//	T - Transparent
	//	R - Reflections
	//	E - Environments
	//
	//	A '-' means that flag is OFF
	//------------------------------------------------------------------------
	std::string GetPrefsString(rndr_Object i_ObjectID)
	{
		std::string pstr;
		switch(l_PrefsObjects[i_ObjectID]->m_Data.m_RendererType.GetValue())
		{
		case 0: pstr += 'H'; break;
		case 1: pstr += 'O'; break;
		case 2: pstr += 'D'; break;
		case 3: pstr += 'S'; break;
		default: pstr += '-'; break;
		};
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bMultipassOn.GetValue()) pstr += 'S'; else pstr += '-';
//		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bHeadlightOn.GetValue()) pstr += 'H'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bMatteMode.GetValue()) pstr += 'M'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableDOF.GetValue()) pstr += 'D'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableGlow.GetValue()) pstr += 'G'; else pstr += '-';
//		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableAmbientPass.GetValue()) pstr += 'A'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableLitPass.GetValue()) pstr += 'L'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableTransparent.GetValue()) pstr += 'T'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableReflection.GetValue()) pstr += 'R'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableEnvironment.GetValue()) pstr += 'E'; else pstr += '-';
		return pstr;

		//	unused flags in string
		//m_bToneMap;
		//m_bBlueShift;
		//m_HDRDebugMode;
	}

	//------------------------------------------------------------------------
	// Get longer string describing the render flags, using words
	//	instead of codes.
	//------------------------------------------------------------------------
	std::string GetPrefsStringLong(rndr_Object i_ObjectID)
	{
		std::string pstr;
		rprfPrefsData &rdata = l_PrefsObjects[i_ObjectID]->m_Data;
		switch(rdata.m_RendererType.GetValue())
		{
		case 0: pstr += "HDR Renderer"; break;
		case 1: pstr += "Ambient Occlusion Renderer"; break;
		case 2: pstr += "Depth Renderer"; break;
		case 3: pstr += "Shadows Only Renderer"; break;
		case 4: pstr += "Velocity Map Renderer"; break;
		default: pstr += "-"; break;
		};
		if (rdata.m_bMultipassOn.GetValue()) pstr += " / Shadows";
//		if (rdata.m_bHeadlightOn.GetValue()) pstr += " / Headlight";
		if (rdata.m_bMatteMode.GetValue()) pstr += " / Matte";
		if (rdata.m_bEnableDOF.GetValue()) pstr += " / Depth of Field";
		if (rdata.m_bEnableGlow.GetValue()) pstr += " / Glow";
//		if (rdata.m_bEnableAmbientPass.GetValue()) pstr += " / Ambient Pass";
		if (rdata.m_bEnableLitPass.GetValue()) pstr += " / Lit Pass";
		if (rdata.m_bEnableTransparent.GetValue()) pstr += " / Transparent";
		if (rdata.m_bEnableReflection.GetValue()) pstr += " / Reflections";
		if (rdata.m_bEnableEnvironment.GetValue()) pstr += " / Environment";
		return pstr;
	}

	//------------------------------------------------------------------------
	//	set next render pass
	//------------------------------------------------------------------------
	void IncrementPass(rndr_Object i_ObjectID)
	{
		rprfPrefsData &rdata = l_PrefsObjects[i_ObjectID]->m_Data;
		int p = rdata.m_RenderPassVP.GetValue();
		p = p+1;
		if (p >= rdata.m_RenderPassVP.GetNumTags())
			p = 0;
		rdata.m_RenderPassVP.SetValue(p);
	}

	//------------------------------------------------------------------------
	//	set previous render pass
	//------------------------------------------------------------------------
	void DecrementPass(rndr_Object i_ObjectID)
	{
		rprfPrefsData &rdata = l_PrefsObjects[i_ObjectID]->m_Data;
		int p = rdata.m_RenderPassVP.GetValue();
		p = p-1;
		if (p < 0)
			p = rdata.m_RenderPassVP.GetNumTags() - 1;
		rdata.m_RenderPassVP.SetValue(p);
	}
}
