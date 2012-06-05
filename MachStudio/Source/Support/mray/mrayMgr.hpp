/*****************************************************************************\
**	mrayMgr.hpp
**
**		Provides method for looking up a Exportd object throughout all systems
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MRAY_MGR_HPP
#error mrayMgr.hpp multiply included
#endif
#define MRAY_MGR_HPP

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
struct mrayExportData;
struct mrayGlobalData;
struct mrayOptionsData;
class mrayExportInterest;
class fsLocator;
class itString;
class camCamera;
class mrayExporter;
class matTexture;

#include <string>
#include <map>
#include <vector>

//============================================================================
//============================================================================
namespace mrayMgr
{
	//--------------------------------------------------------------------
	//	ClearWrittenTextures()
	//--------------------------------------------------------------------
	void ClearWrittenTextures();

	//--------------------------------------------------------------------
	// GetMRayExportActive()
	//--------------------------------------------------------------------
	bool GetMRayExportActive();

	//--------------------------------------------------------------------
	// WriteMasterBatch()
	//--------------------------------------------------------------------
	std::string WriteMasterBatch( const fsLocator& i_Root );

	//----------------------------------------------------------------------------
	// SetupMRayBatch()
	//----------------------------------------------------------------------------
	fsLocator SetupMRayBatch(const fsLocator& i_Root, mrayGlobalData & io_GlobalData,
							 std::string params, bool i_ShowConsole);

	//--------------------------------------------------------------------
	// LaunchMRayMasterBatch()
	//--------------------------------------------------------------------
	void LaunchMRayMasterBatch(std::string i_MasterBatchPath);

	//--------------------------------------------------------------------
	// GetPotentialExportData()
	//--------------------------------------------------------------------
	mrayExportData GetPotentialExportData(mrayGlobalData & o_GlobalData);

	//--------------------------------------------------------------------
	// DoExport()
	//--------------------------------------------------------------------
	void DoExport( const mrayExportData &i_Data , mrayExporter& exporter );

	//--------------------------------------------------------------------
	//	RegisterExportInterest() - add a Export interest to the system
	//--------------------------------------------------------------------
	void RegisterExportInterest( mrayExportInterest* i_pInterest );

	//--------------------------------------------------------------------
	// UnRegisterExportInterest()
	//--------------------------------------------------------------------
	void UnRegisterExportInterest( mrayExportInterest* i_pInterest );

	//--------------------------------------------------------------------
	// Reset()
	//--------------------------------------------------------------------
	void Reset();

	//--------------------------------------------------------------------
	//	ResetCapture()
	//--------------------------------------------------------------------
	void ResetCapture();

	//--------------------------------------------------------------------
	// SetMRayLoc()
	//--------------------------------------------------------------------
	void SetMRayLoc( fsLocator i_Loc );

	//--------------------------------------------------------------------
	// SetMRayViewerLoc()
	//--------------------------------------------------------------------
	void SetMRayViewerLoc( fsLocator i_Loc );

	//--------------------------------------------------------------------
	// MentalRayEntry()
	//--------------------------------------------------------------------
	void MentalRayEntry(float i_AspectRatio, camCamera* i_Camera, g3dPrefs::g3dRenderPrefs i_RenderPrefs,
						fsLocator i_FileName, itString i_CurrentScene, int i_Width, int i_Height,
						int i_FilterFunc, float i_FilterWidth, int i_CaptureSampling,
						const mrayOptionsData& i_Options,
						bool & o_err, std::string & o_err_msg);

	//--------------------------------------------------------------------
	//  InsertPotentialTexture()
	//--------------------------------------------------------------------
	void InsertPotentialTexture(std::string i_Name, matTexture* i_MapTex, fsLocator i_TexLoc,
								mrayGlobalData & io_GlobalData, bool i_bRewrite);


	//--------------------------------------------------------------------
	//  AddMetaSLShader()
	//--------------------------------------------------------------------
	void AddMetaSLShader(mrayGlobalData & o_GlobalData, 
		const itString& i_ShaderNameIt, 
		const fsLocator& i_ShaderLoc);

	//----------------------------------------------------------------------------
	// SetupMRayDirectories()
	//----------------------------------------------------------------------------
	void SetupMRayDirectories(const fsLocator& i_Root , mrayGlobalData & o_GlobalData, bool i_ApplyFrameNumber);

	//----------------------------------------------------------------------------
	// WriteShaders()
	//----------------------------------------------------------------------------
	void WriteShaders( mrayGlobalData & io_GlobalData );

	//----------------------------------------------------------------------------
	// WriteTextures()
	//----------------------------------------------------------------------------
	void WriteTextures( mrayGlobalData & io_GlobalData );

	//----------------------------------------------------------------------------
	// LaunchStandaloneMray()
	//----------------------------------------------------------------------------
	void LaunchStandaloneMray( mrayGlobalData & io_GlobalData, fsLocator i_MRayLoc, fsLocator i_MrayViewerLoc, fsLocator i_Root,
							   std::string params, fsLocator i_MiLoc, bool i_ShowConsole );

};
