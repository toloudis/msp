/*****************************************************************************
**	effRendermanOverrideDataParser.cpp
**
**		effRendermanOverrideDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effRendermanOverrideDataParser.hpp"

#include "Graphics/eff/effRendermanOverrideData.hpp"

#include "Graphics/mat/matTexturePathUtil.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyFilePath.hpp"
#include "Core/prty/prtyTextureFileName.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_RMOV = chDefs::MakeName('R', 'M', 'O', 'V');

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
void effRendermanOverrideDataParser::Read(chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effRendermanOverrideData& data = dynamic_cast<effRendermanOverrideData &>(o_Shader);

	prtyFilePath shaderLoc;
	shaderLoc.Read(i_Reader);

	fsLocator locShaderLocation = shaderLoc.GetValue();
	resolve_fullpath(locShaderLocation, i_Reader.GetLocator());
	data.m_ShaderLocation = locShaderLocation;

	i_Reader.Read(data.m_ParamList);
	i_Reader.Read(data.m_AttributeList);
	i_Reader.Read(data.m_bOverrideShader);
	i_Reader.Read(data.m_bOverrideAttributes);
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effRendermanOverrideDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effRendermanOverrideData& data = dynamic_cast<const effRendermanOverrideData &>(i_Shader);

	o_Writer.WriteChunkHeader( GetChunkName(), 1, false );

	prtyFilePath shaderLoc("", data.m_ShaderLocation);
	shaderLoc.Write(o_Writer);

	o_Writer.Write(data.m_ParamList);
	o_Writer.Write(data.m_AttributeList);
	o_Writer.Write(data.m_bOverrideShader);
	o_Writer.Write(data.m_bOverrideAttributes);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effRendermanOverrideDataParser::Create() const
{
	return new effRendermanOverrideData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effRendermanOverrideDataParser::GetChunkName()
{
	return c_RMOV;
}
