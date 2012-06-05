/********************************************************************************************\
**  ProjectSetupDataParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Features/ProjectSetup/Data/ProjectSetupDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


//============================================================================
//============================================================================
namespace ProjectSetupDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_PSUD = chDefs::MakeName('P', 'S', 'U', 'D');	// Project SetUp Data
const chDefs::Name c_PSUI = chDefs::MakeName('P', 'S', 'U', 'I');	//	+-- Project SetUp Info
const chDefs::Name c_SCNL = chDefs::MakeName('S', 'C', 'N', 'L');	//	+-- SCeNe List
const chDefs::Name c_SCND = chDefs::MakeName('S', 'C', 'N', 'D');	//	  +-- SCeNe Data



//------------------------------------------------------------------------
//   ReadSceneData
//------------------------------------------------------------------------
void ReadSceneData(	chReader& io_Reader,
					chDefs::Version i_Version,
					chDefs::Size i_Size,
					ProjectSetupSceneData& o_Data )
{
	chChunkParserUtil::Read( io_Reader, o_Data.m_SceneName );
	chChunkParserUtil::Read( io_Reader, o_Data.m_SceneDesc );
}

//------------------------------------------------------------------------
//   WriteSceneData
//------------------------------------------------------------------------
void WriteSceneData(	chWriter& o_Writer,
						const ProjectSetupSceneData& i_Data )
{
	const int l_cSCND_VERSION = 0;
	o_Writer.WriteChunkHeader( c_SCND, l_cSCND_VERSION, false );

	chChunkParserUtil::Write( o_Writer, i_Data.m_SceneName );
	chChunkParserUtil::Write( o_Writer, i_Data.m_SceneDesc );

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void ReadSceneListData( chReader& io_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						ProjectSetupData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	if (i_Version == 0)
	{
		// Earlier version wrote number of scenes
		int entries;
		chChunkParserUtil::Read( io_Reader, entries );
	}

	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_SCND )
		{
			ProjectSetupSceneData scene_data;
			ReadSceneData( io_Reader, version, size, scene_data );
			if (!scene_data.m_SceneName.empty())
				o_Data.m_Scenes.push_back(scene_data);
		}
		io_Reader.FinishChunk();
	}
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void WriteSceneListData( chWriter& io_Writer,
						const ProjectSetupData& i_Data )
{
	//	Write List
	const int l_cSCNL_VERSION = 1;
	io_Writer.WriteChunkHeader( c_SCNL, l_cSCNL_VERSION, true );

	// Version 1 doesn't write number of scenes anymore
	// (shouldn't mix numbers and chunks within one chunk)
	//chChunkParserUtil::Write( io_Writer, (envType::Int32)(i_Data.m_Scenes.size()) );

	//
	for (int i=0; i<i_Data.m_Scenes.size(); i++)
	{
		WriteSceneData( io_Writer, i_Data.m_Scenes[i] );
	}

	io_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadProjectData
//------------------------------------------------------------------------
void ReadProjectData(	chReader& io_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						ProjectSetupData& o_Data )
{
	chChunkParserUtil::Read( io_Reader, o_Data.m_ProjectName );
	chChunkParserUtil::Read( io_Reader, o_Data.m_ProjectDesc );
	chChunkParserUtil::Read( io_Reader, o_Data.m_ProjectDirectory );

	o_Data.m_bDirty = false;
}

//------------------------------------------------------------------------
//   WriteProjectData
//------------------------------------------------------------------------
void WriteProjectData(	chWriter& o_Writer,
						const ProjectSetupData& i_Data )
{
	const int l_cPSUI_VERSION = 0;
	o_Writer.WriteChunkHeader( c_PSUI, l_cPSUI_VERSION, false );

	chChunkParserUtil::Write( o_Writer, i_Data.m_ProjectName );
	chChunkParserUtil::Write( o_Writer, i_Data.m_ProjectDesc );
	chChunkParserUtil::Write( o_Writer, i_Data.m_ProjectDirectory );

	o_Writer.FinishChunk();
}

}	// local namespace

//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_PSUD;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				ProjectSetupData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	//DBG_LOG( "Reading the data" );
	i_Reader.ReadChunkHeader(name, version, size);

	DBG_ASSERT( (name == c_PSUD), "Invalid chunk for project setup file" );

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		//DBG_LOG2( "reading chunk %x %x", name, c_PSUD );

		if ( name == c_SCNL )
		{
			ReadSceneListData( i_Reader, version, size, o_Data );
		}
		else if ( name == c_PSUI )
		{
			ReadProjectData( i_Reader, version, size, o_Data );
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const ProjectSetupData& i_Data )
{
	const int l_cPSUD_VERSION = 1;
	o_Writer.WriteChunkHeader( c_PSUD, l_cPSUD_VERSION, true );

	WriteProjectData( o_Writer, i_Data );
	WriteSceneListData( o_Writer, i_Data );

	o_Writer.FinishChunk();
}


}	// end of namespace

