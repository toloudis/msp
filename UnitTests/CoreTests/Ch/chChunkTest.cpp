/*****************************************************************************
**  chChunkTest.cpp
**
**      Round-trips a tree of chunks through the binary and text chunk
**      writers and readers.
**
**      Ported from TestTerawatt/App/chTest/chTestMain.cpp.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#ifndef CH_BINREADER_HPP
#include "Core/ch/chBinReader.hpp"
#endif
#ifndef CH_BINWRITER_HPP
#include "Core/ch/chBinWriter.hpp"
#endif
#ifndef CH_TXTREADER_HPP
#include "Core/ch/chTxtReader.hpp"
#endif
#ifndef CH_TXTWRITER_HPP
#include "Core/ch/chTxtWriter.hpp"
#endif
#ifndef CH_EXCEPTIONX_HPP
#include "Core/ch/chExceptionX.hpp"
#endif
#ifndef GF_FILEBIN_HPP
#include "Core/gf/gfFileBin.hpp"
#endif
#ifndef GF_FILETXT_HPP
#include "Core/gf/gfFileTxt.hpp"
#endif

#include <gtest/gtest.h>

#include <map>
#include <random>
#include <string>
#include <vector>

namespace
{

const chDefs::Name c_ChunkName = chDefs::MakeName('C', 'h', 'n', 'k');

struct Chunk
{
	chDefs::Name m_Name = c_ChunkName;
	chDefs::Version m_Version = 0;

	std::vector<Chunk> m_Children;

	int8_t m_Data1 = 0;
	uint8_t m_Data2 = 0;
	int16_t m_Data3 = 0;
	int32_t m_Data4 = 0;
	float m_Data5 = 0;
	std::string m_Data6;
	std::string m_Data7;
};

//	MakeTree builds a 3-level tree: a root with i_NumChildren containers,
//	each holding i_NumGrandChildren leaf chunks filled with data.
//
Chunk MakeTree(int i_NumChildren, int i_NumGrandChildren)
{
	std::mt19937 rng(1234);
	chDefs::Version version = 1;

	auto make_leaf = [&]()
	{
		Chunk leaf;
		leaf.m_Version = version++;
		leaf.m_Data1 = int8_t(rng());
		leaf.m_Data2 = uint8_t(rng());
		leaf.m_Data3 = int16_t(rng());
		leaf.m_Data4 = int32_t(rng());
		// whole numbers, so the text format stores them exactly
		leaf.m_Data5 = float(rng() % 100000);
		leaf.m_Data6 = "single-byte string";
		leaf.m_Data7 = u8"text string Crème brûlée";
		return leaf;
	};

	Chunk root;
	root.m_Version = version++;
	root.m_Children.resize(i_NumChildren);
	for( Chunk& child : root.m_Children )
	{
		child.m_Version = version++;
		for( int i = 0 ; i < i_NumGrandChildren ; i++ )
			child.m_Children.push_back(make_leaf());
	}
	return root;
}

void WriteChunk(chWriter& io_Writer, const Chunk& i_Chunk)
{
	bool container = !i_Chunk.m_Children.empty();
	io_Writer.WriteChunkHeader(i_Chunk.m_Name, i_Chunk.m_Version, container);

	if( container )
	{
		for( const Chunk& child : i_Chunk.m_Children )
			WriteChunk(io_Writer, child);
	}
	else
	{
		io_Writer.Write(i_Chunk.m_Data1);
		io_Writer.Write(i_Chunk.m_Data2);
		io_Writer.Write(i_Chunk.m_Data3);
		io_Writer.Write(i_Chunk.m_Data4);
		io_Writer.Write(i_Chunk.m_Data5);
		io_Writer.Write(i_Chunk.m_Data6.c_str());
		// a text string (an itString before), read back with ReadLegacyWideString
		io_Writer.Write(i_Chunk.m_Data7);
	}

	io_Writer.FinishChunk();
}

//	ReadChunk reads the chunk at the reader's position into o_Chunk.
//	A real parser would know from the chunk name whether it is a
//	container; here the first two levels are containers.
//
// \return false if there are no more chunks at this level
//
bool ReadChunk(chReader& io_Reader, Chunk& o_Chunk, int i_Level)
{
	chDefs::Size size;
	if( !io_Reader.ReadChunkHeader(o_Chunk.m_Name, o_Chunk.m_Version, size) )
		return false;

	if( i_Level < 2 )
	{
		Chunk child;
		while( ReadChunk(io_Reader, child, i_Level + 1) )
		{
			o_Chunk.m_Children.push_back(child);
			child = Chunk();
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
		io_Reader.ReadLegacyWideString(o_Chunk.m_Data7);
	}

	io_Reader.FinishChunk();
	return true;
}

//	ExpectSameTree compares two chunk trees. The text format does not
//	store versions (they read back as 0), so i_CompareVersions is off
//	for it.
//
void ExpectSameTree(const Chunk& i_Expected, const Chunk& i_Actual, bool i_CompareVersions, const std::string& i_Path = "root")
{
	SCOPED_TRACE(i_Path);
	EXPECT_EQ(i_Expected.m_Name, i_Actual.m_Name);
	if( i_CompareVersions )
		EXPECT_EQ(i_Expected.m_Version, i_Actual.m_Version);

	ASSERT_EQ(i_Expected.m_Children.size(), i_Actual.m_Children.size());
	if( !i_Expected.m_Children.empty() )
	{
		for( size_t i = 0 ; i < i_Expected.m_Children.size() ; i++ )
			ExpectSameTree(i_Expected.m_Children[i], i_Actual.m_Children[i], i_CompareVersions, i_Path + "/" + std::to_string(i));
		return;
	}

	EXPECT_EQ(i_Expected.m_Data1, i_Actual.m_Data1);
	EXPECT_EQ(i_Expected.m_Data2, i_Actual.m_Data2);
	EXPECT_EQ(i_Expected.m_Data3, i_Actual.m_Data3);
	EXPECT_EQ(i_Expected.m_Data4, i_Actual.m_Data4);
	EXPECT_EQ(i_Expected.m_Data5, i_Actual.m_Data5);
	EXPECT_EQ(i_Expected.m_Data6, i_Actual.m_Data6);
	EXPECT_EQ(i_Expected.m_Data7, i_Actual.m_Data7);
}

}

TEST(chChunk, BinaryRoundTrip)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("ChunkTest.bin");
	fsFileUtil::CreateFile(file);

	const Chunk written = MakeTree(20, 5);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		write_file.WriteHeader();
		chBinWriter writer(write_file);
		WriteChunk(writer, written);
	}

	Chunk read;
	{
		gfFileBin read_file(file, fsFileStream::e_ReadOnly);
		read_file.ReadHeader();
		chBinReader reader(read_file);
		ASSERT_NO_THROW(ReadChunk(reader, read, 0));
	}

	ExpectSameTree(written, read, true);
}

TEST(chChunk, TextRoundTrip)
{
	std::map<std::string, chDefs::Name> names;
	names["Chunk"] = c_ChunkName;

	ctTempDir dir;
	fsLocator file = dir.GetLocator("ChunkTest.txt");
	fsFileUtil::CreateFile(file);

	const Chunk written = MakeTree(20, 5);
	{
		gfFileTxt write_file(file, fsFileStream::e_WriteOnly);
		chTxtWriter writer(write_file);
		writer.SetMap(&names);
		WriteChunk(writer, written);
	}

	Chunk read;
	{
		gfFileTxt read_file(file, fsFileStream::e_ReadOnly);
		chTxtReader reader(read_file);
		reader.SetMap(&names);
		ASSERT_NO_THROW(ReadChunk(reader, read, 0));
	}

	ExpectSameTree(written, read, false);
}
