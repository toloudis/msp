/*****************************************************************************
**	rndrPrefsMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
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
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"

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
		const char* lc_Key_EnableFur		= "EnableFur";
		const char* lc_Key_EnableGlow		= "EnableGlow";
		const char* lc_Key_MatteMode		= "MatteMode";
//		const char* lc_Key_HeadlightOn		= "HeadlightOn";
		const char* lc_Key_EnableOutline	= "EnableOutline";
		const char* lc_Key_EnableTransparency	= "EnableTransparency";
		const char* lc_Key_EnableReflection	= "EnableReflection";
		const char* lc_Key_EnableEnvironment	= "EnableEnvironment";
		const char* lc_Key_EnableAmbient	= "EnableAmbient";
		const char* lc_Key_EnableLitPass	= "EnableLitPass";
		const char* lc_Key_FurQuality		= "FurQuality";
		const char* lc_Key_BlueShift		= "BlueShift";
		const char* lc_Key_ToneMap			= "ToneMap";
		const char* lc_Key_LowResolution	= "LowResolution";
		const char* lc_Key_EnableAO			= "AmbientOcclusion";
		const char* lc_Key_ProjLtFrustumCull	= "ProjLtFrustumCull";
		const char* lc_Key_HDRAA			= "HDR_AA";
		const char* lc_Key_SSAO				= "SSAO";
		const char* lc_Key_SSAOSteps		= "SSAOSteps";
		const char* lc_Key_SSAODirections	= "SSAODirections";

		const char * lc_ViewportPrefsFileName_Orig			= "RenderPrefs.cfg";
		const char * lc_ViewportPrefsFileName				= "RenderViewportPrefs.cfg";
		const char * lc_ViewportPrefsFileName_Default		= "RenderViewportPrefs-Default.cfg";
		const char * lc_RenderFullPrefsFileName				= "RenderFullPrefs.cfg";
		const char * lc_RenderFullPrefsFileName_Default		= "RenderFullPrefs-Default.cfg";
		const char * lc_RenderQuickPrefsFileName			= "RenderQuickPrefs.cfg";
		const char * lc_RenderQuickPrefsFileName_Default	= "RenderQuickPrefs-Default.cfg";


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
				rndrPrefsObject(const char* i_ConfigFile, const char*i_DefaultConfigFile);

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

			private:
				//--------------------------------------------------------------------
				//--------------------------------------------------------------------
				void RegisterProperties();

				//--------------------------------------------------------------------
				// Callbacks for when properties change, updates member data
				//--------------------------------------------------------------------
				virtual void UpdateShadows(prtyProperty *i_pProperty, bool i_bDirty);
				//virtual void UpdateHeadlight(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateDOF(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateFur(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateGlow(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateMatte(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateOutline(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdatePasses(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateRenderer(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateHDR(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateEnvironment(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateResolution(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateAO(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateProjLtFrustumCull(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateSSAO(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateSSAOQuality(prtyProperty *i_pProperty, bool i_bDirty);

			protected:
				//--------------------------------------------------------------------
				// Convert UI specific combo box index to a known enum type
				//--------------------------------------------------------------------
				g3dSceneRendererCreate::RendererType GetRendererType(int i_ComboBoxIndex);
			public:
				rndrPrefsData m_Data;
				g3dPrefs::g3dRenderPrefs m_ActualPrefs;

			private:
				std::string m_PrefsFilename;
				std::string m_PrefsDefaultFilename;
		};

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		rndrPrefsObject::rndrPrefsObject(const char* i_ConfigFile, const char*i_DefaultConfigFile)
		{
			SetConfigFilename( i_ConfigFile, i_DefaultConfigFile );

			RegisterProperties();
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void rndrPrefsObject::Apply()
		{
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
							if (value < m_Data.m_RendererType.GetMaximum())
								m_Data.m_RendererType.SetValue(value);
						};
						if (keyname == lc_Key_ShadowsOn)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bShadowsOn.SetValue(value);};
						if (keyname == lc_Key_EnableDOF)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableDOF.SetValue(value);};
						if (keyname == lc_Key_EnableFur)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableFur.SetValue(value);};
						if (keyname == lc_Key_EnableGlow)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableGlow.SetValue(value);};
						if (keyname == lc_Key_EnableOutline)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableOutline.SetValue(value);};
						if (keyname == lc_Key_MatteMode)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMatteMode.SetValue(value);};
						//if (keyname == lc_Key_HeadlightOn)
						//	{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bHeadlightOn.SetValue(value);};
						if (keyname == lc_Key_EnableTransparency)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableTransparent.SetValue(value);};
						if (keyname == lc_Key_EnableReflection)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableReflection.SetValue(value);};
						if (keyname == lc_Key_EnableEnvironment)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableEnvironment.SetValue(value);};
						if (keyname == lc_Key_EnableAmbient)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableAmbientPass.SetValue(value);};
						if (keyname == lc_Key_EnableLitPass)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableLitPass.SetValue(value);};
						if (keyname == lc_Key_FurQuality)
							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_FurQuality.SetValue(value);};
						if (keyname == lc_Key_BlueShift)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bBlueShift.SetValue(value);};
						if (keyname == lc_Key_ToneMap)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bToneMap.SetValue(value);};
						if (keyname == lc_Key_LowResolution)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bLowResolution.SetValue(value);};
						if (keyname == lc_Key_EnableAO)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableAO.SetValue(value);};
						if (keyname == lc_Key_ProjLtFrustumCull)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bProjLightFrustumCull.SetValue(value);};
						if (keyname == lc_Key_HDRAA)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bHDRAA.SetValue(value);};
						if (keyname == lc_Key_SSAO)
							{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableSSAO.SetValue(value);};
						if (keyname == lc_Key_SSAOSteps)
							{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSAONumSteps.SetValue(value);};
						if (keyname == lc_Key_SSAODirections)
							{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_SSAONumDirs.SetValue(value);};
					}
				}

				guiXMLTextReader::Close();
			}
			catch (fsXMLErrorX& /*filename*/)
			{
				char msg[512];
				sprintf(msg, "There is a problem reading the file (%s)", cfgpath.c_str());
				guiMessageBox::Show(msg, "Error reading XML file", guiMessageBox::e_OKOnly);
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
			guiXMLTextWriter::WriteElement(lc_Key_Renderer, m_Data.m_RendererType.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_ShadowsOn, m_Data.m_bShadowsOn.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableDOF, m_Data.m_bEnableDOF.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableFur, m_Data.m_bEnableFur.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableGlow, m_Data.m_bEnableGlow.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_MatteMode, m_Data.m_bMatteMode.GetValue());
//			guiXMLTextWriter::WriteElement(lc_Key_HeadlightOn, m_Data.m_bHeadlightOn.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableOutline, m_Data.m_bEnableOutline.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableTransparency, m_Data.m_bEnableTransparent.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableReflection, m_Data.m_bEnableReflection.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableEnvironment, m_Data.m_bEnableEnvironment.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableAmbient, m_Data.m_bEnableAmbientPass.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableLitPass, m_Data.m_bEnableLitPass.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_FurQuality, m_Data.m_FurQuality.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_BlueShift, m_Data.m_bBlueShift.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_ToneMap, m_Data.m_bToneMap.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_LowResolution, m_Data.m_bLowResolution.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_EnableAO, m_Data.m_bEnableAO.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_ProjLtFrustumCull, m_Data.m_bProjLightFrustumCull.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_HDRAA, m_Data.m_bHDRAA.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSAO, m_Data.m_bEnableSSAO.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSAOSteps, m_Data.m_SSAONumSteps.GetValue());
			guiXMLTextWriter::WriteElement(lc_Key_SSAODirections, m_Data.m_SSAONumDirs.GetValue());

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

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//virtual 
		void rndrPrefsObject::RegisterProperties()
		{
			prtyPropertyUIInfo* pPUII;
			prtyRangedFloatUIInfo* pRFUII;
			prtyComboBoxUIInfo* pCBUII;

			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_RendererType), "Renderer", "Select the viewport renderer");
			AddProperty( pCBUII );

			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bShadowsOn), "Lighting", "Use one render pass per light");
			AddProperty( pPUII );
			//pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bHeadlightOn), "Lighting", "Turn on directional light in direction of camera, turning off other lights");
			//AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMatteMode), "Matte", "Render Matte");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bLowResolution), "Resolution", "Low Resolution");
			AddProperty( pPUII );

			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableDOF), "Passes", "Allow DOF rendering feature");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableFur), "Passes", "Allow fur rendering feature");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableGlow), "Passes", "Allow Glow rendering feature");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableOutline), "Passes", "Allow Outline rendering feature");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableAmbientPass), "Passes", "Render Ambient Pass");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableEnvironment), "Passes", "Render Environments");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableLitPass), "Passes", "Render Lit Pass");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableTransparent), "Passes", "Render transparent objects");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableDeferredTransparency), "Passes", "Draw particles last");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableReflection), "Passes", "Render dynamic reflections");
			AddProperty( pPUII );

//			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableAO), "AO", "Use ambient occlusion textures");
//			AddProperty( pPUII );
//			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bRecalcAOPerFrame), "AO", "Recompute all AO receivers globally on each frame");
//			AddProperty( pPUII );

			pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_FurQuality), "Fur", "Quality");
			pRFUII->SetMinimum(0.0f);
			pRFUII->SetMaximum(1.0f);
			pRFUII->SetDecimalPlaces(1);
			pRFUII->SetNumTicks(11);
			AddProperty( pRFUII );

			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_HDRDebugMode), "HDR", "HDR Debug Mode");
			pCBUII->AddItem(std::string("Full Render"),0);
			pCBUII->AddItem(std::string("Clamped HDR Buffer"),1);
			pCBUII->AddItem(std::string("Scaled HDR Buffer"),2);
			pCBUII->AddItem(std::string("Pixel Luminances"),3);
			pCBUII->AddItem(std::string("DOF blurriness"),4);
			pCBUII->AddItem(std::string("1st Luminance pass"),5);
			pCBUII->AddItem(std::string("Bright pass"),6);
			pCBUII->AddItem(std::string("Bloom source"),7);
			pCBUII->AddItem(std::string("Bloom"),8);
			pCBUII->AddItem(std::string("Star"),9);
			AddProperty( pCBUII );

			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bToneMap), "HDR", "HDR Tone Map");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bBlueShift), "HDR", "HDR Blue Shift");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bHDRAA), "HDR", "HDR hardware multisample antialiasing");
			AddProperty( pPUII );

			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bProjLightFrustumCull), "Lighting", "Projected Light Frustum Culling");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bProjLightsOn), "Lighting", "Turn on all Proj Lights");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bPtLightsOn), "Lighting", "Turn on all Point Lights");
			AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bDoShadowMapGen), "Lighting", "Enable shadow map generation");
			AddProperty( pPUII );

			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableSSAO), "SSAO", "Enable SSAO");
			AddProperty( pPUII );
			pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_SSAOQuality), "SSAO", "Sampling quality presets");
			AddProperty( pCBUII );
			pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_SSAONumSteps), "SSAO", "number of steps");
			pRFUII->SetMinimum(0.0000);
			pRFUII->SetMaximum(40.0);
			pRFUII->SetDecimalPlaces(0);
			pRFUII->SetNumTicks(41);
			AddProperty(pRFUII);
			pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_SSAONumDirs), "SSAO", "number of directions (max=32)");
			pRFUII->SetMinimum(1.0000);
			pRFUII->SetMaximum(32.0);
			pRFUII->SetDecimalPlaces(0);
			pRFUII->SetNumTicks(32);
			AddProperty(pRFUII);
			pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_SSAOEnableBlur), "SSAO", "enable blur pass");
			AddProperty( pPUII );


			// TODO : split these into their own atomic update functions, and have them update the "real" application data!

			m_Data.m_bShadowsOn.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateShadows));
			m_Data.m_bEnableDOF.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateDOF));
			m_Data.m_bEnableFur.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateFur));
			m_Data.m_bEnableGlow.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateGlow));
			m_Data.m_bEnableOutline.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateOutline));
			m_Data.m_bEnableAmbientPass.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableLitPass.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bEnableTransparent.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));
			m_Data.m_bMatteMode.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateMatte));
//			m_Data.m_bHeadlightOn.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHeadlight));
			m_Data.m_RendererType.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateRenderer));

			m_Data.m_bToneMap.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHDR));
			m_Data.m_bBlueShift.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHDR));
			m_Data.m_HDRDebugMode.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHDR));
			m_Data.m_bHDRAA.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateHDR));

			m_Data.m_FurQuality.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateFur));
			m_Data.m_bEnableReflection.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateReflection));
			m_Data.m_bEnableEnvironment.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateEnvironment));
			m_Data.m_bLowResolution.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateResolution));

//			m_Data.m_bEnableAO.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateAO));
//			m_Data.m_bRecalcAOPerFrame.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateAO));

			m_Data.m_bProjLightFrustumCull.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateProjLtFrustumCull));
			m_Data.m_bProjLightsOn.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateProjLtFrustumCull));
			m_Data.m_bPtLightsOn.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateProjLtFrustumCull));
			m_Data.m_bDoShadowMapGen.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateProjLtFrustumCull));

			m_Data.m_bEnableDeferredTransparency.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdatePasses));

			m_Data.m_bEnableSSAO.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAO));
			m_Data.m_SSAONumSteps.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAO));
			m_Data.m_SSAONumDirs.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAO));
			m_Data.m_SSAOEnableBlur.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAO));
			m_Data.m_SSAOQuality.AddCallback(new prtyCallbackWrapper<rndrPrefsObject>(this, &rndrPrefsObject::UpdateSSAOQuality));

		}

		//--------------------------------------------------------------------
		// Convert UI specific combo box index to a known enum type
		//--------------------------------------------------------------------
		g3dSceneRendererCreate::RendererType rndrPrefsObject::GetRendererType(int i_ComboBoxIndex)
		{
			switch(i_ComboBoxIndex)
			{
			case 0:
				return g3dSceneRendererCreate::e_Default;
			case 1:
				return g3dSceneRendererCreate::e_AmbientOcclusion;
			case 2:
				return g3dSceneRendererCreate::e_Depth;
			default:
				return g3dSceneRendererCreate::e_Default;
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void rndrPrefsObject::UpdateShadows(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bShadowsOn = m_Data.m_bShadowsOn.GetValue();
			this->m_ActualPrefs.m_bHeadlightOn = (!m_Data.m_bShadowsOn.GetValue());
			rndrPrefsUtil::SetMultipassRendering(this->m_ActualPrefs.m_bShadowsOn);
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		//void rndrPrefsObject::UpdateHeadlight(prtyProperty *i_pProperty, bool i_bDirty)
		//{	
		//	this->m_ActualPrefs.m_bHeadlightOn = m_Data.m_bHeadlightOn.GetValue();
		//	WritePrefsToConfigFile();
		//	update_statusbar_renderprefs();
		//}
		void rndrPrefsObject::UpdateDOF(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bEnableDOF = m_Data.m_bEnableDOF.GetValue();
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateFur(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bEnableFur = m_Data.m_bEnableFur.GetValue();
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();

			this->m_ActualPrefs.m_FurQuality = 10 - (int)(10.0f*m_Data.m_FurQuality.GetValue());
		}
		void rndrPrefsObject::UpdateGlow(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bEnableGlow = m_Data.m_bEnableGlow.GetValue();
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateMatte(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bRenderMatte = m_Data.m_bMatteMode.GetValue();
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateOutline(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			m_ActualPrefs.m_bEnableOutline = m_Data.m_bEnableOutline.GetValue();
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdatePasses(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bEnableAmbientPass = m_Data.m_bEnableAmbientPass.GetValue();
			this->m_ActualPrefs.m_bEnableLitPass = m_Data.m_bEnableLitPass.GetValue();
			this->m_ActualPrefs.m_bEnableTransparent = m_Data.m_bEnableTransparent.GetValue();
			this->m_ActualPrefs.m_bEnableDeferredTransparency = m_Data.m_bEnableDeferredTransparency.GetValue();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateRenderer(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			mnmApp::SetRenderer(GetRendererType(m_Data.m_RendererType.GetValue()));
			this->m_ActualPrefs.m_RendererType = GetRendererType(m_Data.m_RendererType.GetValue());
			mtrlScriptObject::RecreateRenderTargets();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateHDR(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_HDRToneMap = m_Data.m_bToneMap.GetValue();
			this->m_ActualPrefs.m_HDRBlueShift = m_Data.m_bBlueShift.GetValue();
			this->m_ActualPrefs.m_HDRDebugMode = m_Data.m_HDRDebugMode.GetValue();
			this->m_ActualPrefs.m_bHDRAA = m_Data.m_bHDRAA.GetValue();
		}
		void rndrPrefsObject::UpdateReflection(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bEnableReflection = m_Data.m_bEnableReflection.GetValue();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateEnvironment(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bEnableEnvironment = m_Data.m_bEnableEnvironment.GetValue();
			WritePrefsToConfigFile();
			update_statusbar_renderprefs();
		}
		void rndrPrefsObject::UpdateResolution(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bLowResolution = m_Data.m_bLowResolution.GetValue();
			WritePrefsToConfigFile();
		}
		void rndrPrefsObject::UpdateAO(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bEnableAO = m_Data.m_bEnableAO.GetValue();
			this->m_ActualPrefs.m_bRecalcAOPerFrame = m_Data.m_bRecalcAOPerFrame.GetValue();
		}
		void rndrPrefsObject::UpdateProjLtFrustumCull(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bProjLightFrustumCull = m_Data.m_bProjLightFrustumCull.GetValue();
			this->m_ActualPrefs.m_bProjLightsOn = m_Data.m_bProjLightsOn.GetValue();
			this->m_ActualPrefs.m_bPtLightsOn = m_Data.m_bPtLightsOn.GetValue();
			this->m_ActualPrefs.m_bDoShadowMapGen = m_Data.m_bDoShadowMapGen.GetValue();
			WritePrefsToConfigFile();
		}
		void rndrPrefsObject::UpdateSSAO(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_bEnableSSAO = m_Data.m_bEnableSSAO.GetValue();
			this->m_ActualPrefs.m_SSAONumSteps = m_Data.m_SSAONumSteps.GetValue();
			this->m_ActualPrefs.m_SSAONumDirs = m_Data.m_SSAONumDirs.GetValue();
			this->m_ActualPrefs.m_bEnableSSAOBlur = m_Data.m_SSAOEnableBlur.GetValue();
			WritePrefsToConfigFile();
		}
		void rndrPrefsObject::UpdateSSAOQuality(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			switch(m_Data.m_SSAOQuality.GetValue())
			{
			case 0:
				m_Data.m_SSAONumSteps.SetValue(10);
				m_Data.m_SSAONumDirs.SetValue(5);
				break;
			case 1:
				m_Data.m_SSAONumSteps.SetValue(50);
				m_Data.m_SSAONumDirs.SetValue(20);
				break;
			case 2:
				m_Data.m_SSAONumSteps.SetValue(200);
				m_Data.m_SSAONumDirs.SetValue(32);
				break;
			}
		}


		//==========================================================================
		//
		//	Preferences for RenderFull
		//
		//==========================================================================
		class rndrPrefsRenderFullObject : public rndrPrefsObject
		{
			public:
				//------------------------------------------------------------------------
				//------------------------------------------------------------------------
				rndrPrefsRenderFullObject(const char* i_ConfigFile, const char*i_DefaultConfigFile);

			private:
				//--------------------------------------------------------------------
				// Callbacks for when properties change, updates member data
				//--------------------------------------------------------------------
				virtual void UpdateShadows(prtyProperty *i_pProperty, bool i_bDirty);
				virtual void UpdateRenderer(prtyProperty *i_pProperty, bool i_bDirty);
		};

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		rndrPrefsRenderFullObject::rndrPrefsRenderFullObject(const char* i_ConfigFile, const char*i_DefaultConfigFile)
		: rndrPrefsObject( i_ConfigFile, i_DefaultConfigFile )
		{
		}
		void rndrPrefsRenderFullObject::UpdateShadows(prtyProperty *i_pProperty, bool i_bDirty)
		{
			this->m_ActualPrefs.m_bShadowsOn = m_Data.m_bShadowsOn.GetValue();
			this->m_ActualPrefs.m_bHeadlightOn = (!m_Data.m_bShadowsOn.GetValue()); 
			WritePrefsToConfigFile();
		}
		void rndrPrefsRenderFullObject::UpdateRenderer(prtyProperty *i_pProperty, bool i_bDirty)
		{	
			this->m_ActualPrefs.m_RendererType = GetRendererType(m_Data.m_RendererType.GetValue());
		}

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
			delete l_PrefsObjects[e_RenderFullPrefs];
			delete l_PrefsObjects[e_RenderQuickPrefs];
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
			l_PrefsObjects[e_ViewportPrefs]		= new rndrPrefsObject(lc_ViewportPrefsFileName, lc_ViewportPrefsFileName_Default);
			l_PrefsObjects[e_RenderFullPrefs]	= new rndrPrefsRenderFullObject(lc_RenderFullPrefsFileName, lc_RenderFullPrefsFileName_Default);
			l_PrefsObjects[e_RenderQuickPrefs]	= new rndrPrefsRenderFullObject(lc_RenderQuickPrefsFileName, lc_RenderQuickPrefsFileName_Default);

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
		pCmd = new cmaCommandSimple("Render Preferences", 
									"Tools", 
									"View Render Preferences",
									
									&rndrPrefsDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "Tools", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Tools", "Render Preferences", pCmd );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rndrPrefsData& Data(rndr_Object i_ObjectID)
	{
		if ( !l_bDataReadIn )
		{
			ReadPrefs(i_ObjectID);
			l_bDataReadIn = true;
		}

		return l_PrefsObjects[i_ObjectID]->m_Data;
	}
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
		DBG_ASSERT0( i_pInterest != 0, "NULL interest" );

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
		DBG_ASSERT0( i_pInterest != 0, "NULL interest" );

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
	//	F - Fur
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
		default: pstr += '-'; break;
		};
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bShadowsOn.GetValue()) pstr += 'S'; else pstr += '-';
//		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bHeadlightOn.GetValue()) pstr += 'H'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bMatteMode.GetValue()) pstr += 'M'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableDOF.GetValue()) pstr += 'D'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableFur.GetValue()) pstr += 'F'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableGlow.GetValue()) pstr += 'G'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableAmbientPass.GetValue()) pstr += 'A'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableLitPass.GetValue()) pstr += 'L'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableTransparent.GetValue()) pstr += 'T'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableReflection.GetValue()) pstr += 'R'; else pstr += '-';
		if (l_PrefsObjects[i_ObjectID]->m_Data.m_bEnableEnvironment.GetValue()) pstr += 'E'; else pstr += '-';
		return pstr;

		//	unused flags in string
		//m_bToneMap;
		//m_bBlueShift;
		//m_HDRDebugMode;
		//m_FurQuality;
	}

	//------------------------------------------------------------------------
	// Get longer string describing the render flags, using words
	//	instead of codes.
	//------------------------------------------------------------------------
	std::string GetPrefsStringLong(rndr_Object i_ObjectID)
	{
		std::string pstr;
		rndrPrefsData &rdata = l_PrefsObjects[i_ObjectID]->m_Data;
		switch(rdata.m_RendererType.GetValue())
		{
		case 0: pstr += "HDR Renderer"; break;
		case 1: pstr += "Ambient Occlusion Renderer"; break;
		case 2: pstr += "Depth Renderer"; break;
		default: pstr += "-"; break;
		};
		if (rdata.m_bShadowsOn.GetValue()) pstr += " / Shadows";
//		if (rdata.m_bHeadlightOn.GetValue()) pstr += " / Headlight";
		if (rdata.m_bMatteMode.GetValue()) pstr += " / Matte";
		if (rdata.m_bEnableDOF.GetValue()) pstr += " / Depth of Field";
		if (rdata.m_bEnableFur.GetValue()) pstr += " / Fur";
		if (rdata.m_bEnableGlow.GetValue()) pstr += " / Glow";
		if (rdata.m_bEnableAmbientPass.GetValue()) pstr += " / Ambient Pass";
		if (rdata.m_bEnableLitPass.GetValue()) pstr += " / Lit Pass";
		if (rdata.m_bEnableTransparent.GetValue()) pstr += " / Transparent";
		if (rdata.m_bEnableReflection.GetValue()) pstr += " / Reflections";
		if (rdata.m_bEnableEnvironment.GetValue()) pstr += " / Environment";
		return pstr;

	}


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetRendererIndex(g3dSceneRendererCreate::RendererType i_Type)
	{
		switch(i_Type)
		{
		case g3dSceneRendererCreate::e_Default:
			return 0;
		case g3dSceneRendererCreate::e_AmbientOcclusion:
			return 1;
		case g3dSceneRendererCreate::e_Depth:
			return 2;
		default:
			return 0;
		}
	}
}
