/*****************************************************************************
**	effLambertDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effLambertDataParser.hpp"

#include "Graphics/eff/effLambertData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_LAMB = chDefs::MakeName('L', 'A', 'M', 'B');
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effLambertDataParser::Read(chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effLambertData& data = dynamic_cast<effLambertData &>(o_Shader);

	chChunkParserUtil::Read(i_Reader, data.m_ColorAmbient);
	chChunkParserUtil::Read(i_Reader, data.m_ColorDiffuse);
	chChunkParserUtil::Read(i_Reader, data.m_ColorEmissive);
	i_Reader.Read(data.m_BumpMapScale);
	i_Reader.Read(data.m_NameDiffuse);
	i_Reader.Read(data.m_NameNormalMap);
	i_Reader.Read(data.m_UV.m_UScale);
	i_Reader.Read(data.m_UV.m_VScale);
	i_Reader.Read(data.m_UV.m_UTrans);
	i_Reader.Read(data.m_UV.m_VTrans);
	i_Reader.Read(data.m_UV.m_UVAngle);

	if( i_Version >= 1 )
	{
		i_Reader.Read(data.m_Transparency);
		i_Reader.Read(data.m_NameTransparencyMap);
	}
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effLambertDataParser::Read(chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mdlMaterialInfo& o_MaterialInfo,
								effShaderParams& o_Shader) const
{
	effLambertData data;
	Read(i_Reader, i_Version, i_Size, data);
	o_MaterialInfo.UVTransform() = data.m_UV;
	o_Shader.SetVersion(1);
	o_Shader.AddParam(new effParamFloat("g_bumpMapScale", "g_bumpMapScale", data.m_BumpMapScale));
	o_Shader.AddParam(new effParamColor("g_emissive", "g_emissive", data.m_ColorEmissive));
	o_Shader.AddParam(new effParamColor("g_ambient", "g_ambient", data.m_ColorAmbient));
	o_Shader.AddParam(new effParamColor("g_diffuse", "g_diffuse", data.m_ColorDiffuse));
	o_Shader.AddParam(new effParamFloat("g_transparency", "g_transparency", data.m_Transparency));
	o_Shader.AddParam(new effParamTexture("diffuseMap", "diffuseMap", itString(data.m_NameDiffuse.c_str())));
	o_Shader.AddParam(new effParamTexture("normalMap", "normalMap", itString(data.m_NameNormalMap.c_str())));
	o_Shader.AddParam(new effParamTexture("transparencyMap", "transparencyMap", itString(data.m_NameTransparencyMap.c_str())));
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effLambertDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effLambertData& data = dynamic_cast<const effLambertData &>(i_Shader);

	static const chDefs::Version s_Version = 1;
	o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	chChunkParserUtil::Write(o_Writer, data.m_ColorAmbient);
	chChunkParserUtil::Write(o_Writer, data.m_ColorDiffuse);
	chChunkParserUtil::Write(o_Writer, data.m_ColorEmissive);
	o_Writer.Write(data.m_BumpMapScale);
	o_Writer.Write(data.m_NameDiffuse);
	o_Writer.Write(data.m_NameNormalMap);
	o_Writer.Write(data.m_UV.m_UScale);
	o_Writer.Write(data.m_UV.m_VScale);
	o_Writer.Write(data.m_UV.m_UTrans);
	o_Writer.Write(data.m_UV.m_VTrans);
	o_Writer.Write(data.m_UV.m_UVAngle);

	//version 1
	o_Writer.Write(data.m_Transparency);
	o_Writer.Write(data.m_NameTransparencyMap);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effLambertDataParser::Create() const
{
	return new effLambertData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effLambertDataParser::GetChunkName()
{
	return c_LAMB;
}
