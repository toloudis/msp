/*****************************************************************************\
**	rmanExport.hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef RMAN_EXPORT_HPP
#error rmanExport.hpp multiply included
#endif
#define RMAN_EXPORT_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
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
#ifndef RMAN_EXPORTDATA_HPP
#include "ImportExport/rman/export/rmanExportData.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
struct rmanExportData;
struct rmanUVTransformData;
struct g3dAmbientEnvState;
class fsLocator;
class itString;
class maRotation;
class camCamera;
class g3dSceneNode;
class g3dFragment;
class maFloatRGBA;
class g3dFragment;

typedef unsigned char BYTE;

#include <map>
#include <vector>
#include <fstream>

//============================================================================
//============================================================================
namespace rmanExport
{

	//--------------------------------------------------------------------
	// ExportPolygonMesh()
	//--------------------------------------------------------------------
	void ExportPolygonMesh(rmanGlobalData & io_GlobalData, rmanGeometryData i_GeometryData , const g3dSceneNode * i_pNode);

	//--------------------------------------------------------------------
	// ExportProjLight()
	//--------------------------------------------------------------------
	void ExportProjLight(rmanGlobalData & io_GlobalData, rmanProjLightData i_ProjLightData);

	//--------------------------------------------------------------------
	// ExportPointLight()
	//--------------------------------------------------------------------
	void ExportPointLight(rmanGlobalData & io_GlobalData, rmanPointLightData i_PointLightData);

	//--------------------------------------------------------------------
	// ExportMasterRib()
	//--------------------------------------------------------------------
	void ExportMasterRib(rmanGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	// ExportShadowMaps()
	//--------------------------------------------------------------------
	void ExportShadowMaps(rmanGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	// ExportReflectionMaps()
	//--------------------------------------------------------------------
	void ExportReflectionMaps(rmanGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	// ExportPhotonMap()
	//--------------------------------------------------------------------
	//void ExportPhotonMap(rmanGlobalData & io_GlobalData);

	////--------------------------------------------------------------------
	//// ExportSubdivisionMesh()
	////--------------------------------------------------------------------
	//void ExportSubdivisionMesh( std::vector<envType::UInt32>* i_Indices,
	//										  std::vector<maPoint3d>* i_Vertices,
	//										  std::vector<maPoint2d>* i_UVs,
	//										  std::string i_SubdivName,
	//										  const g3dSceneNode * i_pNode, 
	//										  bool i_bVisible,
	//										  maMatrix4x4 i_Transform,
	//										  std::string i_BaseName,
	//										  std::vector< g3dFragment* > i_Fragment, 
	//										  g3dAmbientEnvState * i_AmbientData);

};
