/*****************************************************************************
**  chMemWriterTest.cpp
**
**      chMemWriter writes chunks into memory exactly as chBinWriter writes
**      them into a file.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#ifndef CH_MEMWRITER_HPP
#include "Core/ch/chMemWriter.hpp"
#endif
#ifndef CH_BINREADER_HPP
#include "Core/ch/chBinReader.hpp"
#endif
#ifndef CH_BINWRITER_HPP
#include "Core/ch/chBinWriter.hpp"
#endif
#ifndef GF_FILEBIN_HPP
#include "Core/gf/gfFileBin.hpp"
#endif

#include <gtest/gtest.h>

#include <fstream>
#include <iterator>
#include <vector>

namespace
{

const chDefs::Name c_Outer = chDefs::MakeName('O', 'u', 't', 'r');
const chDefs::Name c_Inner = chDefs::MakeName('I', 'n', 'n', 'r');
const uint8_t c_Raw[7] = { 1, 2, 3, 4, 5, 6, 7 };

void WriteSample(chWriter& o_Writer, float i_Float = 2.5f)
{
	o_Writer.WriteChunkHeader(c_Outer, 3, true);
	o_Writer.WriteChunkHeader(c_Inner, 7, false);
	o_Writer.Write(int32_t(-42));
	o_Writer.Write(i_Float);
	o_Writer.Write(true);
	o_Writer.Write("abc");
	o_Writer.Write(std::string("text string"));
	o_Writer.Write(c_Raw, int64_t(sizeof(c_Raw)));
	o_Writer.FinishChunk();
	o_Writer.FinishChunk();
}

std::vector<uint8_t> ReadFileBytes(const std::filesystem::path& i_Path)
{
	std::ifstream in(i_Path, std::ios::binary);
	return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

}	// namespace

TEST(chMemWriter, MatchesBinWriter)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("Bin.bin");
	fsFileUtil::CreateFile(file);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		chBinWriter writer(write_file);
		WriteSample(writer);
	}
	const std::vector<uint8_t> file_bytes = ReadFileBytes(dir.GetPath() / "Bin.bin");

	chMemWriter mem;
	WriteSample(mem);
	const std::vector<uint8_t>& mem_bytes = mem.GetBytes();

	ASSERT_FALSE(mem_bytes.empty());
	ASSERT_GE(file_bytes.size(), mem_bytes.size());
	EXPECT_TRUE(std::equal(mem_bytes.begin(), mem_bytes.end(), file_bytes.end() - mem_bytes.size()));
}

TEST(chMemWriter, Deterministic)
{
	chMemWriter a, b, c;
	WriteSample(a);
	WriteSample(b);
	WriteSample(c, 2.75f);
	EXPECT_EQ(a.GetBytes(), b.GetBytes());
	EXPECT_NE(a.GetBytes(), c.GetBytes());
}

TEST(chMemWriter, ReadsBack)
{
	chMemWriter mem;
	WriteSample(mem);

	//	as a file: the header, which says the strings are UTF-8, then the
	//	chunks
	ctTempDir dir;
	fsLocator file = dir.GetLocator("Mem.bin");
	fsFileUtil::CreateFile(file);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		write_file.WriteHeader();
		write_file.Write(int64_t(mem.GetBytes().size()), mem.GetBytes().data());
	}

	gfFileBin read_file(file, fsFileStream::e_ReadOnly);
	read_file.ReadHeader();
	chBinReader reader(read_file);

	chDefs::Name name = 0;
	chDefs::Version version = 0;
	chDefs::Size size = 0;
	ASSERT_TRUE(reader.ReadChunkHeader(name, version, size));
	EXPECT_EQ(c_Outer, name);
	EXPECT_EQ(3, version);
	ASSERT_TRUE(reader.ReadChunkHeader(name, version, size));
	EXPECT_EQ(c_Inner, name);
	EXPECT_EQ(7, version);

	int32_t i = 0;
	float f = 0;
	bool b = false;
	std::string s;
	std::string w;
	uint8_t raw[7] = {};
	reader.Read(i);
	reader.Read(f);
	reader.Read(b);
	reader.Read(s);
	reader.ReadLegacyWideString(w);
	reader.Read(raw, int64_t(sizeof(raw)));
	EXPECT_EQ(-42, i);
	EXPECT_EQ(2.5f, f);
	EXPECT_TRUE(b);
	EXPECT_EQ("abc", s);
	EXPECT_EQ("text string", w);
	EXPECT_TRUE(std::equal(raw, raw + 7, c_Raw));
	reader.FinishChunk();
	reader.FinishChunk();
}

TEST(chMemWriter, Locator)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("Doc.mab");
	chMemWriter mem(file);
	EXPECT_TRUE(mem.GetLocator() == file);
}
