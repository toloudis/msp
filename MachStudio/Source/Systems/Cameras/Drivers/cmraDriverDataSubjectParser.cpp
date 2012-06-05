/********************************************************************************************\
**  cmraDriverDataSubjectParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverDataSubjectParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/name/nameMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//	Constants
	//------------------------------------------------------------------------
	const chDefs::Name c_CDSU = chDefs::MakeName('C', 'D', 'S', 'U');
	const chDefs::Name c_SUBI = chDefs::MakeName('S', 'U', 'B', 'I');

	const int l_SUBI_VERSION = 0;
	const int l_CDSU_VERSION = 0;

	//------------------------------------------------------------------------
	//   ReadSubjectInfo
	//------------------------------------------------------------------------
	void ReadSubjectInfo(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						cmraDriverDataSubjectInfo &o_Driver )
	{
		std::string name;
		int NumNames;

		chChunkParserUtil::Read(i_Reader, NumNames);
		o_Driver.m_ObjectNames.resize( NumNames );

		for ( int i = 0 ; i < NumNames ; i++ )
		{
			nameUID uid;
			chChunkParserUtil::Read(i_Reader, uid);
			chChunkParserUtil::Read(i_Reader, name);

			o_Driver.m_ObjectNames[i].SetString(name);
			o_Driver.m_ObjectNames[i].SetUID(uid);
		}
	}

	//------------------------------------------------------------------------
	//   WriteSubjectInfo
	//------------------------------------------------------------------------
	void WriteSubjectInfo(	chWriter& o_Writer,
							const cmraDriverDataSubjectInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_SUBI, l_SUBI_VERSION, false );

		chChunkParserUtil::Write(o_Writer, (envType::Int32)(i_Driver.m_ObjectNames.size()) );

		int NumNames = i_Driver.m_ObjectNames.size();

		for ( int i = 0 ; i < NumNames ; i++ )
		{
			chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectNames[i].GetUID() );
			chChunkParserUtil::Write(o_Writer, i_Driver.m_ObjectNames[i].GetString() );
		}

		o_Writer.FinishChunk();
	}

}	// local namespace


namespace cmraDriverDataSubjectParser
{
	//--------------------------------------------------------------------
	// Returns chunk name used by this parser
	//--------------------------------------------------------------------
	chDefs::Name GetChunkName()
	{
		return c_CDSU;
	}

	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	void Read(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				cmraDriverDataSubjectInfo& o_Driver )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_SUBI )
			{
				ReadSubjectInfo(i_Reader, version, size, o_Driver);
			}
			i_Reader.FinishChunk();
		}
	}

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	void Write( chWriter& o_Writer,
				const cmraDriverDataSubjectInfo& i_Driver )
	{
		o_Writer.WriteChunkHeader( c_CDSU, l_CDSU_VERSION, true );

		WriteSubjectInfo(o_Writer, i_Driver);

		o_Writer.FinishChunk();
	}

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	cmraDriverDataSubjectInfo* Create()
	{
		return new cmraDriverDataSubjectInfo();
	}
}



