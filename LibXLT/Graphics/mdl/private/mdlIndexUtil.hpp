/****************************************************************************\
**	mdlIndexUtil.hpp
**
**		mdlIndexUtil supplies functions for handling 16 and 32 bit indices.
**	It is used to support the geometry-parsing namespaces.
**
**	StudioGPU
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
#include <map>


//============================================================================
//	Forward References
//============================================================================
class chReader;
class chWriter;


//============================================================================
//	Any of these mdlIndexUtil functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
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

	//----------------------------------------------------------------------------
	//	Remapping array
	//----------------------------------------------------------------------------
	void WriteRemap(chWriter& o_Writer,
					const std::multimap<int, int>& i_VertexRemap);
	void ReadRemap(chReader& i_Reader,
				   std::multimap<int, int>& o_VertexRemap);

	//--------------------------------------------------------------------
	// Alters value array and indices so that no duplicates
	//	are in the value array. Comparison within epsilon
	//--------------------------------------------------------------------
	template <class T>
	void CompressValues(const std::vector<T> &i_Values, 
						std::vector<T> &o_NewValues, 
						std::vector<envType::UInt32> &o_Remapping)
	{
		// n-squared algorithm, could be improved with hash table
		// or similar data structure
		const int num_values = i_Values.size();
		o_Remapping.resize(num_values);
		int ind, unique = 0;
		for (int v=0; v<num_values; ++v)
		{
			const T& value = i_Values[v];
			for (ind=0; ind<unique; ++ind)
			{
				// This comparison checks for equality within an
				// epsilon value for maVectors. Different than operator==()
				if ( (value - o_NewValues[ind]).LengthSqr() < maConstants::c_fEpsilon)
				{
					// Close enough to be equal
					o_Remapping[v] = ind;
					break;
				}
			}
			if (ind == unique)
			{
				// Didn't find a match
				o_NewValues.push_back( value );
				o_Remapping[v] = unique;
				unique++;
			}
		}
	}

	//--------------------------------------------------------------------
	// Alters value array and indices so that no duplicates
	//	are in the value array. Comparison within epsilon
	//--------------------------------------------------------------------
	template <class T>
	void CompressIndices(std::vector<T> &io_Values, 
						std::vector< std::vector<envType::UInt32> > &io_Indices)
	{
		std::vector<T> new_values;
		std::vector<envType::UInt32> remap;
		CompressValues(io_Values, new_values, remap);

		// Now, see if we shrunk up the list at all
		const int num_values = io_Values.size();
		const int unique = new_values.size();
		if (unique < num_values)
		{
			if ( mdlDefs::GetVerboseMode() )
			{
				DBG_TEXT("Shrunk list, old=" << num_values << " new=" << unique);
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

