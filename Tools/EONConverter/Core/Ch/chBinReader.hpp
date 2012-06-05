/*****************************************************************************
**  chBinReader.hpp
**
**      chBinReader defines the chBinReader class, which is a chReader that
**	operates on binary chunk files.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CH_BINREADER_HPP
#error chBinReader.hpp multiply included
#endif
#define CH_BINREADER_HPP

#ifndef CH_READER_HPP
#include "Core/ch/chReader.hpp"
#endif

#include <stack>
#include <vector>

class gfFileBin;

class chBinReader : public chReader
{
	public:

		//====================================================================
		//====================================================================
		chBinReader(gfFileBin& io_File);

		//====================================================================
		//====================================================================
		virtual ~chBinReader();

		//====================================================================
		//	ReadChunkHeader reads a chunk header in the file at the
		//	current position.  If there is a problem with the header,
		//	it will throw a chInvalidChunkX.  Returns true if more chunks to 
		//	read, false if no more.
		//	The size is the size of the chunk not including the header.
		//====================================================================
		virtual bool ReadChunkHeader(	chDefs::Name& o_Name, 
										chDefs::Version& o_Version,
										chDefs::Size& o_Size);

		//====================================================================
		//	FinishChunk will cause the reader to advance to the next position
		//	after the end of the current chunk.  Every ReadChunkHeader
		//	must be matched by a FinishChunk.
		//====================================================================
		virtual void FinishChunk();

		//====================================================================
		//	Read functions - These will be overridden to read the data 
		//	given in the appropriate format.
		//	It would have been nice to make these template member functions,
		//	but I think there would be problems getting them instantiated
		//	so that they are part of the abstract base interface.
		//====================================================================
		virtual void Read(envType::Int8& o_Val);
		virtual void Read(envType::UInt8& o_Val);
		virtual void Read(envType::Int16& o_Val);
		virtual void Read(envType::UInt16& o_Val);
		virtual void Read(envType::Int32& o_Val);
		virtual void Read(envType::UInt32& o_Val);
		virtual void Read(envType::Int64& o_Val);
		virtual void Read(envType::UInt64& o_Val);
		virtual void Read(envType::Float32& o_Val);
		virtual void Read(envType::Float64& o_Val);

		//====================================================================
		//	This Read reads a string called either "TRUE" or "FALSE" and 
		//  converts it into a bool value.
		//====================================================================
		virtual void Read(bool& o_Val);

		//====================================================================
		//	This Read reads a NULL-terminated single-byte character string.
		//====================================================================
		virtual void Read(std::string& o_Val);

		//====================================================================
		//	This Read Reads a single-byte character string with a length
		//	given by i_Length.
		//====================================================================
		virtual void Read(std::string& o_Val, int i_Length);

		//====================================================================
		//	This Read reads a NULL-terminated Unicode character string.  
		//====================================================================
		virtual void Read(itString& o_Val);

		//====================================================================
		//	This Read Reads a Unicode character string with a length
		//	given by i_Length.
		//====================================================================
		virtual void Read(itString& o_Val, int i_Length);

		//====================================================================
		//	Read chunk of binary data; this is only for
		//	the implementations, it is not a function of chReader base class
		//====================================================================
		virtual	void Read(void* i_Data, int i_NumBytes);

		//========================================================================
		//	GetLocator returns this file's associated locator
		//========================================================================
		const fsLocator& GetLocator() const;

	private:

		gfFileBin& m_File;
		int m_FileLength;
		std::stack<int, std::vector<int> > m_ChunkEnds;

};

