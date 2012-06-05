/****************************************************************************\
**	rmanExporter.hpp
**
**		rmanExporter provides an API for writing to Maya ascii files.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef RMAN_EXPORTER_HPP
#error rmanExporter.hpp multiply included
#endif
#define RMAN_EXPORTER_HPP

#ifndef RMAN_EXPORTDATA_HPP
#include "ImportExport/rman/export/rmanExportData.hpp"
#endif

#include <fstream>

//============================================================================
//============================================================================
class rmanExporter
{
public:
	//--------------------------------------------------------------------
	// rmanExporter()
	//--------------------------------------------------------------------
	rmanExporter( rmanGlobalData i_RenderData );

	//--------------------------------------------------------------------
	// ~rmanExporter()
	//--------------------------------------------------------------------
	~rmanExporter();

	//--------------------------------------------------------------------
	// ExportMesh()
	//--------------------------------------------------------------------
	void ExportMesh( rmanGeometryData i_GeometryData );

	//--------------------------------------------------------------------
	// ExportPointLight()
	//--------------------------------------------------------------------
	void ExportPointLight( rmanPointLightData i_PointLightData );

	//--------------------------------------------------------------------
	// ExportProjLight()
	//--------------------------------------------------------------------
	void ExportProjLight( rmanProjLightData i_ProjLightData );

	//--------------------------------------------------------------------
	// ExportMasterRib()
	//--------------------------------------------------------------------
	void ExportMasterRib();

	//--------------------------------------------------------------------
	// ExportShadowMaps()
	//--------------------------------------------------------------------
	void ExportShadowMaps();

	//--------------------------------------------------------------------
	// ExportShadowMaps()
	//--------------------------------------------------------------------
	void ExportReflectionMaps();

	//--------------------------------------------------------------------
	// GetGlobalData()
	//--------------------------------------------------------------------
	rmanGlobalData GetGlobalData();

	//--------------------------------------------------------------------
	// UpdateGlobalObjectLightMap()
	//--------------------------------------------------------------------
	void UpdateGlobalObjectLightMap(std::map< std::string , std::vector< std::string > > i_Map);

	//--------------------------------------------------------------------
	// UpdateGlobalLightSetLights()
	//--------------------------------------------------------------------
	void UpdateGlobalLightSetLights(std::vector< std::string > i_Lights);

private:
	rmanGlobalData m_GlobalData;

};

