/*****************************************************************************
**	effBlinnDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effBlinnDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Graphics/eff/effBlinnData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_BLIN = chDefs::MakeName('B', 'L', 'I', 'N');
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effBlinnDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effBlinnData& data = dynamic_cast<effBlinnData &>(o_Shader);

	chChunkParserUtil::Read(i_Reader, data.m_ColorAmbient);
	chChunkParserUtil::Read(i_Reader, data.m_ColorDiffuse);
	chChunkParserUtil::Read(i_Reader, data.m_ColorEmissive);
	chChunkParserUtil::Read(i_Reader, data.m_ColorSpecular);
	i_Reader.Read(data.m_SpecularPower);
	i_Reader.Read(data.m_BumpMapScale);
	i_Reader.Read(data.m_Reflectivity);
	i_Reader.Read(data.m_IOR);
	i_Reader.Read(data.m_UV.m_UScale);
	i_Reader.Read(data.m_UV.m_VScale);
	i_Reader.Read(data.m_UV.m_UTrans);
	i_Reader.Read(data.m_UV.m_VTrans);
	i_Reader.Read(data.m_UV.m_UVAngle);
	i_Reader.Read(data.m_NameDiffuse);
	i_Reader.Read(data.m_NameSpecular);
	i_Reader.Read(data.m_NameGloss);
	i_Reader.Read(data.m_NameEnvironment);
	i_Reader.Read(data.m_NameNormalMap);
	i_Reader.Read(data.m_NameReflectFactorMap);

	if (i_Version >= 1)
	{
		i_Reader.Read(data.m_NameIORMap);
		i_Reader.Read(data.m_DiffuseRoughness);
	}
	if( i_Version >= 2 )
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
void effBlinnDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mdlMaterialInfo& o_MaterialInfo,
								effShaderParams& o_Shader) const
{
	effBlinnData data;
	Read(i_Reader, i_Version, i_Size, data);
	o_MaterialInfo.UVTransform() = data.m_UV;
	o_Shader.SetVersion(1);
	o_Shader.AddParam(new effParamFloat("g_shininess", "g_shininess", data.m_SpecularPower));
	o_Shader.AddParam(new effParamFloat("g_bumpMapScale", "g_bumpMapScale", data.m_BumpMapScale));
	o_Shader.AddParam(new effParamFloat("g_reflectivity", "g_reflectivity", data.m_Reflectivity));
	o_Shader.AddParam(new effParamFloat("g_transparency", "g_transparency", data.m_Transparency));
	o_Shader.AddParam(new effParamColor("g_emissive", "g_emissive", data.m_ColorEmissive));
	o_Shader.AddParam(new effParamColor("g_ambient", "g_ambient", data.m_ColorAmbient));
	o_Shader.AddParam(new effParamColor("g_diffuse", "g_diffuse", data.m_ColorDiffuse));
	o_Shader.AddParam(new effParamColor("g_specular", "g_specular", data.m_ColorSpecular));
	o_Shader.AddParam(new effParamFloat("g_IOR", "g_IOR", data.m_IOR));
	o_Shader.AddParam(new effParamFloat("g_roughness", "g_roughness", data.m_DiffuseRoughness));
	o_Shader.AddParam(new effParamTexture("diffuseMap", "diffuseMap", itString(data.m_NameDiffuse.c_str())));
	o_Shader.AddParam(new effParamTexture("normalMap", "normalMap", itString(data.m_NameNormalMap.c_str())));
	o_Shader.AddParam(new effParamTexture("cubeMap", "cubeMap", itString(data.m_NameEnvironment.c_str())));
	o_Shader.AddParam(new effParamTexture("specularMap", "specularMap", itString(data.m_NameSpecular.c_str())));
	o_Shader.AddParam(new effParamTexture("glossMap", "glossMap", itString(data.m_NameGloss.c_str())));
	o_Shader.AddParam(new effParamTexture("reflectFactorMap", "reflectFactorMap", itString(data.m_NameReflectFactorMap.c_str())));
	o_Shader.AddParam(new effParamTexture("iorMap", "iorMap", itString(data.m_NameIORMap.c_str())));
	o_Shader.AddParam(new effParamTexture("transparencyMap", "transparencyMap", itString(data.m_NameTransparencyMap.c_str())));
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effBlinnDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effBlinnData& data = dynamic_cast<const effBlinnData &>(i_Shader);

	static const chDefs::Version s_Version = 2;
	o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	chChunkParserUtil::Write(o_Writer, data.m_ColorAmbient);
	chChunkParserUtil::Write(o_Writer, data.m_ColorDiffuse);
	chChunkParserUtil::Write(o_Writer, data.m_ColorEmissive);
	chChunkParserUtil::Write(o_Writer, data.m_ColorSpecular);
	o_Writer.Write(data.m_SpecularPower);
	o_Writer.Write(data.m_BumpMapScale);
	o_Writer.Write(data.m_Reflectivity);
	o_Writer.Write(data.m_IOR);
	o_Writer.Write(data.m_UV.m_UScale);
	o_Writer.Write(data.m_UV.m_VScale);
	o_Writer.Write(data.m_UV.m_UTrans);
	o_Writer.Write(data.m_UV.m_VTrans);
	o_Writer.Write(data.m_UV.m_UVAngle);
	o_Writer.Write(data.m_NameDiffuse);
	o_Writer.Write(data.m_NameSpecular);
	o_Writer.Write(data.m_NameGloss);
	o_Writer.Write(data.m_NameEnvironment);
	o_Writer.Write(data.m_NameNormalMap);
	o_Writer.Write(data.m_NameReflectFactorMap);

	// version 1
	o_Writer.Write(data.m_NameIORMap);
	o_Writer.Write(data.m_DiffuseRoughness);

	//version 2
	o_Writer.Write(data.m_Transparency);
	o_Writer.Write(data.m_NameTransparencyMap);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effBlinnDataParser::Create() const
{
	return new effBlinnData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effBlinnDataParser::GetChunkName()
{
	return c_BLIN;
}
