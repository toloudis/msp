/*****************************************************************************
**	rprfPrefsUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/rprf/rprfPrefsUtil.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Support/rprf/rprfPrefsCategory.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp" 
#include "Core/prty/prtyFileChooserUIInfo.hpp" 
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dScene.hpp"

//============================================================================
//============================================================================
namespace rprfPrefsUtil
{
	namespace
	{
		bool l_bUpdate = false;
		bool l_bUpdateVP = false;
		shared_ptr<prtyPropertyCallback> l_Callback;
		//map to keep each category object organized
		std::map<std::string, rprfPrefsCategory*> l_CategoryMap;
		
		//------------------------------------------------------------------------
		//  Function ptr, callback when something is written to the buffer
		//------------------------------------------------------------------------
		void (*l_prefUpdateFunction)();
		void (*l_prefUpdateFunctionVP)();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		rprfPrefsCategory* get_category_object(std::string i_CategoryName)
		{
			//set the temp map to be the viewport map or the full prefs map
			std::map<std::string, rprfPrefsCategory*> tempMap;
			tempMap = l_CategoryMap;

			//now return the category object based off the category name
			std::map<std::string, rprfPrefsCategory*>::iterator it = tempMap.begin();
			it = tempMap.find(i_CategoryName);
			if(it != tempMap.end())
			{
				return (*it).second;
			}

			return NULL;
		}

		//methods to define which render types show certain categories
		/*
			Render Type IDs:
				0 - "Default HDR"
				1 - "AO Only"
				2 - "Depth Buffer"
				3 - "Shadow Mask"
				4 - "Illumination Only"
				5 - "Normals"
				6 - "Dirty Matte"
				7 - "Wireframe"
				8 - "Materials"
				9 - "Reflection Only"
				10 - "Global Illumination"
				11 - "Velocity Map"
		*/
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_lighting_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			//o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			o_pCategory->AddRenderer(3);
			//o_pCategory->AddRenderer(4);
			//o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			//o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			//o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_illumination_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			//o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			//o_pCategory->AddRenderer(3);
			o_pCategory->AddRenderer(4);
			//o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			//o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			//o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_alpha_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			//o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			//o_pCategory->AddRenderer(3);
			//o_pCategory->AddRenderer(4);
			o_pCategory->AddRenderer(5);
			//o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			//o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_resolution_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			//o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			//o_pCategory->AddRenderer(3);
			//o_pCategory->AddRenderer(4);
			//o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			//o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			//o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_passes_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			//o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			//o_pCategory->AddRenderer(3);
			//o_pCategory->AddRenderer(4);
			//o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			//o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			//o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		// all render types have hdr
		//--------------------------------------------------------------------
		void setup_hdr_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			o_pCategory->AddRenderer(1);
			o_pCategory->AddRenderer(2);
			o_pCategory->AddRenderer(3);
			o_pCategory->AddRenderer(4);
			o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			o_pCategory->AddRenderer(7);
			o_pCategory->AddRenderer(8);
			o_pCategory->AddRenderer(9);
			o_pCategory->AddRenderer(10);
			o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		// all render typs have tessellation
		//--------------------------------------------------------------------
		void setup_tessellation_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			o_pCategory->AddRenderer(1);
			o_pCategory->AddRenderer(2);
			o_pCategory->AddRenderer(3);
			o_pCategory->AddRenderer(4);
			o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			o_pCategory->AddRenderer(7);
			o_pCategory->AddRenderer(8);
			o_pCategory->AddRenderer(9);
			o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		// enabled in default and dirty matte
		//--------------------------------------------------------------------
		void setup_transparency_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			//o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			//o_pCategory->AddRenderer(3);
			//o_pCategory->AddRenderer(4);
			//o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			//o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			//o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_motionblur_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			//o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			//o_pCategory->AddRenderer(3);
			//o_pCategory->AddRenderer(4);
			//o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			//o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			//o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_ao_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			//o_pCategory->AddRenderer(3);
			//o_pCategory->AddRenderer(4);
			//o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			//o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			//o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_gi_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			//o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			//o_pCategory->AddRenderer(3);
			//o_pCategory->AddRenderer(4);
			//o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			//o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_hair_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			o_pCategory->AddRenderer(1);
			o_pCategory->AddRenderer(2);
			o_pCategory->AddRenderer(3);
			o_pCategory->AddRenderer(4);
			o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			o_pCategory->AddRenderer(7);
			o_pCategory->AddRenderer(8);
			o_pCategory->AddRenderer(9);
			o_pCategory->AddRenderer(10);
			o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_rpf_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			o_pCategory->AddRenderer(1);
			o_pCategory->AddRenderer(2);
			o_pCategory->AddRenderer(3);
			o_pCategory->AddRenderer(4);
			o_pCategory->AddRenderer(5);
			o_pCategory->AddRenderer(6);
			o_pCategory->AddRenderer(7);
			o_pCategory->AddRenderer(8);
			o_pCategory->AddRenderer(9);
			o_pCategory->AddRenderer(10);
			o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void setup_rman_category(rprfPrefsCategory* o_pCategory)
		{
			//0-9 render types
			o_pCategory->AddRenderer(0);
			//o_pCategory->AddRenderer(1);
			//o_pCategory->AddRenderer(2);
			//o_pCategory->AddRenderer(3);
			//o_pCategory->AddRenderer(4);
			//o_pCategory->AddRenderer(5);
			//o_pCategory->AddRenderer(6);
			//o_pCategory->AddRenderer(7);
			//o_pCategory->AddRenderer(8);
			//o_pCategory->AddRenderer(9);
			//o_pCategory->AddRenderer(10);
			//o_pCategory->AddRenderer(11);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_properties(rprfPrefsData& o_Data, bool i_bIsViewport)
		{	
			bool choseMspEngine = o_Data.m_RendererEngine.GetValue() == g3dSceneRenderEngineCreate::e_Default ? true : false;
			bool choseRmanEngine = o_Data.m_RendererEngine.GetValue() == g3dSceneRenderEngineCreate::e_RmanPrman ? true : false;
			bool choseMRayEngine = o_Data.m_RendererEngine.GetValue() == g3dSceneRenderEngineCreate::e_MentalRay ? true : false;

			int rendererID = 0;

			if ( !choseMspEngine )
			{
				o_Data.m_RendererType.SetValue(0);
			}
			else
			{
				if(!i_bIsViewport)
				{
					// allow all render prefs in capture mode options for render layers.
					rendererID = 0;//o_Data.m_RendererType.GetValue();
				}
				else
				{
					rendererID = o_Data.m_RendererTypeVP.GetValue();
				}
			}

			bool bVisible = true;
			if(l_CategoryMap["Lighting"] != NULL)
			{
				bVisible = l_CategoryMap["Lighting"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bMultipassOn.SetVisible(bVisible);
				o_Data.m_bProjLightFrustumCull.SetVisible(bVisible);
				o_Data.m_bProjLightsOn.SetVisible(bVisible);
				o_Data.m_bPtLightsOn.SetVisible(bVisible);
				o_Data.m_bDoShadowMapGen.SetVisible(bVisible);
			}

			if(l_CategoryMap["Illumination Renderer"] != NULL)
			{
				bVisible = l_CategoryMap["Illumination Renderer"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bIlluminationUsesNormals.SetVisible(bVisible);
			}

			if(l_CategoryMap["Alpha"] != NULL)
			{
				bVisible = l_CategoryMap["Alpha"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bEnableInvisibleMaskBlack.SetVisible(bVisible);
				//o_Data.m_bMatteMode.SetVisible(bVisible);
			}

			if(l_CategoryMap["Resolution"] != NULL)
			{
				bVisible = l_CategoryMap["Resolution"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bLowResolution.SetVisible(bVisible);
			}

			if(l_CategoryMap["Advanced Render Flags"] != NULL)
			{
				bVisible = l_CategoryMap["Advanced Render Flags"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bEnableDOF.SetVisible(bVisible);
				o_Data.m_bEnableGlow.SetVisible(bVisible);
				o_Data.m_bEnableOutline.SetVisible(bVisible);
				o_Data.m_bEnableEnvironment.SetVisible(bVisible);
				o_Data.m_bEnableLitPass.SetVisible(bVisible);
				o_Data.m_bEnableTransparent.SetVisible(bVisible);
				o_Data.m_bEnableDiffuseLighting.SetVisible(bVisible);
				o_Data.m_bEnableSpecularLighting.SetVisible(bVisible);
				o_Data.m_bEnableShadows.SetVisible(bVisible);
				o_Data.m_bEnableInvisibleCastShadows.SetVisible(bVisible);
				o_Data.m_bEnableInvisibleInReflections.SetVisible(bVisible);
				o_Data.m_bEnableReflection.SetVisible(bVisible);
				o_Data.m_bEnableHair.SetVisible(bVisible);
			}

			if(l_CategoryMap["HDR"] != NULL)
			{
				bVisible = l_CategoryMap["HDR"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_HDRDebugMode.SetVisible(bVisible);
				o_Data.m_bToneMap.SetVisible(bVisible);
				o_Data.m_bHDRAA.SetVisible(bVisible);
				o_Data.m_bCaptureToneMapped.SetVisible(bVisible);
			}

			if(l_CategoryMap["Hardware Tessellation"] != NULL)
			{
				bVisible = l_CategoryMap["Hardware Tessellation"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bUseHardwareTessellation.SetVisible(bVisible);
//				o_Data.m_PixelSubdivLimit.SetVisible(bVisible);
			}

			if(l_CategoryMap["MotionBlur"] != NULL)
			{
				bVisible = l_CategoryMap["MotionBlur"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bMotionBlurEnable.SetVisible(bVisible);
				o_Data.m_MotionBlurSamples.SetVisible(bVisible);
				o_Data.m_MotionBlurPercent.SetVisible(bVisible);
			}

			if(l_CategoryMap["Transparency"] != NULL)
			{
				bVisible = l_CategoryMap["Transparency"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_TransparencyMode.SetVisible(bVisible);
				o_Data.m_bDebugDepthPeel.SetVisible(bVisible);
				o_Data.m_bDebugSinglePeel.SetVisible(bVisible);
				o_Data.m_nDebugDepthPeelLayers.SetVisible(bVisible);
			}

			if(l_CategoryMap["Ambient Occlusion"] != NULL)
			{
				bVisible = l_CategoryMap["Ambient Occlusion"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bUseAOVolumes.SetVisible(bVisible);
				o_Data.m_SSAONumSteps.SetVisible(bVisible);
				o_Data.m_SSAONumDirs.SetVisible(bVisible);
				o_Data.m_SSAOEnableBlur.SetVisible(bVisible);
				o_Data.m_bEnableSSAODepthPeeling.SetVisible(bVisible);
				o_Data.m_SSAONumLayers.SetVisible(bVisible);
				o_Data.m_SSAOQuality.SetVisible(bVisible);
				o_Data.m_bEnableSSAO.SetVisible(bVisible);
			}

			if(l_CategoryMap["Global Illumination"] != NULL)
			{
				bVisible = l_CategoryMap["Global Illumination"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bUseLPVGI.SetVisible(bVisible);
				o_Data.m_SSGINumSteps.SetVisible(bVisible);
				o_Data.m_SSGINumDirs.SetVisible(bVisible);
				o_Data.m_SSGIEnableBlur.SetVisible(bVisible);
				o_Data.m_bEnableSSGIDepthPeeling.SetVisible(bVisible);
				o_Data.m_SSGINumLayers.SetVisible(bVisible);
				o_Data.m_SSGIQuality.SetVisible(bVisible);
				o_Data.m_bEnableSSGI.SetVisible(bVisible);
			}

			if(l_CategoryMap["Hair"] != NULL )
			{
				bVisible = l_CategoryMap["Hair"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bEnableHair.SetVisible(bVisible);
				o_Data.m_bHairLines.SetVisible(bVisible);
				o_Data.m_HairTransparencyMode.SetVisible(bVisible);
//				o_Data.m_HairShadowType.SetVisible(bVisible);
//				o_Data.m_HairShadowRes.SetVisible(bVisible);
				o_Data.m_HairTessellation.SetVisible(bVisible);
//				o_Data.m_HairVertexLimit.SetVisible(bVisible);
//				o_Data.m_HairStrandSkip.SetVisible(bVisible);
				o_Data.m_HairSubPixelPower.SetVisible(bVisible);
				o_Data.m_bHairDepthPeel.SetVisible(bVisible);
				o_Data.m_nHairDepthPeelLayers.SetVisible(bVisible);
				o_Data.m_HairInterpolationCount.SetVisible(bVisible);
				o_Data.m_HairClumpRadius.SetVisible(bVisible);
			}

			if(l_CategoryMap["Render Passes from File"] != NULL )
			{
				bVisible = l_CategoryMap["Render Passes from File"]->GetCategoryVisible(rendererID) && choseMspEngine;
				o_Data.m_bRPF_AO.SetVisible(bVisible);
				o_Data.m_bRPF_GI.SetVisible(bVisible);
				o_Data.m_bRPF_Refl.SetVisible(bVisible);
				o_Data.m_bRPF_ShadowMask.SetVisible(bVisible);
				o_Data.m_bRPF_Beauty.SetVisible(bVisible);
			}

			if(l_CategoryMap["RenderMan"] != NULL )
			{
				bVisible = choseRmanEngine;
				//o_Data.m_bRmanRenderRIB.SetVisible(bVisible);
				//o_Data.m_bRmanGenShadowMaps.SetVisible(bVisible);
				//o_Data.m_bRmanGenReflectionMaps.SetVisible(bVisible);
				//o_Data.m_nRmanAArate.SetVisible(bVisible);
				o_Data.m_RmanShadingRate.SetVisible(bVisible);
				o_Data.m_RmanOutType.SetVisible(bVisible);
				o_Data.m_RmanBatchContent.SetVisible(bVisible);
				o_Data.m_bRmanCacheTextures.SetVisible(bVisible);
				o_Data.m_bRmanDisableWarnings.SetVisible(bVisible);
				//o_Data.m_bRmanTextureBatch.SetVisible(bVisible);
				o_Data.m_bRmanAOEnable.SetVisible(bVisible);
				o_Data.m_RmanAOsamples.SetVisible(bVisible);
				//o_Data.m_RmanAOMaxDist.SetVisible(bVisible);
				o_Data.m_RmanAOMaxVariation.SetVisible(bVisible);
				//o_Data.m_RmanAOConeAngle.SetVisible(bVisible);
				o_Data.m_RmanReflType.SetVisible(bVisible);
				o_Data.m_bRmanReflEnable.SetVisible(bVisible);
				o_Data.m_bRmanShadowEnable.SetVisible(bVisible);
				o_Data.m_RmanShadowType.SetVisible(bVisible);
				//o_Data.m_RmanShadowMinSamples.SetVisible(bVisible);
				//o_Data.m_RmanShadowSamples.SetVisible(bVisible);
				//o_Data.m_RmanShadowBias.SetVisible(bVisible);
				//o_Data.m_RmanShadowSoftness.SetVisible(bVisible);
				o_Data.m_bRmanGIEnable.SetVisible(bVisible);
				o_Data.m_RmanGIsamples.SetVisible(bVisible);
				//o_Data.m_RmanGIMaxDist.SetVisible(bVisible);
				o_Data.m_RmanGIMaxVariation.SetVisible(bVisible);
				//o_Data.m_RmanGIConeAngle.SetVisible(bVisible);
				//o_Data.m_RmanFilterType.SetVisible(bVisible);
				//o_Data.m_RmanFilterWidth.SetVisible(bVisible);
				o_Data.m_RmanNumCores.SetVisible(bVisible);
				o_Data.m_RmanTexMemory.SetVisible(bVisible);
				o_Data.m_RmanBucketSize.SetVisible(bVisible);
				o_Data.m_RmanBucketOrder.SetVisible(bVisible);
				//o_Data.m_RmanGridSize.SetVisible(bVisible);
				o_Data.m_RmanRayDepth.SetVisible(bVisible);
				o_Data.m_bRmanTonemapEnable.SetVisible(bVisible);
			}


			if(l_CategoryMap["MentalRay"] != NULL )
			{
				bVisible = choseMRayEngine;
				//o_Data.m_MrayVerbosity.SetVisible(bVisible);
				o_Data.m_MrayNumReflBounces.SetVisible(bVisible);
				o_Data.m_MrayNumRefrBounces.SetVisible(bVisible);
				o_Data.m_MrayMaxTraceDepth.SetVisible(bVisible);
				o_Data.m_bMrayAO.SetVisible(bVisible);
				o_Data.m_MrayAOSamples.SetVisible(bVisible);
				o_Data.m_bMrayFinalGather.SetVisible(bVisible);
				//o_Data.m_bMrayFGBlur.SetVisible(bVisible);
				o_Data.m_MrayFGNDiffuse.SetVisible(bVisible);
				o_Data.m_MrayFGNRefl.SetVisible(bVisible);
				o_Data.m_MrayFGNRefr.SetVisible(bVisible);
				o_Data.m_MrayFGNRays.SetVisible(bVisible);
				//o_Data.m_MrayFGColor.SetVisible(bVisible);
				o_Data.m_MrayOutputFormat.SetVisible(bVisible);
				o_Data.m_MrayNumThreads.SetVisible(bVisible);
				o_Data.m_MrayMemoryLimit.SetVisible(bVisible);
				o_Data.m_bMrayEnableReflections.SetVisible(bVisible);
				o_Data.m_bMrayEnableShadows.SetVisible(bVisible);
				o_Data.m_MrayShadowType.SetVisible(bVisible);
				o_Data.m_bMrayRewriteAssets.SetVisible(bVisible);
				o_Data.m_MrayVerbosityLevel.SetVisible(bVisible);
				o_Data.m_bMrayFGMapEnable.SetVisible(bVisible);
				o_Data.m_MrayFGMapRebuild.SetVisible(bVisible);
				o_Data.m_MrayFGMapPath.SetVisible(bVisible);
				o_Data.m_MrayReflSamples.SetVisible(bVisible);
				o_Data.m_bMrayOverrideMSPSampling.SetVisible(bVisible);
				o_Data.m_MrayMinCaptureSamples.SetVisible(bVisible);
				o_Data.m_MrayMaxCaptureSamples.SetVisible(bVisible);
				o_Data.m_MRayAAContrast.SetVisible(bVisible);
				o_Data.m_bMrayProgressive.SetVisible(bVisible);
				o_Data.m_bMRayTonemapEnable.SetVisible(bVisible);
				o_Data.m_bEnableIBL.SetVisible(bVisible);
				o_Data.m_IBLQuality.SetVisible(bVisible);
				o_Data.m_IBLMapRes.SetVisible(bVisible);
				o_Data.m_IBLScale.SetVisible(bVisible);
				o_Data.m_IBLSampleNum.SetVisible(bVisible);
				o_Data.m_bMrayIgnoreBadTex.SetVisible(bVisible);
				o_Data.m_bMrayDisplayPreview.SetVisible(bVisible);				
				o_Data.m_MrayProgSubsamplingSize.SetVisible(bVisible);
				o_Data.m_MrayProgSubsamplingMode.SetVisible(bVisible);
				o_Data.m_MrayProgSubsamplingPattern.SetVisible(bVisible);
				o_Data.m_MrayProgMinSamples.SetVisible(bVisible);
				o_Data.m_MrayProgMaxSamples.SetVisible(bVisible);
				o_Data.m_MrayProgMaxTime.SetVisible(bVisible);
				o_Data.m_MrayProgErrorThreshold.SetVisible(bVisible);
			}
		}
	}

	//------------------------------------------------------------------------
	// Init
	//------------------------------------------------------------------------
	void Initialize()
	{
		//initialize each category type and add them to the proper maps
		rprfPrefsCategory* cur_category;

		cur_category = new rprfPrefsCategory("Lighting");
		l_CategoryMap["Lighting"] = cur_category; 

		cur_category = new rprfPrefsCategory("Illumination Renderer");
		l_CategoryMap["Illumination Renderer"] = cur_category; 

		cur_category = new rprfPrefsCategory("Alpha");
		l_CategoryMap["Alpha"] = cur_category; 
		
		cur_category = new rprfPrefsCategory("Resolution");
		l_CategoryMap["Resolution"] = cur_category; 
		
		cur_category = new rprfPrefsCategory("Advanced Render Flags");
		l_CategoryMap["Advanced Render Flags"] = cur_category; 
		
		cur_category = new rprfPrefsCategory("HDR");
		l_CategoryMap["HDR"] = cur_category; 
		
		cur_category = new rprfPrefsCategory("Hardware Tessellation");
		l_CategoryMap["Hardware Tessellation"] = cur_category; 
		
		cur_category = new rprfPrefsCategory("MotionBlur");
		l_CategoryMap["MotionBlur"] = cur_category; 
		
		cur_category = new rprfPrefsCategory("Transparency");
		l_CategoryMap["Transparency"] = cur_category; 
		
		cur_category = new rprfPrefsCategory("Ambient Occlusion");
		l_CategoryMap["Ambient Occlusion"] = cur_category; 

		cur_category = new rprfPrefsCategory("Global Illumination");
		l_CategoryMap["Global Illumination"] = cur_category; 

		cur_category = new rprfPrefsCategory("Hair");
		l_CategoryMap["Hair"] = cur_category; 

		cur_category = new rprfPrefsCategory("Render Passes from File");
		l_CategoryMap["Render Passes from File"] = cur_category; 

		cur_category = new rprfPrefsCategory("RenderMan");
		l_CategoryMap["RenderMan"] = cur_category; 

		cur_category = new rprfPrefsCategory("MentalRay");
		l_CategoryMap["MentalRay"] = cur_category; 

		//setup category options
		SetupCategories();
	}

	//------------------------------------------------------------------------
	// DeInit
	//------------------------------------------------------------------------
	void DeInitialize()
	{
		//delete contents of full prefs map
		std::map<std::string, rprfPrefsCategory*>::iterator it, end = l_CategoryMap.end();
		for(it = l_CategoryMap.begin(); it != end; ++it)
		{
			delete (*it).second;
		}
	}

	//------------------------------------------------------------------------
	//  Both viewport and render layer prefs will use this function to register
	//	their properties
	//------------------------------------------------------------------------
	void RegisterPrefProperties(prtyObject* o_PrefsObject, rprfPrefsData& i_Data, bool i_bIsViewport)
	{
		prtyPropertyUIInfo* pPUII;
		prtyRangedFloatUIInfo* pRFUII;
		prtyComboBoxUIInfo* pCBUII;
		prtyNumericUpDownUIInfo* pNUDUII;
		//prtyFloatEditUIInfo* pFEUII;

		if(i_bIsViewport)
		{
//			pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RendererTypeVP), "Renderer", "Select the viewport render type");
//			o_PrefsObject->AddProperty( pCBUII );
			pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RenderPassVP), "Renderer", "Select the viewport render pass");
			o_PrefsObject->AddProperty( pCBUII );
		}
		else
		{
			pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RendererEngine), "Renderer", "Select the render layer render engine");
			o_PrefsObject->AddProperty( pCBUII );
			// removing this in capture because the choice of renderer is now in the Passes UI
//			pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RendererType), "Renderer", "Select the render layer render type");			
//			o_PrefsObject->AddProperty( pCBUII );
		}

#ifdef _DEBUG
		if(i_bIsViewport)
		{
			pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_HDRDebugMode), "DEBUG", "HDR Debug Mode");
#if(SGPU_APP != MS_CORE)
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
#else
			pCBUII->AddItem(std::string("Full Render"),0);
			pCBUII->AddItem(std::string("[N/A] Clamped HDR Buffer"),1);
			pCBUII->AddItem(std::string("[N/A] Scaled HDR Buffer"),2);
			pCBUII->AddItem(std::string("[N/A] Pixel Luminances"),3);
			pCBUII->AddItem(std::string("DOF blurriness"),4);
			pCBUII->AddItem(std::string("[N/A] 1st Luminance pass"),5);
			pCBUII->AddItem(std::string("[N/A] Bright pass"),6);
			pCBUII->AddItem(std::string("[N/A] Bloom source"),7);
			pCBUII->AddItem(std::string("[N/A] Bloom"),8);
			pCBUII->AddItem(std::string("[N/A] Star"),9);
#endif
			o_PrefsObject->AddProperty( pCBUII );
		}
		if(i_bIsViewport)
		{
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bToneMap), "DEBUG", "HDR Tone Map");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bLowResolution), "DEBUG", "Low Resolution");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bProjLightFrustumCull), "DEBUG", "Projected Light Frustum Culling");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bProjLightsOn), "DEBUG", "Turn on all Proj Lights");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bPtLightsOn), "DEBUG", "Turn on all Point Lights");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bDoShadowMapGen), "DEBUG", "Enable shadow map generation");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableLitPass), "DEBUG", "Render Lit Pass");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableTransparent), "DEBUG", "Render transparent objects");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableDiffuseLighting), "DEBUG", "Enable Diffuse");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableSpecularLighting), "DEBUG", "Enable Specular");
			o_PrefsObject->AddProperty( pPUII );
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableDOF), "DEBUG", "Allow DOF rendering feature");
			o_PrefsObject->AddProperty( pPUII );
		}
#endif // _DEBUG

		if(i_bIsViewport)
		{
//			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMultipassOn), "Renderer", "Preview mode");
//			o_PrefsObject->AddProperty( pPUII );
		}

	//			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bBlueShift), "HDR", "HDR Blue Shift");
	//			o_PrefsObject->AddProperty( pPUII );
		if(!i_bIsViewport)
		{
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bCaptureToneMapped), "HDR", "Capture Tonemapped Pixels");
			o_PrefsObject->AddProperty( pPUII );
		}

		//pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bHeadlightOn), "Lighting", "Turn on directional light in direction of camera, turning off other lights");
		//o_PrefsObject->AddProperty( pPUII );
	
//		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bIlluminationUsesNormals), "Illumination Renderer", "Take normals into account");
//		o_PrefsObject->AddProperty( pPUII );
	
		//pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMatteMode), "Alpha", "Render Alpha");
		//o_PrefsObject->AddProperty( pPUII );
	
		if(i_bIsViewport)
		{
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bHDRAA), "Advanced Render Flags", "Hardware multisample antialiasing");
			o_PrefsObject->AddProperty( pPUII );
		}

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bUseHardwareTessellation), "Advanced Render Flags", "Enable");
		pPUII->SetDescription( "Enable use of hardware tessellation for Displacement mapped objects.");
	#if(SGPU_APP == MS_CORE)
		pPUII->SetReadOnly( true );
	#endif
		o_PrefsObject->AddProperty( pPUII );

//		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_PixelSubdivLimit), "Advanced Render Flags", "Number of pixels of smallest polygon edge.");
//		pRFUII->SetMinimum(0.0000);
//		pRFUII->SetMaximum(50.0);
//		pRFUII->SetDecimalPlaces(0);
//		pRFUII->SetNumTicks(50);
//		o_PrefsObject->AddProperty(pRFUII);

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableGlow), "Advanced Render Flags", "Allow Glow rendering feature");
		o_PrefsObject->AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableOutline), "Advanced Render Flags", "Allow Outline rendering feature");
		o_PrefsObject->AddProperty( pPUII );
	//			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableAmbientPass), "Advanced Render Flags", "Render Ambient Pass");
	//			o_PrefsObject->AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableEnvironment), "Advanced Render Flags", "Render Environments");
		o_PrefsObject->AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableShadows), "Advanced Render Flags", "Enable Shadows");
		o_PrefsObject->AddProperty( pPUII );
		if(!i_bIsViewport)
		{
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableInvisibleMaskBlack), "Advanced Render Flags", "Enable Invisible Objects Mask");
			o_PrefsObject->AddProperty( pPUII );
		}
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableInvisibleCastShadows), "Advanced Render Flags", "Enable Invisible Objects Cast Shadows");
		o_PrefsObject->AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableInvisibleInReflections), "Advanced Render Flags", "Enable Invisible Objects In Reflections");
		o_PrefsObject->AddProperty( pPUII );
		
	//			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableDeferredTransparency), "Advanced Render Flags", "Draw particles last");
	//			o_PrefsObject->AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableReflection), "Advanced Render Flags", "Render dynamic reflections");
		o_PrefsObject->AddProperty( pPUII );
	
//			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableAO), "AO", "Use ambient occlusion textures");
//			o_PrefsObject->AddProperty( pPUII );
//			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRecalcAOPerFrame), "AO", "Recompute all AO receivers globally on each frame");
//			o_PrefsObject->AddProperty( pPUII );


	//pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRenderWireframe), "Wireframe", "Enable");
	//o_PrefsObject->AddProperty( pPUII );


	#ifdef ENABLE_MOTIONBLUR
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMotionBlurEnable), "Motion Blur", "Allow Motion Blur rendering feature");
		o_PrefsObject->AddProperty( pPUII );
		prtyRangedFloatUIInfo* pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_MotionBlurSamples), "Motion Blur", "Number of Samples");
		pRFUII->SetMinimum(1.0);
		pRFUII->SetMaximum(50.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(49);
		o_PrefsObject->AddProperty(pRFUII);
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_MotionBlurPercent), "Motion Blur", "Percent of Blur");
		pRFUII->SetMinimum(0.0000);
		pRFUII->SetMaximum(100.0);
		pRFUII->SetDecimalPlaces(2);
		pRFUII->SetNumTicks(100);
		o_PrefsObject->AddProperty(pRFUII);
	#endif
	
		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_TransparencyMode), "Transparency", "Select the accuracy of transparency");
		o_PrefsObject->AddProperty( pCBUII );
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bDebugDepthPeel), "Transparency", "Enable inspection of a depth peeled layer");
		o_PrefsObject->AddProperty( pPUII );
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_nDebugDepthPeelLayers), "Transparency", "Number of layers");
		pRFUII->SetMinimum(0.0000);
		pRFUII->SetMaximum(50.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(50);
		o_PrefsObject->AddProperty(pRFUII);
#ifdef _DEBUG
		if(i_bIsViewport)
		{
			pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bDebugSinglePeel), "Transparency", "Enable inspection of a single depth peeled layer");
			o_PrefsObject->AddProperty( pPUII );
		}
#endif

#ifdef HAIR_SUPPORTED
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableHair), "Hair", "Enable Hair");
		o_PrefsObject->AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bHairLines), "Hair", "Display hair as lines");
		o_PrefsObject->AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_HairTransparencyMode), "Hair", "Select the accuracy of transparency");
		o_PrefsObject->AddProperty( pCBUII );
/*
		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_HairShadowRes), "Hair", "Resolution of shadow maps");
		o_PrefsObject->AddProperty( pCBUII );

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_HairShadowType), "Hair", "Type of shadow algorithm");
		o_PrefsObject->AddProperty( pCBUII );
*/
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_HairTessellation), "Hair", "Number of subdivisions");
		pRFUII->SetMinimum(1.0000);
		pRFUII->SetMaximum(14.0);
		pRFUII->SetDecimalPlaces(2);
		pRFUII->SetRestrictFlag( true );
		o_PrefsObject->AddProperty(pRFUII);

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_HairInterpolationCount), "Hair", "Strands per Clump");
		pRFUII->SetMinimum(1.0000);
		pRFUII->SetMaximum(64.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(64);
		o_PrefsObject->AddProperty(pRFUII);

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_HairClumpRadius), "Hair", "Radius of Clump");
		pRFUII->SetMinimum(0.0000);
		pRFUII->SetMaximum(10.0);
		pRFUII->SetDecimalPlaces(2);
		o_PrefsObject->AddProperty(pRFUII);

/*
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_HairVertexLimit), "Hair", "Maximum vertices before batching in (K)");
		pRFUII->SetMinimum(1.0000);
		pRFUII->SetMaximum(1000.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(999);
		pRFUII->SetRestrictFlag( true );
		o_PrefsObject->AddProperty(pRFUII);

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_HairStrandSkip), "Hair", "Skip (N) vertices for faster display.");
		pRFUII->SetMinimum(0.0000);
		pRFUII->SetMaximum(1000.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(1000);
		pRFUII->SetRestrictFlag( true );
		o_PrefsObject->AddProperty(pRFUII);
*/
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_HairSubPixelPower), "Hair", "Power factor for Sub-Pixel Antialiasing.");
		pRFUII->SetMinimum(0.0000);
		pRFUII->SetMaximum(4.0);
		pRFUII->SetDecimalPlaces(2);
		pRFUII->SetRestrictFlag( true );
		o_PrefsObject->AddProperty(pRFUII);

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bHairDepthPeel), "Hair", "Enable inspection of a depth peeled layer");
		o_PrefsObject->AddProperty( pPUII );

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_nHairDepthPeelLayers), "Hair", "Number of layers");
		pRFUII->SetMinimum(0.0000);
		pRFUII->SetMaximum(100.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(100);
		o_PrefsObject->AddProperty(pRFUII);
#endif

//#ifdef ACTIVATE_RENDERMAN

	if( !i_bIsViewport )
	{

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRPF_AO), "Render Passes from File", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRPF_GI), "Render Passes from File", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRPF_Refl), "Render Passes from File", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		//pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRPF_ShadowMask), "Render Passes from File", "Enable");
		//o_PrefsObject->AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRPF_Beauty), "Render Passes from File", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		//pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RmanBatchContent), "RenderMan Exporter", "Batch File Action");
		//o_PrefsObject->AddProperty( pCBUII );

		//pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanTextureBatch), "RenderMan Exporter", "Launch Batch File");
		//o_PrefsObject->AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanCacheTextures), "RenderMan Export Settings", "Rewrite Geometry and Textures");
		o_PrefsObject->AddProperty( pPUII );

		//pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanRenderRIB), "RenderMan Options", "Launch Renderer Each Frame");
		//o_PrefsObject->AddProperty( pPUII );

		//pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanGenShadowMaps), "RenderMan Options", "Generate Shadow Maps");
		//o_PrefsObject->AddProperty( pPUII );

		//pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanGenReflectionMaps), "RenderMan Options", "Generate Reflection Maps");
		//o_PrefsObject->AddProperty( pPUII );


		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RmanOutType), "RenderMan Render Settings", "Output Type");
		o_PrefsObject->AddProperty( pCBUII );

		//pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RmanFilterType), "RenderMan Render Settings", "Pixel Filter");
		//o_PrefsObject->AddProperty( pCBUII );

		//pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanFilterWidth), "RenderMan Render Settings", "Filter Width");
		//pNUDUII->SetDecimalPlaces(2);
		//pNUDUII->SetIncrement(0.01f);
		//pNUDUII->SetMinimum(1);
		//pNUDUII->SetMaximum(256);
		//pNUDUII->SetRestrictFlag(true);
		//o_PrefsObject->AddProperty( pNUDUII );

		//pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_nRmanAArate), "RenderMan Render Settings", "Sampling Rate");
		//pNUDUII->SetDecimalPlaces(0);
		//pNUDUII->SetMinimum(1);
		//pNUDUII->SetMaximum(256);
		//pNUDUII->SetRestrictFlag(true);
		//o_PrefsObject->AddProperty( pNUDUII );

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_RmanShadingRate), "RenderMan Render Settings", "Shading Rate");
		pRFUII->SetDecimalPlaces(3);
		pRFUII->SetMinimum(0.001f);
		pRFUII->SetMaximum(1.0f);
		pRFUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty(pRFUII);

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RmanBucketOrder), "RenderMan Render Settings", "Bucket Order");
		o_PrefsObject->AddProperty( pCBUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanBucketSize), "RenderMan Render Settings", "Bucket Size");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1.0f);
		pNUDUII->SetMaximum(256.0f);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanRayDepth), "RenderMan Render Settings", "Ray Tracing Depth");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1.0f);
		pNUDUII->SetMaximum(30.0f);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanNumCores), "RenderMan Performance Settings", "Number of Render Threads");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1.0f);
		pNUDUII->SetMaximum(64.0f);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanTexMemory), "RenderMan Performance Settings", "Texture Memory");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1.0f);
		pNUDUII->SetMaximum(65536.0f);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanDisableWarnings), "RenderMan Performance Settings", "Disable Warnings");
		o_PrefsObject->AddProperty( pPUII );

		//pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanGridSize), "RenderMan Performance Settings", "Grid Size");
		//pNUDUII->SetDecimalPlaces(0);
		//pNUDUII->SetMinimum(1);
		//pNUDUII->SetMaximum(1024);
		//pNUDUII->SetRestrictFlag(true);
		//o_PrefsObject->AddProperty( pNUDUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanTonemapEnable), "RenderMan Tonemapping", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanReflEnable), "RenderMan Reflection and Refraction", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RmanReflType), "RenderMan Reflection and Refraction", "Refl & Refr Type");
		o_PrefsObject->AddProperty( pCBUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanShadowEnable), "RenderMan Shadows", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_RmanShadowType), "RenderMan Shadows", "Shadow Type");
		o_PrefsObject->AddProperty( pCBUII );

		//pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanShadowMinSamples), "RenderMan Shadows", "Min Samples");
		//pNUDUII->SetDecimalPlaces(0);
		//pNUDUII->SetMinimum(1);
		//pNUDUII->SetMaximum(1024);
		//o_PrefsObject->AddProperty( pNUDUII );

		//pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanShadowSamples), "RenderMan Shadows", "Max Samples");
		//pNUDUII->SetDecimalPlaces(0);
		//pNUDUII->SetMinimum(1);
		//pNUDUII->SetMaximum(1024);
		//o_PrefsObject->AddProperty( pNUDUII );

		//pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_RmanShadowBias), "RenderMan Shadows", "Bias");
		//pRFUII->SetDecimalPlaces(0);
		//pRFUII->SetMinimum(0);
		//pRFUII->SetMaximum(1000);
		//o_PrefsObject->AddProperty(pRFUII);

		//pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_RmanShadowSoftness), "RenderMan Shadows", "Softness");
		//pRFUII->SetDecimalPlaces(2);
		//pRFUII->SetMinimum(0);
		//pRFUII->SetMaximum(1);
		//o_PrefsObject->AddProperty(pRFUII);

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanAOEnable), "RenderMan Ambient Occlusion", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanAOsamples), "RenderMan Ambient Occlusion", "Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(1024);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		//pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_RmanAOMaxDist), "RenderMan Ambient Occlusion", "Max Distance");
		//pRFUII->SetDecimalPlaces(0);
		//pRFUII->SetMinimum(0);
		//pRFUII->SetMaximum(100);
		////pRFUII->SetRestrictFlag(true);
		//o_PrefsObject->AddProperty(pRFUII);

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_RmanAOMaxVariation), "RenderMan Ambient Occlusion", "Max Variation (scaled to range from 0 to 100)");
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetMinimum(0);
		pRFUII->SetMaximum(100);
		o_PrefsObject->AddProperty(pRFUII);

		//pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_RmanAOConeAngle), "RenderMan Ambient Occlusion", "Cone Angle");
		//pRFUII->SetDecimalPlaces(2);
		//pRFUII->SetMinimum(0);
		//pRFUII->SetMaximum(90);
		//o_PrefsObject->AddProperty(pRFUII);

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bRmanGIEnable), "RenderMan Color Bleeding", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_RmanGIsamples), "RenderMan Color Bleeding", "Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(1024);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		//pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_RmanGIMaxDist), "RenderMan Color Bleeding", "Max Distance");
		//pRFUII->SetDecimalPlaces(0);
		//pRFUII->SetMinimum(0);
		//pRFUII->SetMaximum(100);
		////pRFUII->SetRestrictFlag(true);
		//o_PrefsObject->AddProperty(pRFUII);

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_RmanGIMaxVariation), "RenderMan Color Bleeding", "Max Variation (scaled to range from 0 to 100)");
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetMinimum(0);
		pRFUII->SetMaximum(100);
		pRFUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty(pRFUII);

		//pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_RmanGIConeAngle), "RenderMan GI", "Cone Angle");
		//pRFUII->SetDecimalPlaces(2);
		//pRFUII->SetMinimum(0);
		//pRFUII->SetMaximum(90);
		//o_PrefsObject->AddProperty(pRFUII);

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayRewriteAssets), "mental ray Export Settings", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayOverrideMSPSampling), "mental ray Sampling Settings", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayMinCaptureSamples), "mental ray Sampling Settings", "Min Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(-8);
		pNUDUII->SetMaximum(8);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayMaxCaptureSamples), "mental ray Sampling Settings", "Max Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(-8);
		pNUDUII->SetMaximum(8);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_MRayAAContrast), "mental ray Sampling Settings", "AA Contrast");
		pRFUII->SetDecimalPlaces(2);
		pRFUII->SetMinimum(0);
		pRFUII->SetMaximum(1);
		o_PrefsObject->AddProperty(pRFUII);

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_MrayOutputFormat), "mental ray Render Settings", "Output Type");
		o_PrefsObject->AddProperty( pCBUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayNumReflBounces), "mental ray Render Settings", "Reflection bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(32);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayNumRefrBounces), "mental ray Render Settings", "Refraction bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(32);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );
		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayMaxTraceDepth), "mental ray Render Settings", "Total bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(64);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayDisplayPreview), "mental ray Render Settings", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayIgnoreBadTex), "mental ray Render Settings", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayNumThreads), "mental ray Performance Settings", "Number of Render Threads");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayMemoryLimit), "mental ray Performance Settings", "Memory Limit (MB)");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		//pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayVerbosity), "mental ray Performance Settings", "Output verbosity");
		//pNUDUII->SetDecimalPlaces(0);
		//pNUDUII->SetMinimum(0);
		//pNUDUII->SetMaximum(5);
		//pNUDUII->SetRestrictFlag(true);
		//o_PrefsObject->AddProperty( pNUDUII );

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_MrayVerbosityLevel), "mental ray Performance Settings", "Verbosity Level");
		o_PrefsObject->AddProperty( pCBUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMRayTonemapEnable), "mental ray Tonemapping", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayEnableReflections), "mental ray Reflection and Refraction", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayReflSamples), "mental ray Reflection and Refraction", "Number of Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(64);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayEnableShadows), "mental ray Shadows", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_MrayShadowType), "mental ray Shadows", "Shadow Type");
		o_PrefsObject->AddProperty( pCBUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayAO), "mental ray Ambient Occlusion", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayAOSamples), "mental ray Ambient Occlusion", "Number of Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(1024);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		// IBL properties
		/*pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableIBL), "mental ray IBL", "Enable");
		pPUII->SetReadOnly(true);
		o_PrefsObject->AddProperty( pPUII );*/
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_IBLQuality), "mental ray IBL", "IBL Quality");
		pRFUII->SetDecimalPlaces(1);
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(10.0f);
		o_PrefsObject->AddProperty(pRFUII);
		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_IBLMapRes), "mental ray IBL", "IBL Resolution");
		o_PrefsObject->AddProperty( pCBUII );
		/*pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_IBLScale), "mental ray IBL", "IBL Scale");
		pRFUII->SetDecimalPlaces(1);
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(10.0f);
		o_PrefsObject->AddProperty(pRFUII);*/
		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_IBLSampleNum), "mental ray IBL", "Number of Samples");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(64);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayFinalGather), "mental ray Final Gather", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		/*pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayFGBlur), "mental ray Final Gather", "Enable blur");
		o_PrefsObject->AddProperty( pPUII );*/

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayFGNDiffuse), "mental ray Final Gather", "Diffuse bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(10);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayFGNRefl), "mental ray Final Gather", "Refl bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(10);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayFGNRefr), "mental ray Final Gather", "Refr bounces");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(10);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayFGNRays), "mental ray Final Gather", "num rays");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(1);
		pNUDUII->SetMaximum(10000);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayFGMapEnable), "mental ray Final Gather Map", "Enable FG Map");
		o_PrefsObject->AddProperty( pPUII );

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_MrayFGMapRebuild), "mental ray Final Gather Map", "FG Map Rebuilding");
		o_PrefsObject->AddProperty( pCBUII );

		pPUII = new prtyFileChooserUIInfo(&(i_Data.m_MrayFGMapPath), "mental ray Final Gather Map", "FG Map Location");
		o_PrefsObject->AddProperty( pPUII );



		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bMrayProgressive), "mental ray Progressive Rendering", "Enable");
		o_PrefsObject->AddProperty( pPUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayProgSubsamplingSize), "mental ray Progressive Rendering", "Subsampling Size");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(32);
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_MrayProgSubsamplingMode), "mental ray Progressive Rendering", "Subsampling Mode");
		o_PrefsObject->AddProperty( pCBUII );

		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_MrayProgSubsamplingPattern), "mental ray Progressive Rendering", "Subsampling Pattern");
		o_PrefsObject->AddProperty( pCBUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayProgMinSamples), "mental ray Progressive Rendering", "Min Samples");
		pNUDUII->SetDecimalPlaces(0);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayProgMaxSamples), "mental ray Progressive Rendering", "Max Samples");
		pNUDUII->SetDecimalPlaces(0);
		o_PrefsObject->AddProperty( pNUDUII );

		pNUDUII = new prtyNumericUpDownUIInfo(&(i_Data.m_MrayProgMaxTime), "mental ray Progressive Rendering", "Max Render Time");
		pNUDUII->SetDecimalPlaces(0);
		pNUDUII->SetMinimum(0);
		pNUDUII->SetMaximum(21600); // an hour
		pNUDUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty( pNUDUII );

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_MrayProgErrorThreshold), "mental ray Progressive Rendering", "Error Threshold");
		pRFUII->SetDecimalPlaces(2);
		pRFUII->SetMinimum(0.0f);
		pRFUII->SetMaximum(1.0f);
		o_PrefsObject->AddProperty(pRFUII);

	}

	}
	
	//------------------------------------------------------------------------
	// register the ssao property for the pref object
	//------------------------------------------------------------------------
	void RegisterSSAOEnable(prtyObject* o_PrefsObject, rprfPrefsData& i_Data)
	{
		prtyPropertyUIInfo* pPUII;

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableSSAO), "Ambient Occlusion", "Enable AO");
		o_PrefsObject->AddProperty( pPUII );
	}

	//------------------------------------------------------------------------
	// register the ssao sampling
	//------------------------------------------------------------------------
	void RegisterSSAOSampling(prtyObject* o_PrefsObject, rprfPrefsData& i_Data) 
	{
		prtyComboBoxUIInfo* pCBUII;		
		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_SSAOQuality), "Ambient Occlusion", "Sampling quality presets");
		o_PrefsObject->AddProperty( pCBUII );
	}

	//------------------------------------------------------------------------
	// register the ssao gui for the pref object
	//------------------------------------------------------------------------
	void RegisterSSAOgui(prtyObject* o_PrefsObject, rprfPrefsData& i_Data)
	{
		prtyPropertyUIInfo* pPUII;
		prtyRangedFloatUIInfo* pRFUII;

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bUseAOVolumes), "Ambient Occlusion", "Use AO Volumes algorithm");
		o_PrefsObject->AddProperty( pPUII );
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_SSAONumSteps), "Ambient Occlusion", "number of steps");
		pRFUII->SetMinimum(0.0000);
		pRFUII->SetMaximum(40.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(41);
		pRFUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty(pRFUII);
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_SSAONumDirs), "Ambient Occlusion", "number of directions (max=32)");
		pRFUII->SetMinimum(1.0000);
		pRFUII->SetMaximum(32.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(31);
		pRFUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty(pRFUII);
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_SSAOEnableBlur), "Ambient Occlusion", "enable blur pass");
		o_PrefsObject->AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableSSAODepthPeeling), "Ambient Occlusion", "enable multiple depths");
		o_PrefsObject->AddProperty( pPUII );
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_SSAONumLayers), "Ambient Occlusion", "number of depth layers");
		pRFUII->SetMinimum(1.0);
		pRFUII->SetMaximum(4.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(3);
		pRFUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty(pRFUII);
	}

	//------------------------------------------------------------------------
	// register the ssgi property for the pref object
	//------------------------------------------------------------------------
	void RegisterSSGIEnable(prtyObject* o_PrefsObject, rprfPrefsData& i_Data)
	{
		prtyPropertyUIInfo* pPUII;

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableSSGI), "Global Illumination", "Enable GI");
		o_PrefsObject->AddProperty( pPUII );
	}

	//------------------------------------------------------------------------
	// register the ssgi sampling
	//------------------------------------------------------------------------
	void RegisterSSGISampling(prtyObject* o_PrefsObject, rprfPrefsData& i_Data) 
	{
		prtyComboBoxUIInfo* pCBUII;		
		pCBUII = new prtyComboBoxUIInfo(&(i_Data.m_SSGIQuality), "Global Illumination", "Sampling quality presets");
		o_PrefsObject->AddProperty( pCBUII );
	}

	//------------------------------------------------------------------------
	// register the ssao gui for the pref object
	//------------------------------------------------------------------------
	void RegisterSSGIgui(prtyObject* o_PrefsObject, rprfPrefsData& i_Data)
	{
		prtyPropertyUIInfo* pPUII;
		prtyRangedFloatUIInfo* pRFUII;

		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bUseLPVGI), "Global Illumination", "Use LPV GI algorithm");
		o_PrefsObject->AddProperty( pPUII );

		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_SSGINumSteps), "Global Illumination", "number of steps");
		pRFUII->SetMinimum(0.0000);
		pRFUII->SetMaximum(40.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(41);
		pRFUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty(pRFUII);
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_SSGINumDirs), "Global Illumination", "number of directions (max=32)");
		pRFUII->SetMinimum(1.0000);
		pRFUII->SetMaximum(32.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(31);
		pRFUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty(pRFUII);
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_SSGIEnableBlur), "Global Illumination", "enable blur pass");
		o_PrefsObject->AddProperty( pPUII );
		pPUII = new prtyCheckBoxUIInfo(&(i_Data.m_bEnableSSGIDepthPeeling), "Global Illumination", "enable multiple depths");
		o_PrefsObject->AddProperty( pPUII );
		pRFUII = new prtyRangedFloatUIInfo(&(i_Data.m_SSGINumLayers), "Global Illumination", "number of depth layers");
		pRFUII->SetMinimum(1.0);
		pRFUII->SetMaximum(4.0);
		pRFUII->SetDecimalPlaces(0);
		pRFUII->SetNumTicks(3);
		pRFUII->SetRestrictFlag(true);
		o_PrefsObject->AddProperty(pRFUII);
	}

	//------------------------------------------------------------------------
	// For each category in the render pref dialog, we need to specify which
	// ones are populated for each render type
	//------------------------------------------------------------------------
	void SetupCategories()
	{
		rprfPrefsCategory* category;

		category = get_category_object("Lighting");
		if(category != NULL)
			setup_lighting_category(category);

		category = get_category_object("Illumination Renderer");
		if(category != NULL)
			setup_illumination_category(category);

		category = get_category_object("Alpha");
		if(category != NULL)
			setup_alpha_category(category);
		
		category = get_category_object("Resolution");
		if(category != NULL)
			setup_resolution_category(category);
		
		category = get_category_object("Advanced Render Flags");
		if(category != NULL)
			setup_passes_category(category);
		
		category = get_category_object("HDR");
		if(category != NULL)
			setup_hdr_category(category);
		
		category = get_category_object("Hardware Tessellation");
		if(category != NULL)
			setup_tessellation_category(category);

		category = get_category_object("MotionBlur");
		if(category != NULL)
			setup_motionblur_category(category);
		
		category = get_category_object("Transparency");
		if(category != NULL)
			setup_transparency_category(category);
		
		category = get_category_object("Ambient Occlusion");
		if(category != NULL)
			setup_ao_category(category);

		category = get_category_object("Global Illumination");
		if(category != NULL)
			setup_gi_category(category);

		category = get_category_object("Hair");
		if(category != NULL)
			setup_hair_category(category);

		category = get_category_object("Render Passes from File");
		if(category != NULL)
			setup_rpf_category(category);

		category = get_category_object("RenderMan");
		if(category != NULL)
			setup_rman_category(category);
	}

	//------------------------------------------------------------------------
	// When the render type is changed, we want to change which categories
	// are visible
	//------------------------------------------------------------------------
	void UpdateGUICategories(rprfPrefsData& o_Data, bool i_bIsViewport)
	{
		update_properties(o_Data, i_bIsViewport);
		if( !i_bIsViewport )
			l_bUpdate = true;
		else
			l_bUpdateVP = true;
	}

	//------------------------------------------------------------------------
	// Set the update function pointers, these function will update the 
	// appropriate render pref dialog after a render type has changed
	//------------------------------------------------------------------------
	void SetUpdateFunction(void (*i_UpdateFunction)())
	{
		l_prefUpdateFunction = i_UpdateFunction;
	}

	void SetUpdateFunctionVP(void (*i_UpdateFunctionVP)())
	{
		l_prefUpdateFunctionVP = i_UpdateFunctionVP;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateDialog()
	{
		if(l_bUpdate)
		{
			if(l_prefUpdateFunction != NULL)
				l_prefUpdateFunction();
			l_bUpdate = false;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateDialogVP()
	{
		if(l_bUpdateVP)
		{
//			if(l_prefUpdateFunctionVP != NULL)
//				l_prefUpdateFunctionVP();
			l_bUpdateVP = false;
		}
	}

	//------------------------------------------------------------------------
	// Return whether or not a render type is enabled
	//------------------------------------------------------------------------
	bool RenderTypeEnabled( int i_RenderType )
	{
		switch( i_RenderType )
		{
			case 0:
				// Default
			#if(SGPU_APP == MS_CORE)
				return true;
			#endif
				return true;

			case 1:
				//AO
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;

			case 2:
				//Depth
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;

			case 3:
				//Shadow
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
				
			case 4:
				//Illumination
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
				
			case 5:
				//Normals
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
				
			case 6:
				//Dirty Matte
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
				
			case 7:
				//Wireframe
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;

			case 8:
				//Materials
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
			case 9:
				//Reflection only
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
			case 10:
				//GI
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;

			case 11:
				//Velocity Map
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
				

			default:
				//	catch-all
				return false;
		}
	}

	//------------------------------------------------------------------------
	// Return whether or not a HDR layer is valid
	//------------------------------------------------------------------------
	bool HDRLayerEnabled( int i_HDRLayer )
	{
		switch( i_HDRLayer )
		{
			case 0:
				// Full
			#if(SGPU_APP == MS_CORE)
				return true;
			#endif
				return true;

			case 1:
				//Clamped HDR
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;

			case 2:
				//Scaled HDR
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;

			case 3:
				//Pixel Luminances
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
				
			case 4:
				//DOF
			#if(SGPU_APP == MS_CORE)
				return true;
			#endif
				return true;
				
			case 5:
				//1st Luminance
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
				
			case 6:
				//Bright Pass
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
				
			case 7:
				//Bloom source
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;

			case 8:
				//Bloom
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;
				
			case 9:
				//Star
			#if(SGPU_APP == MS_CORE)
				return false;
			#endif
				return true;

			default:
				//defualt
				return false;
		}
	}

} // end namespace rprfPrefsUtil