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
	uint32_t ReadNumber(chReader& i_Reader, bool b32Bit)
	{
		uint32_t num;
		if (b32Bit)
		{
			i_Reader.Read(num);
		}
		else
		{
			uint16_t num16;
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
		uint16_t num;
		i_Reader.Read(num);
		return (num == 4);
	}

	//------------------------------------------------------------------------
	// Read array of indices - handling differences between
	//	16 and 32 bit indices.
	//------------------------------------------------------------------------
	void ReadIndexSet(chReader& i_Reader, 
						std::vector<uint32_t> &o_Set, 
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
					i_Reader.Read(&o_Set[0], i_NumIndices * sizeof(uint32_t));
				}
			}
			else
			{
				// Read a 16 bit array at once, then set each value into our 32-bit array
				std::vector<uint16_t> set_16(i_NumIndices);
				i_Reader.Read(&set_16[0], i_NumIndices * sizeof(uint16_t));
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
		int32_t num = i_VertexRemap.size();
		o_Writer.Write(num);
		std::multimap<int, int>::const_iterator it;
		for (it = i_VertexRemap.begin(); it != i_VertexRemap.end(); ++it)
		{
			chChunkParserUtil::Write(o_Writer, int32_t(it->first));
			chChunkParserUtil::Write(o_Writer, int32_t(it->second));
		}
	}
	void ReadRemap(chReader& i_Reader,
					std::multimap<int, int>& o_VertexRemap)
	{
		int32_t num = 0;
		i_Reader.Read(num);
		if (num > 0)
		{
			// Read all values in one step, then sort into map
			std::vector<int32_t> map_vals(num*2);
			chChunkParserUtil::ReadArray(i_Reader, map_vals, num*2);

			int32_t *ptr = &map_vals[0];
			int32_t first;
			for (int32_t cur = 0; cur < num; ++cur)
			{
				first = *ptr++;					// it->first
				o_VertexRemap.insert(std::pair<int, int>(first, *ptr++)); // it->second
						
			}
		}
	}

}	// end of namespace

