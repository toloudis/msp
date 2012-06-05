/*****************************************************************************
**	effGlowDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effGlowDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/prty/prtyFilePath.hpp"
#include "Core/prty/prtyTextureFileName.hpp"
#include "Graphics/eff/effGlowData.hpp"
#include "Graphics/mat/matTexturePathUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_GLOW = chDefs::MakeName('G', 'L', 'O', 'W');

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

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void resolve_fullpath(prtyTextureFileName &io_FilePath, const fsLocator& i_ContainingFile)
	{
		fsLocator tex_loc = io_FilePath.GetValue();
		if (matTexturePathUtil::ResolveFullPath(tex_loc, i_ContainingFile,
			matShaderParser::GetAllowSingleFilenameTextures(), matShaderParser::GetResolveAbsolutePaths()))
		{
			prtyTextureFileData tex;
			tex.m_TextureLocator = tex_loc;
			tex.m_CurrentCallback = io_FilePath.GetFullValue().m_CurrentCallback;
			io_FilePath.SetValue(tex);
		}
	}
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effGlowDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effGlowData& data = dynamic_cast<effGlowData &>(o_Shader);

	if (i_Version < 1)
	{
		std::string sGlowMask;
		i_Reader.Read(sGlowMask);
		fsLocator locGlowMask;
		if (sGlowMask.length() > 0)
		{
			locGlowMask.Push(itString(sGlowMask.c_str()));
			resolve_fullpath(locGlowMask, i_Reader.GetLocator());
		}
		data.m_NameGlowMask = locGlowMask;
	}
	else
	{
		prtyTextureFileName prtyGlowMask;
		prtyGlowMask.ReadTexture(i_Reader);

		fsLocator locGlowMask = prtyGlowMask.GetValue();
		resolve_fullpath(locGlowMask, i_Reader.GetLocator());
		//resolve_fullpath(prtyGlowMask, i_Reader.GetLocator());
		data.m_NameGlowMask = locGlowMask;
	}
	i_Reader.Read(data.m_GlowAmount);
	chChunkParserUtil::Read(i_Reader, data.m_GlowScale);
	i_Reader.Read(data.m_GlowSize);
	i_Reader.Read(data.m_bConstantGlow);

}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effGlowDataParser::Write( chWriter& o_Writer,
							const effShaderData& i_Shader ) const
{
	const effGlowData& data = dynamic_cast<const effGlowData &>(i_Shader);

	o_Writer.WriteChunkHeader( GetChunkName(), 1, false );

	// version 1 promotes m_NameGlowMask to a fsLocator from a std::string.
	prtyTextureFileName glowMask("", data.m_NameGlowMask);
	glowMask.WriteTexture(o_Writer);

	o_Writer.Write(data.m_GlowAmount);
	chChunkParserUtil::Write(o_Writer, data.m_GlowScale);
	o_Writer.Write(data.m_GlowSize);
	o_Writer.Write(data.m_bConstantGlow);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effGlowDataParser::Create() const
{
	return new effGlowData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effGlowDataParser::GetChunkName()
{
	return c_GLOW;
}
