/*****************************************************************************
**	effPhongDataParser.cpp
**
**		effPhongDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effPhongDataParser.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_PHNG = chDefs::MakeName('P', 'H', 'N', 'G');
}


//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effPhongDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								effShaderData& o_Shader ) const
{
	effPhongData& data = dynamic_cast<effPhongData &>(o_Shader);

	chChunkParserUtil::Read(i_Reader, data.m_ColorAmbient);
	chChunkParserUtil::Read(i_Reader, data.m_ColorDiffuse);
	chChunkParserUtil::Read(i_Reader, data.m_ColorEmissive);
	chChunkParserUtil::Read(i_Reader, data.m_ColorSpecular);
	i_Reader.Read(data.m_SpecularPower);
	i_Reader.Read(data.m_BumpMapScale);
	i_Reader.Read(data.m_Reflectivity);
	i_Reader.Read(data.m_UV.m_UScale);
	i_Reader.Read(data.m_UV.m_VScale);
	i_Reader.Read(data.m_NameDiffuse);
	i_Reader.Read(data.m_NameSpecular);
	i_Reader.Read(data.m_NameGloss);
	i_Reader.Read(data.m_NameEnvironment);
	i_Reader.Read(data.m_NameNormalMap);

	if (i_Version >= 1)
	{
		i_Reader.Read(data.m_UV.m_UTrans);
		i_Reader.Read(data.m_UV.m_VTrans);
		i_Reader.Read(data.m_UV.m_UVAngle);

		if (i_Version >= 2)
		{
			i_Reader.Read(data.m_NameReflectFactorMap);

			if( i_Version >= 3)
			{
				i_Reader.Read(data.m_Transparency);
				i_Reader.Read(data.m_NameTransparencyMap);

				if( i_Version >= 4)
				{
					//deprecated, read into dummy vars
					float fvar;
					std::string sString;
					i_Reader.Read(fvar);
					i_Reader.Read(fvar);
					i_Reader.Read(sString);
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
void effPhongDataParser::Read(	chReader& i_Reader,
								chDefs::Version i_Version,
								chDefs::Size i_Size,
								mdlMaterialInfo& o_MaterialInfo,
								effShaderParams& o_Shader) const
{
	effPhongData data;
	Read(i_Reader, i_Version, i_Size, data);
	o_MaterialInfo.UVTransform() = data.m_UV;
	data.AddToParams(o_Shader);
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effPhongDataParser::Write( chWriter& o_Writer,
								const effShaderData& i_Shader ) const
{
	const effPhongData& data = dynamic_cast<const effPhongData &>(i_Shader);

	static const chDefs::Version s_Version = 4;
	o_Writer.WriteChunkHeader( GetChunkName(), s_Version, false );

	chChunkParserUtil::Write(o_Writer, data.m_ColorAmbient);
	chChunkParserUtil::Write(o_Writer, data.m_ColorDiffuse);
	chChunkParserUtil::Write(o_Writer, data.m_ColorEmissive);
	chChunkParserUtil::Write(o_Writer, data.m_ColorSpecular);
	o_Writer.Write(data.m_SpecularPower);
	o_Writer.Write(data.m_BumpMapScale);
	o_Writer.Write(data.m_Reflectivity);
	o_Writer.Write(data.m_UV.m_UScale);
	o_Writer.Write(data.m_UV.m_VScale);
	o_Writer.Write(data.m_NameDiffuse);
	o_Writer.Write(data.m_NameSpecular);
	o_Writer.Write(data.m_NameGloss);
	o_Writer.Write(data.m_NameEnvironment);
	o_Writer.Write(data.m_NameNormalMap);

	// added in version 1
	o_Writer.Write(data.m_UV.m_UTrans);
	o_Writer.Write(data.m_UV.m_VTrans);
	o_Writer.Write(data.m_UV.m_UVAngle);

	// added version 2
	o_Writer.Write(data.m_NameReflectFactorMap);

	//version 3
	o_Writer.Write(data.m_Transparency);
	o_Writer.Write(data.m_NameTransparencyMap);

	//version 4 (displacement mapping)
	//deprecated (moved to effShaderParams), write blank values
	float fval = 0;
	std::string sString;
	o_Writer.Write(fval);
	o_Writer.Write(fval);
	o_Writer.Write(sString);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effPhongDataParser::Create() const
{
	return new effPhongData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effPhongDataParser::GetChunkName()
{
	return c_PHNG;
}
