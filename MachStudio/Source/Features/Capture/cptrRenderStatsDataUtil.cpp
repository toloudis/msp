/*****************************************************************************
**	cptrRenderStatsDataUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderStatsDataUtil.hpp"

#include "Features/Capture/cptrRenderStatsDialogUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/app/appTime.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"

#include <sstream>


//============================================================================
//============================================================================
namespace cptrRenderStatsDataUtil
{
	namespace
	{
		const char* lc_RenderStats_FileName = "RenderStats.cfg";
		const char* lc_RenderStats_Group	= "RenderStats";
		const char* lc_RenderStats_Opened	= "Opened";

		cptrRenderStatsData	l_CSData;
	};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrRenderStatsData& Data()
{
	return l_CSData;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ClearAllData()
{
	l_CSData.m_Scenes.resize(0);
	l_CSData.m_CurrentSceneIndex = -1;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void StartBatch( std::string& i_BatchFilename )
{
	ClearAllData();

	l_CSData.m_BatchName	= i_BatchFilename;
	l_CSData.m_fStartTime	= appTime::GetTime();
	l_CSData.m_CurrentSceneIndex = -1;

	//DBG_WARNING1("Start Batch (%s)", i_BatchFilename.c_str());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void EndBatch()
{
	if ( l_CSData.m_Scenes.size() > 0 )
	{
		EndScene();
	}

	l_CSData.m_fEndTime = appTime::GetTime();

	//DBG_WARNING0("End Batch");
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void StartScene( std::string& i_SceneName )
{
	//	automatically end the last scene
	//
	if ( l_CSData.m_Scenes.size() > 0 )
	{
		EndScene();
	}
	else
	{
		//	if this isn't a batch, set the start time to the first scene so
		//	the render will start at 0
		//
		if ( l_CSData.m_fStartTime == 0 )
		{
			l_CSData.m_fStartTime	= appTime::GetTime();
		}
	}

	l_CSData.m_CurrentSceneIndex = l_CSData.m_Scenes.size();
	int resize_index = l_CSData.m_CurrentSceneIndex + 1;
	l_CSData.m_Scenes.resize( resize_index );

	int csi = l_CSData.m_CurrentSceneIndex;
	l_CSData.m_Scenes[csi].m_SceneName	= i_SceneName;
	l_CSData.m_Scenes[csi].m_fStartTime	= appTime::GetTime();
	l_CSData.m_Scenes[csi].m_CurrentCameraIndex = -1;
	l_CSData.m_Scenes[csi].m_CurrentLayerIndex = -1;

	//DBG_WARNING1("Start Scene %d", csi );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void EndScene()
{
	EndCamera();

	int csi = l_CSData.m_CurrentSceneIndex;

	l_CSData.m_Scenes[csi].m_fEndTime = appTime::GetTime();

	//DBG_WARNING1("End Scene %d", csi );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void StartCamera( std::string& i_CameraName, std::string& i_CameraDesc )
{
	int csi = l_CSData.m_CurrentSceneIndex;

	//	automatically end the last camera
	if ( l_CSData.m_Scenes[csi].m_Cameras.size() > 0 )
	{
		EndCamera();
		//l_CSData.m_Scenes[csi].m_Cameras[cci-1].m_fEndTime	= appTime::GetTime();
	}

	l_CSData.m_Scenes[csi].m_CurrentCameraIndex = l_CSData.m_Scenes[csi].m_Cameras.size();
	int resize_index = l_CSData.m_Scenes[csi].m_Cameras.size() + 1;
	l_CSData.m_Scenes[csi].m_Cameras.resize(resize_index);

	int cci = l_CSData.m_Scenes[csi].m_CurrentCameraIndex;

	l_CSData.m_Scenes[csi].m_Cameras[cci].m_CameraName	= i_CameraName;
	l_CSData.m_Scenes[csi].m_Cameras[cci].m_CameraDesc	= i_CameraDesc;
	l_CSData.m_Scenes[csi].m_Cameras[cci].m_fStartTime	= appTime::GetTime();
	l_CSData.m_Scenes[csi].m_Cameras[cci].m_CurrentCameraRenderBlockIndex = -1;

	std::string camname = l_CSData.m_Scenes[csi].m_Cameras[cci].m_CameraName;
	std::string camdesc = l_CSData.m_Scenes[csi].m_Cameras[cci].m_CameraDesc;
	//DBG_LOG4("=----> Start Camera %d-%d (%s)[%s]", csi, cci, camname.c_str(), camdesc.c_str());

	//DBG_WARNING1("Start Camera %d", cci );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void EndCamera()
{
	EndCameraRenderBlock();

	int csi = l_CSData.m_CurrentSceneIndex;
	int cci = l_CSData.m_Scenes[csi].m_CurrentCameraIndex;

	l_CSData.m_Scenes[csi].m_Cameras[cci].m_fEndTime	= appTime::GetTime();

	//DBG_WARNING1("End Camera %d", cci );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void StartCameraRenderBlock( std::string& i_CameraRenderBlockName )
{
	int csi = l_CSData.m_CurrentSceneIndex;
	int cci = l_CSData.m_Scenes[csi].m_CurrentCameraIndex;

	//	automatically end the last block
	if ( l_CSData.m_Scenes[csi].m_Cameras[cci].m_RenderBlocks.size() > 0 )
	{
		EndCameraRenderBlock();
		//l_CSData.m_Scenes[csi].m_Cameras[cci].m_RenderBlocks[cbi-1].m_fEndTime	= appTime::GetTime();
	}

	l_CSData.m_Scenes[csi].m_Cameras[cci].m_CurrentCameraRenderBlockIndex = l_CSData.m_Scenes[csi].m_Cameras[cci].m_RenderBlocks.size();
	l_CSData.m_Scenes[csi].m_Cameras[cci].m_RenderBlocks.resize( l_CSData.m_Scenes[csi].m_Cameras[cci].m_CurrentCameraRenderBlockIndex+1 );

	int cbi = l_CSData.m_Scenes[csi].m_Cameras[cci].m_CurrentCameraRenderBlockIndex;

	l_CSData.m_Scenes[csi].m_Cameras[cci].m_RenderBlocks[cbi].m_CameraRenderBlockName = i_CameraRenderBlockName;
	l_CSData.m_Scenes[csi].m_Cameras[cci].m_RenderBlocks[cbi].m_fStartTime = appTime::GetTime();

	//DBG_WARNING1("Start RenderBlock %d", cbi );
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void EndCameraRenderBlock()
{
	int csi = l_CSData.m_CurrentSceneIndex;
	int cci = l_CSData.m_Scenes[csi].m_CurrentCameraIndex;
	int cbi = l_CSData.m_Scenes[csi].m_Cameras[cci].m_CurrentCameraRenderBlockIndex;

	if ( l_CSData.m_Scenes[csi].m_Cameras[cci].m_RenderBlocks.size() == 0)
		return;
	//DBG_WARNING1("End RenderBlock %d", cbi );

	l_CSData.m_Scenes[csi].m_Cameras[cci].m_RenderBlocks[cbi].m_fEndTime = appTime::GetTime();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void StartRenderLayer(std::string& i_LayerName)
{
	int csi = l_CSData.m_CurrentSceneIndex;

	//	automatically end the last camera
	if ( l_CSData.m_Scenes[csi].m_Layers.size() > 0 )
	{
		//EndRenderLayer();
	}

	l_CSData.m_Scenes[csi].m_CurrentLayerIndex = l_CSData.m_Scenes[csi].m_Layers.size();
	int resize_index = l_CSData.m_Scenes[csi].m_Layers.size() + 1;
	l_CSData.m_Scenes[csi].m_Layers.resize(resize_index);

	int cli = l_CSData.m_Scenes[csi].m_CurrentLayerIndex;

	l_CSData.m_Scenes[csi].m_Layers[cli].m_LayerName	= i_LayerName;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void EndRenderLayer()
{

}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void add_start_and_end_time( float i_fStartTime, float i_fEndTime, std::string& o_OutputString )
{
	
	std::string buffer;
	std::string time_string;
	//	format this time in HMSM always since it is ELAPSED TIME and not timeline time
	//	always subtract the starting time so the time's start time is "0"
	//
	std::ostringstream oss;
	tmlnTimeUtil::GetTimeStringInHMSM( maTime::FromSeconds(i_fStartTime - l_CSData.m_fStartTime), time_string );
	oss <<"start "<<time_string<<"\n";
	buffer = oss.str();
	//sprintf( buffer, "start %s\r\n", time_string.c_str() );
	if ( i_fEndTime > i_fStartTime )
	{
		o_OutputString += buffer;
		tmlnTimeUtil::GetTimeStringInHMSM(maTime::FromSeconds(i_fEndTime - l_CSData.m_fStartTime), time_string);
		//sprintf( buffer, "end %s\r\n", time_string.c_str() );
		oss << "end "<<time_string<<"\n";
		//buffer += oss.str().c_str();
		
		o_OutputString += buffer;
		tmlnTimeUtil::GetTimeStringInHMSM(maTime::FromSeconds(i_fEndTime - i_fStartTime), time_string);
		//sprintf( buffer, "duration %s\r\n", time_string.c_str() );
		oss << "duration "<<time_string<<"\n";
		buffer += oss.str();

	}
	o_OutputString += buffer;
}

//------------------------------------------------------------------------
//	Build an output string based on all the gathered data.
//------------------------------------------------------------------------
void BuildOutputString( std::string& o_OutputString )
{
	std::string buffer;
	int sloop, cloop;
	//int rloop;

	strcpy( (char*)buffer.c_str(), "Render Log\r\n" );
	o_OutputString += buffer;
	std::stringstream oss;
	//	batch info
	//
	if (l_CSData.m_BatchName.length() > 0)
	{
		//sprintf( buffer, "Batch Filename %s\r\n", l_CSData.m_BatchName.c_str() );
		oss << "Batch Filename "<<l_CSData.m_BatchName <<"\n";
		buffer += oss.str();
		o_OutputString += buffer;

		add_start_and_end_time( l_CSData.m_fStartTime, l_CSData.m_fEndTime, o_OutputString );
	}

	int csi = l_CSData.m_CurrentSceneIndex;
	if ( csi < 0 ) return;
	//int cci = l_CSData.m_Scenes[csi].m_CurrentCameraIndex;
	//if ( cci < 0 ) return;
	//int cbi = l_CSData.m_Scenes[csi].m_Cameras[cci].m_CurrentCameraRenderBlockIndex;
	//if ( cbi < 0 ) return;

	//	scene/camera/render block info
	//
	int num_scenes = l_CSData.m_Scenes.size();
	for (sloop = 0; sloop < num_scenes ; ++sloop )
	{
		oss << "\r\nScene "<<l_CSData.m_Scenes[sloop].m_SceneName<<" \r\n";
		//sprintf( buffer, "\r\nScene %s\r\n", l_CSData.m_Scenes[sloop].m_SceneName.c_str() );
		buffer += oss.str();
		o_OutputString += buffer;
		add_start_and_end_time( l_CSData.m_Scenes[sloop].m_fStartTime, l_CSData.m_Scenes[sloop].m_fEndTime, o_OutputString );
		//DBG_LOG( buffer );

		int num_cameras = l_CSData.m_Scenes[sloop].m_Cameras.size();
		//DBG_LOG3("stats for scene [%s] %d cameras=%d", l_CSData.m_Scenes[sloop].m_SceneName.c_str(), num_scenes, num_cameras);

		for (cloop = 0; cloop < num_cameras ; ++cloop )
		{
			//DBG_LOG("Camera #" << cloop);
			std::string camname = l_CSData.m_Scenes[sloop].m_Cameras[cloop].m_CameraName;
			std::string camdesc = l_CSData.m_Scenes[sloop].m_Cameras[cloop].m_CameraDesc;
			//sprintf( buffer, "\r\nCamera %s - %s\r\n",  camname.c_str(),
			//											camdesc.c_str());
			
			oss <<"\r\nCamera "<<	camname<<" - " << camdesc <<"\r\n";
			buffer += oss.str();
			o_OutputString += buffer;

			int layer_index = l_CSData.m_Scenes[sloop].m_CurrentLayerIndex;
			std::string layer_name = l_CSData.m_Scenes[sloop].m_Layers[cloop].m_LayerName;
			std::stringstream layer_stream;
			layer_stream << "Render Layer - " << layer_name << "\r\n";
			o_OutputString += layer_stream.str();

			add_start_and_end_time( l_CSData.m_Scenes[sloop].m_Cameras[cloop].m_fStartTime, l_CSData.m_Scenes[sloop].m_Cameras[cloop].m_fEndTime, o_OutputString );
			//DBG_LOG( buffer );

			//int num_blocks = l_CSData.m_Scenes[csi].m_Cameras[cci].m_RenderBlocks.size();
			//for (rloop = 0; rloop < num_blocks ; ++rloop )
			//{
			//	sprintf( buffer, "\r\nRender Block %s\r\n", l_CSData.m_Scenes[sloop].m_Cameras[cloop].m_RenderBlocks[rloop].m_CameraRenderBlockName.c_str() );
			//	o_OutputString += buffer;
			//	add_start_and_end_time( l_CSData.m_Scenes[sloop].m_Cameras[cloop].m_RenderBlocks[rloop].m_fStartTime, 
			//							l_CSData.m_Scenes[sloop].m_Cameras[cloop].m_RenderBlocks[rloop].m_fEndTime, 
			//							o_OutputString );
			//}
		}
	}

	//strcpy( buffer, "\r\nEnd of Render Stats\r\n" );
	//o_OutputString += buffer;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ReadPrefsFromConfigFile(fsLocator& i_ConfigFile, cptrRenderStatsData& o_Data)
{
	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

	guiXMLTextReader::Open(cfgpath.c_str());

	//	read in the preferences
	//
	guiXMLTextReader::gui_Node_Type node_type;
	std::string keyname, strvalue;
	while ((node_type = guiXMLTextReader::ReadNode(keyname,strvalue)) != guiXMLTextReader::e_EOF)
	{
		if ((node_type == guiXMLTextReader::e_Text) && (keyname.length() > 0))
		{
			if (keyname == lc_RenderStats_Opened)	
				{bool value; guiXMLTextReader::Convert(strvalue, value); o_Data.m_bOpenOnRender.SetValue(value);};
		}
	}

	guiXMLTextReader::Close();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ReadRenderStats(cptrRenderStatsData& o_Data)
{
	//
	//	main stats
	//
	//l_bReadingData = true;
	fsLocator cfgdir;
	cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	cfgdir.Push( lc_RenderStats_FileName );

	if (fsFileUtil::FileExists(cfgdir))
	{
		ReadPrefsFromConfigFile(cfgdir, o_Data);
	}
	//l_bReadingData = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void WritePrefsToConfigFile(const fsLocator i_ConfigFile, const cptrRenderStatsData& i_Data)
{
	//	if the code is in the middle of reading the file, don't allow writing.
	//if (l_bReadingData)
	//	return;

	std::string cfgpath;
	fsFileUtil::LocatorToANSIFilename(i_ConfigFile,cfgpath);

	//	open and start the file
	guiXMLTextWriter::Open(cfgpath.c_str());
	guiXMLTextWriter::WriteStartElement(lc_RenderStats_Group);

	//	write the actual values
	guiXMLTextWriter::WriteElement(lc_RenderStats_Opened, i_Data.m_bOpenOnRender.GetValue());

	//	finish it up
	guiXMLTextWriter::WriteEndElement();
	guiXMLTextWriter::Close();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void WriteRenderStats(const cptrRenderStatsData& i_Data)
{
	std::string cfgpath;
	fsLocator cfgdir;
	cfgdir = gfPaths::GetPath(mnmPaths::e_Configs);
	cfgdir.Push( lc_RenderStats_FileName );

	WritePrefsToConfigFile(cfgdir, i_Data);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool IsRenderStatsVisible()
{
	return l_CSData.m_bOpenOnRender.GetValue();
}

//------------------------------------------------------------------------
//	set the data component
//------------------------------------------------------------------------
void SetRenderStatsVisible(bool i_bShowing)
{
	l_CSData.m_bOpenOnRender.SetValue(i_bShowing);
}

//------------------------------------------------------------------------
//  AddToMenu() - add cptrRenderStats actions to menus
//------------------------------------------------------------------------
void  AddToMenu()
{
	//
	//	commands
	//
	int menu_id;
	cmaCommand* pCmd;

	//	COMMAND: Render Log window
	pCmd = new cmaCommandSimple("Render Log", 
								"Render", 
								"View the Render Log Window",
								&cptrRenderStatsDialogUtil::Show );
	menu_id = guiMenuMgr::AddMenuItem( "Render", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	cmmSystemDialogUtil::AddSystemCommand( "Render", "Render Log", pCmd );
}

}	// end of namespace cptrRenderStatsDataUtil
