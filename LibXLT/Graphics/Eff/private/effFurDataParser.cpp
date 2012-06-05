/*****************************************************************************
**	effFurDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effFurDataParser.hpp"

#include "Graphics/eff/effFurData.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_SFUR = chDefs::MakeName('S', 'F', 'U', 'R');
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effFurDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effFurData& data = dynamic_cast<effFurData &>(o_Shader);

	i_Reader.Read(data.m_TextureFolder);
	i_Reader.Read(data.m_NumShells);
	i_Reader.Read(data.m_LengthScale);
	chChunkParserUtil::Read(i_Reader, data.m_SpreadScale);
	i_Reader.Read(data.m_ShellFader);
	i_Reader.Read(data.m_bShowFins);
	i_Reader.Read(data.m_FinFader);
	i_Reader.Read(data.m_bColorSourcing);
	i_Reader.Read(data.m_bFurThinning);
	i_Reader.Read(data.m_bAnisotropic);
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effFurDataParser::Write( chWriter& o_Writer,
						const effShaderData& i_Shader ) const
{
	const effFurData& data = dynamic_cast<const effFurData &>(i_Shader);

	o_Writer.WriteChunkHeader( GetChunkName(), 0, false );

	o_Writer.Write(data.m_TextureFolder);
	o_Writer.Write(data.m_NumShells);
	o_Writer.Write(data.m_LengthScale);
	chChunkParserUtil::Write(o_Writer, data.m_SpreadScale);
	o_Writer.Write(data.m_ShellFader);
	o_Writer.Write(data.m_bShowFins);
	o_Writer.Write(data.m_FinFader);
	o_Writer.Write(data.m_bColorSourcing);
	o_Writer.Write(data.m_bFurThinning);
	o_Writer.Write(data.m_bAnisotropic);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effFurDataParser::Create() const
{
	return new effFurData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effFurDataParser::GetChunkName()
{
	return c_SFUR;
}
