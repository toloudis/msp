/********************************************************************************************\
**  fgmtFragmentParser.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Support/fgmt/fgmtFragmentParser.hpp"

#include "Support/fgmt/fgmtFragmentData.hpp"
#include "Support/fgmt/fgmtFragmentsAOData.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"
#include "Core/dbg/dbgLog.hpp"

namespace fgmtFragmentParser
{

namespace
{
	//========================================================================
	//========================================================================
	const chDefs::Name c_FRGD = chDefs::MakeName('F', 'R', 'G', 'D');	// fragment data
	const chDefs::Name c_FAOD = chDefs::MakeName('F', 'A', 'O', 'D');	// fragment AO data

	//========================================================================
	//   ReadFragmentData
	//========================================================================
	void ReadFragmentData(	chReader& i_Reader,
					chDefs::Version i_Version,
					fgmtFragmentData& o_Data )
	{
		if (i_Version < 5)
		{
			// LEGACY FORMAT before switching to prty (version 5).

			std::string fragment_name;
			chChunkParserUtil::Read(i_Reader, fragment_name );
			o_Data.SetFragmentName(fragment_name);

			bool flag;
			chChunkParserUtil::Read(i_Reader, flag);
			o_Data.m_bCastsShadow.SetValue(flag);
			chChunkParserUtil::Read(i_Reader, flag);
			o_Data.m_bReceivesShadow.SetValue( flag );
			chChunkParserUtil::Read(i_Reader, flag);
			o_Data.m_bShadowHull.SetValue( flag );
			chChunkParserUtil::Read(i_Reader, flag);
			o_Data.m_bDoubleSided.SetValue( flag );

			if (i_Version >= 1)
			{
				bool aoflag;
				chChunkParserUtil::Read(i_Reader, aoflag);
				o_Data.m_bReceivesOcclusion.SetValue(aoflag);
				chChunkParserUtil::Read(i_Reader, aoflag );
				o_Data.m_bIsOccluder.SetValue(aoflag);
				chChunkParserUtil::Read(i_Reader, aoflag );
				o_Data.m_bAOInherit.SetValue(aoflag);
				int aoivalue;
				chChunkParserUtil::Read(i_Reader, aoivalue);
				o_Data.m_AOTextureResolution.SetValue(aoivalue);
				float aofvalue;
				chChunkParserUtil::Read(i_Reader, aofvalue);
//				o_Data.m_AODistAtten.SetValue(aofvalue);
				chChunkParserUtil::Read(i_Reader, aofvalue);
//				o_Data.m_AOEpsilon.SetValue(aofvalue);
				chChunkParserUtil::Read(i_Reader, aofvalue);
//				o_Data.m_AOTriAtten.SetValue(aofvalue);
				chChunkParserUtil::Read(i_Reader, aofvalue);
//				o_Data.m_AOZoneRadius.SetValue(aofvalue);
				std::string strvalue;
				chChunkParserUtil::Read(i_Reader, strvalue);
				// read in to user texture, could be overridden by other user texture.
				o_Data.m_AOUserTextureName.SetValue(itString(strvalue.c_str()));

				if (i_Version >= 2)
				{
					chChunkParserUtil::Read(i_Reader, aoflag);
//					o_Data.m_bAODoubleSidedOccluder.SetValue(aoflag);

					if (i_Version >= 3)
					{
						chChunkParserUtil::Read(i_Reader, strvalue);
						if (strvalue.length() != 0)
							o_Data.m_AOUserTextureName.SetValue(itString(strvalue.c_str()));

						if (i_Version >= 4)
						{
							chChunkParserUtil::Read(i_Reader, aofvalue);
							o_Data.m_AOBlendFactor.SetValue(aofvalue);
						}
					}
				}
			}
		}
		else
		{
			std::string fragment_name;
			chChunkParserUtil::Read(i_Reader, fragment_name );
			o_Data.SetFragmentName(fragment_name);

			o_Data.m_bCastsShadow.Read(i_Reader);
			o_Data.m_bReceivesShadow.Read(i_Reader);
			o_Data.m_bShadowHull.Read(i_Reader);
			o_Data.m_bDoubleSided.Read(i_Reader);
			// added version 1
			o_Data.m_bReceivesOcclusion.Read(i_Reader);
			o_Data.m_bIsOccluder.Read(i_Reader);
			o_Data.m_bAOInherit.Read(i_Reader);
			o_Data.m_AOTextureResolution.Read(i_Reader);

			if (i_Version < 6)
			{
				prtyFloat aoDistAtten, aoEpsilon, aoTriAtten, aoZoneRadius;
				aoDistAtten.Read(i_Reader);
				aoEpsilon.Read(i_Reader);
				aoTriAtten.Read(i_Reader);
				aoZoneRadius.Read(i_Reader);

				// read in to user texture. could be overridden by other user texture.
				o_Data.m_AOUserTextureName.Read(i_Reader);
				// added version 2
				prtyBoolean aoDoubleSidedOccluder;
				aoDoubleSidedOccluder.Read(i_Reader);
				// added in version 3
				prtyFileName aoTextureName;
				aoTextureName.Read(i_Reader);
				if (aoTextureName.GetValue().GetLength() != 0)
					o_Data.m_AOUserTextureName = aoTextureName;
				// added in version 4
				o_Data.m_AOBlendFactor.Read(i_Reader);
			}
			else
			{
				// read in to user texture. could be overridden by other user texture.
				if (i_Version < 7)
					o_Data.m_AOUserTextureName.Read(i_Reader);

				// added in version 3
				prtyFileName aoTextureName;
				aoTextureName.Read(i_Reader);
				// if empty, dont overwrite prty that may have been if already read in above
				if (aoTextureName.GetValue().GetLength() != 0)
					o_Data.m_AOUserTextureName = aoTextureName;
				// added in version 4
				o_Data.m_AOBlendFactor.Read(i_Reader);
				
				// added in version 6
				o_Data.m_nSamples.Read(i_Reader);
				o_Data.m_DepthBias.Read(i_Reader);
				o_Data.m_SamplingResolution.Read(i_Reader);

				if (i_Version >= 7)
				{
					o_Data.m_bSiblingOccludeOnly.Read(i_Reader);
					o_Data.m_bSelfOccludeOnly.Read(i_Reader);
					o_Data.m_bStaticAO.Read(i_Reader);

					if (i_Version >= 8)
					{
						o_Data.m_AODistanceCutoff.Read(i_Reader);
					}
				}
			}
		}
	}

	void ReadFragmentsAOData(chReader& i_Reader,
		chDefs::Version i_Version,
		fgmtFragmentsAOData& o_AOData)
	{
		o_AOData.m_bInherit.Read(i_Reader);
		o_AOData.m_bIsOccluder.Read(i_Reader);
		o_AOData.m_bReceivesOcclusion.Read(i_Reader);
		o_AOData.m_bSelfOccludeOnly.Read(i_Reader);
		o_AOData.m_nSamples.Read(i_Reader);
		o_AOData.m_SamplingResolution.Read(i_Reader);
		o_AOData.m_DepthBias.Read(i_Reader);
		o_AOData.m_TextureResolution.Read(i_Reader);
		o_AOData.m_BlendFactor.Read(i_Reader);

		if (i_Version > 0)
		{
			// added version 1
			o_AOData.m_bTexturesInSceneFolder.Read(i_Reader);
			o_AOData.m_bStatic.Read(i_Reader);

			if (i_Version > 1)
			{
				// added version 2
				o_AOData.m_DistanceCutoff.Read(i_Reader);
			}
		}
	}

	//========================================================================
	//   WriteFragmentData
	//========================================================================
	void WriteFragmentData(	chWriter& o_Writer,
						    const fgmtFragmentData& i_Data )
	{
		static const int s_Version = 8;
		o_Writer.WriteChunkHeader( c_FRGD, s_Version, false );

		chChunkParserUtil::Write(o_Writer, i_Data.GetFragmentName() );
		i_Data.m_bCastsShadow.Write(o_Writer);
		i_Data.m_bReceivesShadow.Write(o_Writer);
		i_Data.m_bShadowHull.Write(o_Writer);
		i_Data.m_bDoubleSided.Write(o_Writer);

		// added version 1
		i_Data.m_bReceivesOcclusion.Write(o_Writer);
		i_Data.m_bIsOccluder.Write(o_Writer);
		i_Data.m_bAOInherit.Write(o_Writer);
		i_Data.m_AOTextureResolution.Write(o_Writer);
		// removed for version 6
//		i_Data.m_AODistAtten.Write(o_Writer);// obsolete data fields
//		i_Data.m_AOEpsilon.Write(o_Writer);// obsolete data fields
//		i_Data.m_AOTriAtten.Write(o_Writer);// obsolete data fields
//		i_Data.m_AOZoneRadius.Write(o_Writer);// obsolete data fields
//		i_Data.m_AOTextureName.Write(o_Writer);

		// added version 2, removed version 6
//		i_Data.m_bAODoubleSidedOccluder.Write(o_Writer);

		// added in version 3
		i_Data.m_AOUserTextureName.Write(o_Writer);
		// added in version 4
		i_Data.m_AOBlendFactor.Write(o_Writer);

		// added in version 6
		i_Data.m_nSamples.Write(o_Writer);
		i_Data.m_DepthBias.Write(o_Writer);
		i_Data.m_SamplingResolution.Write(o_Writer);

		// added in version 7
		i_Data.m_bSiblingOccludeOnly.Write(o_Writer);
		i_Data.m_bSelfOccludeOnly.Write(o_Writer);
		i_Data.m_bStaticAO.Write(o_Writer);

		// added in version 8
		i_Data.m_AODistanceCutoff.Write(o_Writer);

		o_Writer.FinishChunk();

	}

	void WriteFragmentsAOData(chWriter& o_Writer,
		const fgmtFragmentsAOData& i_AOData)
	{
		static const int s_Version = 2;
		o_Writer.WriteChunkHeader( c_FAOD, s_Version, false );

		i_AOData.m_bInherit.Write(o_Writer);
		i_AOData.m_bIsOccluder.Write(o_Writer);
		i_AOData.m_bReceivesOcclusion.Write(o_Writer);
		i_AOData.m_bSelfOccludeOnly.Write(o_Writer);
		i_AOData.m_nSamples.Write(o_Writer);
		i_AOData.m_SamplingResolution.Write(o_Writer);
		i_AOData.m_DepthBias.Write(o_Writer);
		i_AOData.m_TextureResolution.Write(o_Writer);
		i_AOData.m_BlendFactor.Write(o_Writer);

		// version 1
		i_AOData.m_bTexturesInSceneFolder.Write(o_Writer);
		i_AOData.m_bStatic.Write(o_Writer);

		// version 2
		i_AOData.m_DistanceCutoff.Write(o_Writer);

		o_Writer.FinishChunk();
	}
}



//========================================================================
//   ReadFragmentData
//========================================================================
void ReadFragmentData(	chReader& i_Reader,
				chDefs::Version i_Version,
				chDefs::Size i_Size,
				std::vector<fgmtFragmentData>& o_Fragments,
				fgmtFragmentsAOData& o_AOData)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;

	while( i_Reader.ReadChunkHeader(name, version, size) )
	{
		if (name == c_FAOD)
		{
			ReadFragmentsAOData(i_Reader, version, o_AOData);
		}
		else if (name == c_FRGD)
		{
			fgmtFragmentData data;
			ReadFragmentData(i_Reader, version, data);
			o_Fragments.push_back(data);
		}
		i_Reader.FinishChunk();
	}
}

//========================================================================
//   WriteFragmentData
//========================================================================
void WriteFragmentData(	chWriter& o_Writer,
					  const std::vector<fgmtFragmentData>& i_Fragments,
					  const fgmtFragmentsAOData& i_AOData)
{
	WriteFragmentsAOData(o_Writer, i_AOData);

	int num_fragments = i_Fragments.size();
	//DBG_LOG1("Num fragments writing: %d", num_fragments);
	for (int i=0; i<num_fragments; i++)
	{
		// Write fragment info
		WriteFragmentData( o_Writer, i_Fragments[i] );
	}
}


}	// end of namespace
