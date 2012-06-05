/*****************************************************************************
**  AnimUtil.hpp
**
**   Namespace for writing animation values such that duplicates are removed.   
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef ANIM_UTIL_HPP
#error AnimUtil.hpp multiply included
#endif
#define ANIM_UTIL_HPP

#ifndef CH_CHUNKPARSERUTIL_HPP
#include "Core/Ch/chChunkParserUtil.hpp"
#endif 


#include <map>

namespace AnimUtil
{

	//========================================================================
	//========================================================================
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

	//========================================================================
	// writes keys to file. doesn't write 3 keys with same value in row
	// in order to reduce file size.
	//========================================================================
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

}