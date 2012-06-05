/*****************************************************************************
**	effHairDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effHairDataParser.hpp"

#include "Graphics/eff/effHairData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_HAIR = chDefs::MakeName('H', 'A', 'I', 'R');
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effHairDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effHairData& data = dynamic_cast<effHairData &>(o_Shader);

	i_Reader.Read(data.m_BumpMapScale);
	i_Reader.Read(data.m_UV.m_UScale);
	i_Reader.Read(data.m_UV.m_VScale);
	i_Reader.Read(data.m_NameBase);
	i_Reader.Read(data.m_NameAlpha);
	i_Reader.Read(data.m_NameSpecularShift);
	i_Reader.Read(data.m_NameSpecularMask);
	i_Reader.Read(data.m_NameNormalMap);

	chChunkParserUtil::Read(i_Reader, data.m_HairBaseColor);
	chChunkParserUtil::Read(i_Reader, data.m_SpecularColor0);
	i_Reader.Read(data.m_SpecularExponent0);
	i_Reader.Read(data.m_SpecularShift0);
	chChunkParserUtil::Read(i_Reader, data.m_SpecularColor1);
	i_Reader.Read(data.m_SpecularExponent1);
	i_Reader.Read(data.m_SpecularShift1);

	if (i_Version >= 1)
	{
		// added in version 1
		i_Reader.Read(data.m_Reflectivity);

		if (i_Version >= 2)
		{
			// added in version 2
			i_Reader.Read(data.m_UV.m_UTrans);
			i_Reader.Read(data.m_UV.m_VTrans);
			i_Reader.Read(data.m_UV.m_UVAngle);
		}

	}
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effHairDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mdlMaterialInfo& o_MaterialInfo,
								effShaderParams& o_Shader) const
{
	effHairData data;
	Read(i_Reader, i_Version, i_Size, data);
	o_MaterialInfo.UVTransform() = data.m_UV;
	o_Shader.SetVersion(1);
	o_Shader.AddParam(new effParamFloat("g_bumpMapScale", "g_bumpMapScale", data.m_BumpMapScale));
	o_Shader.AddParam(new effParamFloat("g_reflectivity", "g_reflectivity", data.m_Reflectivity));
	o_Shader.AddParam(new effParamColor("g_ambient", "g_ambient", data.m_HairBaseColor));
	o_Shader.AddParam(new effParamTexture("tBase", "tBase", itString(data.m_NameBase.c_str())));
	o_Shader.AddParam(new effParamTexture("tAlpha", "tAlpha", itString(data.m_NameAlpha.c_str())));
	o_Shader.AddParam(new effParamTexture("tSpecularShift", "tSpecularShift", itString(data.m_NameSpecularShift.c_str())));
	o_Shader.AddParam(new effParamTexture("tSpecularMask", "tSpecularMask", itString(data.m_NameSpecularMask.c_str())));
	o_Shader.AddParam(new effParamTexture("tNormalMap", "tNormalMap", itString(data.m_NameNormalMap.c_str())));
	o_Shader.AddParam(new effParamColor("g_specularColor0", "g_specularColor0", data.m_SpecularColor0));
	o_Shader.AddParam(new effParamColor("g_specularColor1", "g_specularColor1", data.m_SpecularColor1));
	o_Shader.AddParam(new effParamColor("g_hairBaseColor", "g_hairBaseColor", data.m_HairBaseColor));
	o_Shader.AddParam(new effParamFloat("g_specularExp0", "g_specularExp0", data.m_SpecularExponent0));
	o_Shader.AddParam(new effParamFloat("g_specularExp1", "g_specularExp1", data.m_SpecularExponent1));
	o_Shader.AddParam(new effParamFloat("g_specularShift0", "g_specularShift0", data.m_SpecularShift0));
	o_Shader.AddParam(new effParamFloat("g_specularShift1", "g_specularShift1", data.m_SpecularShift1));
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effHairDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effHairData& data = dynamic_cast<const effHairData &>(i_Shader);

	static const chDefs::Version s_Version = 2;
	o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	o_Writer.Write(data.m_BumpMapScale);
	o_Writer.Write(data.m_UV.m_UScale);
	o_Writer.Write(data.m_UV.m_VScale);
	o_Writer.Write(data.m_NameBase);
	o_Writer.Write(data.m_NameAlpha);
	o_Writer.Write(data.m_NameSpecularShift);
	o_Writer.Write(data.m_NameSpecularMask);
	o_Writer.Write(data.m_NameNormalMap);

	chChunkParserUtil::Write(o_Writer, data.m_HairBaseColor);
	chChunkParserUtil::Write(o_Writer, data.m_SpecularColor0);
	o_Writer.Write(data.m_SpecularExponent0);
	o_Writer.Write(data.m_SpecularShift0);
	chChunkParserUtil::Write(o_Writer, data.m_SpecularColor1);
	o_Writer.Write(data.m_SpecularExponent1);
	o_Writer.Write(data.m_SpecularShift1);

	// added in version 1
	o_Writer.Write(data.m_Reflectivity);

	// added in version 2
	o_Writer.Write(data.m_UV.m_UTrans);
	o_Writer.Write(data.m_UV.m_VTrans);
	o_Writer.Write(data.m_UV.m_UVAngle);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effHairDataParser::Create() const
{
	return new effHairData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effHairDataParser::GetChunkName()
{
	return c_HAIR;
}
