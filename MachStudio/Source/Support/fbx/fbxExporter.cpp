/****************************************************************************\
**	fbxExporter.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/fbx/fbxExporter.hpp"

#include "ImportExport/fbx/export/fbxExport.hpp"

//--------------------------------------------------------------------
// fbxExporter()
//--------------------------------------------------------------------
fbxExporter::fbxExporter( fsLocator i_FullFilePath )
{
#ifdef USE_FBX_IMPORTEXPORT
	fbxExport::BeginExportToFBX(i_FullFilePath); 
#endif
}
	
//--------------------------------------------------------------------
// ~fbxExporter()
//--------------------------------------------------------------------
fbxExporter::~fbxExporter()
{
}

//--------------------------------------------------------------------
// ExportMesh()
//--------------------------------------------------------------------
void fbxExporter::ExportMesh( fbxGeometryData i_GeometryData )
{
#ifdef USE_FBX_IMPORTEXPORT
	fbxExport::ExportMeshToFBX(i_GeometryData,i_GeometryData.m_Node);
#endif
}

//--------------------------------------------------------------------
// ExportMesh()
//--------------------------------------------------------------------
void fbxExporter::ExportCamera( fbxCameraData i_CameraData )
{
#ifdef USE_FBX_IMPORTEXPORT
	fbxExport::ExportCameraToFBX(i_CameraData);
#endif
}

//--------------------------------------------------------------------
// EndExport()
//--------------------------------------------------------------------
void fbxExporter::EndExport( fsLocator i_FullFilePath )
{
#ifdef USE_FBX_IMPORTEXPORT
	fbxExport::EndExportToFBX( i_FullFilePath );
#endif
}