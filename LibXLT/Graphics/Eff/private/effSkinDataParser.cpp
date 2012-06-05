/*****************************************************************************
**	effSkinDataParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effSkinDataParser.hpp"

#include "Graphics/eff/effSkinData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_SKIN = chDefs::MakeName('S', 'K', 'I', 'N');
}


//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effSkinDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effSkinData& data = dynamic_cast<effSkinData &>(o_Shader);

	chChunkParserUtil::Read(i_Reader, data.m_SpecColor);
	i_Reader.Read(data.m_SpecPower);
	i_Reader.Read(data.m_SpecGloss);
	i_Reader.Read(data.m_SpecFresnel);
	i_Reader.Read(data.m_FresnelPower);
	i_Reader.Read(data.m_FresnelGloss);
	chChunkParserUtil::Read(i_Reader, data.m_TransColIn);
	chChunkParserUtil::Read(i_Reader, data.m_TransColOut);
	chChunkParserUtil::Read(i_Reader, data.m_TransColBack);
	i_Reader.Read(data.m_TransMultiplier);
	i_Reader.Read(data.m_TransRampOff);
	i_Reader.Read(data.m_MicroScale);
	i_Reader.Read(data.m_NameDiffTex);
	i_Reader.Read(data.m_NameNormalTex);
	i_Reader.Read(data.m_NameMicroTex);
	i_Reader.Read(data.m_NameSpecTex);
	i_Reader.Read(data.m_NameTransTex);
	i_Reader.Read(data.m_BumpMapScale);
	i_Reader.Read(data.m_UV.m_UScale);
	i_Reader.Read(data.m_UV.m_VScale);
	if (i_Version >= 1)
	{
		i_Reader.Read(data.m_NameSpecPowerTex);
		if (i_Version >= 2)
		{
			i_Reader.Read(data.m_UV.m_UTrans);
			i_Reader.Read(data.m_UV.m_VTrans);
			i_Reader.Read(data.m_UV.m_UVAngle);
			if (i_Version >= 3)
			{
				i_Reader.Read(data.m_NameCubeMapTex);
				i_Reader.Read(data.m_NameReflectFactorTex);

				if( i_Version >= 4 )
				{
					i_Reader.Read(data.m_Transparency);
					i_Reader.Read(data.m_NameTransparencyTex);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effSkinDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mdlMaterialInfo& o_MaterialInfo,
								effShaderParams& o_Shader) const
{
	effSkinData data;
	Read(i_Reader, i_Version, i_Size, data);
	o_MaterialInfo.UVTransform() = data.m_UV;
	o_Shader.SetVersion(1);
	o_Shader.AddParam(new effParamFloat("g_bumpMapScale", "g_bumpMapScale", data.m_BumpMapScale));
	// set default:
	o_Shader.AddParam(new effParamFloat("g_reflectivity", "g_reflectivity", 1.0f));
	o_Shader.AddParam(new effParamFloat("g_transparency", "g_transparency", data.m_Transparency));
	o_Shader.AddParam(new effParamColor("g_specColor", "g_specColor", data.m_SpecColor));
	o_Shader.AddParam(new effParamFloat("g_specPower", "g_specPower", data.m_SpecPower));
	o_Shader.AddParam(new effParamFloat("g_specGloss", "g_specGloss", data.m_SpecGloss));
	o_Shader.AddParam(new effParamFloat("g_specFresnel", "g_specFresnel", data.m_SpecFresnel));
	o_Shader.AddParam(new effParamFloat("g_fresnelPower", "g_fresnelPower", data.m_FresnelPower));
	o_Shader.AddParam(new effParamFloat("g_fresnelGloss", "g_fresnelGloss", data.m_FresnelGloss));
	o_Shader.AddParam(new effParamColor("g_transColIn", "g_transColIn", data.m_TransColIn));
	o_Shader.AddParam(new effParamColor("g_transColOut", "g_transColOut", data.m_TransColOut));
	o_Shader.AddParam(new effParamColor("g_transColBack", "g_transColBack", data.m_TransColBack));
	o_Shader.AddParam(new effParamFloat("g_transMultiplier", "g_transMultiplier", data.m_TransMultiplier));
	o_Shader.AddParam(new effParamFloat("g_transRampOff", "g_transRampOff", data.m_TransRampOff));
	o_Shader.AddParam(new effParamFloat("g_microScale", "g_microScale", data.m_MicroScale));
	o_Shader.AddParam(new effParamTexture("diffTex", "diffTex", itString(data.m_NameDiffTex.c_str())));
	o_Shader.AddParam(new effParamTexture("normalTex", "normalTex", itString(data.m_NameNormalTex.c_str())));
	o_Shader.AddParam(new effParamTexture("microTex", "microTex", itString(data.m_NameMicroTex.c_str())));
	o_Shader.AddParam(new effParamTexture("specTex", "specTex", itString(data.m_NameSpecTex.c_str())));
	o_Shader.AddParam(new effParamTexture("specPowerTex", "specPowerTex", itString(data.m_NameSpecPowerTex.c_str())));
	o_Shader.AddParam(new effParamTexture("transTex", "transTex", itString(data.m_NameTransTex.c_str())));
	o_Shader.AddParam(new effParamTexture("cubeTex", "cubeTex", itString(data.m_NameCubeMapTex.c_str())));
	o_Shader.AddParam(new effParamTexture("reflectFactorTex", "reflectFactorTex", itString(data.m_NameReflectFactorTex.c_str())));
	o_Shader.AddParam(new effParamTexture("transparencyTex", "transparencyTex", itString(data.m_NameTransparencyTex.c_str())));

	//	debugging
	//o_Shader.Dump();
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effSkinDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effSkinData& data = dynamic_cast<const effSkinData &>(i_Shader);

	static const chDefs::Version s_Version = 4;
	o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	// version 0:
	chChunkParserUtil::Write(o_Writer, data.m_SpecColor);
	o_Writer.Write(data.m_SpecPower);
	o_Writer.Write(data.m_SpecGloss);
	o_Writer.Write(data.m_SpecFresnel);
	o_Writer.Write(data.m_FresnelPower);
	o_Writer.Write(data.m_FresnelGloss);
	chChunkParserUtil::Write(o_Writer, data.m_TransColIn);
	chChunkParserUtil::Write(o_Writer, data.m_TransColOut);
	chChunkParserUtil::Write(o_Writer, data.m_TransColBack);
	o_Writer.Write(data.m_TransMultiplier);
	o_Writer.Write(data.m_TransRampOff);
	o_Writer.Write(data.m_MicroScale);
	o_Writer.Write(data.m_NameDiffTex);
	o_Writer.Write(data.m_NameNormalTex);
	o_Writer.Write(data.m_NameMicroTex);
	o_Writer.Write(data.m_NameSpecTex);
	o_Writer.Write(data.m_NameTransTex);
	o_Writer.Write(data.m_BumpMapScale);
	o_Writer.Write(data.m_UV.m_UScale);
	o_Writer.Write(data.m_UV.m_VScale);
	// version 1:
	o_Writer.Write(data.m_NameSpecPowerTex);
	// added in version 2
	o_Writer.Write(data.m_UV.m_UTrans);
	o_Writer.Write(data.m_UV.m_VTrans);
	o_Writer.Write(data.m_UV.m_UVAngle);
	// added in version 3 
	o_Writer.Write(data.m_NameCubeMapTex);
	o_Writer.Write(data.m_NameReflectFactorTex);

	//version 4
	o_Writer.Write(data.m_Transparency);
	o_Writer.Write(data.m_NameTransparencyTex);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effSkinDataParser::Create() const
{
	return new effSkinData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effSkinDataParser::GetChunkName()
{
	return c_SKIN;
}
