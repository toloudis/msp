/*****************************************************************************
**	effToonDataParser.cpp
**
**		effToonDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effToonDataParser.hpp"

#include "Graphics/eff/effToonData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_TOON = chDefs::MakeName('T', 'O', 'O', 'N');
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effToonDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effToonData& data = dynamic_cast<effToonData &>(o_Shader);

	chChunkParserUtil::Read(i_Reader, data.m_ColorAmbient);
	chChunkParserUtil::Read(i_Reader, data.m_ColorMidtone);
	chChunkParserUtil::Read(i_Reader, data.m_ColorDiffuse);
	chChunkParserUtil::Read(i_Reader, data.m_ColorSpecular);
	i_Reader.Read(data.m_NameDiffuse);
	i_Reader.Read(data.m_NameGradientMap);
	i_Reader.Read(data.m_UV.m_UScale);
	i_Reader.Read(data.m_UV.m_VScale);
	i_Reader.Read(data.m_UV.m_UTrans);
	i_Reader.Read(data.m_UV.m_VTrans);
	i_Reader.Read(data.m_UV.m_UVAngle);
	i_Reader.Read(data.m_Transparency);
	i_Reader.Read(data.m_NameTransparencyMap);
	i_Reader.Read(data.m_Transition1);
	i_Reader.Read(data.m_Transition2);
	i_Reader.Read(data.m_Transition3);
	i_Reader.Read(data.m_SpecularEnable);
	i_Reader.Read(data.m_Smoothness);
}

//--------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//--------------------------------------------------------------------
void effToonDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mdlMaterialInfo& o_MaterialInfo,
								effShaderParams& o_Shader) const
{
	effToonData data;
	Read(i_Reader, i_Version, i_Size, data);
	o_MaterialInfo.UVTransform() = data.m_UV;

	o_Shader.SetVersion(1);
	o_Shader.AddParam(new effParamColor("g_ambient", "g_ambient", data.m_ColorAmbient));
	o_Shader.AddParam(new effParamColor("g_ambient", "g_ambient", data.m_ColorMidtone));
	o_Shader.AddParam(new effParamColor("g_diffuse", "g_diffuse", data.m_ColorDiffuse));
	o_Shader.AddParam(new effParamColor("g_specular", "g_specular", data.m_ColorSpecular));
	o_Shader.AddParam(new effParamTexture("diffuseMap", "diffuseMap", itString(data.m_NameDiffuse.c_str())));
	o_Shader.AddParam(new effParamTexture("gradientMap", "gradientMap", itString(data.m_NameGradientMap.c_str())));
	o_Shader.AddParam(new effParamFloat("g_transparency", "g_transparency", data.m_Transparency));
	o_Shader.AddParam(new effParamTexture("transparencyMap", "transparencyMap", itString(data.m_NameTransparencyMap.c_str())));
	o_Shader.AddParam(new effParamFloat("g_colorTransition0", "g_colorTransition0", data.m_Transition1));
	o_Shader.AddParam(new effParamFloat("g_colorTransition1", "g_colorTransition1", data.m_Transition2));
	o_Shader.AddParam(new effParamFloat("g_colorTransition2", "g_colorTransition2", data.m_Transition3));
	o_Shader.AddParam(new effParamBool("g_hasSpecular", "g_hasSpecular", data.m_SpecularEnable));
	o_Shader.AddParam(new effParamFloat("g_smoothness", "g_smoothness", data.m_Smoothness));
}


//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effToonDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effToonData& data = dynamic_cast<const effToonData &>(i_Shader);

	static const chDefs::Version s_Version = 0;
	o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	chChunkParserUtil::Write(o_Writer, data.m_ColorAmbient);
	chChunkParserUtil::Write(o_Writer, data.m_ColorMidtone);
	chChunkParserUtil::Write(o_Writer, data.m_ColorDiffuse);
	chChunkParserUtil::Write(o_Writer, data.m_ColorSpecular);
	o_Writer.Write(data.m_NameDiffuse);
	o_Writer.Write(data.m_NameGradientMap);
	o_Writer.Write(data.m_UV.m_UScale);
	o_Writer.Write(data.m_UV.m_VScale);
	o_Writer.Write(data.m_UV.m_UTrans);
	o_Writer.Write(data.m_UV.m_VTrans);
	o_Writer.Write(data.m_UV.m_UVAngle);
	o_Writer.Write(data.m_Transparency);
	o_Writer.Write(data.m_NameTransparencyMap);
	o_Writer.Write(data.m_Transition1);
	o_Writer.Write(data.m_Transition2);
	o_Writer.Write(data.m_Transition3);
	o_Writer.Write(data.m_SpecularEnable);
	o_Writer.Write(data.m_Smoothness);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effToonDataParser::Create() const
{
	return new effToonData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effToonDataParser::GetChunkName()
{
	return c_TOON;
}
