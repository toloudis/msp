/****************************************************************************\
**  rmpDataParserUtil.cpp
**   
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/rmp/private/rmpDataParserUtil.hpp"
#include "Support/rmp/rmpObject.hpp"

#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/ch/chExceptionX.hpp"
#include "Core/ch/chReader.hpp"

//============================================================================
//============================================================================
namespace rmpDataParserUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	namespace
	{
		const chDefs::Name c_RMPD = chDefs::MakeName('R', 'M', 'P', 'D');	// +-- ramp data

		//------------------------------------------------------------------------
		//   ReadData
		//------------------------------------------------------------------------
		void read_ramp_data( chReader& i_Reader,
							 chDefs::Version i_Version,
							 chDefs::Size i_Size,
							 rmpObject& o_RampData )
		{
			if( i_Version > 0 )
			{
				o_RampData.m_Data.m_Gradient.Read(i_Reader);
				o_RampData.m_Data.m_Shape.Read(i_Reader);
				o_RampData.m_Data.m_Interpolation.Read(i_Reader);
				o_RampData.m_Data.m_TexSize.Read(i_Reader);
				o_RampData.m_Data.m_UWave.Read(i_Reader);
				o_RampData.m_Data.m_UWaveFreq.Read(i_Reader);
				o_RampData.m_Data.m_VWave.Read(i_Reader);
				o_RampData.m_Data.m_VWaveFreq.Read(i_Reader);
				o_RampData.m_Data.m_Noise.Read(i_Reader);
				o_RampData.m_Data.m_NoiseFreq.Read(i_Reader);
			}
		}

		//------------------------------------------------------------------------
		//   WriteData
		//------------------------------------------------------------------------
		void write_ramp_data( chWriter& o_Writer,
							  const rmpObject& i_RampData )
		{
			//version 1
			i_RampData.m_Data.m_Gradient.Write(o_Writer);
			i_RampData.m_Data.m_Shape.Write(o_Writer);
			i_RampData.m_Data.m_Interpolation.Write(o_Writer);
			i_RampData.m_Data.m_TexSize.Write(o_Writer);
			i_RampData.m_Data.m_UWave.Write(o_Writer);
			i_RampData.m_Data.m_UWaveFreq.Write(o_Writer);
			i_RampData.m_Data.m_VWave.Write(o_Writer);
			i_RampData.m_Data.m_VWaveFreq.Write(o_Writer);
			i_RampData.m_Data.m_Noise.Write(o_Writer);
			i_RampData.m_Data.m_NoiseFreq.Write(o_Writer);

		}
	}

	//------------------------------------------------------------------------
	//   ReadData
	//------------------------------------------------------------------------
	void ReadData(	chReader& i_Reader,
					rmpObject& o_RampData )
	{
		chDefs::Name name;
		chDefs::Version version;
		chDefs::Size size;

		while( i_Reader.ReadChunkHeader(name, version, size) )
		{
			if ( name == c_RMPD )
			{
				read_ramp_data(i_Reader, version, size, o_RampData);	
			}
			i_Reader.FinishChunk();
		}
	}

	//------------------------------------------------------------------------
	//   ReadData for core level code i.e. prtyTextureFileName
	//------------------------------------------------------------------------
	prtyObject* ReadDataCore(	chReader& i_Reader )
	{
		rmpObject* pRampObject = new rmpObject();
		ReadData(i_Reader, *pRampObject);

		return pRampObject;
	}

	//------------------------------------------------------------------------
	//   WriteData
	//------------------------------------------------------------------------
	void WriteData(	chWriter& o_Writer,
					const rmpObject& i_RampData )
	{
		const int l_cRMPD_VERSION = 1;
		o_Writer.WriteChunkHeader( c_RMPD, l_cRMPD_VERSION, true );

		write_ramp_data(o_Writer, i_RampData);

		o_Writer.FinishChunk();
	}

	//------------------------------------------------------------------------
	//   WriteData for core level code i.e. prtyTextureFileName
	//------------------------------------------------------------------------
	void WriteDataCore(	chWriter& o_Writer,
						prtyObject* i_RampData )
	{
		rmpObject* pRampObject = dynamic_cast<rmpObject*>(i_RampData);
		if(!pRampObject)
			return;

		WriteData(o_Writer, *pRampObject);
	}

}  //end rmpDataParserUtil namespace