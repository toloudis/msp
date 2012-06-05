/********************************************************************************************\
**  SceneSetupDataParser.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#include "Features/SceneSetup/Data/SceneSetupDataParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Support/tmln/tmlnParser.hpp"


namespace SceneSetupDataParser
{

namespace
{
//------------------------------------------------------------------------
//------------------------------------------------------------------------
const chDefs::Name c_SPDA = chDefs::MakeName('S', 'P', 'D', 'A');	// main data
const chDefs::Name c_SPID = chDefs::MakeName('S', 'P', 'I', 'D');	// +-- project data
const chDefs::Name c_SCND = chDefs::MakeName('S', 'C', 'N', 'D');	// +-- scene data


//------------------------------------------------------------------------
//   ReadSceneProjectData
//------------------------------------------------------------------------
void ReadSceneProjectData(	chReader& io_Reader,
							chDefs::Version i_Version,
							SceneSetupProjectInfo& o_Data )
{
	chChunkParserUtil::Read( io_Reader, o_Data.m_ProjectName );
	chChunkParserUtil::Read( io_Reader, o_Data.m_ProjectDirectory );

	if ( i_Version == 0 )
	{
		std::string junk;
		chChunkParserUtil::Read( io_Reader, junk );					// location name
		chChunkParserUtil::Read( io_Reader, junk );					// location desc
		chChunkParserUtil::Read( io_Reader, o_Data.m_SceneName );
		chChunkParserUtil::Read( io_Reader, junk );					// scene desc
	}
	else
	{
		chChunkParserUtil::Read( io_Reader, o_Data.m_SceneName );
	}
}

//------------------------------------------------------------------------
//   WriteSceneProjectData
//------------------------------------------------------------------------
void WriteSceneProjectData(	chWriter& o_Writer,
							const SceneSetupProjectInfo& i_Data )
{
	const int l_cSPID_VERSION = 1;
	o_Writer.WriteChunkHeader( c_SPID, l_cSPID_VERSION, false );

	chChunkParserUtil::Write( o_Writer, i_Data.m_ProjectName );
	chChunkParserUtil::Write( o_Writer, i_Data.m_ProjectDirectory );
	chChunkParserUtil::Write( o_Writer, i_Data.m_SceneName );

	std::string pdir;
	DBG_LOG0("writing----PROJECT SETTINGS-----------");
	DBG_LOG1("Scene Project Directory (%s)", i_Data.m_ProjectDirectory.c_str() );
	DBG_LOG1("Scene Project Name      (%s)", i_Data.m_ProjectName.c_str() );
	DBG_LOG1("Scene Name              (%s)", i_Data.m_SceneName.c_str() );

	o_Writer.FinishChunk();
}

//------------------------------------------------------------------------
//   ReadSceneData
//------------------------------------------------------------------------
void ReadSceneData(	chReader& io_Reader,
				    chDefs::Version i_Version,
					ScenePropertiesData& o_Data )
{
	chChunkParserUtil::Read( io_Reader, o_Data.m_Notes );
}

//------------------------------------------------------------------------
//   WriteSceneData
//------------------------------------------------------------------------
void WriteSceneData(	chWriter& o_Writer,
						const ScenePropertiesData& i_Data )
{
	const int l_cSCND_VERSION = 0;
	o_Writer.WriteChunkHeader( c_SCND, l_cSCND_VERSION, false );

	chChunkParserUtil::Write( o_Writer, i_Data.m_Notes );

	o_Writer.FinishChunk();
}

}	// local namespace


//------------------------------------------------------------------------
//  returns chunk name for this data type
//------------------------------------------------------------------------
chDefs::Name  GetChunkName()
{
	return c_SPDA;
}

//------------------------------------------------------------------------
//   ReadData
//------------------------------------------------------------------------
void ReadData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				SceneSetupData& o_Data )
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == c_SPID )
		{
			ReadSceneProjectData( i_Reader, version, o_Data.m_Data );
		}
		else if ( name == c_SCND )
		{
			ReadSceneData( i_Reader, version, o_Data.m_PropertiesData );
		}

		i_Reader.FinishChunk();
	}
}

//------------------------------------------------------------------------
//   WriteData
//------------------------------------------------------------------------
void WriteData(	chWriter& o_Writer,
				const SceneSetupData& i_Data )
{
	const int l_cSPDA_VERSION = 1;
	o_Writer.WriteChunkHeader( c_SPDA, l_cSPDA_VERSION, true );

	WriteSceneProjectData( o_Writer, i_Data.m_Data );
	WriteSceneData( o_Writer, i_Data.m_PropertiesData );

	o_Writer.FinishChunk();
}

}	// end of namespace

