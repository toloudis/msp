/*****************************************************************************\
**	fbxExport.hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_EXPORT_HPP
#error fbxMgr.hpp multiply included
#endif
#define FBX_EXPORT_HPP

//============================================================================
//	Forward References
//============================================================================
struct fbxGeometryData;
struct fbxCameraData;
class g3dSceneNode;
class fsLocator;

//============================================================================
//============================================================================
namespace fbxExport
{

	//--------------------------------------------------------------------
	// BeginExportToFBX()
	//--------------------------------------------------------------------
	void BeginExportToFBX(fsLocator i_FullFilePath);

	//--------------------------------------------------------------------
	// ExportMeshToFBX()
	//--------------------------------------------------------------------
	void ExportMeshToFBX(fbxGeometryData i_GeometryData, const g3dSceneNode * i_pNode);

	//--------------------------------------------------------------------
	// ExportCameraToFBX()
	//--------------------------------------------------------------------
	void ExportCameraToFBX(fbxCameraData i_CameraData);

	//--------------------------------------------------------------------
	// EndExportToFBX()
	//--------------------------------------------------------------------
	void EndExportToFBX(fsLocator i_FullFilePath);
	

};
