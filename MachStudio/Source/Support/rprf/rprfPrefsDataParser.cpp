/********************************************************************************************\
**  rprfPrefsDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2005-7 - All Rights Reserved
\********************************************************************************************/
#include "Support/rprf/rprfPrefsDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"


//============================================================================
//============================================================================
namespace rprfPrefsDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_PRFD = chDefs::MakeName('P', 'R', 'F', 'D');	// prefs data

//------------------------------------------------------------------------
// never change this function! it is frozen at v6 of the prfd chunk.
//------------------------------------------------------------------------
std::string GetRendererTypePermIDFromV6(envType::UInt8 i_PrtyValue)
{
	switch(i_PrtyValue)
	{
	case 0:
		return "HDR";
	case 1:
		return "AO";
	case 2:
		return "Depth";
	case 3:
		return "ShdwMask";
	case 4:
		return "Normal";
	case 5:
		return "Velocity";
	default:
		DBG_ASSERT(false, "Unknown renderer type read from pre-v7 PRFD chunk");
		return "";
	};
}

//------------------------------------------------------------------------
// never change this function! it is frozen at v17 of the prfd chunk.
//------------------------------------------------------------------------
std::string GetRendererTypePermIDFromV17(envType::UInt8 i_PrtyValue)
{
	switch(i_PrtyValue)
	{
	case 0:
		return "HDR";
	case 1:
		return "AO";
	case 2:
		return "Depth";
	case 3:
		return "ShdwMask";
	case 4:
		return "Illum";
	case 5:
		return "Normal";
	case 6:
		return "Dirty";
	case 7:
		return "Wire";
	case 8: 
		return "Material";
	case 9:
		return "Refl";
	case 10:
		return "Velocity";
	default:
		DBG_ASSERT(false, "Unknown renderer type enum index for writing to file.");
		return "";
	};
}

//------------------------------------------------------------------------
// This function gets the permID to write out to disk based on the current
// enum order. This must be maintained to match the m_RendererType enum 
// defined in rprfPrefsData.
//------------------------------------------------------------------------
std::string GetRendererTypePermID(envType::UInt8 i_PrtyValue)
{
	switch(i_PrtyValue)
	{
	case 0:
		return "HDR";
	case 1:
		return "AO";
	case 2:
		return "Depth";
	case 3:
		return "ShdwMask";
	case 4:
		return "Illum";
	case 5:
		return "Normal";
	case 6:
		return "Dirty";
	case 7:
		return "Wire";
	case 8: 
		return "Material";
	case 9:
		return "Refl";
	case 10:
		return "GI";
	case 11:
		return "Velocity";
	default:
		DBG_ASSERT(false, "Unknown renderer type enum index for writing to file.");
		return "";
	};
}

//------------------------------------------------------------------------
// When reading a perm ID, map it to the correct enum value in the current
// enum used in rprfPrefsData::m_RendererType 
//------------------------------------------------------------------------
envType::UInt8 GetRendererTypeFromPermID(std::string i_PermID, chDefs::Version i_Version)
{
	DBG_ASSERT(i_Version >= 6, "Renderer type IDs not used before v7");
	if (i_PermID == "HDR")
		return 0;
	else if (i_PermID == "AO")
		return 1;
	else if (i_PermID == "Depth")
		return 2;
	else if (i_PermID == "ShdwMask")
		return 3;
	else if (i_PermID == "Illum")
		return 4;
	else if (i_PermID == "Normal")
		return 5;
	else if (i_PermID == "Dirty")
		return 6;
	else if (i_PermID == "Wire")
		return 7;
	// When Materials was added as a perm id, it was on a v1.2 branch. 
	// (around 11/23/09)
	// The branch code was never added to the trunk until 2 months later.
	// When the branch code was added, "Material" was used erroneously.
	// The current perm ID is Material but Materials is here to allow those 
	// old files to load correctly. So both permIDs will be recognized.
	else if ((i_PermID == "Material") || (i_PermID == "Materials"))
		return 8;
	else if (i_PermID == "Refl")
		return 9;
	else if (i_PermID == "GI")
		return 10;
	else if (i_PermID == "Velocity")
		return 11;

	DBG_ASSERT(false, "Unknown renderer permanent ID " << i_PermID);
	return 0;
}

}  // local namespace


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_PRFD;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				rprfPrefsData& o_PrefsData )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_PRFD )
		{
			ReadPrefsData(i_Reader, version, o_PrefsData);	
		}
		else
		{
			//DBG_ASSERT(false, "invalid chunk header");
			DBG_TRACE("invalid chunk header" << name);
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const rprfPrefsData& i_PrefsData )
{
	WritePrefsData( o_Writer, i_PrefsData);
}

//------------------------------------------------------------------------
//   ReadPrefsData
//------------------------------------------------------------------------
void ReadPrefsData(	chReader& io_Reader, chDefs::Version i_Version,
					rprfPrefsData& o_PrefsData )
{
	o_PrefsData.m_bMultipassOn.Read( io_Reader );

	// m_bEnableDOF is now a debug-only render pref and some old files have 
	// it saved with a non-default value. So don't read any values from disk
	// for this parameter:

//	o_PrefsData.m_bEnableDOF.Read( io_Reader );
	prtyBoolean bEnableDOFPlaceHolder;
	bEnableDOFPlaceHolder.Read( io_Reader );
	
	o_PrefsData.m_bEnableGlow.Read( io_Reader );
	o_PrefsData.m_bEnableOutline.Read( io_Reader );
	//o_PrefsData.m_bEnableAmbientPass.Read( io_Reader );
	o_PrefsData.m_bEnableLitPass.Read( io_Reader );
	o_PrefsData.m_bEnableTransparent.Read( io_Reader );
	o_PrefsData.m_bEnableDiffuseLighting.Read( io_Reader );
	o_PrefsData.m_bEnableSpecularLighting.Read( io_Reader );
	o_PrefsData.m_bMatteMode.Read( io_Reader );
	//o_PrefsData.m_bHeadlightOn.Read( io_Reader );

	o_PrefsData.m_RendererType.Read( io_Reader );
	if (i_Version < 6)
	{
		std::string permID = GetRendererTypePermIDFromV6(o_PrefsData.m_RendererType.GetValue());
		o_PrefsData.m_RendererType.SetValue(GetRendererTypeFromPermID(permID, 6));
	}
	else if (i_Version < 17)
	{
		std::string permID = GetRendererTypePermIDFromV17(o_PrefsData.m_RendererType.GetValue());
		o_PrefsData.m_RendererType.SetValue(GetRendererTypeFromPermID(permID, 17));
	}
	
	//o_PrefsData.m_bBlueShift.Read( io_Reader );
	o_PrefsData.m_bToneMap.Read( io_Reader );
	o_PrefsData.m_HDRDebugMode.Read( io_Reader );
	o_PrefsData.m_bEnableReflection.Read( io_Reader );
	o_PrefsData.m_bEnableEnvironment.Read( io_Reader );
	o_PrefsData.m_bLowResolution.Read( io_Reader );
	//o_PrefsData.m_bEnableAO.Read( io_Reader );
	//o_PrefsData.m_bRecalcAOPerFrame.Read( io_Reader );
	o_PrefsData.m_bProjLightFrustumCull.Read( io_Reader );
	o_PrefsData.m_bProjLightsOn.Read( io_Reader );
	o_PrefsData.m_bPtLightsOn.Read( io_Reader );
	o_PrefsData.m_bDoShadowMapGen.Read( io_Reader );
	//o_PrefsData.m_bEnableDeferredTransparency.Read( io_Reader );
	o_PrefsData.m_bHDRAA.Read( io_Reader );
	o_PrefsData.m_bEnableSSAO.Read( io_Reader );
	o_PrefsData.m_SSAOQuality.Read( io_Reader );
	o_PrefsData.m_SSAOQualityVP.Read( io_Reader );
	o_PrefsData.m_SSAONumSteps.Read( io_Reader );
	o_PrefsData.m_SSAONumDirs.Read( io_Reader );
	o_PrefsData.m_SSAOEnableBlur.Read( io_Reader );

	if (i_Version > 1)
	{
		o_PrefsData.m_bEnableShadows.Read( io_Reader );

		if( i_Version > 2 )
		{
			//--motion blur--
			o_PrefsData.m_bMotionBlurEnable.Read( io_Reader );
			o_PrefsData.m_MotionBlurSamples.Read( io_Reader );
			o_PrefsData.m_MotionBlurPercent.Read( io_Reader );
		}

		if( i_Version > 3 )
		{
			//hardware tessellation and wireframe
			o_PrefsData.m_bUseHardwareTessellation.Read( io_Reader );
			o_PrefsData.m_bRenderWireframe.Read( io_Reader );
		}

		if( i_Version > 4 )
		{
			//new ssoa params
			o_PrefsData.m_bEnableSSAODepthPeeling.Read( io_Reader );
			o_PrefsData.m_SSAONumLayers.Read( io_Reader );
		}

		if ( i_Version > 5 )
		{
			std::string rTypeID;
			io_Reader.Read(rTypeID);
			o_PrefsData.m_RendererType.SetValue(GetRendererTypeFromPermID(rTypeID, i_Version));
		}

		if( i_Version > 6 )
		{
			//new transparency params
			o_PrefsData.m_TransparencyMode.Read( io_Reader );
		}

		if( i_Version > 7 )
		{
			//new transparency params
			o_PrefsData.m_bIlluminationUsesNormals.Read( io_Reader );
		}

		if (i_Version > 8)
		{
			// new enable invisible objects cast shadows params
			o_PrefsData.m_bEnableInvisibleCastShadows.Read( io_Reader );
		}

		if( i_Version > 9)
		{
			o_PrefsData.m_HairShadowType.Read( io_Reader );
			o_PrefsData.m_HairShadowRes.Read( io_Reader );
			o_PrefsData.m_HairTessellation.Read( io_Reader );
			o_PrefsData.m_HairVertexLimit.Read( io_Reader );
			o_PrefsData.m_HairStrandSkip.Read( io_Reader );
		}

		if( i_Version > 10 )
		{
			o_PrefsData.m_bCaptureToneMapped.Read( io_Reader );
		}

		if( i_Version > 11 )
		{
			o_PrefsData.m_bEnableInvisibleCastShadows.Read( io_Reader );
			o_PrefsData.m_bEnableInvisibleMaskBlack.Read( io_Reader );
		}

		if( i_Version > 12 )
		{
			o_PrefsData.m_HairTransparencyMode.Read( io_Reader );
		}

		if( i_Version > 13 )
		{
			o_PrefsData.m_RendererEngine.Read( io_Reader );
		}

		if( i_Version > 14 )
		{
			o_PrefsData.m_bRmanRenderRIB.Read( io_Reader );
			o_PrefsData.m_bRmanGenShadowMaps.Read( io_Reader );
			o_PrefsData.m_bRmanGenReflectionMaps.Read( io_Reader );
		}
		if( i_Version > 15 )
		{
			o_PrefsData.m_nRmanAArate.Read( io_Reader );
		}
		if( i_Version > 16 )
		{
			// Read version 17 data
			// will read new GI data here
		}
		if( i_Version > 17 )
		{
			o_PrefsData.m_bHairDepthPeel.Read( io_Reader );
			o_PrefsData.m_nHairDepthPeelLayers.Read( io_Reader );
		}
		if( i_Version > 18 )
		{
			o_PrefsData.m_RmanOutType.Read( io_Reader );
		}
		if( i_Version > 19 )
		{
			o_PrefsData.m_bRmanTextureBatch.Read( io_Reader );
		}
		if( i_Version > 20 )
		{
			o_PrefsData.m_RmanAOsamples.Read( io_Reader );
			o_PrefsData.m_RmanAOMaxDist.Read( io_Reader );
			o_PrefsData.m_RmanAOMaxVariation.Read( io_Reader );
		}
		if( i_Version > 21 )
		{
			o_PrefsData.m_RmanAOConeAngle.Read( io_Reader );	
		}
		if( i_Version > 22 )
		{
			o_PrefsData.m_RmanBatchContent.Read( io_Reader );	
		}
		if( i_Version > 23 )
		{
			o_PrefsData.m_RmanShadingRate.Read( io_Reader );	
		}	
		if( i_Version > 24 )
		{
			o_PrefsData.m_bRmanAOEnable.Read( io_Reader );	
		}
		if( i_Version > 25 )
		{
			o_PrefsData.m_bRmanReflEnable.Read( io_Reader );
			o_PrefsData.m_RmanReflType.Read( io_Reader );
		}	
		if (i_Version > 26)
		{
			o_PrefsData.m_bEnableInvisibleInReflections.Read( io_Reader );
		}
		if (i_Version > 27)
		{
			o_PrefsData.m_bRmanShadowEnable.Read( io_Reader );
			o_PrefsData.m_RmanShadowType.Read( io_Reader );
			o_PrefsData.m_RmanShadowMinSamples.Read( io_Reader );
			o_PrefsData.m_RmanShadowSamples.Read( io_Reader );
			o_PrefsData.m_RmanShadowBias.Read( io_Reader );
			o_PrefsData.m_RmanShadowSoftness.Read( io_Reader );
		}
		if (i_Version > 28)
		{
			o_PrefsData.m_bRmanGIEnable.Read( io_Reader );
			o_PrefsData.m_RmanGIsamples.Read( io_Reader );
			o_PrefsData.m_RmanGIMaxDist.Read( io_Reader );
			o_PrefsData.m_RmanGIMaxVariation.Read( io_Reader );
			o_PrefsData.m_RmanGIConeAngle.Read( io_Reader );
		}
		if (i_Version > 29)
		{
			o_PrefsData.m_RmanFilterType.Read( io_Reader );
			o_PrefsData.m_RmanFilterWidth.Read( io_Reader );
		}
		if (i_Version > 30)
		{
			o_PrefsData.m_bRmanCacheTextures.Read( io_Reader );
		}
		if (i_Version > 31)
		{
			o_PrefsData.m_RmanNumCores.Read( io_Reader );
		}
		if (i_Version > 32)
		{
			o_PrefsData.m_RmanTexMemory.Read( io_Reader );
			o_PrefsData.m_RmanBucketOrder.Read( io_Reader );
			o_PrefsData.m_RmanBucketSize.Read( io_Reader );
			o_PrefsData.m_RmanGridSize.Read( io_Reader );
			o_PrefsData.m_RmanRayDepth.Read( io_Reader );
		}	
		if (i_Version > 33)
		{
			o_PrefsData.m_bRmanDisableWarnings.Read( io_Reader );
		}	

		if( i_Version > 34 )
		{
			// property removed in v50 of this struct
			prtyEnum dummyTransparencyAlphaMode;
			dummyTransparencyAlphaMode.Read( io_Reader );
		}

		if( i_Version > 35 )
		{
			o_PrefsData.m_MrayVerbosity.Read( io_Reader );
			o_PrefsData.m_MrayNumReflBounces.Read( io_Reader );
			o_PrefsData.m_MrayNumRefrBounces.Read( io_Reader );
			o_PrefsData.m_bMrayFinalGather.Read( io_Reader );
			o_PrefsData.m_MrayFGNDiffuse.Read( io_Reader );
			o_PrefsData.m_MrayFGNRefl.Read( io_Reader );
			o_PrefsData.m_MrayFGNRefr.Read( io_Reader );
			o_PrefsData.m_MrayFGNRays.Read( io_Reader );
			o_PrefsData.m_MrayFGColor.Read( io_Reader );
			o_PrefsData.m_MrayOutputFormat.Read( io_Reader );
			o_PrefsData.m_MrayNumThreads.Read( io_Reader );
			o_PrefsData.m_MrayMemoryLimit.Read( io_Reader );
			o_PrefsData.m_bMrayEnableReflections.Read( io_Reader );
			o_PrefsData.m_bMrayEnableShadows.Read( io_Reader );
		}

		if ( i_Version > 36 )
		{
			o_PrefsData.m_bMrayAO.Read( io_Reader );
		}
		if ( i_Version > 37 )
		{
			o_PrefsData.m_MrayAOSamples.Read( io_Reader );
		}
		if ( i_Version > 38 )
		{
			o_PrefsData.m_MrayMaxTraceDepth.Read( io_Reader );
		}
		if ( i_Version > 39 )
		{
			o_PrefsData.m_bMrayFGBlur.Read( io_Reader );
		}
		if ( i_Version > 40 )
		{
			o_PrefsData.m_bMrayRewriteAssets.Read( io_Reader );
			o_PrefsData.m_MrayVerbosityLevel.Read( io_Reader );			
		}
		if ( i_Version > 41 )
		{
			o_PrefsData.m_MrayShadowType.Read( io_Reader );
		}
		if ( i_Version > 42 )
		{
			o_PrefsData.m_bMrayFGMapEnable.Read( io_Reader );
			o_PrefsData.m_MrayFGMapRebuild.Read( io_Reader );
			o_PrefsData.m_MrayFGMapPath.Read( io_Reader );
		}
		if ( i_Version > 43 )
		{
			o_PrefsData.m_MrayReflSamples.Read( io_Reader );
		}
		if ( i_Version > 44 )
		{
			o_PrefsData.m_bMrayOverrideMSPSampling.Read( io_Reader );
			o_PrefsData.m_MrayMinCaptureSamples.Read( io_Reader );
			o_PrefsData.m_MrayMaxCaptureSamples.Read( io_Reader );
			o_PrefsData.m_MRayAAContrast.Read( io_Reader );
		}
		if ( i_Version > 45 )
		{
			o_PrefsData.m_bMrayProgressive.Read( io_Reader );
		}
		if ( i_Version > 46 )
		{
			o_PrefsData.m_bRmanTonemapEnable.Read( io_Reader );
			o_PrefsData.m_bMRayTonemapEnable.Read( io_Reader );
		}
		if ( i_Version > 47 )
		{
			o_PrefsData.m_bMrayIgnoreBadTex.Read( io_Reader );
		}
		if ( i_Version > 48 )
		{
			o_PrefsData.m_bMrayDisplayPreview.Read( io_Reader );
		}
		if ( i_Version > 49 )
		{
			o_PrefsData.m_MrayProgSubsamplingSize.Read( io_Reader );
			o_PrefsData.m_MrayProgSubsamplingMode.Read( io_Reader );
			o_PrefsData.m_MrayProgSubsamplingPattern.Read( io_Reader );
			o_PrefsData.m_MrayProgMinSamples.Read( io_Reader );
			o_PrefsData.m_MrayProgMaxSamples.Read( io_Reader );
			o_PrefsData.m_MrayProgMaxTime.Read( io_Reader );
			o_PrefsData.m_MrayProgErrorThreshold.Read( io_Reader );
		}
		if (i_Version > 50)
		{
			o_PrefsData.m_bUseAOVolumes.Read( io_Reader );
			o_PrefsData.m_bEnableHair.Read( io_Reader );
			o_PrefsData.m_bHairLines.Read( io_Reader );
			o_PrefsData.m_HairSubPixelPower.Read( io_Reader );
			o_PrefsData.m_HairInterpolationCount.Read( io_Reader );
			o_PrefsData.m_HairClumpRadius.Read( io_Reader );
			o_PrefsData.m_bRPF_AO.Read( io_Reader );
			o_PrefsData.m_bRPF_GI.Read( io_Reader );
			o_PrefsData.m_bRPF_Refl.Read( io_Reader );
			o_PrefsData.m_bRPF_Beauty.Read( io_Reader );
			o_PrefsData.m_bEnableSSGI.Read( io_Reader );
			o_PrefsData.m_SSGIQuality.Read( io_Reader );
			o_PrefsData.m_SSGINumSteps.Read( io_Reader );
			o_PrefsData.m_SSGINumDirs.Read( io_Reader );
			o_PrefsData.m_SSGIEnableBlur.Read( io_Reader );
			o_PrefsData.m_bEnableSSGIDepthPeeling.Read( io_Reader );
			o_PrefsData.m_SSGINumLayers.Read( io_Reader );
			o_PrefsData.m_bUseLPVGI.Read( io_Reader );
			o_PrefsData.m_IBLQuality.Read( io_Reader );
			o_PrefsData.m_IBLMapRes.Read( io_Reader );
			o_PrefsData.m_IBLSampleNum.Read( io_Reader );
		}
	}
}

//------------------------------------------------------------------------
//   WritePrefsData
//------------------------------------------------------------------------
void WritePrefsData(chWriter& o_Writer,
					const rprfPrefsData& i_PrefsData )
{
	const int l_cPRFD_VERSION = 51;
	o_Writer.WriteChunkHeader( c_PRFD, l_cPRFD_VERSION, false );

	i_PrefsData.m_bMultipassOn.Write( o_Writer );
	i_PrefsData.m_bEnableDOF.Write( o_Writer );
	i_PrefsData.m_bEnableGlow.Write( o_Writer );
	i_PrefsData.m_bEnableOutline.Write( o_Writer );
	//i_PrefsData.m_bEnableAmbientPass.Write( o_Writer );
	i_PrefsData.m_bEnableLitPass.Write( o_Writer );
	i_PrefsData.m_bEnableTransparent.Write( o_Writer );
	i_PrefsData.m_bEnableDiffuseLighting.Write( o_Writer );
	i_PrefsData.m_bEnableSpecularLighting.Write( o_Writer );
	i_PrefsData.m_bMatteMode.Write( o_Writer );
	//i_PrefsData.m_bHeadlightOn.Write( o_Writer );
	i_PrefsData.m_RendererType.Write( o_Writer );
	//i_PrefsData.m_bBlueShift.Write( o_Writer );
	i_PrefsData.m_bToneMap.Write( o_Writer );
	i_PrefsData.m_HDRDebugMode.Write( o_Writer );
	i_PrefsData.m_bEnableReflection.Write( o_Writer );
	i_PrefsData.m_bEnableEnvironment.Write( o_Writer );
	i_PrefsData.m_bLowResolution.Write( o_Writer );
	//i_PrefsData.m_bEnableAO.Write( o_Writer );
	//i_PrefsData.m_bRecalcAOPerFrame.Write( o_Writer );
	i_PrefsData.m_bProjLightFrustumCull.Write( o_Writer );
	i_PrefsData.m_bProjLightsOn.Write( o_Writer );
	i_PrefsData.m_bPtLightsOn.Write( o_Writer );
	i_PrefsData.m_bDoShadowMapGen.Write( o_Writer );
	//i_PrefsData.m_bEnableDeferredTransparency.Write( o_Writer );
	i_PrefsData.m_bHDRAA.Write( o_Writer );
	i_PrefsData.m_bEnableSSAO.Write( o_Writer );
	i_PrefsData.m_SSAOQuality.Write( o_Writer );
	i_PrefsData.m_SSAOQualityVP.Write( o_Writer );
	i_PrefsData.m_SSAONumSteps.Write( o_Writer );
	i_PrefsData.m_SSAONumDirs.Write( o_Writer );
	i_PrefsData.m_SSAOEnableBlur.Write( o_Writer );

	// added v2
	i_PrefsData.m_bEnableShadows.Write( o_Writer );

	// added v3
	i_PrefsData.m_bMotionBlurEnable.Write( o_Writer );
	i_PrefsData.m_MotionBlurSamples.Write( o_Writer );
	i_PrefsData.m_MotionBlurPercent.Write( o_Writer );

	//added v4
	i_PrefsData.m_bUseHardwareTessellation.Write( o_Writer );
	i_PrefsData.m_bRenderWireframe.Write( o_Writer );

	//added v5
	i_PrefsData.m_bEnableSSAODepthPeeling.Write( o_Writer );
	i_PrefsData.m_SSAONumLayers.Write( o_Writer );

	// v6 changed ordering of renderer type
	o_Writer.Write(GetRendererTypePermID(i_PrefsData.m_RendererType.GetValue()));

	//added v7
	i_PrefsData.m_TransparencyMode.Write( o_Writer );

	// added v8
	i_PrefsData.m_bIlluminationUsesNormals.Write( o_Writer );

	// added v9
	i_PrefsData.m_bEnableInvisibleCastShadows.Write(o_Writer);

	// added v10
	i_PrefsData.m_HairShadowType.Write(o_Writer);
	i_PrefsData.m_HairShadowRes.Write(o_Writer);
	i_PrefsData.m_HairTessellation.Write(o_Writer);
	i_PrefsData.m_HairVertexLimit.Write(o_Writer);
	i_PrefsData.m_HairStrandSkip.Write(o_Writer);

	//added v11
	i_PrefsData.m_bCaptureToneMapped.Write(o_Writer);

	// added v12
	i_PrefsData.m_bEnableInvisibleCastShadows.Write(o_Writer);
	i_PrefsData.m_bEnableInvisibleMaskBlack.Write(o_Writer);

	// added v13
	i_PrefsData.m_HairTransparencyMode.Write(o_Writer);

	// added v14
	i_PrefsData.m_RendererEngine.Write(o_Writer);

	// added v15
	i_PrefsData.m_bRmanRenderRIB.Write(o_Writer);
	i_PrefsData.m_bRmanGenShadowMaps.Write(o_Writer);
	i_PrefsData.m_bRmanGenReflectionMaps.Write(o_Writer);

	// added v16
	i_PrefsData.m_nRmanAArate.Write(o_Writer);

	// added v17
	// will write new GI data here

	// added v18
	i_PrefsData.m_bHairDepthPeel.Write( o_Writer );
	i_PrefsData.m_nHairDepthPeelLayers.Write( o_Writer );

	// added v19
	i_PrefsData.m_RmanOutType.Write( o_Writer );

	// added v20
	i_PrefsData.m_bRmanTextureBatch.Write( o_Writer );

	// added v21
	i_PrefsData.m_RmanAOsamples.Write( o_Writer );	
	i_PrefsData.m_RmanAOMaxDist.Write( o_Writer );
	i_PrefsData.m_RmanAOMaxVariation.Write( o_Writer );

	// added v22
	i_PrefsData.m_RmanAOConeAngle.Write( o_Writer );

	// added v23
	i_PrefsData.m_RmanBatchContent.Write( o_Writer );

	// added v24
	i_PrefsData.m_RmanShadingRate.Write( o_Writer );

	// added v25
	i_PrefsData.m_bRmanAOEnable.Write( o_Writer );

	// added v26
	i_PrefsData.m_bRmanReflEnable.Write( o_Writer );
	i_PrefsData.m_RmanReflType.Write( o_Writer );

	// added v27 
	i_PrefsData.m_bEnableInvisibleInReflections.Write(o_Writer);

	// added v28
	i_PrefsData.m_bRmanShadowEnable.Write(o_Writer);
	i_PrefsData.m_RmanShadowType.Write(o_Writer);
	i_PrefsData.m_RmanShadowMinSamples.Write(o_Writer);
	i_PrefsData.m_RmanShadowSamples.Write(o_Writer);
	i_PrefsData.m_RmanShadowBias.Write(o_Writer);
	i_PrefsData.m_RmanShadowSoftness.Write(o_Writer);

	// added v29
	i_PrefsData.m_bRmanGIEnable.Write(o_Writer);
	i_PrefsData.m_RmanGIsamples.Write(o_Writer);
	i_PrefsData.m_RmanGIMaxDist.Write(o_Writer);
	i_PrefsData.m_RmanGIMaxVariation.Write(o_Writer);
	i_PrefsData.m_RmanGIConeAngle.Write(o_Writer);

	// added v30
	i_PrefsData.m_RmanFilterType.Write(o_Writer);
	i_PrefsData.m_RmanFilterWidth.Write(o_Writer);

	// added v31
	i_PrefsData.m_bRmanCacheTextures.Write(o_Writer);

	// added v32
	i_PrefsData.m_RmanNumCores.Write(o_Writer);

	// added v33
	i_PrefsData.m_RmanTexMemory.Write(o_Writer);
	i_PrefsData.m_RmanBucketOrder.Write(o_Writer);
	i_PrefsData.m_RmanBucketSize.Write(o_Writer);
	i_PrefsData.m_RmanGridSize.Write(o_Writer);
	i_PrefsData.m_RmanRayDepth.Write(o_Writer);

	// added v34
	i_PrefsData.m_bRmanDisableWarnings.Write(o_Writer);

	//added v35
	// property removed in v50 of this struct
	prtyEnum dummyTransparencyAlphaMode;
	dummyTransparencyAlphaMode.Write( o_Writer );

	//added v36
	i_PrefsData.m_MrayVerbosity.Write( o_Writer );
	i_PrefsData.m_MrayNumReflBounces.Write( o_Writer );
	i_PrefsData.m_MrayNumRefrBounces.Write( o_Writer );
	i_PrefsData.m_bMrayFinalGather.Write( o_Writer );
	i_PrefsData.m_MrayFGNDiffuse.Write( o_Writer );
	i_PrefsData.m_MrayFGNRefl.Write( o_Writer );
	i_PrefsData.m_MrayFGNRefr.Write( o_Writer );
	i_PrefsData.m_MrayFGNRays.Write( o_Writer );
	i_PrefsData.m_MrayFGColor.Write( o_Writer );
	i_PrefsData.m_MrayOutputFormat.Write( o_Writer );
	i_PrefsData.m_MrayNumThreads.Write( o_Writer );
	i_PrefsData.m_MrayMemoryLimit.Write( o_Writer );
	i_PrefsData.m_bMrayEnableReflections.Write( o_Writer );
	i_PrefsData.m_bMrayEnableShadows.Write( o_Writer );

	//added v37	
	i_PrefsData.m_bMrayAO.Write( o_Writer );

	//added v38	
	i_PrefsData.m_MrayAOSamples.Write( o_Writer );

	//added v39
	i_PrefsData.m_MrayMaxTraceDepth.Write( o_Writer );

	//added v40
	i_PrefsData.m_bMrayFGBlur.Write( o_Writer );	

	//added v41
	i_PrefsData.m_bMrayRewriteAssets.Write( o_Writer );	
	i_PrefsData.m_MrayVerbosityLevel.Write( o_Writer );	

	//added v42
	i_PrefsData.m_MrayShadowType.Write( o_Writer );

	//added v43
	i_PrefsData.m_bMrayFGMapEnable.Write( o_Writer );
	i_PrefsData.m_MrayFGMapRebuild.Write( o_Writer );
	i_PrefsData.m_MrayFGMapPath.Write( o_Writer );

	//added v44
	i_PrefsData.m_MrayReflSamples.Write( o_Writer );

	//added v45
	i_PrefsData.m_bMrayOverrideMSPSampling.Write( o_Writer );
	i_PrefsData.m_MrayMinCaptureSamples.Write( o_Writer );
	i_PrefsData.m_MrayMaxCaptureSamples.Write( o_Writer );
	i_PrefsData.m_MRayAAContrast.Write( o_Writer );

	//added v46
	i_PrefsData.m_bMrayProgressive.Write( o_Writer );

	//added v47
	i_PrefsData.m_bRmanTonemapEnable.Write( o_Writer );
	i_PrefsData.m_bMRayTonemapEnable.Write( o_Writer );

	//added v48
	i_PrefsData.m_bMrayIgnoreBadTex.Write( o_Writer );

	//added v49
	i_PrefsData.m_bMrayDisplayPreview.Write( o_Writer );

	//added v50
	i_PrefsData.m_MrayProgSubsamplingSize.Write( o_Writer );
	i_PrefsData.m_MrayProgSubsamplingMode.Write( o_Writer );
	i_PrefsData.m_MrayProgSubsamplingPattern.Write( o_Writer );
	i_PrefsData.m_MrayProgMinSamples.Write( o_Writer );
	i_PrefsData.m_MrayProgMaxSamples.Write( o_Writer );
	i_PrefsData.m_MrayProgMaxTime.Write( o_Writer );
	i_PrefsData.m_MrayProgErrorThreshold.Write( o_Writer );

	//added v51 - missing properties in MSP 2.0 release
	i_PrefsData.m_bUseAOVolumes.Write( o_Writer );
	i_PrefsData.m_bEnableHair.Write( o_Writer );
	i_PrefsData.m_bHairLines.Write( o_Writer );
	i_PrefsData.m_HairSubPixelPower.Write( o_Writer );
	i_PrefsData.m_HairInterpolationCount.Write( o_Writer );
	i_PrefsData.m_HairClumpRadius.Write( o_Writer );
	i_PrefsData.m_bRPF_AO.Write( o_Writer );
	i_PrefsData.m_bRPF_GI.Write( o_Writer );
	i_PrefsData.m_bRPF_Refl.Write( o_Writer );
	i_PrefsData.m_bRPF_Beauty.Write( o_Writer );
	i_PrefsData.m_bEnableSSGI.Write( o_Writer );
	i_PrefsData.m_SSGIQuality.Write( o_Writer );
	i_PrefsData.m_SSGINumSteps.Write( o_Writer );
	i_PrefsData.m_SSGINumDirs.Write( o_Writer );
	i_PrefsData.m_SSGIEnableBlur.Write( o_Writer );
	i_PrefsData.m_bEnableSSGIDepthPeeling.Write( o_Writer );
	i_PrefsData.m_SSGINumLayers.Write( o_Writer );
	i_PrefsData.m_bUseLPVGI.Write( o_Writer );
	i_PrefsData.m_IBLQuality.Write( o_Writer );
	i_PrefsData.m_IBLMapRes.Write( o_Writer );
	i_PrefsData.m_IBLSampleNum.Write( o_Writer );

	o_Writer.FinishChunk();
}

}