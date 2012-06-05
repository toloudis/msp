/****************************************************************************\
**	mrayMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mray/mrayMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/it/itStringUtil.hpp"

#include "Graphics/Mat/matMetaFX.hpp"
#include "Graphics/Mat/matMetaFXParser.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include "Support/mray/mrayExporter.hpp"
#include "Support/mray/mrayExportInterest.hpp"
#include "ImportExport/mray/export/private/mrayExportUtil.hpp"
#include "ImportExport/mray/export/mrayExportData.hpp"
#include "ImportExport/mray/export/mrayExport.hpp"

#include <vector>
#include <windows.h>

#undef CreateDirectory
#undef CreateFile
#undef DeleteFile

//============================================================================
//============================================================================
namespace
{
	std::vector<mrayExportInterest*> l_ExportInterestList;	
	fsLocator l_MRayLoc;
	fsLocator l_MRayViewerLoc;
	std::vector<fsLocator> l_BatchFiles;
	bool l_bMRayExportActive;
	std::vector< std::string > l_WrittenTextures;
}

//----------------------------------------------------------------------------
// SetupMRayDirectories()
//----------------------------------------------------------------------------
void mrayMgr::SetupMRayDirectories(const fsLocator& i_Root , mrayGlobalData & o_GlobalData, bool i_ApplyFrameNumber)
{
	itString currFrameNameIt = o_GlobalData.m_FileName.GetLastName();
	currFrameNameIt.StripExtension();
	std::string currFrameName = itStringUtil::GetStdString(currFrameNameIt);
	std::string sceneName = itStringUtil::GetStdString(o_GlobalData.m_CurrentScene);
	std::string frameNumber = sceneName + "_Frame_" + currFrameName.substr(currFrameName.rfind("_")+1,currFrameName.length()-1);

	fsLocator archivesDir = i_Root;
	fsLocator texturesDir = i_Root;

	archivesDir.Push("mray_assets");
	if ( i_ApplyFrameNumber ) archivesDir.Push(frameNumber.c_str());
	archivesDir.Push("archives");

	texturesDir.Push("mray_assets");
	if ( i_ApplyFrameNumber ) texturesDir.Push(frameNumber.c_str());
	texturesDir.Push("textures");

	fsFileUtil::CreateDirectory(archivesDir);
	fsFileUtil::CreateDirectory(texturesDir);

	o_GlobalData.m_ArchivesLoc = archivesDir;
	o_GlobalData.m_TexturesLoc = texturesDir;
}

//--------------------------------------------------------------------
//  hash()
//--------------------------------------------------------------------
unsigned long hashStr(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

//--------------------------------------------------------------------
//  InsertPotentialTexture()
//--------------------------------------------------------------------
void mrayMgr::InsertPotentialTexture(std::string i_Name, matTexture* i_MapTex, fsLocator i_TexLoc,
									 mrayGlobalData & io_GlobalData, bool i_bRewrite)
{
	if ( i_MapTex )
	{
		bool containsKey = false;
		for(std::map<fsLocator , std::vector< std::string >>::const_iterator it = io_GlobalData.m_FullTextureMap.begin(); it != io_GlobalData.m_FullTextureMap.end(); ++it)
		{
			if ( it->first == i_TexLoc )
			{
				containsKey = true;
				break;
			}
		}

		if ( containsKey )
		{
			for(std::map<fsLocator , std::vector< std::string >>::const_iterator it = io_GlobalData.m_FullTextureMap.begin(); it != io_GlobalData.m_FullTextureMap.end(); ++it)
			{
				if ( it->first == i_TexLoc )
				{
					std::vector<std::string> objects = it->second;
					objects.push_back( i_Name );
					io_GlobalData.m_FullTextureMap[i_TexLoc] = objects;
				}
			}
		}
		else
		{
			std::ostringstream texture;
			std::string hashKeyStr;
			fsFileUtil::LocatorToANSIFilename(i_TexLoc,hashKeyStr);
			unsigned char * hashKeyChar = (unsigned char *)hashKeyStr.c_str();
			unsigned long id = hashStr(hashKeyChar);
			texture << "Tex_" << id;
			std::vector<std::string> objects;
			objects.push_back( i_Name );
			io_GlobalData.m_FullTextureMap.insert( std::pair<fsLocator , std::vector< std::string >>(i_TexLoc,objects) );
			io_GlobalData.m_TextureIDs.insert( std::pair<fsLocator , std::string>(i_TexLoc,texture.str()) );
		}

		std::pair<matTexture*,fsLocator> myPair;
		myPair.first = i_MapTex;
		myPair.second = i_TexLoc;
		io_GlobalData.m_Textures.insert( std::pair<std::string,std::pair<matTexture*,fsLocator>>(i_Name,myPair) );
		io_GlobalData.m_TextureWrite.insert( std::pair<std::string , bool >(i_Name,i_bRewrite) );
	}


	//if ( i_MapTex )
	//{
	//	std::pair<matTexture*,fsLocator> myPair;
	//	myPair.first = i_MapTex;
	//	myPair.second = i_TexLoc;
	//	o_Map.insert( std::pair<std::string,std::pair<matTexture*,fsLocator>>(i_Name,myPair)  );
	//}
}

//--------------------------------------------------------------------
//  AddMetaSLShader()
//--------------------------------------------------------------------
void mrayMgr::AddMetaSLShader(mrayGlobalData & o_GlobalData, 
							  const itString& i_ShaderNameIt, 
							  const fsLocator& i_ShaderLoc)
{
	std::string shaderName = itStringUtil::GetStdString(i_ShaderNameIt);

	std::map< std::string , std::vector< fsLocator > >::iterator shaderEntry;
	shaderEntry = o_GlobalData.m_FullShaderMap.find(shaderName);
	if (shaderEntry != o_GlobalData.m_FullShaderMap.end())
	{
		// found a shader with this name. now look for unique filepath.
		std::vector<fsLocator>::iterator it = std::find(shaderEntry->second.begin(), shaderEntry->second.end(), i_ShaderLoc);
		if (it != shaderEntry->second.end())
		{
			shaderEntry->second.push_back(i_ShaderLoc);
		}
	}
	else
	{
		// no entries found - add a new one.
		std::vector<fsLocator> loc;
		loc.push_back(i_ShaderLoc);
		o_GlobalData.m_FullShaderMap[shaderName] = loc;
	}
}

//--------------------------------------------------------------------
// Gather up the list of the potential things to export and
// return in data structure
//--------------------------------------------------------------------
mrayExportData mrayMgr::GetPotentialExportData(mrayGlobalData & o_GlobalData)
{
	mrayExportData data;

	const int num_interests = l_ExportInterestList.size();
	data.m_SceneData.resize(num_interests);

	for ( int i=0; i < num_interests; i++ )
	{
		//	Set the chunk export data
		data.m_SceneData[i].m_Desc = l_ExportInterestList[i]->GetChunkDesc();
		l_ExportInterestList[i]->GatherSceneData( data.m_SceneData[i] , o_GlobalData );
	}

	return data;
}

//--------------------------------------------------------------------
// Export Maya Ascii file by calling Export functions on each
//	registered interest.
//--------------------------------------------------------------------
void mrayMgr::DoExport( const mrayExportData &i_Data , mrayExporter& exporter )
{
	const int num_interests = l_ExportInterestList.size();

	// Export potential data in specified order
	for ( int i=0; i < num_interests; i++ )
		if ( i_Data.m_SceneData[i].m_Desc == "LightSets" )
			l_ExportInterestList[i]->Export( exporter , i_Data.m_SceneData[i] );

	mrayExport::WriteSectionHeader(exporter.GetFileStream(),"Point Lights");
	for ( int i=0; i < num_interests; i++ )
		if ( i_Data.m_SceneData[i].m_Desc == "PointLights" )
			l_ExportInterestList[i]->Export( exporter , i_Data.m_SceneData[i] );

	mrayExport::WriteSectionHeader(exporter.GetFileStream(),"Projected Lights");
	for ( int i=0; i < num_interests; i++ )
		if ( i_Data.m_SceneData[i].m_Desc == "ProjLights" )
			l_ExportInterestList[i]->Export( exporter , i_Data.m_SceneData[i] );

	mrayExport::WriteSectionHeader(exporter.GetFileStream(),"Geometry and Materials");
	mrayExport::WriteWorldInclude(exporter.GetFileStream(),exporter.GetGlobalData());
	for ( int i=0; i < num_interests; i++ )
		if ( i_Data.m_SceneData[i].m_Desc == "Geometry" )
			l_ExportInterestList[i]->Export( exporter , i_Data.m_SceneData[i] );

	mrayExport::WriteSectionHeader(exporter.GetFileStream(),"Render World");
	exporter.EndExport();
}

//--------------------------------------------------------------------
//	RegisterExportInterest() - add a Export interest to the system
//--------------------------------------------------------------------
void mrayMgr::RegisterExportInterest( mrayExportInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Export Interest" );
	l_ExportInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
// UnRegisterExportInterest
//--------------------------------------------------------------------
void mrayMgr::UnRegisterExportInterest( mrayExportInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( l_ExportInterestList, i_pInterest );
}

//--------------------------------------------------------------------
// Reset()
//--------------------------------------------------------------------
void mrayMgr::Reset()
{
}

//--------------------------------------------------------------------
// SetMRayLoc()
//--------------------------------------------------------------------
void mrayMgr::SetMRayLoc( fsLocator i_Loc )
{
	l_MRayLoc = i_Loc;
}

//--------------------------------------------------------------------
// SetMRayViewerLoc()
//--------------------------------------------------------------------
void mrayMgr::SetMRayViewerLoc( fsLocator i_Loc )
{
	l_MRayViewerLoc = i_Loc;
}

//----------------------------------------------------------------------------
// LaunchStandaloneMray()
//----------------------------------------------------------------------------
void mrayMgr::LaunchStandaloneMray( mrayGlobalData & io_GlobalData, fsLocator i_MRayLoc, fsLocator i_MrayViewerLoc, fsLocator i_Root,
								    std::string params, fsLocator i_MiLoc, bool i_ShowConsole )
{
	itString sceneNameIt = io_GlobalData.m_FileName.GetLastName();
	sceneNameIt.StripExtension();

	itString batchNameIt = sceneNameIt;
	batchNameIt += itString("_mray_batch.bat");

	fsLocator batchFileLoc = i_Root;
	batchFileLoc.Push(batchNameIt);
	itString itFileName;
	fsFileUtil::LocatorToUnicodeString( batchFileLoc, itFileName );
	std::ofstream batchFile( itFileName.GetString() , std::ios::out );

	std::string MRayLoc;
	std::string MiLoc;

	fsFileUtil::LocatorToANSIFilename(io_GlobalData.m_MRayLoc,MRayLoc);
	fsFileUtil::LocatorToANSIFilename(io_GlobalData.m_FileName,MiLoc);

	fsLocator imfDispLoc(io_GlobalData.m_MRayLoc);
	imfDispLoc.Pop();
	imfDispLoc.Push("imf_disp");
	std::string imfDispPath;
	fsFileUtil::LocatorToANSIFilename(imfDispLoc,imfDispPath);

	itString tifNameIt = sceneNameIt;
	tifNameIt += itString( (std::string(".") + mrayExportUtil::GetMRayFileFormat(io_GlobalData.m_Options.m_OutputFormat)).c_str() );
	fsLocator tifFileLoc = i_Root;
	tifFileLoc.Push( tifNameIt );
	std::string tifNameStr;
	fsFileUtil::LocatorToANSIFilename(tifFileLoc,tifNameStr);	

	std::string batch_exec_str;
	std::string mray_exec;
	
	bool launchWindowsViewer = false;
	std::string viewer_exec = "";

	// Use imf_disp
	if ( i_MrayViewerLoc.GetNumNames() != 0 && 
		 fsFileUtil::FileExists( i_MrayViewerLoc ) &&
		 i_MrayViewerLoc.GetLastName() == itString("imf_disp.exe") &&
		 !io_GlobalData.m_bRenderingGIOnly &&
		 !io_GlobalData.m_bRenderingReflectionsOnly)
	{
		std::string MRayViewerLoc;
		fsFileUtil::LocatorToANSIFilename(i_MrayViewerLoc,MRayViewerLoc);
		if ( io_GlobalData.m_Options.m_bDisplayPreviewer )
		{
			mray_exec = "\"" + MRayLoc + "\" " + params + " -imgpipe 1 \"" + MiLoc + "\"" + " | \"" + MRayViewerLoc + "\" -";	
		}
		else
		{
			mray_exec = "\"" + MRayLoc + "\" " + params + " \"" + MiLoc + "\"";
		}
	} 

	// Use windows photo viewer
	else
	{
		std::string mray_exec = "\"" + MRayLoc + "\" " + params +  " \"" + MiLoc + "\"";
		if ( io_GlobalData.m_Options.m_bDisplayPreviewer )
		{
			std::string ext = mrayExportUtil::GetMRayFileFormat( io_GlobalData.m_Options.m_OutputFormat );
			if ( ext == "bmp" || ext == "jpg" || ext == "png" || ext == "tif" )
			{
				viewer_exec = "rundll32.exe C:\\WINDOWS\\System32\\shimgvw.dll,ImageView_Fullscreen " + tifNameStr;	
				launchWindowsViewer = true;
			}
		}
	}

	WinExec(mray_exec.c_str(),i_ShowConsole);
	if ( launchWindowsViewer ) WinExec(viewer_exec.c_str(),i_ShowConsole);
}

//----------------------------------------------------------------------------
// GetMRayExportActive()
//----------------------------------------------------------------------------
bool mrayMgr::GetMRayExportActive()
{
	return l_bMRayExportActive;
}


//--------------------------------------------------------------------
//	ResetCapture()
//--------------------------------------------------------------------
void mrayMgr::ResetCapture()
{
	if ( l_bMRayExportActive )
	{
		l_bMRayExportActive = false;
		l_BatchFiles.clear();
	}
}

//----------------------------------------------------------------------------
// LaunchMRayMasterBatch()
//----------------------------------------------------------------------------
void mrayMgr::LaunchMRayMasterBatch(std::string i_MasterBatchPath)
{
	WinExec(i_MasterBatchPath.c_str(),true);
}

//----------------------------------------------------------------------------
// WriteMasterBatch()
//----------------------------------------------------------------------------
std::string mrayMgr::WriteMasterBatch( const fsLocator& i_Root )
{
	fsLocator masterBatchLoc = i_Root;
	masterBatchLoc.Push("MRay_Master_Batch.bat");
	itString itFileName;
	fsFileUtil::LocatorToUnicodeString( masterBatchLoc, itFileName );
	std::ofstream batchFile( itFileName.GetString() , std::ios::out );

	for ( int i = 0 ; i < l_BatchFiles.size() ; i++ )
	{
		std::string currBatch;
		fsFileUtil::LocatorToANSIFilename( l_BatchFiles[i] , currBatch );
		batchFile << "CALL " << currBatch.c_str() << std::endl;
	}

	return itStringUtil::GetStdString( itFileName );
}

//----------------------------------------------------------------------------
// SetupMRayBatch()
//----------------------------------------------------------------------------
fsLocator mrayMgr::SetupMRayBatch(const fsLocator& i_Root, mrayGlobalData & io_GlobalData,
						   std::string params, bool i_ShowConsole)
{
	itString sceneNameIt = io_GlobalData.m_FileName.GetLastName();
	sceneNameIt.StripExtension();

	itString batchNameIt = sceneNameIt;
	batchNameIt += itString("_mray_batch.bat");

	fsLocator batchFileLoc = i_Root;
	batchFileLoc.Push(batchNameIt);
	itString itFileName;
	fsFileUtil::LocatorToUnicodeString( batchFileLoc, itFileName );
	std::ofstream batchFile( itFileName.GetString() , std::ios::out );

	std::string MRayLoc;
	std::string MiLoc;

	fsFileUtil::LocatorToANSIFilename(io_GlobalData.m_MRayLoc,MRayLoc);
	fsFileUtil::LocatorToANSIFilename(io_GlobalData.m_FileName,MiLoc);

	fsLocator imfDispLoc(io_GlobalData.m_MRayLoc);
	imfDispLoc.Pop();
	imfDispLoc.Push("imf_disp");
	std::string imfDispPath;
	fsFileUtil::LocatorToANSIFilename(imfDispLoc,imfDispPath);

	itString tifNameIt = sceneNameIt;
	tifNameIt += itString( (std::string(".") + mrayExportUtil::GetMRayFileFormat(io_GlobalData.m_Options.m_OutputFormat)).c_str() );
	fsLocator tifFileLoc = i_Root;
	tifFileLoc.Push( tifNameIt );
	std::string tifNameStr;
	fsFileUtil::LocatorToANSIFilename(tifFileLoc,tifNameStr);	

	// Delete previous rendered file to ensure we're next looking at a new one
//#ifdef _DEBUG
//	batchFile << "del \"" << tifNameStr << "\"" << std::endl;
//#endif

	std::string batch_exec_str;

	// Uncomment this to redirect the output to a file.
	// As a result, the command output will not be visible in the console window.
	// Note this is dos command-specific syntax.  
	const std::string redirect_str = ""; //" > raylog.txt 2>&1";

	// Use imf_disp
	if ( l_MRayViewerLoc.GetNumNames() != 0 && 
		 fsFileUtil::FileExists( l_MRayViewerLoc ) &&
		 l_MRayViewerLoc.GetLastName() == itString("imf_disp.exe") &&
		 !io_GlobalData.m_bRenderingGIOnly &&
		 !io_GlobalData.m_bRenderingReflectionsOnly)
	{
		std::string MRayViewerLoc;
		fsFileUtil::LocatorToANSIFilename(l_MRayViewerLoc,MRayViewerLoc);
		std::string mray_exec;
		if ( io_GlobalData.m_Options.m_bDisplayPreviewer )
		{
			mray_exec = "\"" + MRayLoc + "\" " + params + " -imgpipe 1 \"" + MiLoc + "\"" + " | \"" + MRayViewerLoc + "\" -";	
		}
		else
		{
			mray_exec = "\"" + MRayLoc + "\" " + params + " \"" + MiLoc + "\"";
		}
		batchFile << mray_exec << redirect_str << std::endl;
		batch_exec_str = "\"" + itStringUtil::GetStdString(itFileName) + "\"";
	} 

	// Use windows photo viewer
	else
	{
		std::string mray_exec = "\"" + MRayLoc + "\" " + params +  " \"" + MiLoc + "\"";	
		batchFile << mray_exec << redirect_str << std::endl;
		batch_exec_str = "\"" + itStringUtil::GetStdString(itFileName) + "\"";
		if ( io_GlobalData.m_Options.m_bDisplayPreviewer )
		{
			batch_exec_str = "\"" + itStringUtil::GetStdString(itFileName) + "\"";
			std::string ext = mrayExportUtil::GetMRayFileFormat( io_GlobalData.m_Options.m_OutputFormat );
			if ( ext == "bmp" || ext == "jpg" || ext == "png" || ext == "tif" )
			{
				std::string viewer_exec = "rundll32.exe C:\\WINDOWS\\System32\\shimgvw.dll,ImageView_Fullscreen " + tifNameStr;	
				batchFile << viewer_exec << std::endl;
			}
		}
	}

	fsLocator batch_exec_loc;
	fsFileUtil::ANSIFilenameToLocator(batch_exec_str,batch_exec_loc);

	return batch_exec_loc;
}

//--------------------------------------------------------------------
// SetupPasses()
//--------------------------------------------------------------------
void SetupPasses( mrayGlobalData & io_GlobalData,
	   			  g3dPrefs::g3dRenderPrefs i_RenderPrefs)
{

	io_GlobalData.m_bRenderEnvironments = i_RenderPrefs.m_bEnableEnvironment;
	io_GlobalData.m_bRenderLit = i_RenderPrefs.m_bEnableLitPass;
	io_GlobalData.m_bRenderDiffuse = i_RenderPrefs.m_bEnableDiffuseLighting;
	io_GlobalData.m_bRenderSpecular = i_RenderPrefs.m_bEnableSpecularLighting;
	io_GlobalData.m_bRenderTransparent = i_RenderPrefs.m_bEnableTransparent;

	io_GlobalData.m_bRenderingBeautyOnly = false;
	io_GlobalData.m_bRenderingShadowsOnly = false;
	io_GlobalData.m_bRenderingNormalsOnly = false;
	io_GlobalData.m_bRenderingReflectionsOnly = false;
	io_GlobalData.m_bRenderingIlluminationOnly = false;
	io_GlobalData.m_bRenderingGIOnly = false;
	io_GlobalData.m_bRenderingAOOnly = false;

	switch(i_RenderPrefs.m_RendererType)
	{
		case g3dSceneRendererTypes::e_AmbientOcclusion:
			io_GlobalData.m_bRenderingAOOnly = true;
			break;
		case g3dSceneRendererTypes::e_ShadowMask:
			io_GlobalData.m_bRenderingShadowsOnly = true;
			break;
		case g3dSceneRendererTypes::e_Normals:
			io_GlobalData.m_bRenderingNormalsOnly = true;
			break;
		case g3dSceneRendererTypes::e_IlluminationOnly:
			io_GlobalData.m_bRenderingIlluminationOnly = true;
			break;
		case g3dSceneRendererTypes::e_ReflectionOnly:
			io_GlobalData.m_bRenderingReflectionsOnly = true;
			break;
		case g3dSceneRendererTypes::e_MrayFinalGather:
			io_GlobalData.m_bRenderingGIOnly = true;
			break;
	};

	if ( (i_RenderPrefs.m_RendererType == g3dSceneRendererTypes::e_HDR) && 
		 (i_RenderPrefs.m_bEnableEnvironment) &&
		 (i_RenderPrefs.m_bEnableLitPass) &&
		 (i_RenderPrefs.m_bEnableDiffuseLighting) &&
		 (i_RenderPrefs.m_bEnableSpecularLighting) )
	{
		io_GlobalData.m_bRenderingBeautyOnly = true;
	}

}

//--------------------------------------------------------------------
//	ClearWrittenTextures()
//--------------------------------------------------------------------
void mrayMgr::ClearWrittenTextures()
{
	l_WrittenTextures.clear();
}

//----------------------------------------------------------------------------
// WriteMSLfromMFX()
// also returns the shader name from the MFX data
//----------------------------------------------------------------------------
void WriteMSLfromMFX(const fsLocator& i_MFXShaderLoc, const fsLocator& i_MSLShaderLoc,
					 std::string& o_ShaderName)
{
	// Load shader from multiple format effect file (MFX)
	matMetaFX shader_data;
	matMetaFXParser::ReadMetaFX( i_MFXShaderLoc, shader_data );
	// If we have metaSL data (have to check for correct version),
	// then load that data as the shader.
	DBG_ASSERT(shader_data.m_MetaSLSource.m_bHasData, "MFX shader without metaSL source!");
	bool bUseMetaSL = (shader_data.m_MetaSLSource.m_bHasData &&
		matMetaFX::CompareVersion(shader_data.m_MetaSLSource.m_Version, matMetaFX::GetCurrentMetaSLVersion()));

	// no error check here for now. assume things are ok.(?)

	// save the shader name for later.
	o_ShaderName = shader_data.m_ShaderName;

	// Create new file
	if( fsFileUtil::FileExists(i_MSLShaderLoc) )
		fsFileUtil::DeleteFile(i_MSLShaderLoc);
	fsFileUtil::CreateFile(i_MSLShaderLoc);

	gfFileBin ofile(i_MSLShaderLoc, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
	ofile.Write(shader_data.m_MetaSLSource.m_Data.size(),
		&shader_data.m_MetaSLSource.m_Data[0]);
}

//----------------------------------------------------------------------------
// WriteShaders()
// this function also stores the new shader path for later use.
//----------------------------------------------------------------------------
void mrayMgr::WriteShaders( mrayGlobalData & io_GlobalData )
{
	fsLocator mfxShaderLoc;
	mrayMetaSLShaderData mslShader;

	for(std::map<std::string, std::vector<fsLocator>>::const_iterator it = io_GlobalData.m_FullShaderMap.begin(); it != io_GlobalData.m_FullShaderMap.end(); ++it)
	{
		std::string mslShaderName = it->first;

		// found a shader with this name. now look for filepath.
		if (it->second.size() > 1)
		{
			bool found = false;
			int instance = 0;
			for (int i = 0; i < it->second.size(); i++)
			{
				std::ostringstream oss;
				oss << it->first << "_" << i << ".msl";
				mslShaderName = oss.str();

				// set the directory where the extracted msl goes
				mslShader.m_MSLLocator = io_GlobalData.m_TexturesLoc;
				mslShader.m_MSLLocator.Push(mslShaderName.c_str());

				mfxShaderLoc = it->second[i];

				WriteMSLfromMFX(mfxShaderLoc, mslShader.m_MSLLocator, mslShader.m_ShaderName);
				io_GlobalData.m_MSLShaderMap[mfxShaderLoc] = mslShader;
			}
		}
		else
		{
			std::ostringstream oss;
			oss << it->first << ".msl";
			mslShaderName = oss.str();

			// set the directory where the extracted msl goes
			mslShader.m_MSLLocator = io_GlobalData.m_TexturesLoc;
			mslShader.m_MSLLocator.Push(mslShaderName.c_str());

			mfxShaderLoc = it->second[0];

			WriteMSLfromMFX(mfxShaderLoc, mslShader.m_MSLLocator, mslShader.m_ShaderName);
			io_GlobalData.m_MSLShaderMap[mfxShaderLoc] = mslShader;
		}
	}
}

//----------------------------------------------------------------------------
// WriteTextures()
//----------------------------------------------------------------------------
void mrayMgr::WriteTextures( mrayGlobalData & io_GlobalData )
{
	for(std::map<std::string, std::pair<matTexture*,fsLocator>>::const_iterator it = io_GlobalData.m_Textures.begin(); it != io_GlobalData.m_Textures.end(); ++it)
	{ 

		if ( mrayExportUtil::TextureIsRewritten(it->first,io_GlobalData) )
		{
			// This texture already processed
			if ( !envSTLHelpers::Contains(l_WrittenTextures,it->first) )
			{
				fsLocator texturePath = mrayExportUtil::LookupTexturePath(it->first,io_GlobalData);
				itString pngFile = itString(mrayExportUtil::LookupGeneratedTexName(texturePath,io_GlobalData).c_str());
				pngFile += itString(".png");

				fsLocator pngLoc = io_GlobalData.m_TexturesLoc;
				pngLoc.Push(pngFile);

				// Rewrite assets - YES
				if ( io_GlobalData.m_Options.m_bRewriteAssets )
				{
					matTextureMgr::SaveTextureToRgbaPNG( it->second.first , pngLoc );
				}

				// Rewrite assets - NO
				else
				{
					// Write TIF if file does not exist
					if ( !fsFileUtil::FileExists( pngLoc )  )
					{
						matTextureMgr::SaveTextureToRgbaPNG( it->second.first , pngLoc );
					}
				}
			}

			l_WrittenTextures.push_back( it->first );

		}
	}
}

//--------------------------------------------------------------------
// MentalRayEntry()
//--------------------------------------------------------------------
void mrayMgr::MentalRayEntry(float i_AspectRatio, camCamera* i_Camera, g3dPrefs::g3dRenderPrefs i_RenderPrefs,
							 fsLocator i_FileName, itString i_CurrentScene, int i_Width, int i_Height,
							 int i_FilterFunc, float i_FilterWidth, int i_CaptureSampling,
							 const mrayOptionsData& i_Options,
							 bool & o_err, std::string & o_err_msg)
{
	l_bMRayExportActive = true;

	// Store info coming from capture
	mrayGlobalData renderData;

	renderData.m_Options = i_Options;

	renderData.m_AspectRatio = i_AspectRatio;
	renderData.m_Camera = i_Camera;
	renderData.m_FileName = i_FileName;
	renderData.m_CurrentScene = i_CurrentScene;
	renderData.m_Width = i_Width;
	renderData.m_Height = i_Height;
	renderData.m_FilterFunc = i_FilterFunc;
	renderData.m_FilterWidth = i_FilterWidth;
	renderData.m_CaptureSampling = i_CaptureSampling;
	renderData.m_MRayLoc = l_MRayLoc;

	if ( l_MRayLoc.GetNumNames() == 0 )
	{
		o_err = true;
		o_err_msg = "No path to \"ray.exe\" has been specified in MachStudio preferences. Aborting.\n\n";
		return;
	} 
	if ( !fsFileUtil::FileExists( l_MRayLoc ) || l_MRayLoc.GetLastName() != itString("ray.exe") )
	{
		o_err = true;
		o_err_msg = "Correct path to \"ray.exe\" has not been specified in MachStudio preferences. Aborting.\n\n";
		return;
	}

	SetupPasses(renderData,i_RenderPrefs);

	fsLocator rootLoc = i_FileName;
	rootLoc.Pop();

	SetupMRayDirectories(rootLoc,renderData, true);

	itString worldName = i_FileName.GetLastName();
	worldName.StripExtension();
	worldName += itString("_WORLD.mi");
	renderData.m_WorldName = renderData.m_ArchivesLoc;
	renderData.m_WorldName.Push(worldName);

	// Gather everything
	mrayExportData myExportData = GetPotentialExportData(renderData);

	// Write textures
	WriteTextures( renderData );
	WriteShaders( renderData );

	// Export everything
	itString itFileName;
	fsFileUtil::LocatorToUnicodeString( i_FileName, itFileName );
	mrayExporter exporter( itFileName.GetString() , renderData );
	DoExport( myExportData , exporter );

	// bin library path will be the mental ray exe path for now:
	fsLocator libPathLoc = l_MRayLoc;
	libPathLoc.Pop();
	std::string libPathStr;
	fsFileUtil::LocatorToANSIFilename(libPathLoc,libPathStr);

	// Launch MR
	std::string rootStr;
	fsFileUtil::LocatorToANSIFilename(rootLoc,rootStr);
	std::ostringstream params;
	params << "-verbose " << (renderData.m_Options.m_VerbosityLevel+1) << 
		      " -threads " << renderData.m_Options.m_NumThreads << 
			  " -memory " << renderData.m_Options.m_MemoryLimit << 
			  " -texture_continue " << (renderData.m_Options.m_bIgnoreBadTex?"on":"off") <<
			  " -L \"" << libPathStr << "\"" <<
			  " -file_dir \"" << rootStr << "\"";

	//LaunchMentalRay( renderData.m_MRayLoc, params, i_FileName , true );

	fsLocator batchFileLoc = SetupMRayBatch(rootLoc,renderData,params.str(),true);

	l_BatchFiles.push_back( batchFileLoc );
}