/*****************************************************************************
**	effOutlineDataParser.cpp
**
**		effOutlineDataParser handles the creation of different types shader 
**		data objects.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effOutlineDataParser.hpp"

#include "Graphics/eff/effOutlineData.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chReader.hpp"


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const chDefs::Name c_OUTLINE = chDefs::MakeName('O', 'T', 'L', 'N');
	const int          C_OUTLINE_VERSION = 4;
}

//----------------------------------------------------------------------------
//	Read is called by a parser utility when the chunk name has been read
//  from the data file.  It will parse from the chunk into the given
//  parsable object.
//----------------------------------------------------------------------------
void effOutlineDataParser::Read(	chReader& i_Reader,
						chDefs::Version i_Version,
						chDefs::Size i_Size,
						effShaderData& o_Shader ) const
{
	effOutlineData& data = dynamic_cast<effOutlineData &>(o_Shader);

	i_Reader.Read(data.m_OutlineMinAngle);
	i_Reader.Read(data.m_OutlineThickness);
	chChunkParserUtil::Read(i_Reader, data.m_OutlineColor);
	chChunkParserUtil::Read(i_Reader, data.m_OutlineViewSize);

	if( i_Version >= 1 )
	{
		i_Reader.Read(data.m_OutlineDepthScale);
		if( i_Version >= 2 )
		{
			float dummy;
			i_Reader.Read(dummy);
			i_Reader.Read(data.m_bUseDepths);
			i_Reader.Read(data.m_bUseNormals);
			if( i_Version >= 3 )
			{
				i_Reader.Read(data.m_OutlineMaxAngle);
				if( i_Version >= 4 )
				{
					i_Reader.Read(data.m_OutlineMinWidth);
					i_Reader.Read(data.m_OutlineMaxWidth);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
//	Write is called to parse data from the given object to the given
//	chWriter.  i_Shader is the object being written.
//----------------------------------------------------------------------------
void effOutlineDataParser::Write( chWriter& o_Writer,
						const effShaderData& i_Shader ) const
{
	const effOutlineData& data = dynamic_cast<const effOutlineData &>(i_Shader);

	o_Writer.WriteChunkHeader( GetChunkName(), C_OUTLINE_VERSION, false );

	o_Writer.Write(data.m_OutlineMinAngle);
	o_Writer.Write(data.m_OutlineThickness);
	chChunkParserUtil::Write(o_Writer, data.m_OutlineColor);
	chChunkParserUtil::Write(o_Writer, data.m_OutlineViewSize);
	o_Writer.Write(data.m_OutlineDepthScale);	//version 1
	float dummy = 0;
	//	o_Writer.Write(data.m_OutlineMagnitude);	//version 2
	o_Writer.Write(dummy);						//(deprecated)
	o_Writer.Write(data.m_bUseDepths);
	o_Writer.Write(data.m_bUseNormals);
	o_Writer.Write(data.m_OutlineMaxAngle);		//version 3
	o_Writer.Write(data.m_OutlineMinWidth);		//version 4
	o_Writer.Write(data.m_OutlineMaxWidth);

	o_Writer.FinishChunk();
}

//----------------------------------------------------------------------------
// Create should be overridden to create the user specific parsable object.
// Users of parsers should call create to create the object that is then
// passed to Read.
//----------------------------------------------------------------------------
effShaderData* effOutlineDataParser::Create() const
{
	return new effOutlineData;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effOutlineDataParser::GetChunkName()
{
	return c_OUTLINE;
}
