/****************************************************************************\
**  mdlHairParser.cpp
**
**      mdlHairParser.hpp provides functions to parse hairinfo
**		from gxb file
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlHairParser.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"
#include "Graphics/mdl/mdlHairInfo.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"
#include "Graphics/mdl/private/mdlIndexUtil.hpp"


//============================================================================
//============================================================================
namespace mdlHairParser
{
	namespace
	{
		const chDefs::Name c_HRFO = chDefs::MakeName('H', 'R', 'F', 'O');
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
		const chDefs::Name c_NINT = chDefs::MakeName('N', 'I', 'N', 'T');
		const chDefs::Name c_HSTR = chDefs::MakeName('H', 'S', 'T', 'R');
		const chDefs::Name c_HSFI = chDefs::MakeName('H', 'S', 'F', 'I');
		const chDefs::Name c_HSCV = chDefs::MakeName('H', 'S', 'C', 'V');
		const chDefs::Name c_MTLS = chDefs::MakeName('M', 'T', 'L', 'S');
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool ReadHSTR(	chReader& i_Reader,
		chDefs::Version i_Version,
		chDefs::Size i_Size,
		mdlHairStrand& o_HairStrand,
		int i_NumVerticesPerStrand
		)
	{
		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;
		
		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				//Chunk containing all the parameters of the strand
				if( name == c_HSFI )
				{
					//
					/*
					float m_RootRadius;
					float m_TipRadius;
					float m_Opacity;
					float m_Specular;
					float m_Gloss;
					float m_AmbDiff;

					maVector3d m_RootColor;
					maVector3d m_TipColor;
					maVector3d m_SurfaceNormal;
					*/

					chChunkParserUtil::Read(i_Reader, o_HairStrand.Material.m_RootRadius);
					chChunkParserUtil::Read(i_Reader, o_HairStrand.Material.m_TipRadius);
					chChunkParserUtil::Read(i_Reader, o_HairStrand.Material.m_Opacity);
					chChunkParserUtil::Read(i_Reader, o_HairStrand.Material.m_Specular);
					chChunkParserUtil::Read(i_Reader, o_HairStrand.Material.m_Gloss);
					chChunkParserUtil::Read(i_Reader, o_HairStrand.Material.m_AmbientDiffuse);

					chChunkParserUtil::Read(i_Reader, o_HairStrand.Material.m_RootColor);
					chChunkParserUtil::Read(i_Reader, o_HairStrand.Material.m_TipColor);
					chChunkParserUtil::Read(i_Reader, o_HairStrand.Material.m_SurfaceNormal);

				}
				//read in the control vertices of the strand
				else if( name == c_HSCV )
				{
					o_HairStrand.m_ControlPoints.resize( i_NumVerticesPerStrand);
					std::vector< maVector3d > tempControlPoints( i_NumVerticesPerStrand  );
					chChunkParserUtil::ReadArray( i_Reader, tempControlPoints, i_NumVerticesPerStrand );
					for( int i=0; i < i_NumVerticesPerStrand ; ++i )
					{
						o_HairStrand.m_ControlPoints[i].Position = tempControlPoints[i];
					}
				}
				i_Reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;

			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			DBG_LOG("mdlImport::read_HSTR(): Invalid chunk in " << filename.c_str());
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}
		return true;

	}

	//----------------------------------------------------------------------------
	//	Read HairInfo chunk from a binary file
	//	Returns true on success, false if the chunk need be skipped
	//	On error, throws an exception
	//----------------------------------------------------------------------------
	bool ReadHRFO(	
		chReader& i_Reader,
		chDefs::Version i_Version,
		chDefs::Size i_Size,
		mdlHairInfo& o_HairInfo,
		mdlMatInfoTable *i_MaterialTable
		)
	{
		//Log the chunk
		if( mdlDefs::GetVerboseMode() )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			DBG_TEXT("");
			DBG_TEXT("mdlMeshParser::ReadHRFO loading from %s" << filename.c_str());
		}

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;
		
		//have we read the NINT  chunk?
		bool did_read_NINT = false;

		try
		{
			while( i_Reader.ReadChunkHeader(name, version, size) )
			{
				//HairName
				if( name == c_NNAM )
				{
					i_Reader.Read(o_HairInfo.m_HairName);
				}
				//Initialization Variables
				else if( name == c_NINT )
				{
					did_read_NINT = true;
					//number of vertices in the HairInfo (bga - do we need this anymore?)
					envType::UInt32 num;
					i_Reader.Read(num);
					o_HairInfo.m_nHairVertices = static_cast<int> ( num );

					//number of strands in the HairInfo
					i_Reader.Read(num);
					//reserve the strands to that many number
					o_HairInfo.m_Strands.reserve( num );

					//number of vertices per hair strands
					i_Reader.Read(num);
					o_HairInfo.m_nVerticesPerStrand = static_cast<int> ( num );
				}
				//Hair Strands
				else if( name == c_HSTR )
				{
					//need to have read the NINT chunk first
					if( !did_read_NINT )
					{
						return false;
					}

					mdlHairStrand strand;
					//Parse the hair strand chunk
					bool bVal = ReadHSTR( i_Reader, version, size, strand, o_HairInfo.m_nVerticesPerStrand  );
					o_HairInfo.m_Strands.push_back(strand);
					DBG_ASSERT( bVal, "ReadHSTR should throw exception on error" );
				}
				else if (name == c_MTLS )
				{
					std::string material_name;
					i_Reader.Read(material_name);

					if (i_MaterialTable)
					{
						mdlMatInfoTable::const_iterator it = i_MaterialTable->find(material_name);
						if (it != i_MaterialTable->end())
							o_HairInfo.m_Material = it->second;
						else
						{
							std::string filename;
							fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
							DBG_LOG("mdlImport::read_GFRG(): Did not find material id '" << material_name.c_str() << "' in table in " << filename.c_str());
							throw mdlInvalidModelFileX(i_Reader.GetLocator());
						}
					}
					else
					{
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
						DBG_LOG("mdlImport::read_HRFO(): Using material ids (" << material_name.c_str() << "), but no material table given in " << filename.c_str());
						throw mdlInvalidModelFileX(i_Reader.GetLocator());
					}

				}
				i_Reader.FinishChunk();
			}
		}
		catch ( const chInvalidChunkX& i_Ex )
		{
			i_Ex;

			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Reader.GetLocator(), filename);
			DBG_LOG("mdlImport::read_GFRG(): Invalid chunk in " << filename.c_str());
			throw mdlInvalidModelFileX(i_Reader.GetLocator());
		}
		return true;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void WriteHSTR( chWriter& o_Writer, const mdlHairStrand& i_HairStrand, int i_NumVerticesPerStrand )
	{
		//each hair strand
		o_Writer.WriteChunkHeader(c_HSTR, 0, true);

		// params of the hair strands
		o_Writer.WriteChunkHeader(c_HSFI, 0, false);
		chChunkParserUtil::Write(o_Writer, i_HairStrand.Material.m_RootRadius);
		chChunkParserUtil::Write(o_Writer, i_HairStrand.Material.m_TipRadius);
		chChunkParserUtil::Write(o_Writer, i_HairStrand.Material.m_Opacity);
		chChunkParserUtil::Write(o_Writer, i_HairStrand.Material.m_Specular);
		chChunkParserUtil::Write(o_Writer, i_HairStrand.Material.m_Gloss);
		chChunkParserUtil::Write(o_Writer, i_HairStrand.Material.m_AmbientDiffuse);

		chChunkParserUtil::Write(o_Writer, i_HairStrand.Material.m_RootColor);
		chChunkParserUtil::Write(o_Writer, i_HairStrand.Material.m_TipColor);
		chChunkParserUtil::Write(o_Writer, i_HairStrand.Material.m_SurfaceNormal);
		o_Writer.FinishChunk();

		//control vertices of the hair strand
		o_Writer.WriteChunkHeader(c_HSCV, 0, false);
		for( int i=0, j=0; i < i_NumVerticesPerStrand; ++i )
		{
			chChunkParserUtil::Write(o_Writer, i_HairStrand.m_ControlPoints[i].Position);
		}
		o_Writer.FinishChunk();//c_HSCV

		o_Writer.FinishChunk();//c_HSTR
	}

	//----------------------------------------------------------------------------
	//	Write HairInfo chunk to a binary file.
	//----------------------------------------------------------------------------
	void WriteHRFO( chWriter& o_Writer, const mdlHairInfo& i_HairInfo )
	{
		//Main  HairInfo chunk
		o_Writer.WriteChunkHeader(c_HRFO, 0, true);

		// HairName
		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		chChunkParserUtil::Write(o_Writer, i_HairInfo.m_HairName);
		o_Writer.FinishChunk();

		envType::UInt32 num;
		//Initialization params of the HairInfo
		o_Writer.WriteChunkHeader(c_NINT, 0, false);
		//number of hairvertices
		num = i_HairInfo.m_nHairVertices;
		o_Writer.Write(num);
		//number of strands
		num = i_HairInfo.m_Strands.size();
		o_Writer.Write(num);
		//number of control vertices per strand
		num = i_HairInfo.m_nVerticesPerStrand;
		o_Writer.Write( num );
		o_Writer.FinishChunk();

		//hair strands
		std::vector< mdlHairStrand >::const_iterator hit;
		for( hit = i_HairInfo.m_Strands.begin(); hit != i_HairInfo.m_Strands.end(); ++hit )
		{
			//write each strand
			const mdlHairStrand &strand = *hit;
			WriteHSTR( o_Writer, strand, i_HairInfo.m_nVerticesPerStrand );
		}

		// Write hair material, if it has been defined
		if (i_HairInfo.m_Material)
		{
			//m_Material
			o_Writer.WriteChunkHeader(c_MTLS, 0, false);
			chChunkParserUtil::Write(o_Writer, i_HairInfo.m_Material->m_Info.GetMaterialName());
			o_Writer.FinishChunk();
		}

		o_Writer.FinishChunk(); //c_HRFO
	}
}	// end of mdlHairParser namespace

