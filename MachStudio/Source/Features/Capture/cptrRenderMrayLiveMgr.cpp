/*****************************************************************************
**	cptrRenderMrayLiveMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderMrayLiveMgr.hpp"

#include "Features/Capture/cptrRenderMrayLive.hpp"
#include "Features/Capture/cptrRenderMrayLiveData.hpp"

#include "Features/Prefs/PrefsDialogUtil.hpp"

#include "Drivers/Sound/tmlnDriverSound.hpp"
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Keyframing/keyfKeyframeUtil.hpp"
#include "Features/MaterialBrowse/mbrwDialogUtil.hpp"
#include "Features/ObjectManip/mnpModeObjectManip.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/pyth/pythProperty.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/mray/mrayMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/Env/envThreadGroup.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/private/fsFileNotifyMgr.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyButtonUIInfo.hpp"
#include "Core/prty/prtyUnits.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/Eff/effOcclusionData.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/Mat/matTextureMgr.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"
#include "Graphics/vtx/vtxVertexAnimBudget.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/cam3d/cam3dManipUtil.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "Tool/icn/icnIconScale.hpp"

#include <string>
#include <vector>

#ifdef USE_WXWIDGETS
#include <wx/tooltip.h>
#endif

#undef CreateDirectory


//============================================================================
//============================================================================
namespace cptrRenderMrayLiveMgr
{
	namespace
	{
		const char* lc_Key_bMrayAO				= "bMrayAO";
		const char* lc_Key_MrayAOSamples		= "MrayAOSamples";
		const char* lc_Key_bMrayFinalGather		= "bMrayFinalGather";
		const char* lc_Key_MrayFGNDiffuse		= "MrayFGNDiffuse";
		const char* lc_Key_MrayFGNRefl			= "MrayFGNRefl";
		const char* lc_Key_MrayFGNRefr			= "MrayFGNRefr";
		const char* lc_Key_MrayFGNRays			= "MrayFGNRays";
		const char* lc_Key_MrayNumReflBounces	= "MrayNumReflBounces";
		const char* lc_Key_MrayNumRefrBounces	= "MrayNumRefrBounces";
		const char* lc_Key_MrayMaxTraceDepth	= "MrayMaxTraceDepth";
		const char* lc_Key_MrayVerbosity		= "MrayVerbosity";
		const char* lc_Key_MrayOutputFormat		= "MrayOutputFormat";
		const char* lc_Key_MrayNumThreads		= "MrayNumThreads";
		const char* lc_Key_MrayMemoryLimit		= "MrayMemoryLimit";
		const char* lc_Key_bMrayEnableReflections	= "bMrayEnableReflections";
		const char* lc_Key_bMrayEnableShadows		= "bMrayEnableShadows";
		const char* lc_Key_MrayShadowType		= "MrayShadowType";
		const char* lc_Key_bMrayRewriteAssets	= "bMrayRewriteAssets";
		const char* lc_Key_MrayVerbosityLevel	= "MrayVerbosityLevel";
		const char* lc_Key_bMrayFGMapEnable		= "bMrayFGMapEnable";
		const char* lc_Key_MrayFGMapRebuild		= "MrayFGMapRebuild";
		const char* lc_Key_MrayFGMapPath		= "MrayFGMapPath";
		const char* lc_Key_MrayReflSamples		= "MrayReflSamples";
		const char* lc_Key_bMrayIgnoreBadTex	= "bMrayIgnoreBadTex";
		const char* lc_Key_bMrayDisplayPreview	= "bMrayDisplayPreview";
		const char* lc_Key_bMrayOverrideMSPSampling		= "bMrayOverrideMSPSampling";
		const char* lc_Key_MrayMinCaptureSamples		= "MrayMinCaptureSamples";
		const char* lc_Key_MrayMaxCaptureSamples		= "MrayMaxCaptureSamples";
		const char* lc_Key_MRayAAContrast		= "MRayAAContrast";
		const char* lc_Key_bMRayTonemapEnable	= "bMRayTonemapEnable";
		const char* lc_Key_bEnableIBL			= "bEnableIBL";
		const char* lc_Key_IBLQuality			= "IBLQuality";
		const char* lc_Key_IBLMapRes			= "IBLMapRes";
		const char* lc_Key_IBLScale				= "IBLScale";
		const char* lc_Key_IBLSampleNum			= "IBLSampleNum";
		const char* lc_Key_bMrayProgressive		= "bMrayProgressive";
		const char* lc_Key_MrayProgSubsamplingSize		= "MrayProgSubsamplingSize";
		const char* lc_Key_MrayProgSubsamplingMode		= "MrayProgSubsamplingMode";
		const char* lc_Key_MrayProgSubsamplingPattern	= "MrayProgSubsamplingPattern";
		const char* lc_Key_MrayProgMinSamples	= "MrayProgMinSamples";
		const char* lc_Key_MrayProgMaxSamples	= "MrayProgMaxSamples";
		const char* lc_Key_MrayProgMaxTime		= "MrayProgMaxTime";
		const char* lc_Key_MrayProgErrorThreshold		= "MrayProgErrorThreshold";
		const char* lc_Key_MrayRenderPass		= "MrayRenderPass";
		const char* lc_Key_MrayPixelFilterSize	= "MrayPixelFilterSize";
		const char* lc_Key_MrayPixelFilterType	= "MrayPixelFilterType";

		//const char* lc_Key_MRU			= "MRUHistory";
		//const char* lc_Key_PTimeCode	= "PlaybackTimeCode";
		//const char* lc_Key_PLoopAtEnd	= "PlaybackLoopAtEnd";

		const char * lc_PrefsFileName			= "MrayLivePrefs.cfg";
		const char * lc_PrefsFileName_Default	= "MrayLivePrefs-Default.cfg";

		std::vector<cptrRenderMrayLiveDataInterest*>	l_PrefsInterestList;

		bool	l_bDataReadIn = false;

		bool	l_MrayRunning = true;

		//====================================================================
		//====================================================================
		class cptrRenderMrayLivePrefsObject : public prtyObject
		{
			public:
				//------------------------------------------------------------
				//------------------------------------------------------------
				cptrRenderMrayLivePrefsObject()
				{
					RegisterProperties();		
				}
				~cptrRenderMrayLivePrefsObject()
				{		
				}

				//------------------------------------------------------------
				//------------------------------------------------------------
				void WritePrefs();
				void ReadPrefs();
				void ApplyPrefs();

				cptrRenderMrayLiveData m_Data;

			private:
				//------------------------------------------------------------
				//------------------------------------------------------------
				void ReadPrefsFromConfigFile(fsLocator& i_ConfigFile);
				void WritePrefsToConfigFile();

				//------------------------------------------------------------
				//------------------------------------------------------------
				virtual void RegisterProperties();

				void ToggleRender(prtyProperty* i_pProperty, bool i_bDirty);

		};


		static cptrRenderMrayLivePrefsObject* l_pcptrRenderMrayLivePrefsObject = 0;

		
		//====================================================================
		//====================================================================
		class PrefsNameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve the string "Preferences" into our property object
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("Preferences"))
				{
					return l_pcptrRenderMrayLivePrefsObject;
				}
				return NULL;
			}
		};

		shared_ptr<PrefsNameResolver> l_NameResolver(new PrefsNameResolver);

		//==========================================================================
		//==========================================================================
		class PrefsTextureAdjuster : public matTextureAdjuster
		{
			//------------------------------------------------------------------------
			//	Implementation of matTextureAdjuster virtual function:
			//	GetReduce returns the amount the texture should be reduced in 
			//	each dimension
			//------------------------------------------------------------------------
			virtual void GetReduce(const fsLocator& i_Locator, 
								   int& o_WidthReduce, 
								   int& o_HeightReduce) const
			{
				// Make sure font for 3d window is not reduced
				if (i_Locator.GetLastName() == itString(mnmConstants::c_ViewerFontName))
					o_WidthReduce = o_HeightReduce = 0;
//				else
//					o_WidthReduce = o_HeightReduce = cptrRenderMrayLiveMgr::Data().m_TextureReduce.GetValue();
			}
		};
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderMrayLivePrefsObject::ReadPrefsFromConfigFile(fsLocator& i_ConfigFile)
	{
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

		guiXMLTextReader::Open(cfgpath.c_str());

		//	read in the preferences
		guiXMLTextReader::gui_Node_Type node_type;
		std::string keyname, strvalue;

		while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
		{
			if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
			{

				if (keyname == lc_Key_bMrayAO)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMrayAO.SetValue(value);};

				if (keyname == lc_Key_MrayAOSamples)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayAOSamples.SetValue(value);};

				if (keyname == lc_Key_bMrayFinalGather)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMrayFinalGather.SetValue(value);};

				if (keyname == lc_Key_MrayFGNDiffuse)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayFGNDiffuse.SetValue(value);};

				if (keyname == lc_Key_MrayFGNRefl)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayFGNRefl.SetValue(value);};

				if (keyname == lc_Key_MrayFGNRefr)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayFGNRefr.SetValue(value);};

				if (keyname == lc_Key_MrayFGNRays)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayFGNRays.SetValue(value);};

				if (keyname == lc_Key_MrayNumReflBounces)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayNumReflBounces.SetValue(value);};

				if (keyname == lc_Key_MrayNumRefrBounces)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayNumRefrBounces.SetValue(value);};

				if (keyname == lc_Key_MrayMaxTraceDepth)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayMaxTraceDepth.SetValue(value);};

				if (keyname == lc_Key_MrayOutputFormat)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayOutputFormat.SetValue(value);};

				if (keyname == lc_Key_MrayNumThreads)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayNumThreads.SetValue(value);};

				if (keyname == lc_Key_MrayMemoryLimit)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayMemoryLimit.SetValue(value);};

				if (keyname == lc_Key_bMrayEnableReflections)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMrayEnableReflections.SetValue(value);};

				if (keyname == lc_Key_bMrayEnableShadows)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMrayEnableShadows.SetValue(value);};

				if (keyname == lc_Key_MrayShadowType)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayShadowType.SetValue(value);};

				if (keyname == lc_Key_bMrayRewriteAssets)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMrayRewriteAssets.SetValue(value);};

				if (keyname == lc_Key_MrayVerbosityLevel)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayVerbosityLevel.SetValue(value);};

				if (keyname == lc_Key_bMrayFGMapEnable)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMrayFGMapEnable.SetValue(value);};

				if (keyname == lc_Key_MrayFGMapRebuild)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayFGMapRebuild.SetValue(value);};

				if (keyname == lc_Key_MrayFGMapPath)
					{fsLocator value; fsFileUtil::ANSIFilenameToLocator(strvalue, value); m_Data.m_MrayFGMapPath.SetValue(value);};

				if (keyname == lc_Key_MrayReflSamples)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayReflSamples.SetValue(value);};

				if (keyname == lc_Key_MRayAAContrast)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MRayAAContrast.SetValue(value);};

				if (keyname == lc_Key_bMrayIgnoreBadTex)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMrayIgnoreBadTex.SetValue(value);};

				if (keyname == lc_Key_bMrayDisplayPreview)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMrayDisplayPreview.SetValue(value);};

				if (keyname == lc_Key_bMRayTonemapEnable)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMRayTonemapEnable.SetValue(value);};

				if (keyname == lc_Key_bEnableIBL)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bEnableIBL.SetValue(value);};

				if (keyname == lc_Key_IBLQuality)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_IBLQuality.SetValue(value);};

				if (keyname == lc_Key_IBLMapRes)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_IBLMapRes.SetValue(value);};

				if (keyname == lc_Key_IBLScale)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_IBLScale.SetValue(value);};

				if (keyname == lc_Key_IBLSampleNum)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_IBLSampleNum.SetValue(value);};

				if (keyname == lc_Key_bMrayProgressive)
					{bool value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_bMrayProgressive.SetValue(value);};

				if (keyname == lc_Key_MrayProgSubsamplingSize)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayProgSubsamplingSize.SetValue(value);};

				if (keyname == lc_Key_MrayProgSubsamplingMode)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayProgSubsamplingMode.SetValue(value);};
				
				if (keyname == lc_Key_MrayProgSubsamplingPattern)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayProgSubsamplingPattern.SetValue(value);};

				if (keyname == lc_Key_MrayProgMinSamples)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayProgMinSamples.SetValue(value);};

				if (keyname == lc_Key_MrayProgMaxSamples)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayProgMaxSamples.SetValue(value);};

				if (keyname == lc_Key_MrayProgMaxTime)
					{envType::Int32 value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayProgMaxTime.SetValue(value);};

				if (keyname == lc_Key_MrayProgErrorThreshold)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayProgErrorThreshold.SetValue(value);};

				if (keyname == lc_Key_MrayRenderPass)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayRenderPass.SetValue(value);};

				if (keyname == lc_Key_MrayPixelFilterSize)
					{float value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_IBLQuality.SetValue(value);};

				if (keyname == lc_Key_MrayPixelFilterType)
					{int value; guiXMLTextReader::Convert(strvalue, value); m_Data.m_MrayPixelFilterSize.SetValue(value);};

			}
		}

		guiXMLTextReader::Close();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderMrayLivePrefsObject::ReadPrefs()
	{
		std::string cfgpath;
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
		cfgdir.Push(lc_PrefsFileName);
		DBG_TRACE("MrayLivePrefs.cfg (read)=" << cfgdir);

		if (fsFileUtil::FileExists(cfgdir))
		{
			ReadPrefsFromConfigFile(cfgdir);
		}
		else
		{
			// no hot key config, so check for default
			cfgdir = gfPaths::GetPath(mnmPaths::e_DefaultConfigs);
			cfgdir.Push( lc_PrefsFileName_Default );
			if (fsFileUtil::FileExists(cfgdir))
			{
				ReadPrefsFromConfigFile(cfgdir);
			}
		}

		//	take the data and apply it as necessary
		//
		ApplyPrefs();

	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderMrayLivePrefsObject::ApplyPrefs()
	{
		//tmlnDriver::SetDefaultBlendType( (tmlnDriver::BlendType)m_Data.m_DefaultDriverBlend.GetValue() );
		//vtxVertexAnimBudget::SetBudgetLimitInMBs( m_Data.m_VertexAnimationBudget.GetValue() );
		//smdlSubdivCharacter::SetSubdivMode( smdlSubdivCharacter::e_SubdivCatmullClark );
		//keyfKeyframeUtil::SetAutoKey( m_Data.m_bAutoKey.GetValue() );		
		//cmmObjectDialogUtil::SetViewPropertyKeyButtons( m_Data.m_bPropKeyButtons.GetValue() );
		//mnpModeObjectManip::SetLocalSpaceTranslation( m_Data.m_bLocalSpaceTranslation.GetValue() );
		//mbrwDialogUtil::SetInitialDirectory( m_Data.m_MaterialBrowserHome.GetValue() );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//virtual 
	void cptrRenderMrayLivePrefsObject::RegisterProperties()
	{
		prtyPropertyUIInfo* pPUII;
		prtyNumericUpDownUIInfo* pNUDUII;
		prtyRangedFloatUIInfo* pRFUII;
		prtyComboBoxUIInfo* pCBUII;	

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayRewriteAssets), "Export Settings", "Enable");
		AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_MrayPixelFilterType), "Sampling Settings", "Output Type");
		AddProperty( pCBUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayPixelFilterSize), "Sampling Settings", "Min Samples");
		pNUDUII->SetDecimalPlaces(2);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(8);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayMinCaptureSamples), "Sampling Settings", "Min Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(-8);
		pNUDUII->SetMaximum(8);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayMaxCaptureSamples), "Sampling Settings", "Max Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(-8);
		pNUDUII->SetMaximum(8);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_MRayAAContrast), "Sampling Settings", "AA Contrast");
		pRFUII->SetDecimalPlaces(2);
		pRFUII->SetMinimum(0);
		pRFUII->SetMaximum(1);
		AddProperty(pRFUII);

		//pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_MrayOutputFormat), "mental ray Render Settings", "Output Type");
		//AddProperty( pCBUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayNumReflBounces), "Ray Tracing Settings", "Reflection bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(32);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayNumRefrBounces), "Ray Tracing Settings", "Refraction bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(32);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayMaxTraceDepth), "Ray Tracing Settings", "Total bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(64);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		//pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayDisplayPreview), "mental ray Render Settings", "Enable");
		//AddProperty( pPUII );

		//pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayIgnoreBadTex), "mental ray Render Settings", "Enable");
		//AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayNumThreads), "Performance Settings", "Number of Render Threads");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayMemoryLimit), "Performance Settings", "Memory Limit (MB)");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_MrayVerbosityLevel), "mental ray Performance Settings", "Verbosity Level");
		AddProperty( pCBUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMRayTonemapEnable), "Tonemapping", "Enable");
		AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayEnableReflections), "Reflection and Refraction", "Enable");
		AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayReflSamples), "Reflection and Refraction", "Number of Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(64);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayEnableShadows), "Shadows", "Enable");
		AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_MrayShadowType), "Shadows", "Shadow Type");
		AddProperty( pCBUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayAO), "Ambient Occlusion", "Enable");
		AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayAOSamples), "Ambient Occlusion", "Number of Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(1024);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		// IBL properties
		/*pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnableIBL), "mental ray IBL", "Enable");
		pPUII->SetReadOnly(true);
		AddProperty( pPUII );*/
		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_IBLQuality), "IBL", "IBL Quality");
		pRFUII->SetDecimalPlaces(1);
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(10.0f);
		AddProperty(pRFUII);
		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_IBLMapRes), "IBL", "IBL Resolution");
		AddProperty( pCBUII );
		/*pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_IBLScale), "mental ray IBL", "IBL Scale");
		pRFUII->SetDecimalPlaces(1);
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(10.0f);
		AddProperty(pRFUII);*/
		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_IBLSampleNum), "IBL", "Number of Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(64);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayFinalGather), "Final Gather", "Enable");
		AddProperty( pPUII );

		/*pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayFGBlur), "mental ray Final Gather", "Enable blur");
		AddProperty( pPUII );*/

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayFGNDiffuse), "Final Gather", "Diffuse bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(10);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayFGNRefl), "Final Gather", "Refl bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(10);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayFGNRefr), "Final Gather", "Refr bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(10);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayFGNRays), "Final Gather", "num rays");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(10000);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );
		
		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayFGMapEnable), "Final Gather Map", "Enable FG Map");
		AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_MrayFGMapRebuild), "Final Gather Map", "FG Map Rebuilding");
		AddProperty( pCBUII );

		pPUII = new prtyFileChooserUIInfo(&(m_Data.m_MrayFGMapPath), "Final Gather Map", "FG Map Location");
		AddProperty( pPUII );



		pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bMrayProgressive), "Progressive Rendering", "Enable");
		AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayProgSubsamplingSize), "Progressive Rendering", "Subsampling Size");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(32);
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_MrayProgSubsamplingMode), "Progressive Rendering", "Subsampling Mode");
		AddProperty( pCBUII );

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_MrayProgSubsamplingPattern), "Progressive Rendering", "Subsampling Pattern");
		AddProperty( pCBUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayProgMinSamples), "Progressive Rendering", "Min Samples");
		pNUDUII->SetDecimalPlaces(0);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayProgMaxSamples), "Progressive Rendering", "Max Samples");
		pNUDUII->SetDecimalPlaces(0);
		AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_MrayProgMaxTime), "Progressive Rendering", "Max Render Time");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(21600); // an hour
		pNUDUII->SetRestrictFlag(true);
		AddProperty( pNUDUII );

		pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_MrayProgErrorThreshold), "Progressive Rendering", "Error Threshold");
		pRFUII->SetDecimalPlaces(2);
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		AddProperty(pRFUII);

		pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_MrayRenderPass), "Render", "Render Pass");
		AddProperty( pCBUII );	

		prtyButtonUIInfo* pBUII;
		pBUII = new prtyButtonUIInfo(&(m_Data.m_bStartStop), "Render", "Start Render");
		pBUII->SetText(std::string("Start Preview"));
		AddProperty( pBUII );

		pBUII = new prtyButtonUIInfo(&(m_Data.m_CompositeBeauty), "Viewport Compositing", "Start Render");
		pBUII->SetText(std::string("Start Composite"));
		AddProperty( pBUII );
		pBUII->SetReadOnly(true);

		pBUII = new prtyButtonUIInfo(&(m_Data.m_CompositeAO), "Viewport Compositing", "Start Render");
		pBUII->SetText(std::string("Start Composite"));
		AddProperty( pBUII );
		pBUII->SetReadOnly(true);

		pBUII = new prtyButtonUIInfo(&(m_Data.m_CompositeFG), "Viewport Compositing", "Start Render");
		pBUII->SetText(std::string("Start Composite"));
		AddProperty( pBUII );
		pBUII->SetReadOnly(true);

		pBUII = new prtyButtonUIInfo(&(m_Data.m_CompositeRefl), "Viewport Compositing", "Start Render");
		pBUII->SetText(std::string("Start Composite"));
		AddProperty( pBUII );
		pBUII->SetReadOnly(true);


		//	Register callbacks for items that need to be updated immediately.
		
		m_Data.m_bStartStop.AddCallback(new prtyCallbackWrapper<cptrRenderMrayLivePrefsObject>(this, &cptrRenderMrayLivePrefsObject::ToggleRender));
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderMrayLivePrefsObject::WritePrefs()
	{
		try
		{
			// Update any values that are not tracked in UI
			//m_Data.m_MaterialBrowserHome.SetValue( mbrwDialogUtil::GetInitialDirectory() );

			WritePrefsToConfigFile();
		}
		catch (fsInvalidLocatorX& i_Ex)
		{
			fsLocator loc = i_Ex.GetLocator();
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(loc, filename);
			std::string msg = "Cannot write to file\n" + filename + "\nYou might not have permissions.";
			guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void cptrRenderMrayLivePrefsObject::WritePrefsToConfigFile()
	{
		fsLocator cfgdir;
		cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);

		// check if folder exists
		if ( !fsFileUtil::DirectoryExists(cfgdir) )
		{
			fsFileUtil::CreateDirectory(cfgdir);
		}
		
		cfgdir.Push(lc_PrefsFileName);
		std::string cfgpath;
		fsFileUtil::LocatorToANSIFilename(cfgdir,cfgpath);
		DBG_TRACE("MrayLivePrefs.cfg (write)=" << cfgdir);

		//	quick and dirty XML Writer
		guiXMLTextWriter::Open(cfgpath.c_str());
		guiXMLTextWriter::WriteStartElement("Mray_Live_Preferences");

		////	write the actual values
		guiXMLTextWriter::WriteElement(lc_Key_bMrayAO, m_Data.m_bMrayAO.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayAOSamples, m_Data.m_MrayAOSamples.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMrayFinalGather, m_Data.m_bMrayFinalGather.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayFGNDiffuse, m_Data.m_MrayFGNDiffuse.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayFGNRefl, m_Data.m_MrayFGNRefl.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayFGNRefr, m_Data.m_MrayFGNRefr.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayFGNRays, m_Data.m_MrayFGNRays.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayNumReflBounces, m_Data.m_MrayNumReflBounces.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayNumRefrBounces, m_Data.m_MrayNumRefrBounces.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayMaxTraceDepth, m_Data.m_MrayMaxTraceDepth.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayVerbosity, m_Data.m_MrayVerbosity.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayOutputFormat, m_Data.m_MrayOutputFormat.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayNumThreads, m_Data.m_MrayNumThreads.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayMemoryLimit, m_Data.m_MrayMemoryLimit.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMrayEnableReflections, m_Data.m_bMrayEnableReflections.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMrayEnableShadows, m_Data.m_bMrayEnableShadows.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayShadowType, m_Data.m_MrayShadowType.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMrayRewriteAssets, m_Data.m_bMrayRewriteAssets.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayVerbosityLevel, m_Data.m_MrayVerbosityLevel.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMrayFGMapEnable, m_Data.m_bMrayFGMapEnable.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayFGMapRebuild, m_Data.m_MrayFGMapRebuild.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayFGMapPath, m_Data.m_MrayFGMapPath.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayReflSamples, m_Data.m_MrayReflSamples.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMrayIgnoreBadTex, m_Data.m_bMrayIgnoreBadTex.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMrayDisplayPreview, m_Data.m_bMrayDisplayPreview.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMrayOverrideMSPSampling, m_Data.m_bMrayOverrideMSPSampling.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayMinCaptureSamples, m_Data.m_MrayMinCaptureSamples.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayMaxCaptureSamples, m_Data.m_MrayMaxCaptureSamples.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MRayAAContrast, m_Data.m_MRayAAContrast.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMRayTonemapEnable, m_Data.m_bMRayTonemapEnable.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bEnableIBL, m_Data.m_bEnableIBL.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_IBLQuality, m_Data.m_IBLQuality.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_IBLMapRes, m_Data.m_IBLMapRes.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_IBLScale, m_Data.m_IBLScale.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_IBLSampleNum, m_Data.m_IBLSampleNum.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_bMrayProgressive, m_Data.m_bMrayProgressive.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayProgSubsamplingSize, m_Data.m_MrayProgSubsamplingSize.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayProgSubsamplingMode, m_Data.m_MrayProgSubsamplingMode.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayProgSubsamplingPattern, m_Data.m_MrayProgSubsamplingPattern.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayProgMinSamples, m_Data.m_MrayProgMinSamples.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayProgMaxSamples, m_Data.m_MrayProgMaxSamples.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayProgMaxTime, m_Data.m_MrayProgMaxTime.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayProgErrorThreshold, m_Data.m_MrayProgErrorThreshold.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayRenderPass, m_Data.m_MrayRenderPass.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayPixelFilterSize, m_Data.m_MrayPixelFilterSize.GetValue());
		guiXMLTextWriter::WriteElement(lc_Key_MrayPixelFilterType, m_Data.m_MrayPixelFilterType.GetValue());

		//	finish it up
		guiXMLTextWriter::WriteEndElement();
		guiXMLTextWriter::Close();
	}		
	
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadHotKeys()
	{
		cmaCommandMgr::ReadHotKeys();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WriteHotKeys()
	{
		cmaCommandMgr::WriteHotKeys();
	}

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp()
	{
		if (l_pcptrRenderMrayLivePrefsObject)
		{
			// Remove name resolver 
			pythProperty::RemoveNameResolver( l_NameResolver ); 

			delete l_pcptrRenderMrayLivePrefsObject;
			l_pcptrRenderMrayLivePrefsObject = NULL;
		}

		cptrRenderMrayLive::CleanUp();

		//fsLocator mrayLiveRoot = gfPaths::GetPath(gfPaths::e_UserDataPath);
		//mrayLiveRoot.Push("mrayLiveCache");

		//fsFileUtil::DeleteDirectoryRecursively(mrayLiveRoot);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void WritePrefs()
	{
		//
		//	main prefs
		//
		l_pcptrRenderMrayLivePrefsObject->WritePrefs();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ReadPrefs()
	{
		//
		//	main prefs
		//
		if ( l_pcptrRenderMrayLivePrefsObject == 0 )
		{
			l_pcptrRenderMrayLivePrefsObject = new cptrRenderMrayLivePrefsObject();

			// Add name resolver now that we have a property object
			pythProperty::AddNameResolver( l_NameResolver ); 

			// Set the texture adjuster to this pointer so that we can control 
			// the reduction of loaded textures
			//matTextureMgr::SetTextureAdjuster( new PrefsTextureAdjuster() );
			
			// Leaving the property "allow missing textures" as always on now
			matTextureMgr::SetAllowNullTextures(true);
		}

		l_pcptrRenderMrayLivePrefsObject->ReadPrefs();
	}

	//------------------------------------------------------------------------
	//	apply the prefs
	//------------------------------------------------------------------------
	void ApplyPrefs()
	{
		if (l_pcptrRenderMrayLivePrefsObject != NULL)
			l_pcptrRenderMrayLivePrefsObject->ApplyPrefs();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderMrayLiveData& Data()
	{
		if ( !l_bDataReadIn )
		{
			ReadPrefs();
			l_bDataReadIn = true;
		}

		return l_pcptrRenderMrayLivePrefsObject->m_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cptrRenderMrayLiveData& GetDataSimple()
	{
		return l_pcptrRenderMrayLivePrefsObject->m_Data;
	}

	//------------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//------------------------------------------------------------------------
	void RegisterInterest( cptrRenderMrayLiveDataInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		l_PrefsInterestList.push_back( i_pInterest );
	}

	//------------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//------------------------------------------------------------------------
	void UnRegisterInterest( cptrRenderMrayLiveDataInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		envSTLHelpers::RemoveOneValue( l_PrefsInterestList, i_pInterest );
	}

	//------------------------------------------------------------------------
	//	Clear() - clear the interest list
	//------------------------------------------------------------------------
	void ClearInterests()
	{
		l_PrefsInterestList.clear();
	}

	//------------------------------------------------------------------------
	//	This gets called to let the interests know that the data has changed
	//------------------------------------------------------------------------
	void DataUpdated()
	{
		for (int i = 0; i < l_PrefsInterestList.size(); ++i)
		{
			l_PrefsInterestList[i]->RenderMrayLiveDataUpdated(l_pcptrRenderMrayLivePrefsObject->m_Data);
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject()
	{
		return l_pcptrRenderMrayLivePrefsObject;
	}

	void cptrRenderMrayLiveMgr::TryMentalRayLive()
	{
		if ( !(docSingleDocumentMgr::GetFilename().GetNumNames() > 0) )
		{
			guiMessageBox::Show("Please load a scene first.", "mental ray Live Error");
			return;
		}

		cptrRenderMrayLiveData data = l_pcptrRenderMrayLivePrefsObject->m_Data;

		cptrRenderMrayLive::Start(data);

		//l_MrayRunning = !l_MrayRunning;

		//if ( !l_MrayRunning )
		//{
		//	cptrRenderMrayLive::Start(m_Data);
		//}
		//else
		//{
		//	cptrRenderMrayLive::Stop();
		//}
	}

	void cptrRenderMrayLivePrefsObject::ToggleRender(prtyProperty *i_pProperty, bool i_bDirty)
	{	
		TryMentalRayLive();
	}

}
