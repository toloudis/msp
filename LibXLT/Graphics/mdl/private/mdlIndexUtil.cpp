/****************************************************************************\
**	mdlIndexUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlIndexUtil.hpp"
#include "Graphics/mdl/private/mdlDefs.hpp"

#include "Core/ch/chReader.hpp"
#include "Core/ch/chWriter.hpp"
#include "Core/Ch/chChunkParserUtil.hpp"


//============================================================================
//	Any of these mdlIndexUtil functions might throw a mdlInvalidModelFileX or
//	one of the fs exceptions.
//============================================================================
namespace mdlIndexUtil
{
	//------------------------------------------------------------------------
	// read 16 or 32 bit number returning a 32-bit integer.
	//------------------------------------------------------------------------
	envType::UInt32 ReadNumber(chReader& i_Reader, bool b32Bit)
	{
		envType::UInt32 num;
		if (b32Bit)
		{
			i_Reader.Read(num);
		}
		else
		{
			envType::UInt16 num16;
			i_Reader.Read(num16);
			num = num16;
		}
		return num;
	}

	//------------------------------------------------------------------------
	// Read index size (4 or 2) and return bool if 32-bit
	//------------------------------------------------------------------------
	bool ReadIndexSize(chReader& i_Reader)
	{
		envType::UInt16 num;
		i_Reader.Read(num);
		return (num == 4);
	}

	//------------------------------------------------------------------------
	// Read array of indices - handling differences between
	//	16 and 32 bit indices.
	//------------------------------------------------------------------------
	void ReadIndexSet(chReader& i_Reader, 
						std::vector<envType::UInt32> &o_Set, 
						int i_NumIndices, 
						bool i_b32Bit)
	{
		if (i_Reader.CanReadBinaryData())
		{
			if (i_b32Bit)
			{
				if (i_NumIndices > 0)
				{
					// Read the whole array at once. 
					i_Reader.Read(&o_Set[0], i_NumIndices * sizeof(envType::UInt32));
				}
			}
			else
			{
				// Read a 16 bit array at once, then set each value into our 32-bit array
				std::vector<envType::UInt16> set_16(i_NumIndices);
				i_Reader.Read(&set_16[0], i_NumIndices * sizeof(envType::UInt16));
				for (int cur = 0 ; cur < i_NumIndices ; ++cur )
				{
					o_Set[cur] = set_16[cur];
				}
			}
		}
		else
		{
			for (int cur = 0 ; cur < i_NumIndices ; ++cur )
				o_Set[cur] = ReadNumber(i_Reader, i_b32Bit);
		}
	}

	//----------------------------------------------------------------------------
	//	Remapping array
	//----------------------------------------------------------------------------
	void WriteRemap(chWriter& o_Writer,
					 const std::multimap<int, int>& i_VertexRemap)
	{
		envType::Int32 num = i_VertexRemap.size();
		o_Writer.Write(num);
		std::multimap<int, int>::const_iterator it;
		for (it = i_VertexRemap.begin(); it != i_VertexRemap.end(); ++it)
		{
			chChunkParserUtil::Write(o_Writer, envType::Int32(it->first));
			chChunkParserUtil::Write(o_Writer, envType::Int32(it->second));
		}
	}
	void ReadRemap(chReader& i_Reader,
					std::multimap<int, int>& o_VertexRemap)
	{
		envType::Int32 num = 0;
		i_Reader.Read(num);
		if (num > 0)
		{
			// Read all values in one step, then sort into map
			std::vector<envType::Int32> map_vals(num*2);
			chChunkParserUtil::ReadArray(i_Reader, map_vals, num*2);

			envType::Int32 *ptr = &map_vals[0];
			envType::Int32 first;
			for (envType::Int32 cur = 0; cur < num; ++cur)
			{
				first = *ptr++;					// it->first
				o_VertexRemap.insert(std::pair<int, int>(first, *ptr++)); // it->second
						
			}
		}
	}

}	// end of namespace

