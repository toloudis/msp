/*****************************************************************************\
**	rmanMgr.hpp
**
**		Provides method for looking up a Exportd object throughout all systems
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef RMAN_MGR_HPP
#error rmanMgr.hpp multiply included
#endif
#define RMAN_MGR_HPP

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
struct rmanGlobalData;
struct rmanExportData;
struct rmanOptionsData;
class captRenderOutputData;
class rmanExporter;
class matTexture;
class rlyrPassesData;
class rmanExportInterest;
class g3dProjectedLight;
class fsLocator;
class camCamera;
class nameString;

#include <string>
#include <map>
#include <vector>

//============================================================================
//============================================================================
namespace rmanMgr
{
	//--------------------------------------------------------------------
	// GetPotentialExportData()
	//--------------------------------------------------------------------
	rmanExportData GetPotentialExportData(rmanGlobalData & o_GlobalData);

	//--------------------------------------------------------------------
	// DoExport()
	//--------------------------------------------------------------------
	void DoExport( rmanExporter &i_Exporter, const rmanExportData &i_Data );

	//----------------------------------------------------------------------------
	// WriteTiffTextures()
	//----------------------------------------------------------------------------
	void WriteTiffTextures( rmanGlobalData & io_GlobalData );

	//----------------------------------------------------------------------------
	// SetupRendermanBatch()
	//----------------------------------------------------------------------------
	fsLocator SetupRendermanBatch(const fsLocator& i_Root, const rmanExportData &i_Data, 
		 						  fsLocator i_RibPath, std::string i_Renderer, rmanGlobalData & io_GlobalData );

	//----------------------------------------------------------------------------
	// InsertPotentialTexture()
	//----------------------------------------------------------------------------
	void InsertPotentialTexture(std::string i_Name, matTexture* i_MapTex, fsLocator i_TexLoc,		
								rmanGlobalData & io_GlobalData, bool i_bIsRamp);

	//--------------------------------------------------------------------
	//	RegisterExportInterest() - add a Export interest to the system
	//--------------------------------------------------------------------
	void RegisterExportInterest( rmanExportInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterExportInterest() - remove a Export interest from the system.
	//
	//	Note: this will NOT delete the Export interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterExportInterest( rmanExportInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	RegisterExportInterest()
	//--------------------------------------------------------------------
	void SetPrmanLoc( const fsLocator& i_Loc );

	//--------------------------------------------------------------------
	//	GetPrmanLoc()
	//--------------------------------------------------------------------
	fsLocator GetPrmanLoc();

	//--------------------------------------------------------------------
	//	AddBatchFile()
	//--------------------------------------------------------------------
	void AddBatchFile(fsLocator i_BatchFile);

	//--------------------------------------------------------------------
	//	GetBatchCollection()
	//--------------------------------------------------------------------
	std::vector<fsLocator> GetBatchCollection();

	//--------------------------------------------------------------------
	//	GetRmanExportActive()
	//--------------------------------------------------------------------
	bool GetRmanExportActive();

	//--------------------------------------------------------------------
	//	SetRmanExportActive()
	//--------------------------------------------------------------------
	void SetRmanExportActive(bool i_Val);

	//--------------------------------------------------------------------
	//	GetRootLoc()
	//--------------------------------------------------------------------
	fsLocator GetRootLoc();

	//--------------------------------------------------------------------
	//	SetRootLoc()
	//--------------------------------------------------------------------
	void SetRootLoc(fsLocator i_Loc);

	//--------------------------------------------------------------------
	//	ClearWrittenTextures()
	//--------------------------------------------------------------------
	void ClearWrittenTextures();

	//--------------------------------------------------------------------
	//	ResetCapture()
	//--------------------------------------------------------------------
	void ResetCapture();

	//----------------------------------------------------------------------------
	// WriteMasterBatch()
	//----------------------------------------------------------------------------
	fsLocator WriteMasterBatch();

	//----------------------------------------------------------------------------
	// LaunchRendermanBatch()
	//----------------------------------------------------------------------------
	void LaunchRendermanBatch( fsLocator i_Batch, bool i_ShowConsole );

	//----------------------------------------------------------------------------
	// SetupRendermanDirectories()
	//----------------------------------------------------------------------------
	void SetupRendermanDirectories(const fsLocator& i_Root, rmanGlobalData & io_GlobalData);

	//----------------------------------------------------------------------------
	// RendermanEntry()
	//----------------------------------------------------------------------------
	void RendermanEntry( float i_AspectRatio, camCamera* i_Camera, g3dPrefs::g3dRenderPrefs i_RenderPrefs,
				    	fsLocator i_FileName, itString i_CurrentScene, int i_Width, int i_Height,
						int i_FilterFunc, float i_FilterWidth, int i_CaptureSampling,
						const rmanOptionsData & i_Options,
						bool & o_err, std::string & o_err_msg );

};
