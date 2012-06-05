/********************************************************************************************\
**  cmraDriverRenderPassParser.cpp
**
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverRenderPassParser.hpp"

#include "Systems/Cameras/Drivers/cmraDriverRenderPassInfo.hpp"

#include "Support/tmln/tmlnDriverInfoParser.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ch/chChunkParserUtil.hpp"


namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_RPFF = chDefs::MakeName('R', 'P', 'F', 'F');	//cmra RenderPassFromFile driver
	const chDefs::Name c_RPDR = chDefs::MakeName('R', 'P', 'D', 'R');	//cmra RenderPassFromFile entire driver

	//========================================================================
	//   ReadRenderPassInfo
	//========================================================================
	void ReadRenderPassInfo(	chReader& i_Reader,
							chDefs::Version i_Version,
							chDefs::Size i_Size,
							cmraDriverRenderPassInfo &o_Info 
							)
	{
		// texture
		itString savefilename;
		fsLocator texture_locator;
		chChunkParserUtil::Read( i_Reader, savefilename );
		fsFileUtil::UnicodeStringToLocator( savefilename, texture_locator );
		prtyTextureFileData val;
		val = o_Info.m_FirstTextureFileName.GetFullValue();
		val.m_TextureLocator = texture_locator;
		o_Info.m_FirstTextureFileName.SetValue(val);

		// numbers
		chChunkParserUtil::Read(i_Reader, o_Info.m_bEnable);
		chChunkParserUtil::Read(i_Reader, o_Info.m_BlendIntensity);
		chChunkParserUtil::Read(i_Reader, o_Info.m_BlendOp);
		chChunkParserUtil::Read(i_Reader, o_Info.m_NumberOfFrames);
		chChunkParserUtil::Read(i_Reader, o_Info.m_RenderPass);
	}

	//========================================================================
	//   WriteRenderPassInfo
	//========================================================================
	void WriteRenderPassInfo( chWriter& o_Writer,
							const cmraDriverRenderPassInfo &i_Info )
	{
		const int c_RPFF_version = 0;
		o_Writer.WriteChunkHeader( c_RPFF, c_RPFF_version, false );

		// texture
		itString savefilename;
		fsFileUtil::LocatorToUnicodeString( i_Info.m_FirstTextureFileName.GetValue(), savefilename );
		chChunkParserUtil::Write( o_Writer, savefilename );

		// numbers
		chChunkParserUtil::Write(o_Writer, i_Info.m_bEnable);
		chChunkParserUtil::Write(o_Writer, i_Info.m_BlendIntensity);
		chChunkParserUtil::Write(o_Writer, i_Info.m_BlendOp);
		chChunkParserUtil::Write(o_Writer, i_Info.m_NumberOfFrames);
		chChunkParserUtil::Write(o_Writer, i_Info.m_RenderPass);
		
		o_Writer.FinishChunk();
	}

}	// local namespace

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
cmraDriverRenderPassParser::cmraDriverRenderPassParser()
{
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
cmraDriverRenderPassParser::~cmraDriverRenderPassParser()
{
}

//--------------------------------------------------------------------
// Returns chunk name used by this parser
//--------------------------------------------------------------------
//static
chDefs::Name cmraDriverRenderPassParser::GetChunkName()
{
	return c_RPDR;
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void cmraDriverRenderPassParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									tmlnDriverInfo& o_Driver ) const
{
	cmraDriverRenderPassInfo &driver = dynamic_cast<cmraDriverRenderPassInfo &>(o_Driver);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if ( name == tmlnDriverInfoParser::GetChunkName() )
		{
			tmlnDriverInfoParser::ReadDriverInfo(i_Reader, version, size, driver);
		}
		else if ( name == c_RPFF )
		{
			ReadRenderPassInfo(i_Reader, version, size, driver );
		}
		i_Reader.FinishChunk();
	}
}

//--------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Driver is the object being written.
//--------------------------------------------------------------------
void cmraDriverRenderPassParser::Write( chWriter& o_Writer,
									 const tmlnDriverInfo& i_Driver ) const
{
	const cmraDriverRenderPassInfo &driver = dynamic_cast<const cmraDriverRenderPassInfo &>(i_Driver);

	const int c_RPDR_VERSION = 0;
	o_Writer.WriteChunkHeader( c_RPDR, c_RPDR_VERSION, true );

	tmlnDriverInfoParser::WriteDriverInfo( o_Writer, driver );

	WriteRenderPassInfo( o_Writer, driver );

	o_Writer.FinishChunk();
}

//--------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverRenderPassParser::Create() const
{
	return new cmraDriverRenderPassInfo(c_RPDR);
}


