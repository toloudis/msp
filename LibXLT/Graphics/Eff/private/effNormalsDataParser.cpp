/*****************************************************************************
**	effNormalsDataParser.cpp
**
**		effNormalsDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effNormalsDataParser.hpp"

#include "Graphics/eff/effNormalsData.hpp"

#include "Graphics/mat/matTexturePathUtil.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/prty/prtyFilePath.hpp"
#include "Core/prty/prtyTextureFileName.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_NMAP = chDefs::MakeName('N', 'M', 'A', 'P');

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void resolve_fullpath(fsLocator& io_Locator, const fsLocator& i_ContainingFile)
	{
		fsLocator tex_loc = io_Locator;
		if (matTexturePathUtil::ResolveFullPath(tex_loc, i_ContainingFile,
			matShaderParser::GetAllowSingleFilenameTextures(), matShaderParser::GetResolveAbsolutePaths()))
		{
			io_Locator = tex_loc;
		}
	}

}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effNormalsDataParser::Read(chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effNormalsData& data = dynamic_cast<effNormalsData &>(o_Shader);

	prtyTextureFileName prtyNormalMap;
	prtyNormalMap.ReadTexture(i_Reader);

	fsLocator locNormalMap = prtyNormalMap.GetValue();
	resolve_fullpath(locNormalMap, i_Reader.GetLocator());
	data.m_NameNormalMap = locNormalMap;

	i_Reader.Read(data.m_BumpScale);
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effNormalsDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effNormalsData& data = dynamic_cast<const effNormalsData &>(i_Shader);

	o_Writer.WriteChunkHeader( GetChunkName(), 1, false );

	prtyTextureFileName normalMap("", data.m_NameNormalMap);
	normalMap.WriteTexture(o_Writer);

	o_Writer.Write(data.m_BumpScale);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effNormalsDataParser::Create() const
{
	return new effNormalsData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effNormalsDataParser::GetChunkName()
{
	return c_NMAP;
}
