/****************************************************************************\
**	rmanMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/rman/rmanMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/it/itStringUtil.hpp"
//#include "Core/name/nameString.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"

#include "ImportExport/rman/export/private/rmanUtil.hpp"
#include "ImportExport/rman/export/rmanExportData.hpp"

#include "Support/Capt/captRenderOutputData.hpp"
#include "Support/rlyr/data/rlyrPassesData.hpp"
#include "Support/rman/rmanExporter.hpp"
#include "Support/rman/rmanExportInterest.hpp"

#include "Tool/gui/guiProgressDialog.hpp"

#include <vector>
#include <windows.h>

#undef CreateDirectory

#define RMAN_TEXTURE_MAP			0
#define RMAN_SHADOW_MAP				1
#define RMAN_REFLECTIONCUBE_MAP		2
#define RMAN_REFLECTIONPLANAR_MAP	3

//============================================================================
//============================================================================
namespace
{
	std::vector<rmanExportInterest*> l_ExportInterestList;

	// Always active through a set of rman passes
	fsLocator l_PrmanLoc;
	fsLocator l_RootLoc;
	bool l_bRmanExportActive = false;
	std::vector<fsLocator> l_BatchFiles;
	std::vector< std::string > l_WrittenTextures;
}

//--------------------------------------------------------------------
//  hash()
//--------------------------------------------------------------------
unsigned long hash(unsigned char *str)
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
void rmanMgr::InsertPotentialTexture(std::string i_Name, matTexture* i_MapTex, fsLocator i_TexLoc,
									 rmanGlobalData & io_GlobalData, bool i_bIsRamp)
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
			unsigned long id = hash(hashKeyChar);
			texture << "Tex_" << id;
			std::vector<std::string> objects;
			objects.push_back( i_Name );
			io_GlobalData.m_FullTextureMap.insert( std::pair<fsLocator , std::vector< std::string >>(i_TexLoc,objects) );
			io_GlobalData.m_TextureIDs.insert( std::pair<fsLocator , std::string>(i_TexLoc,texture.str()) );
		}

		std::pair<matTexture*,fsLocator> myPair;
		myPair.first = i_MapTex;
		myPair.second = i_TexLoc;
		io_GlobalData.m_Textures.insert( std::pair<std::string,std::pair<matTexture*,fsLocator>>(i_Name,myPair)  );
	}
}

//--------------------------------------------------------------------
// GetPotentialExportData()
//--------------------------------------------------------------------
rmanExportData rmanMgr::GetPotentialExportData(rmanGlobalData & o_GlobalData)
{
	rmanExportData data;

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
// SetupRendermanGlobals()
//--------------------------------------------------------------------
void SetupRendermanGlobals( rmanExportData &o_Data,
									rmanGlobalData &o_GlobalData,
									g3dPrefs::g3dRenderPrefs i_RenderPrefs,
									int i_FilterFunc, 
									float i_FilterWidth, 
									int i_CaptureSampling,
									int i_Width, 
									int i_Height,
									float i_PixelAspectRatio,
									camCamera * i_pCamera,
									std::string i_Engine,
									fsLocator i_RibPath,
									const rmanOptionsData & i_Options,
									std::string i_FrameName)
{
	o_GlobalData.m_Options = i_Options;
	o_GlobalData.m_FrameName = i_FrameName;
	o_GlobalData.m_RmanFilterType = i_FilterFunc;	
	o_GlobalData.m_RmanFilterWidth = i_FilterWidth;
	o_GlobalData.m_RmanAArate = i_CaptureSampling;
	o_GlobalData.m_SceneCamera = i_pCamera;
	o_GlobalData.m_width = i_Width;
	o_GlobalData.m_height = i_Height;
	o_GlobalData.m_PixelAspectRatio = i_PixelAspectRatio;
	o_GlobalData.m_RibPath = i_RibPath;
	o_GlobalData.m_bRenderDOF = i_RenderPrefs.m_bEnableDOF;
	o_GlobalData.m_Engine = i_Engine;

	o_GlobalData.m_CommandNumber = 0;

	o_GlobalData.m_bRenderEnvironments = i_RenderPrefs.m_bEnableEnvironment;
	o_GlobalData.m_bRenderLit = i_RenderPrefs.m_bEnableLitPass;
	o_GlobalData.m_bRenderDiffuse = i_RenderPrefs.m_bEnableDiffuseLighting;
	o_GlobalData.m_bRenderSpecular = i_RenderPrefs.m_bEnableSpecularLighting;
	o_GlobalData.m_bRenderTransparent = i_RenderPrefs.m_bEnableTransparent;

	o_GlobalData.m_bRenderingBeautyOnly = false;
	o_GlobalData.m_bRenderingShadowsOnly = false;
	o_GlobalData.m_bRenderingNormalsOnly = false;
	o_GlobalData.m_bRenderingReflectionsOnly = false;
	o_GlobalData.m_bRenderingIlluminationOnly = false;
	o_GlobalData.m_bRenderingGIOnly = false;
	o_GlobalData.m_bRenderingAOOnly = false;

	switch(i_RenderPrefs.m_RendererType)
	{
		case g3dSceneRendererTypes::e_AmbientOcclusion:
			o_GlobalData.m_bRenderingAOOnly = true;
			break;
		case g3dSceneRendererTypes::e_ShadowMask:
			o_GlobalData.m_bRenderingShadowsOnly = true;
			break;
		case g3dSceneRendererTypes::e_Normals:
			o_GlobalData.m_bRenderingNormalsOnly = true;
			break;
		case g3dSceneRendererTypes::e_IlluminationOnly:
			o_GlobalData.m_bRenderingIlluminationOnly = true;
			break;
		case g3dSceneRendererTypes::e_ReflectionOnly:
			o_GlobalData.m_bRenderingReflectionsOnly = true;
			break;
		case g3dSceneRendererTypes::e_RmanColorBleed:
			o_GlobalData.m_bRenderingGIOnly = true;
			break;
	};

	if ( (i_RenderPrefs.m_RendererType == g3dSceneRendererTypes::e_HDR) && 
		 (i_RenderPrefs.m_bEnableEnvironment) &&
		 (i_RenderPrefs.m_bEnableLitPass) &&
		 (i_RenderPrefs.m_bEnableDiffuseLighting) &&
		 (i_RenderPrefs.m_bEnableSpecularLighting) )
	{
		o_GlobalData.m_bRenderingBeautyOnly = true;
	}

}

//--------------------------------------------------------------------
// Export Maya Ascii file by calling Export functions on each
//	registered interest.
//--------------------------------------------------------------------
void rmanMgr::DoExport( rmanExporter &i_Exporter, const rmanExportData &i_Data )
{
	const int num_interests = l_ExportInterestList.size();

	// Parse potential data
	for ( int i=0; i < num_interests; i++ )
	{
		if ( i_Data.m_SceneData[i].m_Desc == "LightSets" )
			l_ExportInterestList[i]->Export( i_Exporter, i_Data.m_SceneData[i] );
	}
	for ( int i=0; i < num_interests; i++ )
	{
		if ( i_Data.m_SceneData[i].m_Desc == "ProjLights" )
			l_ExportInterestList[i]->Export( i_Exporter, i_Data.m_SceneData[i] );
	}
	for ( int i=0; i < num_interests; i++ )
	{
		if ( i_Data.m_SceneData[i].m_Desc == "PointLights" )
			l_ExportInterestList[i]->Export( i_Exporter, i_Data.m_SceneData[i] );
	}
	for ( int i=0; i < num_interests; i++ )
	{
		if ( i_Data.m_SceneData[i].m_Desc == "Geometry" )
			l_ExportInterestList[i]->Export( i_Exporter, i_Data.m_SceneData[i] );
	}

	// Export Master RIB
	i_Exporter.ExportMasterRib();

	// Shadow Map RIBs
	i_Exporter.ExportShadowMaps();

	// Reflection Map RIBs
	i_Exporter.ExportReflectionMaps();

	// Photon Map RIBs
	//exporter.ExportPhotonMap();
}

//--------------------------------------------------------------------
//	RegisterExportInterest() - add a Export interest to the system
//--------------------------------------------------------------------
void rmanMgr::RegisterExportInterest( rmanExportInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Export Interest" );
	l_ExportInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
//	UnRegisterExportInterest() - remove a Export interest from the system.
//
//	Note: this will NOT delete the Export interest.  It is up to the
//	registerer.
//--------------------------------------------------------------------
void rmanMgr::UnRegisterExportInterest( rmanExportInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( l_ExportInterestList, i_pInterest );
}

//--------------------------------------------------------------------
//	SetPrmanLoc()
//--------------------------------------------------------------------
void rmanMgr::SetPrmanLoc( const fsLocator& i_Loc )
{
	l_PrmanLoc = i_Loc;
}

//--------------------------------------------------------------------
//	GetPrmanLoc()
//--------------------------------------------------------------------
fsLocator rmanMgr::GetPrmanLoc()
{
	return l_PrmanLoc;
}

//--------------------------------------------------------------------
//	AddBatchFile()
//--------------------------------------------------------------------
void rmanMgr::AddBatchFile(fsLocator i_BatchFile)
{
	l_BatchFiles.push_back(i_BatchFile);
}

//--------------------------------------------------------------------
//	GetBatchCollection()
//--------------------------------------------------------------------
std::vector<fsLocator> rmanMgr::GetBatchCollection()
{
	return l_BatchFiles;
}

//--------------------------------------------------------------------
//	GetRmanExportActive()
//--------------------------------------------------------------------
bool rmanMgr::GetRmanExportActive()
{
	return l_bRmanExportActive;
}

//--------------------------------------------------------------------
//	SetRmanExportActive()
//--------------------------------------------------------------------
void rmanMgr::SetRmanExportActive(bool i_Val)
{
	l_bRmanExportActive = i_Val;
}

//--------------------------------------------------------------------
//	GetRootLoc()
//--------------------------------------------------------------------
fsLocator rmanMgr::GetRootLoc()
{
	return l_RootLoc;
}

//--------------------------------------------------------------------
//	SetRootLoc()
//--------------------------------------------------------------------
void rmanMgr::SetRootLoc(fsLocator i_Loc)
{
	l_RootLoc = i_Loc;
}

//--------------------------------------------------------------------
//	ClearWrittenTextures()
//--------------------------------------------------------------------
void rmanMgr::ClearWrittenTextures()
{
	l_WrittenTextures.clear();
}

//--------------------------------------------------------------------
//	ResetCapture()
//--------------------------------------------------------------------
void rmanMgr::ResetCapture()
{
	if ( l_bRmanExportActive )
	{
		l_bRmanExportActive = false;
		l_BatchFiles.clear();
	}
}

//----------------------------------------------------------------------------
// WriteBatchCubemapLine()
//----------------------------------------------------------------------------
void WriteBatchCubemapLine(std::string i_Command, std::string i_Params, std::ofstream & i_Stream,
							 fsLocator i_RendererRoot)
{
	std::string quotedCmd = "\"";
	std::string command;
	i_RendererRoot.Pop();
	fsFileUtil::LocatorToANSIFilename(i_RendererRoot,command);
	command.append(i_Command);
	command.append(" ").append(i_Params);
	quotedCmd.append(command);
	quotedCmd.append("\r\n");
	i_Stream.write(quotedCmd.c_str() , quotedCmd.length());
}

//----------------------------------------------------------------------------
// WriteBatchExecutionLine()
//----------------------------------------------------------------------------
void WriteBatchExecutionLine(std::string i_RibFile, std::ofstream & i_Stream, 
							 fsLocator i_RendererRoot, std::string rendererParams)
{
	std::string quotedCmd = "\"";
	std::string command;
	fsFileUtil::LocatorToANSIFilename(i_RendererRoot,command);
	command.append("\" ").append( rendererParams ).append(" \"").append(i_RibFile).append("\"");
	quotedCmd.append(command);
	quotedCmd.append("\r\n");
	i_Stream.write(quotedCmd.c_str() , quotedCmd.length());
}

//----------------------------------------------------------------------------
// WriteBatchConversionLine()
//----------------------------------------------------------------------------
void WriteBatchConversionLine(std::string i_Command, std::string i_SrcFileNameStr, std::string i_DstFileNameStr,
					std::string i_SrcStr, std::string i_DestStr, std::ofstream & i_Stream,
					fsLocator i_RendererRoot)
{
	std::string quotedCmd = "\"";
	std::string command;
	i_RendererRoot.Pop();
	fsFileUtil::LocatorToANSIFilename(i_RendererRoot,command);
	command.append(i_Command);
	command.append(" \"").append(i_SrcStr).append(i_SrcFileNameStr).append("\" \"").append(i_DestStr).append(i_DstFileNameStr).append("\"");
	quotedCmd.append(command);
	quotedCmd.append("\r\n");
	i_Stream.write(quotedCmd.c_str() , quotedCmd.length());
}

//----------------------------------------------------------------------------
// WriteBatchLine()
//----------------------------------------------------------------------------
void WriteBatchLine(std::string i_Command , std::ofstream & i_Stream)
{
	i_Stream.write(i_Command.c_str() , i_Command.length());
}

//-----

//----------------------------------------------------------------------------
// replace_extension()
//----------------------------------------------------------------------------
std::string replace_extension(std::string context, std::string newExt) {
	int pos = context.rfind('.');
	return context.substr(0,pos).append(newExt);
} 

//----------------------------------------------------------------------------
// GetSuppressedWarnings()
//----------------------------------------------------------------------------
std::string GetSuppressedWarnings(rmanGlobalData i_GlobalData)
{
	std::string warnings = "";
	if ( i_GlobalData.m_Options.m_bRmanDisableWarnings )
	{
		warnings.append( " -woff " );
		warnings.append( "A60001,G04002,G13002,G26006,H08011,P79010,R01001,R01002,R01003,R01004,R01005,R01006,R01007,R01008,R01009,R01012," );
		warnings.append( "R01013,R01014,R01015,R02001,R02002,R02003,R02004,R03001,R04006,R04007,R05001,R05003,R05004,R05005,R05008,R05009," );
		warnings.append( "R05010,R07001,R07002,R07003,R07004,R07005,R07008,R08001,R08002,R08003,R09001,R09002,R09003,R09004,R09005,R09006," );
		warnings.append( "R09007,R09008,R09009,R09010,R09011,R09012,R09014,R09015,R09016,R09017,R09019,R09021,R09022,R09023,R09024,R10001," );
		warnings.append( "R10002,R10003,R10006,R11001,R11003,R11005,R11006,R11007,R15002,R17001,R18001,R18005,R18006,R18007,R18008,R18009," );
		warnings.append( "R19001,R19002,R19005,R19007,R19009,R19011,R19013,R20001,R20002,R20004,R30003,R31002,R31012,R32001,R32002,R32004," );
		warnings.append( "R32005,R32006,R32007,R50005,R50006,R53002,R56001,S01001,S01002,S01005,S01008,S01021,S01022,S31001,S31002,S31004," );
		warnings.append( "S31005,T02004,T10004,T15001,T15002,T15003,T15004,G27002,N02013" );
	} 
	
	return warnings;
}

//----------------------------------------------------------------------------
// StartSmartBatchItem()
//----------------------------------------------------------------------------
void StartSmartBatchItem( std::ofstream & i_Stream, std::string dest, rmanGlobalData & io_GlobalData )
{
	if ( !io_GlobalData.m_Options.m_bRmanRewriteAssets )
	{
		std::ostringstream fileExistCommand;
		fileExistCommand << "@ECHO OFF \r\nIF EXIST \"" << dest << "\" GOTO COMMAND_" << io_GlobalData.m_CommandNumber+1;
		fileExistCommand << "\r\n@ECHO ON\r\n";
		WriteBatchLine( fileExistCommand.str() , i_Stream );
	}
}

//----------------------------------------------------------------------------
// EndSmartBatchItem()
//----------------------------------------------------------------------------
void EndSmartBatchItem( std::ofstream & i_Stream, rmanGlobalData & io_GlobalData )
{
	if ( !io_GlobalData.m_Options.m_bRmanRewriteAssets )
	{
		std::ostringstream nextCommand;
		nextCommand << "\r\n:COMMAND_" << io_GlobalData.m_CommandNumber+1 << "\r\n@ECHO ON\r\n";
		WriteBatchLine( nextCommand.str() , i_Stream );
		io_GlobalData.m_CommandNumber++;
	}
}

//----------------------------------------------------------------------------
// FillRendermanBatchRibs()
//----------------------------------------------------------------------------
void FillRendermanBatchRibs( std::vector<fsLocator> i_Vec, std::string i_Engine,
							 std::ofstream & i_Stream, int i_MapType, rmanGlobalData & io_GlobalData)
{
	for ( int j = 0 ; j < i_Vec.size() ; j++ )
	{
		fsLocator fileNameLoc = i_Vec[j].GetLastName();
		fsLocator srcLoc = io_GlobalData.m_TiffLoc;
		fsLocator destLoc = io_GlobalData.m_TexturesLoc;

		std::string srcFileNameStr;
		std::string dstFileNameStr;
		std::string srcStr;
		std::string destStr;

		fsFileUtil::LocatorToANSIFilename( fileNameLoc , srcFileNameStr );
		fsFileUtil::LocatorToANSIFilename( fileNameLoc , dstFileNameStr );
		fsFileUtil::LocatorToANSIFilename( srcLoc , srcStr );
		fsFileUtil::LocatorToANSIFilename( destLoc , destStr );

		srcStr.append("\\");
		destStr.append("\\");

		if ( i_MapType == RMAN_SHADOW_MAP )
		{
			fsFileUtil::LocatorToANSIFilename( fileNameLoc , srcFileNameStr );
			fsFileUtil::LocatorToANSIFilename( fileNameLoc , dstFileNameStr );

			srcLoc = io_GlobalData.m_ShadowMapsLoc;
			fsFileUtil::LocatorToANSIFilename( srcLoc , srcStr );				
			srcStr.append("\\");

			std::ostringstream execParams;
			execParams << "-progress";
			execParams << GetSuppressedWarnings(io_GlobalData);

			dstFileNameStr = replace_extension(dstFileNameStr,".tex");

			StartSmartBatchItem(i_Stream, destStr + dstFileNameStr, io_GlobalData);

			srcFileNameStr = replace_extension(srcFileNameStr,".rib");
			WriteBatchExecutionLine( srcStr+srcFileNameStr,i_Stream,l_PrmanLoc, execParams.str() );

			srcFileNameStr = replace_extension(srcFileNameStr,".z");
			WriteBatchConversionLine("\\txmake.exe\" -shadow",
				           srcFileNameStr,dstFileNameStr,srcStr,destStr,i_Stream,l_PrmanLoc);

			EndSmartBatchItem(i_Stream, io_GlobalData);

		}
		else if ( i_MapType == RMAN_REFLECTIONCUBE_MAP )
		{
			std::string destCubeMap = destStr + srcFileNameStr.substr(0,srcFileNameStr.length()-10) + ".tex";
			StartSmartBatchItem(i_Stream, destCubeMap, io_GlobalData);

			std::string cubeFacePaths;
			for ( int i = j ; i < 6+j ; i++ )
			{
				std::string currReflectionRib;
				fsFileUtil::LocatorToANSIFilename( i_Vec[i], currReflectionRib );

				srcLoc = io_GlobalData.m_ReflectionMapsLoc;
				fsFileUtil::LocatorToANSIFilename( srcLoc , srcStr );				
				srcStr.append("\\");

				std::string cubeFacePath = srcStr+currReflectionRib;

				std::ostringstream execParams;
				execParams << "-progress";
				execParams << GetSuppressedWarnings(io_GlobalData);

				WriteBatchExecutionLine( cubeFacePath,i_Stream,l_PrmanLoc,execParams.str() );

				std::string currReflectionTif = currReflectionRib;
				currReflectionTif = replace_extension(currReflectionTif,".tif");
				cubeFacePaths.append("\"").append(srcStr+currReflectionTif).append("\" ");
			}
			WriteBatchCubemapLine("\\txmake.exe\" -envcube",cubeFacePaths+"\""+destCubeMap+"\"",i_Stream,l_PrmanLoc);
			j+=5;

			EndSmartBatchItem(i_Stream, io_GlobalData);
		}
		else if ( i_MapType == RMAN_REFLECTIONPLANAR_MAP )
		{
			fsLocator srcLoc = io_GlobalData.m_ReflectionMapsLoc;
			fsFileUtil::LocatorToANSIFilename( srcLoc , srcStr );				
			srcStr.append("\\");

			std::ostringstream execParams;
			execParams << "-progress";
			execParams << GetSuppressedWarnings(io_GlobalData);

			dstFileNameStr = replace_extension(dstFileNameStr,".tex");

			StartSmartBatchItem(i_Stream, destStr + dstFileNameStr, io_GlobalData);

			srcFileNameStr = replace_extension(srcFileNameStr,".rib");
			WriteBatchExecutionLine( srcStr+srcFileNameStr,i_Stream,l_PrmanLoc, execParams.str() );

			srcFileNameStr = replace_extension(srcFileNameStr,".tif");
			WriteBatchConversionLine("\\txmake.exe\" -resize up-",
				           srcFileNameStr,dstFileNameStr,srcStr,destStr,i_Stream,l_PrmanLoc);

			EndSmartBatchItem(i_Stream, io_GlobalData);

		}
	}
}

//----------------------------------------------------------------------------
// FillRendermanBatchTex()
//----------------------------------------------------------------------------
void FillRendermanBatchTex( std::map<std::string,std::pair<matTexture*,fsLocator>> i_Map, std::string i_Engine,
							std::ofstream & i_Stream, int i_MapType, rmanGlobalData & io_GlobalData)
{
	std::vector< itString > writtenTextures;

	for(std::map<std::string, std::pair<matTexture*,fsLocator>>::const_iterator it = i_Map.begin(); it != i_Map.end(); ++it)
	{
		fsLocator srcLoc = io_GlobalData.m_TiffLoc;
		fsLocator destLoc = io_GlobalData.m_TexturesLoc;

		std::string srcStr;
		std::string destStr;

		fsFileUtil::LocatorToANSIFilename( srcLoc , srcStr );
		fsFileUtil::LocatorToANSIFilename( destLoc , destStr );

		srcStr.append("\\");
		destStr.append("\\");

		fsLocator texturePath = rmanUtil::LookupTexturePath(it->first,io_GlobalData);
		itString tiffFile = itString(rmanUtil::LookupGeneratedTexName(texturePath,io_GlobalData).c_str());

		if ( !envSTLHelpers::Contains( writtenTextures , tiffFile ) )
		{
			std::string srcFileNameStr = itStringUtil::GetStdString(tiffFile) + ".tif";
			std::string dstFileNameStr = itStringUtil::GetStdString(tiffFile) + ".tex";	

			StartSmartBatchItem(i_Stream, destStr + dstFileNameStr, io_GlobalData);

			WriteBatchConversionLine("\\txmake.exe\" -mode periodic -resize up-",
						   srcFileNameStr, dstFileNameStr,srcStr,destStr,i_Stream,l_PrmanLoc);

			EndSmartBatchItem(i_Stream, io_GlobalData);
			
			writtenTextures.push_back( tiffFile );
		}

	}
}

//----------------------------------------------------------------------------
// WriteTiffTextures()
//----------------------------------------------------------------------------
void rmanMgr::WriteTiffTextures( rmanGlobalData & io_GlobalData )
{
	for(std::map<std::string, std::pair<matTexture*,fsLocator>>::const_iterator it = io_GlobalData.m_Textures.begin(); it != io_GlobalData.m_Textures.end(); ++it)
	{
		fsLocator texturePath = rmanUtil::LookupTexturePath(it->first,io_GlobalData);
		itString tiffFile = itString(rmanUtil::LookupGeneratedTexName(texturePath,io_GlobalData).c_str());
		tiffFile += itString(".tif");

		fsLocator tiffLoc = io_GlobalData.m_TiffLoc;
		tiffLoc.Push(tiffFile);

		// This texture already processed
		if ( !envSTLHelpers::Contains(l_WrittenTextures,it->first) )
		{
			// Rewrite assets - YES
			if ( io_GlobalData.m_Options.m_bRmanRewriteAssets )
			{
				matTextureMgr::SaveTextureToRgbaTiff( it->second.first , tiffLoc );
			}

			// Rewrite assets - NO
			else
			{
				// Write TIF if file does not exist
				if ( !fsFileUtil::FileExists( tiffLoc )  )
				{
					matTextureMgr::SaveTextureToRgbaTiff( it->second.first , tiffLoc );
				}
			}
		}

		l_WrittenTextures.push_back( it->first );
	}
}


//----------------------------------------------------------------------------
// SetupRendermanDirectories()
//----------------------------------------------------------------------------
void rmanMgr::SetupRendermanDirectories(const fsLocator& i_Root, rmanGlobalData & io_GlobalData)
{
	fsLocator tifDir = i_Root;
	fsLocator reflectionMapsDir = i_Root;
	fsLocator shadowMapsDir = i_Root;
	fsLocator texturesDir = i_Root;
	fsLocator archivesDir = i_Root;
	fsLocator photonMapDir = i_Root;

	tifDir.Push("base_textures");
	reflectionMapsDir.Push("reflection_maps");
	shadowMapsDir.Push("shadow_maps");
	texturesDir.Push("textures");
	archivesDir.Push("archives");
	photonMapDir.Push("photon_maps");

	fsFileUtil::CreateDirectory(tifDir);
	fsFileUtil::CreateDirectory(reflectionMapsDir);
	fsFileUtil::CreateDirectory(shadowMapsDir);
	fsFileUtil::CreateDirectory(texturesDir);
	fsFileUtil::CreateDirectory(archivesDir);
	//fsFileUtil::CreateDirectory(photonMapDir);

	io_GlobalData.m_TiffLoc = tifDir;
	io_GlobalData.m_TexturesLoc = texturesDir;
	io_GlobalData.m_ArchivesLoc = archivesDir;
	io_GlobalData.m_ShadowMapsLoc = shadowMapsDir;
	io_GlobalData.m_ReflectionMapsLoc = reflectionMapsDir;
	io_GlobalData.m_PhotonMapLoc = photonMapDir;
}

//----------------------------------------------------------------------------
// SetupRendermanBatch()
//----------------------------------------------------------------------------
fsLocator rmanMgr::SetupRendermanBatch(const fsLocator& i_Root, const rmanExportData &i_Data, 
		 					  fsLocator i_RibPath, std::string i_Renderer, rmanGlobalData & io_GlobalData )
{
	std::string frameName = io_GlobalData.m_FrameName;

	fsLocator batchFileName = i_Root;
	std::string batch = frameName;
	batch.append("_rman_batch.bat");

	batchFileName.Pop();
	batchFileName.Pop();
	batchFileName.Push(batch.c_str());
	itString itFileName;
	fsFileUtil::LocatorToUnicodeString( batchFileName, itFileName );
	std::ofstream batchFile( itFileName.GetString() , std::ios::out );

	bool skipTextures = false;

	bool skipShadowMaps = io_GlobalData.m_bRenderingNormalsOnly ||
						 (io_GlobalData.m_ProjectedLightNames.size() == 0);

	bool skipReflMaps = io_GlobalData.m_bRenderingAOOnly || 
						io_GlobalData.m_bRenderingNormalsOnly ||
						io_GlobalData.m_bRenderingIlluminationOnly ||
						io_GlobalData.m_bRenderingShadowsOnly ||
						(io_GlobalData.m_ReflObjNames.size() == 0);

	if ( !skipTextures )
	{
		FillRendermanBatchTex( io_GlobalData.m_Textures,
							   io_GlobalData.m_Engine,
							   batchFile,
							   RMAN_TEXTURE_MAP, io_GlobalData);
	}

	if ( !skipShadowMaps )
	{
		FillRendermanBatchRibs( io_GlobalData.m_ShadowMapRibs,
								io_GlobalData.m_Engine,
								batchFile,
								RMAN_SHADOW_MAP,
								io_GlobalData);
	}

	if ( !skipReflMaps )
	{
		FillRendermanBatchRibs( io_GlobalData.m_ReflCubeRibs,
								io_GlobalData.m_Engine,
								batchFile,
								RMAN_REFLECTIONCUBE_MAP,
								io_GlobalData);

		FillRendermanBatchRibs( io_GlobalData.m_ReflPlanarRibs,
								io_GlobalData.m_Engine,
								batchFile,
								RMAN_REFLECTIONPLANAR_MAP,
								io_GlobalData);
	}

	std::string fileName;
	fsFileUtil::LocatorToANSIFilename(i_RibPath,fileName);
	std::string execStart = "\"";
	execStart.append(i_Renderer).append("\" ");
	execStart.append( "-progress" ).append( GetSuppressedWarnings(io_GlobalData) ).append(" \"");
	execStart.append(fileName).append("\"");
	batchFile.write(execStart.c_str() , execStart.length());

	return batchFileName;
}

//----------------------------------------------------------------------------
// LaunchRendermanBatch()
//----------------------------------------------------------------------------
void rmanMgr::LaunchRendermanBatch( fsLocator i_Batch, bool i_ShowConsole )
{
	std::string fileName;
	fsFileUtil::LocatorToANSIFilename(i_Batch,fileName);
	std::string execStart = "\"";
	execStart.append(fileName).append("\"");
	WinExec(execStart.c_str(),i_ShowConsole);
}

//----------------------------------------------------------------------------
// WriteMasterBatch()
//----------------------------------------------------------------------------
fsLocator rmanMgr::WriteMasterBatch()
{
	std::vector<fsLocator> batchCollection = l_BatchFiles;
	fsLocator rootLoc = l_RootLoc;
	rootLoc.Push("RMan_Master_batch.bat");

	itString itFileName;
	fsFileUtil::LocatorToUnicodeString( rootLoc, itFileName );
	std::ofstream batchFile( itFileName.GetString() , std::ios::out );

	for ( int i = 0 ; i < batchCollection.size() ; i++ )
	{
		std::string currBatch;
		fsFileUtil::LocatorToANSIFilename( batchCollection[i] , currBatch );
		batchFile << "CALL \"" << currBatch.c_str() << "\"" << std::endl;
	}

	return rootLoc;
}

//----------------------------------------------------------------------------
// RendermanEntry()
//----------------------------------------------------------------------------
void rmanMgr::RendermanEntry( float i_AspectRatio, camCamera* i_Camera, g3dPrefs::g3dRenderPrefs i_RenderPrefs,
							   fsLocator i_FileName, itString i_CurrentScene, int i_Width, int i_Height,
							   int i_FilterFunc, float i_FilterWidth, int i_CaptureSampling,
							   const rmanOptionsData & i_Options,
							   bool & o_err, std::string & o_err_msg )
{
	fsLocator rootLoc = i_FileName;
	rootLoc.Pop();

	l_bRmanExportActive = true;
	l_RootLoc = rootLoc;

	itString currFrameNameIt = i_FileName.GetLastName();
	currFrameNameIt.StripExtension();
	std::string currFrameName = itStringUtil::GetStdString(currFrameNameIt);
	std::string sceneName = itStringUtil::GetStdString(i_CurrentScene);
	std::string frameNumber = sceneName + "_Frame_" + currFrameName.substr(currFrameName.rfind("_")+1,currFrameName.length()-1);

	std::string engine = "prman";
	std::string renderer = "";

	fsLocator prmanLoc = l_PrmanLoc;

	if ( prmanLoc.GetNumNames() == 0 )
	{
		o_err = true;
		o_err_msg = "No path to \"prman.exe\" has been specified in MachStudio preferences. Aborting.\n\n";
		return;
	} 
	if ( !fsFileUtil::FileExists( prmanLoc ) || prmanLoc.GetLastName() != itString("prman.exe") )
	{
		o_err = true;
		o_err_msg = "Correct path to \"prman.exe\" has not been specified in MachStudio preferences. Aborting.\n\n";
		return;
	}
	fsFileUtil::LocatorToANSIFilename( prmanLoc , renderer );

	fsLocator ribDirectory = i_FileName;
	ribDirectory.Pop();
	ribDirectory.Push("rman_assets");
	ribDirectory.Push(frameNumber.c_str());

	rmanGlobalData globalData;

	SetupRendermanDirectories(ribDirectory,globalData);

	rmanExportData myExportData = GetPotentialExportData(globalData);

	SetupRendermanGlobals( myExportData,
						   globalData,
						   i_RenderPrefs,
						   i_FilterFunc, 
						   i_FilterWidth, 
						   i_CaptureSampling,
						   i_Width,
						   i_Height,
						   i_AspectRatio,
						   i_Camera,
						   engine,
						   i_FileName,
						   i_Options,
						   currFrameName
						   );

	fsLocator worldLoc = globalData.m_ArchivesLoc;
	std::string worldFileName = globalData.m_FrameName + "_WORLD.rib";
	worldLoc.Push(worldFileName.c_str());

	rmanExporter exporter(globalData);

	DoExport( exporter , myExportData );

	WriteTiffTextures( globalData );

	fsLocator batchFile = SetupRendermanBatch(ribDirectory,myExportData,i_FileName,renderer,exporter.GetGlobalData());

	AddBatchFile( batchFile );
	
	o_err = false;
}