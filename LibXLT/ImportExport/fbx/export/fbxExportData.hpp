/********************************************************************************************\
**  fbxExportData.hpp
**
**		Data for the export to Maya ascii.
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef FBX_EXPORTDATA_HPP
#error fbxExportData.hpp multiply included
#endif
#define FBX_EXPORTDATA_HPP


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

// Data for camera
struct fbxCameraData
{
	std::string m_Name;
	maPoint3d m_Position;
	maPoint3d m_Target;
	float m_FOV;
	float m_NearClip;
	float m_FarClip;
	bool m_bEnableALP;
	float m_FocalLength;
	float m_HorizontalAperture;
	bool m_bIsOrthographic;
};

// Data for geometry
struct fbxGeometryData
{
	std::string m_Name;
	std::string m_SubName;
	int m_NumSubdivs;
	int m_NumMeshGroups;
	const g3dSceneNode* m_Node;
	bool m_bHasBeenBaked;
	fsLocator m_BakedPath;
	std::string m_BakeExt;
};

// Data for scene
struct fbxSceneData
{
	std::string m_Desc;
	std::vector<fbxGeometryData> m_GeometryData;
	std::vector<fbxCameraData> m_CameraData;
};

//============================================================================
//============================================================================
struct fbxExportData
{
	fbxExportData();
	
	bool m_bSceneHasBeenBaked;

	// Data for scene
	std::vector<fbxSceneData> m_SceneData;
};

