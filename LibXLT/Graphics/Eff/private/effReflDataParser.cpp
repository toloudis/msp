/*****************************************************************************
**	effReflDataParser.cpp
**
**		effReflDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effReflDataParser.hpp"

#include "Graphics/eff/effReflData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"

//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_REFL = chDefs::MakeName('R', 'E', 'F', 'L');
}


//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effReflDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effReflData& data = dynamic_cast<effReflData &>(o_Shader);

	chChunkParserUtil::Read(i_Reader, data.m_ColorAmbient);
	chChunkParserUtil::Read(i_Reader, data.m_ColorDiffuse);
	chChunkParserUtil::Read(i_Reader, data.m_ColorEmissive);
	chChunkParserUtil::Read(i_Reader, data.m_ColorSpecular);
	i_Reader.Read(data.m_SpecularPower);
	i_Reader.Read(data.m_BumpMapScale);
	i_Reader.Read(data.m_Reflectivity);
	i_Reader.Read(data.m_UV.m_UScale);
	i_Reader.Read(data.m_UV.m_VScale);
	i_Reader.Read(data.m_NameDiffuseMap);
	i_Reader.Read(data.m_NameSpecularMap);
	i_Reader.Read(data.m_NameNormalMap);
	i_Reader.Read(data.m_NameReflectFactorMap);
	i_Reader.Read(data.m_bAutoGenEnvMap);
	i_Reader.Read(data.m_ReflMapResolution);
	i_Reader.Read(data.m_IOR);
	i_Reader.Read(data.m_ReflLOD);
	i_Reader.Read(data.m_RefrLOD);
	i_Reader.Read(data.m_ReflFactor);
	i_Reader.Read(data.m_RefrFactor);
	i_Reader.Read(data.m_FresnelBias);
	i_Reader.Read(data.m_FresnelPower);
	if (i_Version >= 1)
	{
		// added in version 1
		i_Reader.Read(data.m_bIsPlanar);

		if (i_Version >= 2)
		{
			// added in version 2
			i_Reader.Read(data.m_NearPlane);

			if (i_Version >= 3)
			{
				i_Reader.Read(data.m_UV.m_UTrans);
				i_Reader.Read(data.m_UV.m_VTrans);
				i_Reader.Read(data.m_UV.m_UVAngle);

				if( i_Version >= 4 )
				{
					i_Reader.Read(data.m_Transparency);
					i_Reader.Read(data.m_NameTransparencyMap);
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
void effReflDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mdlMaterialInfo& o_MaterialInfo,
								effShaderParams& o_Shader) const
{
	effReflData data;
	Read(i_Reader, i_Version, i_Size, data);
	o_MaterialInfo.UVTransform() = data.m_UV;

	o_MaterialInfo.SetHasReflection(true);
	o_MaterialInfo.ReflectionParams().m_bAutoGenEnvMap = data.m_bAutoGenEnvMap;
	o_MaterialInfo.ReflectionParams().m_ReflMapResolution = data.m_ReflMapResolution;
	o_MaterialInfo.ReflectionParams().m_bIsPlanar = data.m_bIsPlanar;
	o_MaterialInfo.ReflectionParams().m_NearPlane = data.m_NearPlane;

	o_Shader.SetVersion(1);
	o_Shader.AddParam(new effParamFloat("g_shininess", "g_shininess", data.m_SpecularPower));
	o_Shader.AddParam(new effParamFloat("g_bumpMapScale", "g_bumpMapScale", data.m_BumpMapScale));
	o_Shader.AddParam(new effParamFloat("g_reflectivity", "g_reflectivity", data.m_Reflectivity));
	o_Shader.AddParam(new effParamFloat("g_transparency", "g_transparency", data.m_Transparency));
	o_Shader.AddParam(new effParamColor("g_emissive", "g_emissive", data.m_ColorEmissive));
	o_Shader.AddParam(new effParamColor("g_ambient", "g_ambient", data.m_ColorAmbient));
	o_Shader.AddParam(new effParamColor("g_diffuse", "g_diffuse", data.m_ColorDiffuse));
	o_Shader.AddParam(new effParamColor("g_specular", "g_specular", data.m_ColorSpecular));
	o_Shader.AddParam(new effParamTexture("diffuseMap", "diffuseMap", itString(data.m_NameDiffuseMap.c_str())));
	o_Shader.AddParam(new effParamTexture("normalMap", "normalMap", itString(data.m_NameNormalMap.c_str())));
	o_Shader.AddParam(new effParamTexture("specularMap", "specularMap", itString(data.m_NameSpecularMap.c_str())));
	o_Shader.AddParam(new effParamTexture("reflectFactorMap", "reflectFactorMap", itString(data.m_NameReflectFactorMap.c_str())));
	o_Shader.AddParam(new effParamTexture("transparencyMap", "transparencyMap", itString(data.m_NameTransparencyMap.c_str())));
	o_Shader.AddParam(new effParamFloat("IOR", "IOR", data.m_IOR));
	o_Shader.AddParam(new effParamFloat("reflLOD", "reflLOD", data.m_ReflLOD));
	o_Shader.AddParam(new effParamFloat("refrLOD", "refrLOD", data.m_RefrLOD));
	o_Shader.AddParam(new effParamFloat("reflFactor", "reflFactor", data.m_ReflFactor));
	o_Shader.AddParam(new effParamFloat("refrFactor", "refrFactor", data.m_RefrFactor));
	o_Shader.AddParam(new effParamFloat("fresnelBias", "fresnelBias", data.m_FresnelBias));
	o_Shader.AddParam(new effParamFloat("fresnelPower", "fresnelPower", data.m_FresnelPower));
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effReflDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effReflData& data = dynamic_cast<const effReflData &>(i_Shader);

	static const chDefs::Version s_Version = 4;
	o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	// version 0
	chChunkParserUtil::Write(o_Writer, data.m_ColorAmbient);
	chChunkParserUtil::Write(o_Writer, data.m_ColorDiffuse);
	chChunkParserUtil::Write(o_Writer, data.m_ColorEmissive);
	chChunkParserUtil::Write(o_Writer, data.m_ColorSpecular);
	o_Writer.Write(data.m_SpecularPower);
	o_Writer.Write(data.m_BumpMapScale);
	o_Writer.Write(data.m_Reflectivity);
	o_Writer.Write(data.m_UV.m_UScale);
	o_Writer.Write(data.m_UV.m_VScale);
	o_Writer.Write(data.m_NameDiffuseMap);
	o_Writer.Write(data.m_NameSpecularMap);
	o_Writer.Write(data.m_NameNormalMap);
	o_Writer.Write(data.m_NameReflectFactorMap);
	o_Writer.Write(data.m_bAutoGenEnvMap);
	o_Writer.Write(data.m_ReflMapResolution);
	o_Writer.Write(data.m_IOR);
	o_Writer.Write(data.m_ReflLOD);
	o_Writer.Write(data.m_RefrLOD);
	o_Writer.Write(data.m_ReflFactor);
	o_Writer.Write(data.m_RefrFactor);
	o_Writer.Write(data.m_FresnelBias);
	o_Writer.Write(data.m_FresnelPower);
	// added in version 1
	o_Writer.Write(data.m_bIsPlanar);
	// added in version 2
	o_Writer.Write(data.m_NearPlane);
	// added in version 3
	o_Writer.Write(data.m_UV.m_UTrans);
	o_Writer.Write(data.m_UV.m_VTrans);
	o_Writer.Write(data.m_UV.m_UVAngle);

	//version 4
	o_Writer.Write(data.m_Transparency);
	o_Writer.Write(data.m_NameTransparencyMap);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effReflDataParser::Create() const
{
	return new effReflData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effReflDataParser::GetChunkName()
{
	return c_REFL;
}
