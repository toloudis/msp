/*****************************************************************************
**	effDisplacementDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effDisplacementDataParser.hpp"

#include "Graphics/eff/effDisplacementData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mat/matTexturePathUtil.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const chDefs::Name c_DISP = chDefs::MakeName('D', 'I', 'S', 'P');

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
void effDisplacementDataParser::Read(	chReader& i_Reader,
										chDefs::Version i_Version,
										chDefs::Size i_Size,
										effShaderData& o_Shader ) const
{
	effDisplacementData& data = dynamic_cast<effDisplacementData &>(o_Shader);

	i_Reader.Read(data.m_Scale);
	i_Reader.Read(data.m_Bias);
	i_Reader.Read(data.m_Blur);
	i_Reader.Read(data.m_TessellationValue);

	prtyTextureFileName prtyDisplacementMap;
	prtyDisplacementMap.ReadTexture(i_Reader);

	fsLocator locDisplacementMap = prtyDisplacementMap.GetValue();
	resolve_fullpath(locDisplacementMap, i_Reader.GetLocator());
	data.m_NameDisplacementMap = locDisplacementMap;

	if( i_Version >= 1 )
	{
		i_Reader.Read(data.m_ObjUVScale.m_X);
		i_Reader.Read(data.m_ObjUVScale.m_Y);
	}
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parse-able object.
//----------------------------------------------------------------------------
void effDisplacementDataParser::Read(	chReader& i_Reader,
									chDefs::Version i_Version,
									chDefs::Size i_Size,
									mdlMaterialInfo& o_MaterialInfo,
									effShaderParams& o_Shader) const
{
	effDisplacementData data;
	Read(i_Reader, i_Version, i_Size, data);

	o_MaterialInfo.SetHasDisplacement(true);
	o_MaterialInfo.DisplacementParams().m_Scale = data.m_Scale;
	o_MaterialInfo.DisplacementParams().m_Bias = data.m_Bias;
	o_MaterialInfo.DisplacementParams().m_Blur = data.m_Blur;
	o_MaterialInfo.DisplacementParams().m_TessellationValue = data.m_TessellationValue;
	o_MaterialInfo.DisplacementParams().m_ObjUVScale = data.m_ObjUVScale;

	o_Shader.SetVersion(1);
	o_Shader.AddParam(new effParamFloat("g_displacementScale", "g_displacementScale", data.m_Scale ));
	o_Shader.AddParam(new effParamFloat("g_displacementBias", "g_displacementBias", data.m_Bias ));
	o_Shader.AddParam(new effParamFloat("g_displacementBlur", "g_displacementBlur", data.m_Blur ));
	o_Shader.AddParam(new effParamTexture("displacementMap", "displacementMap", data.m_NameDisplacementMap));
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effDisplacementDataParser::Write( chWriter& o_Writer,
									const effShaderData& i_Shader ) const
{
	const effDisplacementData& data = dynamic_cast<const effDisplacementData &>(i_Shader);

	static const chDefs::Version s_Version = 1;
	o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	// version 0
	o_Writer.Write(data.m_Scale);
	o_Writer.Write(data.m_Bias);
	o_Writer.Write(data.m_Blur);
	o_Writer.Write(data.m_TessellationValue);

	prtyTextureFileName dispMap("", data.m_NameDisplacementMap);
	dispMap.WriteTexture(o_Writer);

	// version 1
	o_Writer.Write(data.m_ObjUVScale.m_X);
	o_Writer.Write(data.m_ObjUVScale.m_Y);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effDisplacementDataParser::Create() const
{
	return new effDisplacementData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effDisplacementDataParser::GetChunkName()
{
	return c_DISP;
}
