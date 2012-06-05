/****************************************************************************\
**	mdlCameraAnimWriter.cpp
**
**		mdlCameraAnimWriter.hpp supplies functions used to export camera anim data into
**		our animation file formats	
**
**	StudioGPU
**	Copyright(C) 2003-8 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlCameraAnimWriter.hpp"

#include "Core/Ch/chBinWriter.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"
#include "Core/Ma/maAxisBox.hpp"
#include "Graphics/mdl/mdlExceptionX.hpp"


//============================================================================
//	Any of these mdlCameraAnimWriter functions might throw one of the fs exceptions.
//============================================================================
namespace mdlCameraAnimWriter
{
	//------------------------------------------------------------------------
	//	Chunk types
	//------------------------------------------------------------------------
	namespace
	{
		//animation camera
		const chDefs::Name c_ACAM = chDefs::MakeName('A', 'C', 'A', 'M');
		const chDefs::Name c_APKY = chDefs::MakeName('A', 'P', 'K', 'Y');
		const chDefs::Name c_ARKY = chDefs::MakeName('A', 'R', 'K', 'Y');
		const chDefs::Name c_RORD = chDefs::MakeName('R', 'O', 'R', 'D');
		const chDefs::Name c_AFCL = chDefs::MakeName('A', 'F', 'C', 'L');
		const chDefs::Name c_ACOI = chDefs::MakeName('A', 'C', 'O', 'I');
		const chDefs::Name c_HAPT = chDefs::MakeName('H', 'A', 'P', 'T');
		const chDefs::Name c_VAPT = chDefs::MakeName('V', 'A', 'P', 'T');
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	template<class T>
	struct valkey_struct
	{
		T	val;
		float	time;

		valkey_struct(T v, float t)
		{
			val = v;
			time = t;
		}
	};

	//------------------------------------------------------------------------
	// writes keys to file. doesn't write 3 keys with same value in row
	// in order to reduce file size.
	//------------------------------------------------------------------------
	template<class T>
	void WriteChannel(chWriter &o_Writer, 
		const std::map<float, T> &i_Keys,
		float i_TimeOffset,
		bool i_bWriteDeltas)
	{
		std::vector<valkey_struct<T>> valkeys;

		T  val, lastval;
		bool	bDelay = false;
		float	delay_time;

		std::map<float, T>::const_iterator it;
		for (it = i_Keys.begin(); it != i_Keys.end(); ++it)
		{
			val = it->second;

			// Compare val to previous frame, try not to 
			// write three of the same frame values in a row.
			//
			if ((it != i_Keys.begin()) && (val == lastval))
			{
				// delay this frame
				bDelay = true;
				delay_time = it->first;
			}
			else
			{
				if (bDelay)
				{
					// if delaying, write delayed frame now
					valkeys.push_back(valkey_struct<T>(lastval, delay_time));
					bDelay = false;
				}

				// write current key value
				valkeys.push_back(valkey_struct<T>(val, it->first));
				lastval = val;
			}
		}

		// Hit end of list, if still delaying frame write it now
		if (bDelay)
		{
			valkeys.push_back(valkey_struct<T>(lastval, delay_time));
		}

		// Now actually write keys
		//	
		envType::Int16 num_keys = valkeys.size();
		//cout << "Writing " << num_keys << " keys" << endl;
		o_Writer.Write( num_keys );
		for (int i=0; i<num_keys; i++)
		{
			// Offset the keys by the minimum time
			// to make an animation that starts at zero
			valkeys[i].time -= i_TimeOffset;

			// If we are creating an additive animation based on deltas,
			// subtract the first value from all other frames.
			if (i_bWriteDeltas)
			{
				// Note: this line causes a warning when using bool as the type.
				//  As long as i_bWriteDeltas is false for boolean channels, this 
				//  warning can be ignored.
				valkeys[i].val -= valkeys[0].val;
			}

			chChunkParserUtil::Write( o_Writer, valkeys[i].val );
			o_Writer.Write(valkeys[i].time);
		}
	}

	//----------------------------------------------------------------------------
	//write the camera animation data
	//o_Writer is the binary chunk writer
	//i_Callback, is the callback struct we can call  on each 
	//i_CameraAnimKeys is the animation data
	//----------------------------------------------------------------------------
	void WriteCameraAnim(
		chWriter &io_Writer, 
		WriteCameraAnimCallback  &i_Callback,		
		const std::string &i_CameraName,
		CameraKeys &i_CameraAnimKeys,
		float i_OffsetFrame)
	{	

		bool bWriteDeltas = false;

		io_Writer.WriteChunkHeader( c_ACAM, 0, true);

		io_Writer.WriteChunkHeader(c_APKY, 0, false);
		WriteChannel(io_Writer, i_CameraAnimKeys.m_TranslateKeys, i_OffsetFrame, bWriteDeltas);
		io_Writer.FinishChunk();
		size_t keySize = CameraKeys::EstimateChannelSize( i_CameraAnimKeys.m_TranslateKeys );
		float updateProgress = static_cast< float > ( keySize );
		i_Callback( updateProgress );

		io_Writer.WriteChunkHeader(c_RORD, 0, false);
		io_Writer.Write( i_CameraAnimKeys.m_RotationOrder  );
		io_Writer.FinishChunk();

		io_Writer.WriteChunkHeader(c_ARKY, 0, false);
		WriteChannel( io_Writer, i_CameraAnimKeys.m_RotateKeys, i_OffsetFrame, bWriteDeltas);
		io_Writer.FinishChunk();
		keySize = CameraKeys::EstimateChannelSize( i_CameraAnimKeys.m_RotateKeys );
		updateProgress = static_cast< float > ( keySize );
		i_Callback( updateProgress );

		io_Writer.WriteChunkHeader(c_AFCL, 0, false);
		WriteChannel( io_Writer, i_CameraAnimKeys.m_FocalLengthKeys, i_OffsetFrame, bWriteDeltas);
		io_Writer.FinishChunk();
		keySize = CameraKeys::EstimateChannelSize( i_CameraAnimKeys.m_FocalLengthKeys );
		updateProgress = static_cast< float > ( keySize );
		i_Callback( updateProgress );

		io_Writer.WriteChunkHeader(c_ACOI, 0, false);
		WriteChannel( io_Writer, i_CameraAnimKeys.m_CenterOfInterestKeys, i_OffsetFrame, bWriteDeltas);
		io_Writer.FinishChunk();
		keySize = CameraKeys::EstimateChannelSize( i_CameraAnimKeys.m_CenterOfInterestKeys );
		updateProgress = static_cast< float > ( keySize );
		i_Callback( updateProgress );

		io_Writer.WriteChunkHeader(c_HAPT, 0, false);
		WriteChannel( io_Writer, i_CameraAnimKeys.m_HorizFilmAperKeys, i_OffsetFrame, bWriteDeltas);
		io_Writer.FinishChunk();
		keySize = CameraKeys::EstimateChannelSize( i_CameraAnimKeys.m_HorizFilmAperKeys );
		updateProgress = static_cast< float > ( keySize );
		i_Callback( updateProgress );

		io_Writer.WriteChunkHeader(c_VAPT, 0, false);
		WriteChannel( io_Writer, i_CameraAnimKeys.m_VertFilmAperKeys, i_OffsetFrame, bWriteDeltas);
		io_Writer.FinishChunk();
		keySize = CameraKeys::EstimateChannelSize( i_CameraAnimKeys.m_VertFilmAperKeys );
		updateProgress = static_cast< float > ( keySize );
		i_Callback( updateProgress );

		io_Writer.FinishChunk();
	}
}
