/****************************************************************************\
**  mdlIndexUtil.hpp
**
**      mdlIndexUtil supplies functions for handling 16 and 32 bit indices.
**	It is used to support the geometry-parsing namespaces.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_INDEXUTIL_HPP
#error mdlIndexUtil.hpp multiply included
#endif
#define MDL_INDEXUTIL_HPP

#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 
#ifndef MA_CONSTANTS_HPP
#include "Core/Ma/maConstants.hpp"
#endif 

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class chReader;

//----------------------------------------------------------------------------
//	Any of these mdlIndexUtil functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//----------------------------------------------------------------------------
namespace mdlIndexUtil
{
	//------------------------------------------------------------------------
	// read 16 or 32 bit number returning a 32-bit integer.
	//------------------------------------------------------------------------
    envType::UInt32 ReadNumber(chReader& i_Reader, bool b32Bit);
	
	//------------------------------------------------------------------------
	// Read index size (4 or 2) and return bool if 32-bit
	//------------------------------------------------------------------------
    bool ReadIndexSize(chReader& i_Reader);

	//------------------------------------------------------------------------
	// Read array of indices - handling differences between
	//	16 and 32 bit indices.
	//------------------------------------------------------------------------
    void ReadIndexSet(chReader& i_Reader, 
						std::vector<envType::UInt32> &o_Set, 
						int i_NumIndices, 
						bool i_b32Bit);

	//--------------------------------------------------------------------
	// Alters value array and indices so that no duplicates
	//	are in the value array. Comparison within epsilon
	//--------------------------------------------------------------------
	template <class T>
	void ConvertIndices(std::vector<T> &io_Values, 
						std::vector< std::vector<envType::UInt32> > &io_Indices)
	{
		// n-squared algorithm, could be improved with hash table
		// or similar data structure
		std::vector<T> new_values;
		int unique = 0;
		const int num_values = io_Values.size();
		std::vector<envType::UInt32> remap(num_values);
		for (int v=0; v<num_values; ++v)
		{
			const T& value = io_Values[v];
			int i;
			for (i=0; i<unique; ++i)
			{
				// This comparison checks for equality within an
				// epsilon value for maVectors. Different than operator==()
				if ( (value - new_values[i]).LengthSqr() < maConstants::c_fEpsilon)
				{
					// Close enough to be equal
					remap[v] = i;
					break;
				}
			}
			if (i == unique)
			{
				// Didn't find a match
				new_values.push_back( value );
				remap[v] = unique;
				unique++;
			}
		}

		// Now, see if we shrunk up the list at all
		if (unique < num_values)
		{
			if( mdlDefs::GetVerboseMode() )
			{
				dbgLog::Write("Shrunk list, old %d new %d", num_values, unique);
			}

			// If so, then remap indices
			std::vector< std::vector<envType::UInt32> >::iterator list_it;
			std::vector<envType::UInt32>::iterator it;
			for (list_it = io_Indices.begin(); list_it != io_Indices.end(); ++list_it)
			{
				for (it = list_it->begin(); it != list_it->end(); ++it)
				{
					(*it) = remap[*it];
				}
			}

			// Return the new, smaller list
			io_Values = new_values;
		}
	}


}

