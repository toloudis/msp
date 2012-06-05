/********************************************************************************************\
**  mrayExportData.hpp
**
**		Data for the export to Maya ascii.
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef MRAY_EXPORTDATA_HPP
#error mrayExportData.hpp multiply included
#endif
#define MRAY_EXPORTDATA_HPP


#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif

#include <string>
#include <vector>
#include <map>

struct g3dAmbientEnvState;
class camCamera;
class g3dSceneNode;
class matTexture;

static const std::string normMapEndMray = "-normMap";
static const std::string dispMapEndMray = "-dispMap";
static const std::string projMapEndMray = "-projMap";
static const std::string envDiffMapEndMray = "-envDiffMap";
static const std::string envSpecMapEndMray = "-envSpecMap";

struct mrayGIData
{
	maPoint3d color;
	float radiusNear;
	float radiusFar;
	float angleBias;
	float attenuation;
	float contrast;
	float blurWidth;
	float blurSharpness;	
	int overscanPixels;
	std::string name;

	float RSMGIScale;
	float RSMGISampleRadius;
	int  RSMGISampleNum;
	float GILightScale;
	int RSMSize;
	float LPVScale;
	int LPVIteration;
	int LPVVolumeSize;
	float LPVGIFalloff;
	int LPVRSMSize;
};

struct mrayAOData
{
	maPoint3d color;
	float radiusNear;
	float radiusFar;
	float angleBias;
	float attenuation;
	float contrast;
	float blurWidth;
	float blurSharpness;	
	int overscanPixels;
	std::string name;
};

struct mrayEnvData
{
	maFloatRGBA diffcolor;
	std::string diffTex;
	float diffFactor;
	float diffAngle;
	maFloatRGBA speccolor;
	std::string specTex;
	float specFactor;
	float specAngle;
};

struct mrayMetaSLShaderData
{
	fsLocator m_MSLLocator;
	std::string m_ShaderName;
};

struct mrayOptionsData
{
	int m_Verbosity;
	int m_NumReflBounces;
	int m_NumRefrBounces;
	int m_MrayMaxTraceDepth;
	bool m_bAO;
	int m_AOSamples;
	bool m_bFinalGather;
	bool m_bFGBlur;
	int m_FGNDiffuse;
	int m_FGNRefl;
	int m_FGNRefr;
	int m_FGNRays;
	maFloatRGBA m_FGColor;
	int	m_OutputFormat;
	int	m_NumThreads;
	int	m_MemoryLimit;
	bool m_bEnableReflections;
	bool m_bEnableShadows;
	int m_MrayShadowType;
	bool m_bRewriteAssets;
	int m_VerbosityLevel;
	bool m_bFGMapEnable;
	int m_FGMapRebuild;
	fsLocator m_FGMapPath;
	int m_ReflSamples;
	bool m_bOverrideMSPSampling;
	int m_MinCaptureSamples;
	int m_MaxCaptureSamples;
	float m_AAContrast;
	bool m_bProgressive;
	bool m_bTonemapEnable;
	bool m_bEnableIBL;
	float m_IBLQuality;
	int m_IBLMapRes;
	float m_IBLScale;
	int m_IBLSampleNum;
	bool m_bIgnoreBadTex;
	bool m_bDisplayPreviewer;

	int m_ProgSubsamplingSize;
	int m_ProgSubsamplingMode;
	int m_ProgSubsamplingPattern;
	int m_ProgMinSamples;
	int m_ProgMaxSamples;
	int m_ProgMaxTime;
	float m_ProgErrorThreshold;
};

// Data used in the current Mental Ray render
struct mrayGlobalData
{
	// Location of ray.exe
	fsLocator m_MRayLoc;
	
	// Things coming in from Capture
	mrayOptionsData m_Options;
	camCamera* m_Camera;
	g3dPrefs::g3dRenderPrefs m_RenderPrefs;
	itString m_CurrentScene;
	float m_AspectRatio;
	float m_FilterWidth;
	int m_Width;
	int m_Height;
	int m_FilterFunc;
	int m_CaptureSampling;

	// Instances
	std::vector< std::string > m_PointLightInstances;
	std::vector< std::string > m_ProjLightInstances;
	std::vector< std::string > m_MeshInstances;

	// File paths
	fsLocator m_FileName;
	fsLocator m_WorldName;
	fsLocator m_TexturesLoc;
	fsLocator m_ArchivesLoc;

	// Lightset info
	std::map< std::string , std::vector< std::string > > m_ObjectLightMap;	
	std::vector< std::string > m_LightSetLights;
	std::vector< std::string > m_SceneLights;

	// Texture info
	std::map<std::string, std::pair<matTexture*,fsLocator> > m_Textures;
	std::map< fsLocator , std::vector< std::string > > m_FullTextureMap;
	std::map< fsLocator , std::string > m_TextureIDs;
	std::map<std::string, bool> m_TextureWrite;

	// MetaSL shader info
	std::map<fsLocator, mrayMetaSLShaderData> m_MSLShaderMap;
	std::map< std::string , std::vector< fsLocator > > m_FullShaderMap;

	// Passes info
	bool m_bRenderEnvironments;
	bool m_bRenderLit;
	bool m_bRenderDiffuse;
	bool m_bRenderSpecular;
	bool m_bRenderTransparent;
	bool m_bRenderingBeautyOnly;
	bool m_bRenderingShadowsOnly;
	bool m_bRenderingNormalsOnly;
	bool m_bRenderingReflectionsOnly;
	bool m_bRenderingIlluminationOnly;
	bool m_bRenderingGIOnly;
	bool m_bRenderingAOOnly;

	// World data
	std::vector< std::string > m_WorldData;

	// AO and GI data
	mrayAOData m_AOData;
	mrayGIData m_GIData;

	// Processed assets
	std::vector< std::string > m_DeclaredTextures;
	std::vector< std::string > m_ExportedObjects;

	// Default environment light data
	mrayEnvData m_EnvData;

};

// Data for a proj light
struct mrayProjLightData
{
	std::string m_Name;
	std::string m_InstanceName;
	maFloatRGBA m_Color;
	bool m_bEnabled;
	float m_Intensity;
	maPoint3d m_Position;
	maPoint3d m_Target;
	maVector3d m_Direction;
	float m_Scale;
	float m_Angle;
	float m_Aspect;
	float m_InnerAngle;
	bool m_bCastShadow;
	maFloatRGBA m_ShadowColor;
	float m_ShadowIntensity;
	maPoint3d m_Falloff;
	float m_Range;
	maVector3d m_Left;
	maVector3d m_Up;
	maMatrix4x4 m_CameraMatrix;
	maMatrix4x4 m_ProjMatrix;
	std::string m_TextureName;
	bool m_bAffectsDiffuse;
	bool m_bAffectsSpecular;

	bool m_bDirectional;
	bool m_bPureDirectional;
	bool m_bConeLighting;

	// shadow map properties
	float m_LightSize;
	int	m_DepthMapSize;
	int	m_ShadowQuality;
	float m_DepthBias;

	bool m_bAreaLight;
	int m_AreaLightSampling;
	int m_AreaLightType;
	bool m_bAreaLightVisible;

};

// Data for a point light
struct mrayPointLightData
{
	std::string m_Name;
	std::string m_InstanceName;
	maFloatRGBA m_Color;
	bool m_bEnabled;
	float m_Intensity;
	maPoint3d m_Position;
	maPoint3d m_Falloff;
	bool m_bAffectsDiffuse;
	bool m_bAffectsSpecular;
};

// Data for geometry
struct mrayGeometryData
{
	std::string m_Name;
	std::string m_SubName;
	bool m_bVisible;
	int m_NumSubdivs;
	int m_NumMeshGroups;
	const g3dSceneNode* m_Node;
	g3dAmbientEnvState* m_EnvData;
};

// Data for light sets
struct mrayLightSetData
{
	std::string m_Name;
	std::vector< std::string > m_Lights;
	std::vector< std::string > m_Objects;
};

// Data for scene
struct mraySceneData
{
	std::string m_Desc;
	std::vector<mrayGeometryData> m_GeometryData;
	std::vector<mrayProjLightData> m_ProjLightData;
	std::vector<mrayPointLightData> m_PointLightData;
	std::vector<mrayLightSetData> m_LightSetData;
};

//============================================================================
//============================================================================
struct mrayExportData
{
	mrayExportData();

	// Data for scene
	std::vector<mraySceneData> m_SceneData;
};

