/****************************************************************************\
**	mrayExporter.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mray/mrayExporter.hpp"

#include "Core/fs/fsFileUtil.hpp"

#include "ImportExport/mray/export/mrayExport.hpp"


//--------------------------------------------------------------------
// mrayExporter()
//--------------------------------------------------------------------
mrayExporter::mrayExporter( const itString::CharType* i_FullFilePath , mrayGlobalData i_GlobalData )
: m_OutFile(i_FullFilePath,std::ios::out)
{
	mrayExport::BeginExport(m_OutFile,i_GlobalData);
	m_GlobalData = i_GlobalData;
}
	
//--------------------------------------------------------------------
// ~mrayExporter()
//--------------------------------------------------------------------
mrayExporter::~mrayExporter()
{
}

//--------------------------------------------------------------------
// ExportMesh()
//--------------------------------------------------------------------
void mrayExporter::ExportMesh( mrayGeometryData i_GeometryData )
{
	mrayExport::ExportMesh(m_OutFile,m_GlobalData,i_GeometryData,i_GeometryData.m_Node);
}

//--------------------------------------------------------------------
// ExportPointLight()
//--------------------------------------------------------------------
void mrayExporter::ExportPointLight( mrayPointLightData i_PointLightData )
{
	mrayExport::ExportPointLight(m_OutFile,m_GlobalData,i_PointLightData);
}

//--------------------------------------------------------------------
// ExportProjLight()
//--------------------------------------------------------------------
void mrayExporter::ExportProjLight( mrayProjLightData i_ProjLightData )
{
	mrayExport::ExportProjLight(m_OutFile,m_GlobalData,i_ProjLightData);
}

//--------------------------------------------------------------------
// EndExport()
//--------------------------------------------------------------------
void mrayExporter::EndExport()
{
	mrayExport::EndExport(m_OutFile,m_GlobalData);
}

//--------------------------------------------------------------------
// GetFileStream()
//--------------------------------------------------------------------
std::ofstream & mrayExporter::GetFileStream()
{
	return m_OutFile;
}
//--------------------------------------------------------------------
// GetGlobalData()
//--------------------------------------------------------------------
mrayGlobalData mrayExporter::GetGlobalData()
{
	return m_GlobalData;
}

//--------------------------------------------------------------------
// UpdateGlobalObjectLightMap()
//--------------------------------------------------------------------
void mrayExporter::UpdateGlobalObjectLightMap(std::map< std::string , std::vector< std::string > > i_Map)
{
	m_GlobalData.m_ObjectLightMap = i_Map;
}

//--------------------------------------------------------------------
// UpdateGlobalLightSetLights()
//--------------------------------------------------------------------
void mrayExporter::UpdateGlobalLightSetLights(std::vector< std::string > i_Lights)
{
	m_GlobalData.m_LightSetLights = i_Lights;
}