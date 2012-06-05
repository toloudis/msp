/********************************************************************************************\
**  cmraDriverDataViewTypeParser.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverDataViewTypeParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//	Constants
	//------------------------------------------------------------------------
	const chDefs::Name c_CDVT = chDefs::MakeName('C', 'D', 'V', 'T');
	const chDefs::Name c_VTYP = chDefs::MakeName('V', 'T', 'Y', 'P');

	const int l_CDVT_VERSION = 0;
	const int l_VTYP_VERSION = 0;

	//------------------------------------------------------------------------
	//   ReadViewTypeInfo
	//------------------------------------------------------------------------
	void ReadViewTypeInfo(chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						cmraDriverDataViewTypeInfo &o_Driver )
	{
		chChunkParserUtil::Read(i_Reader, o_Driver.m_ViewType);
		chChunkParserUtil::Read(i_Reader, o_Driver.m_fViewTolerance);
	}

	//------------------------------------------------------------------------
	//   WriteViewTypeInfo
	//------------------------------------------------------------------------
	void WriteViewTypeInfo(	chWriter& o_Writer,
							const cmraDriverDataViewTypeInfo &i_Driver )
	{
		o_Writer.WriteChunkHeader( c_VTYP, l_VTYP_VERSION, false );

		chChunkParserUtil::Write(o_Writer, i_Driver.m_ViewType);
		chChunkParserUtil::Write(o_Writer, i_Driver.m_fViewTolerance);

		o_Writer.FinishChunk();
	}

}	// local namespace


namespace cmraDriverDataViewTypeParser
{
	//--------------------------------------------------------------------
	// Returns chunk name used by this parser
	//--------------------------------------------------------------------
	chDefs::Name cmraDriverDataViewTypeParser::GetChunkName()
	{
		return c_CDVT;
	}

	//--------------------------------------------------------------------
	//	Read is called by a parser utility when the chunk name has been read
	//  from the data file.  It will parse from the chunk into the given
	//  parsable object.
	//--------------------------------------------------------------------
	void Read(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				cmraDriverDataViewTypeInfo& o_Driver )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_VTYP )
			{
				ReadViewTypeInfo(i_Reader, version, size, o_Driver);
			}
			i_Reader.FinishChunk();
		}
	}

	//--------------------------------------------------------------------
	//	Write is called to parse data from the given object to the given
	//	chWriter.  i_Driver is the object being written.
	//--------------------------------------------------------------------
	void Write(	chWriter& o_Writer,
				const cmraDriverDataViewTypeInfo& i_Driver )
	{
		o_Writer.WriteChunkHeader( c_CDVT, l_CDVT_VERSION, true );

		WriteViewTypeInfo(o_Writer, i_Driver);

		o_Writer.FinishChunk();
	}

	//--------------------------------------------------------------------
	// Create should be overridden to create the user specific parsable object.
	// Users of parsers should call create to create the object that is then
	// passed to Read.
	//--------------------------------------------------------------------
	cmraDriverDataViewTypeInfo* Create()
	{
		return new cmraDriverDataViewTypeInfo();
	}
}

