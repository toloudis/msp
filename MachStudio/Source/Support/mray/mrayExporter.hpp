/****************************************************************************\
**	mrayExporter.hpp
**
**		mrayExporter provides an API for writing to Maya ascii files.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MRAY_EXPORTER_HPP
#error mrayExporter.hpp multiply included
#endif
#define MRAY_EXPORTER_HPP

#ifndef MRAY_EXPORTDATA_HPP
#include "ImportExport/mray/export/mrayExportData.hpp"
#endif

#include <fstream>

//============================================================================
//============================================================================
class fsLocator;

//============================================================================
//============================================================================
class mrayExporter
{
public:
	//--------------------------------------------------------------------
	// mrayExporter()
	//--------------------------------------------------------------------
	mrayExporter( const itString::CharType* i_FullFilePath , mrayGlobalData i_RenderData );

	//--------------------------------------------------------------------
	// ~mrayExporter()
	//--------------------------------------------------------------------
	~mrayExporter();

	//--------------------------------------------------------------------
	// ExportMesh()
	//--------------------------------------------------------------------
	void ExportMesh( mrayGeometryData i_GeometryData );

	//--------------------------------------------------------------------
	// ExportPointLight()
	//--------------------------------------------------------------------
	void ExportPointLight( mrayPointLightData i_PointLightData );

	//--------------------------------------------------------------------
	// ExportProjLight()
	//--------------------------------------------------------------------
	void ExportProjLight( mrayProjLightData i_ProjLightData );

	//--------------------------------------------------------------------
	// EndExport()
	//--------------------------------------------------------------------
	void EndExport();

	//--------------------------------------------------------------------
	// GetFileStream()
	//--------------------------------------------------------------------
	std::ofstream & GetFileStream();

	//--------------------------------------------------------------------
	// GetGlobalData()
	//--------------------------------------------------------------------
	mrayGlobalData GetGlobalData();

	//--------------------------------------------------------------------
	// UpdateGlobalObjectLightMap()
	//--------------------------------------------------------------------
	void UpdateGlobalObjectLightMap(std::map< std::string , std::vector< std::string > > i_Map);

	//--------------------------------------------------------------------
	// UpdateGlobalLightSetLights()
	//--------------------------------------------------------------------
	void UpdateGlobalLightSetLights(std::vector< std::string > i_Lights);

private:
	std::ofstream m_OutFile;
	mrayGlobalData m_GlobalData;

};

