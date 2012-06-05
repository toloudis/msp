
#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "chPackage.hpp"
#include "dbgPackage.hpp"
#include "envPackage.hpp"
#include "fsPackage.hpp"
#include "gfPackage.hpp"

#include "chBinReader.hpp"
#include "chBinWriter.hpp"
#include "chTxtReader.hpp"
#include "chTxtWriter.hpp"
#include "chExceptionX.hpp"
#include "dbgLog.hpp"
#include "fsFileStream.hpp"
#include "fsFileUtil.hpp"
#include "fsLocator.hpp"
#include "gfFileBin.hpp"
#include "gfFileTxt.hpp"
#include "gfPaths.hpp"

#include <algorithm>
#include <functional>
#include <vector>
#include <stdlib.h>

namespace
{

int l_ChunkCounter;

struct Chunk
{
	Chunk();

	//	Chunks are equal if their data and all their children are equal
	//
	bool operator == (const Chunk& i_Chunk) const;
	bool operator != (const Chunk& i_Chunk) const { return ! (*this == i_Chunk); }

	chDefs::Name m_Name;
	chDefs::Version m_Version;

	std::vector<Chunk> m_Children;
	
	envType::Int8 m_Data1;
	envType::UInt8 m_Data2;
	envType::Int16 m_Data3;
	envType::Int32 m_Data4;
	envType::Float32 m_Data5;
	std::string m_Data6;
	itString m_Data7;
};

Chunk::Chunk()
{
	m_Name = chDefs::MakeName('C', 'h', 'n', 'k');
	m_Version = l_ChunkCounter++;

	m_Data1 = rand();
	m_Data2 = rand();
	m_Data3 = rand();
	m_Data4 = rand();
	m_Data5 = rand();
	m_Data6 = "single-byte string";
	m_Data7 = "double-byte string";
	m_Data7 += 0; // null terminate
}

//	Chunks are equal if their data and all their children are equal
//
bool Chunk::operator == (const Chunk& i_Chunk) const
{
	if( m_Name != i_Chunk.m_Name )
		return false;

	// Ascii versions are always 0, so need to 
	// have different non-zero versions to be incorrect
	//
	if (( m_Version != i_Chunk.m_Version ) && (i_Chunk.m_Version > 0))
		return false;

	int num_children = m_Children.size();
	
	if( num_children != i_Chunk.m_Children.size() )
		return false;

	if( num_children > 0 )
	{
		int i;

		for( i = 0 ; i < num_children ; i++ )
		{
			if( m_Children[i] != i_Chunk.m_Children[i] ) 
				return false;
		}
	}
	else
	{
		if( m_Data1 != i_Chunk.m_Data1 ) 
			return false;

		if( m_Data2 != i_Chunk.m_Data2 ) 
			return false;

		if( m_Data3 != i_Chunk.m_Data3 ) 
			return false;

		if( m_Data4 != i_Chunk.m_Data4 ) 
			return false;

		if( m_Data5 != i_Chunk.m_Data5 ) 
			return false;

		if( m_Data6 != i_Chunk.m_Data6 ) 
			return false;

		if( m_Data7 != i_Chunk.m_Data7 ) 
			return false;
	}

	return true;
}

void write_chunk(chWriter& io_Writer, const Chunk& i_Chunk)
{
	bool container = i_Chunk.m_Children.size() > 0;
	io_Writer.WriteChunkHeader(	i_Chunk.m_Name,
								i_Chunk.m_Version,
								container);
	
	if( container )
	{
		int i;
		int num = i_Chunk.m_Children.size();

		for( i = 0 ; i < num ; i++ )
			write_chunk(io_Writer, i_Chunk.m_Children[i]);
	}
	else
	{
		io_Writer.Write(i_Chunk.m_Data1);
		io_Writer.Write(i_Chunk.m_Data2);
		io_Writer.Write(i_Chunk.m_Data3);
		io_Writer.Write(i_Chunk.m_Data4);
		io_Writer.Write(i_Chunk.m_Data5);
		io_Writer.Write(i_Chunk.m_Data6.c_str());
		io_Writer.Write(i_Chunk.m_Data7.GetString());
	}

	io_Writer.FinishChunk();
}

bool read_chunk(chReader& io_Reader, Chunk& o_Chunk, int& o_Level)
{
	chDefs::Size size;

	//	this will throw if there isn't actually a chunk to read
	if (!io_Reader.ReadChunkHeader(o_Chunk.m_Name, o_Chunk.m_Version, size))
	{
		return false;
	}
	
	bool container;

	//	in reality, we would know by reading the chunk name and our
	//	definitions whether the chunk was a container or not
	//	here we fake it
	if( o_Level < 2 )
		container = true;
	else
		container = false;

	if( container )
	{
		bool MoreChunks = true;
		while (MoreChunks)
		{
			Chunk temp;
			++o_Level;
			MoreChunks = read_chunk(io_Reader, temp, o_Level);
			--o_Level;

			if (MoreChunks)
			{
				int new_index = o_Chunk.m_Children.size();
				o_Chunk.m_Children.resize(new_index+1);
				Chunk& new_chunk = o_Chunk.m_Children[new_index];
				new_chunk = temp;
			}
		}
	}
	else
	{
		io_Reader.Read(o_Chunk.m_Data1);
		io_Reader.Read(o_Chunk.m_Data2);
		io_Reader.Read(o_Chunk.m_Data3);
		io_Reader.Read(o_Chunk.m_Data4);
		io_Reader.Read(o_Chunk.m_Data5);
		io_Reader.Read(o_Chunk.m_Data6);
		io_Reader.Read(o_Chunk.m_Data7);
	}

	io_Reader.FinishChunk();
	return true;
}

void dump_chunk(const Chunk& i_Chunk, int& o_Tab)
{
	std::string output;
	
	int i;
	for( i = 0 ; i < o_Tab ; i++ )
		output += ' ';

	char name[5];
	*(int*)name = i_Chunk.m_Name;
	name[4] = 0;

	output += name;

	DBG_LOG1("%s", output.c_str());

	int num_children = i_Chunk.m_Children.size();
	for( i = 0 ; i < num_children ; i++ )
	{
		o_Tab += 2;
		dump_chunk(i_Chunk.m_Children[i], o_Tab);
		o_Tab -= 2;
	}
}

void TestBinary()
{
	fsLocator chunk_file_locator = gfPaths::GetAppPath();
	chunk_file_locator.Push(itString("ChunkTest.bin"));

	if( fsFileUtil::FileExists(chunk_file_locator) )
		fsFileUtil::DeleteFile(chunk_file_locator);

	fsFileUtil::CreateFile(chunk_file_locator);
	
	const int num_children = 20;
	Chunk written_chunk;
	written_chunk.m_Children.resize(num_children);
	int i;
	for( i = 0 ; i < num_children ; i++ )
	{
		written_chunk.m_Children[i].m_Children.resize(5);		
	}
	
	{
		gfFileBin write_file(chunk_file_locator, fsFileStream::e_WriteOnly);
		chBinWriter writer(write_file);
		write_chunk(writer, written_chunk);
	}

	Chunk reading_chunk;

	{
		gfFileBin read_file(chunk_file_locator, fsFileStream::e_ReadOnly);
		chBinReader reader(read_file);
		int level = 0;

		try
		{
			read_chunk(reader, reading_chunk, level);
		}
		catch( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_ASSERT0(false, "Binary chunk operations failed (chInvalidChunkX)");
		}
	}

	int tab = 0;
	DBG_LOG0("written chunks***********************************");
	dump_chunk(written_chunk, tab);
	tab = 0;
	DBG_LOG0("read chunks***********************************");
	dump_chunk(reading_chunk, tab);

	DBG_ASSERT0( written_chunk == reading_chunk, "Binary chunk operations failed");
}

void TestAscii()
{
	std::map<std::string, chDefs::Name> str_map;
	str_map["Chunk"] =  chDefs::MakeName('C', 'h', 'n', 'k');

	fsLocator chunk_file_locator = gfPaths::GetAppPath();
	chunk_file_locator.Push(itString("ChunkTest.txt"));

	if( fsFileUtil::FileExists(chunk_file_locator) )
		fsFileUtil::DeleteFile(chunk_file_locator);

	fsFileUtil::CreateFile(chunk_file_locator);
	
	const int num_children = 20;
	Chunk written_chunk;
	written_chunk.m_Children.resize(num_children);
	int i;
	for( i = 0 ; i < num_children ; i++ )
	{
		written_chunk.m_Children[i].m_Children.resize(5);		
	}
	
	{
		gfFileTxt write_file(chunk_file_locator, fsFileStream::e_WriteOnly);
		chTxtWriter writer(write_file);
		writer.SetMap(&str_map);
		write_chunk(writer, written_chunk);
	}

	Chunk reading_chunk;

	{
		gfFileTxt read_file(chunk_file_locator, fsFileStream::e_ReadOnly);
		chTxtReader reader(read_file);
		reader.SetMap(&str_map);
		int level = 0;

		try
		{
			read_chunk(reader, reading_chunk, level);
		}
		catch( const chInvalidChunkX& i_Ex )
		{
			i_Ex;
			DBG_ASSERT0(false, "Ascii chunk operations failed (chInvalidChunkX)");
		}
	}

	int tab = 0;
	DBG_LOG0("written chunks***********************************");
	dump_chunk(written_chunk, tab);
	tab = 0;
	DBG_LOG0("read chunks***********************************");
	dump_chunk(reading_chunk, tab);

	DBG_ASSERT0( written_chunk == reading_chunk, "Ascii chunk operations failed");
}

void DoTests()
{
	TestBinary();
	TestAscii();
}

}


//====================================================================
//====================================================================
int main()
{
	envPackage::Init();
	dbgPackage::Init();
	fsPackage::Init();
	gfPackage::Init();
	chPackage::Init();
	
	DoTests();

	chPackage::CleanUp();
	gfPackage::CleanUp();
	fsPackage::CleanUp();
	dbgPackage::CleanUp();
	envPackage::CleanUp();
	return 0;
}