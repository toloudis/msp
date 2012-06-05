/********************************************************************************************\
**  rmanExportData.hpp
**
**		Data for the export to Maya ascii.
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef RMAN_EXPORTDATA_HPP
#error rmanExportData.hpp multiply included
#endif
#define RMAN_EXPORTDATA_HPP

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

#include <string>
#include <vector>
#include <map>

struct g3dAmbientEnvState;
class camCamera;
class g3dProjectedLight;
class g3dSceneNode;
class g3dFragment;
class matMaterial;
class matTexture;

static const std::string normMapEnd = "-normMap";
static const std::string dispMapEnd = "-dispMap";
static const std::string projMapEnd = "-projMap";
static const std::string envDiffMapEnd = "-envDiffMap";
static const std::string envSpecMapEnd = "-envSpecMap";

struct rmanAOData
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

struct rmanGIData
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

struct rmanOptionsData
{
	bool m_bRmanRewriteAssets;
	int m_RmanOutType;
	float m_RmanShadingRate;
	int m_RmanBucketOrder;
	int m_RmanBucketSize;
	int m_RmanRayDepth;	
	int m_RmanNumCores;
	int m_RmanTexMemory;
	bool m_bRmanDisableWarnings;
	bool m_bRmanReflEnable;
	int m_RmanReflType;	
	bool m_bRmanShadowEnable;
	int m_RmanShadowType;
	bool m_bRmanAOEnable;
	int m_RmanAOsamples;
	float m_RmanAOMaxVariation;
	bool m_bRmanGIEnable;
	int m_RmanGIsamples;
	float m_RmanGIMaxVariation;	
	bool m_bTonemapEnable;
};

struct rmanGlobalData
{
	// UI options
	rmanOptionsData m_Options;

	// Passes data
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

	// AO and GI data
	rmanAOData m_AOData;
	rmanGIData m_GIData;

	// From capture
	int m_RmanFilterType;
	float m_RmanFilterWidth;
	int m_RmanAArate;
	camCamera * m_SceneCamera;
	int m_width;
	int m_height;
	float m_PixelAspectRatio;
	fsLocator m_RibPath;
	bool m_bRenderDOF;
	std::string m_Engine;
	std::string m_FrameName;

	// Locators
	fsLocator m_TiffLoc;
	fsLocator m_TexturesLoc;
	fsLocator m_ArchivesLoc;
	fsLocator m_ShadowMapsLoc;
	fsLocator m_ReflectionMapsLoc;
	fsLocator m_PhotonMapLoc;

	// Texture management
	std::map<std::string, std::pair<matTexture*,fsLocator> > m_Textures;
	std::map< fsLocator , std::vector< std::string > > m_FullTextureMap;
	std::map< fsLocator , std::string > m_TextureIDs;
	int m_CommandNumber;

	// Vectors
	std::vector<std::string> m_SearchPaths;
	std::vector<std::string> m_ShadowMapAttributes;
	std::vector<g3dProjectedLight*> m_ProjectedLights;

	std::vector<std::string> m_ProjectedLightNames;
	std::vector<int> m_ProjectedLightResolutions;
	std::vector<bool> m_ProjectedLightIsConeLighting;

	std::vector<maPoint3d> m_ReflObjPositions;
	std::vector<maMatrix4x4> m_ReflObjTransforms;
	std::vector<g3dFragment*> m_ReflObjFragments;
	std::vector<std::string> m_ReflObjNames;
	std::vector<int> m_ReflCubemapResolutions;
	std::vector<bool> m_ReflObjIsPlanar;
	std::vector<fsLocator> m_ReflCubeRibs;
	std::vector<fsLocator> m_ReflPlanarRibs;
	std::vector<fsLocator> m_ShadowMapRibs;

	std::map< std::string , std::string > m_MasterAttributeMap;
	std::map< std::string , std::string > m_ProjLightMap;
	std::map< std::string , std::string > m_PointLightMap;

	std::map< std::string , std::vector< std::string > >  m_ObjectLightMap;
	std::vector< std::string >  m_LightSetLights;
	std::vector< std::string >  m_SceneLights;

	std::vector< std::string > m_ExportedObjects;
};

// Data for geometry subdivisions
struct rmanSubdivData
{
	std::vector< std::vector<envType::UInt32>* > m_Indices;
	std::vector< std::vector<maPoint3d>* > m_Vertices;
	std::vector< std::vector<maPoint2d>* > m_UVs;
	std::vector< std::string > m_SubdivName;
	std::vector< std::vector< g3dFragment* > > m_SubdivFragments;
};

// Data for geometry meshes inside a subdiv object
struct rmanMeshGroupData
{
	std::vector< std::string > m_MeshGroupName;
	std::vector< g3dSceneNode* > m_MeshGroupNode;
};

// Data for geometry
struct rmanGeometryData
{
	std::string m_Name;
	std::string m_SubName;
	bool m_bVisible;
	maMatrix4x4 m_Transform;
	maPoint3d m_Position;
	const g3dSceneNode* m_pNode;
	int m_NumSubdivs;
	int m_NumMeshGroups;
	rmanSubdivData m_SubdivData;
	rmanMeshGroupData m_MeshGroupData;
	g3dAmbientEnvState* m_AmbientData;
};


// Data for point lights
struct rmanProjLightData
{
	std::string m_Name;
	maPoint3d m_Falloff;
	maPoint3d m_Color;
	float m_Intensity;
	float m_FalloffStart;
	maPoint3d m_Position;
	maPoint3d m_Target;
	float m_Angle;
	float m_Scale;
	float m_Range;
	float m_Aspect;
	maMatrix4x4 m_CameraTransform;
	bool m_bEnableLight;
	bool m_bEnableDiffuse;
	bool m_bEnableSpecular;
	bool m_bAffectsGlow;
	g3dProjectedLight* m_ProjectedLight;
	float m_ShadowSoftness;
	float m_ShadowDepthBias;
	float m_ShadowIntensity;
	float m_ShadowPCSS;
	float m_ShadowQuality;
	int m_ShadowMapRes;
	bool m_ShadowSource;
	maPoint3d m_ShadowColor;	
	std::string m_Ramp;

	float m_Penumbra;
	float m_InnerAngle;
	bool m_bDirectional;
	bool m_bPureDirectional;
	bool m_bConeLighting;
};

// Data for point lights
struct rmanPointLightData
{
	std::string m_Name;
	maPoint3d m_Falloff;
	maPoint3d m_Color;
	float m_Intensity;
	float m_FalloffStart;
	maPoint3d m_Position;
	bool m_bEnableLight;
	bool m_bEnableDiffuse;
	bool m_bEnableSpecular;
	bool m_bAffectsGlow;
};

// Data for light sets
struct rmanLightSetData
{
	std::string m_Name;
	std::vector< std::string > m_Lights;
	std::vector< std::string > m_Objects;
};

// Data for scene
struct rmanSceneData
{
	std::string m_Desc;
	std::vector<rmanGeometryData> m_GeometryData;
	std::vector<rmanPointLightData> m_PointLightData;
	std::vector<rmanProjLightData> m_ProjLightData;
	std::vector<rmanLightSetData> m_LightSetData;
};

//============================================================================
//============================================================================
struct rmanExportData
{
	rmanExportData();

	// Data for scene
	std::vector<rmanSceneData> m_SceneData;
};

