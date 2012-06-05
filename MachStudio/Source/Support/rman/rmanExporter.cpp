/****************************************************************************\
**	rmanExporter.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/rman/rmanExporter.hpp"

#include "ImportExport/rman/export/rmanExport.hpp"

//--------------------------------------------------------------------
// rmanExporter()
//--------------------------------------------------------------------
rmanExporter::rmanExporter( rmanGlobalData i_GlobalData )
{	
	m_GlobalData = i_GlobalData;
}
	
//--------------------------------------------------------------------
// ~rmanExporter()
//--------------------------------------------------------------------
rmanExporter::~rmanExporter()
{
}

//--------------------------------------------------------------------
// ExportMesh()
//--------------------------------------------------------------------
void rmanExporter::ExportMesh( rmanGeometryData i_GeometryData )
{
	rmanExport::ExportPolygonMesh(m_GlobalData,i_GeometryData,i_GeometryData.m_pNode);
}

//--------------------------------------------------------------------
// ExportPointLight()
//--------------------------------------------------------------------
void rmanExporter::ExportPointLight( rmanPointLightData i_PointLightData )
{
	rmanExport::ExportPointLight(m_GlobalData,i_PointLightData);
}

//--------------------------------------------------------------------
// ExportProjLight()
//--------------------------------------------------------------------
void rmanExporter::ExportProjLight( rmanProjLightData i_ProjLightData )
{
	rmanExport::ExportProjLight(m_GlobalData,i_ProjLightData);
}

//--------------------------------------------------------------------
// ExportMasterRib()
//--------------------------------------------------------------------
void rmanExporter::ExportMasterRib()
{
	rmanExport::ExportMasterRib(m_GlobalData);
}

//--------------------------------------------------------------------
// ExportShadowMaps()
//--------------------------------------------------------------------
void rmanExporter::ExportShadowMaps()
{
	rmanExport::ExportShadowMaps(m_GlobalData);
}

//--------------------------------------------------------------------
// ExportShadowMaps()
//--------------------------------------------------------------------
void rmanExporter::ExportReflectionMaps()
{
	rmanExport::ExportReflectionMaps(m_GlobalData);
}

//--------------------------------------------------------------------
// GetGlobalData()
//--------------------------------------------------------------------
rmanGlobalData rmanExporter::GetGlobalData()
{
	return m_GlobalData;
}

//--------------------------------------------------------------------
// UpdateGlobalObjectLightMap()
//--------------------------------------------------------------------
void rmanExporter::UpdateGlobalObjectLightMap(std::map< std::string , std::vector< std::string > > i_Map)
{
	m_GlobalData.m_ObjectLightMap = i_Map;
}

//--------------------------------------------------------------------
// UpdateGlobalLightSetLights()
//--------------------------------------------------------------------
void rmanExporter::UpdateGlobalLightSetLights(std::vector< std::string > i_Lights)
{
	m_GlobalData.m_LightSetLights = i_Lights;
}