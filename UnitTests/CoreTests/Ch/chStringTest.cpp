/*****************************************************************************
**  chStringTest.cpp
**
**      Strings in chunk files: ASCII, accented Latin, CJK and characters
**      outside the Basic Multilingual Plane, through every chunk writer
**      and reader, and files saved in the old format (UTF-16 strings in
**      binary files) still loading.
**
**      The old-format files are in ../Data (see ../Data/README.md); they
**      were written once by chBinWriter as it was before the ch readers
**      and writers moved to UTF-8, and must never be regenerated.
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
#ifndef CH_CHUNKPARSERUTIL_HPP
#include "Core/ch/chChunkParserUtil.hpp"
#endif
#ifndef CH_MEMWRITER_HPP
#include "Core/ch/chMemWriter.hpp"
#endif
#ifndef CH_TXTREADER_HPP
#include "Core/ch/chTxtReader.hpp"
#endif
#ifndef CH_TXTWRITER_HPP
#include "Core/ch/chTxtWriter.hpp"
#endif
#ifndef CH_XMLREADER_HPP
#include "Core/ch/chXMLReader.hpp"
#endif
#ifndef CH_XMLWRITER_HPP
#include "Core/ch/chXMLWriter.hpp"
#endif
#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif
#ifndef GF_FILEBIN_HPP
#include "Core/gf/gfFileBin.hpp"
#endif
#ifndef GF_FILETXT_HPP
#include "Core/gf/gfFileTxt.hpp"
#endif
#ifndef GF_FILEXML_HPP
#include "Core/gf/gfFileXML.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

namespace
{

const chDefs::Name c_Strings = chDefs::MakeName('S', 't', 'r', 's');
const chDefs::Name c_String = chDefs::MakeName('S', 't', 'r', 'g');
const int32_t c_Sentinel = 0x5A5A5A5A;

//	The strings, as UTF-8. The empty one goes only through the binary
//	formats: the text formats write an empty string as nothing at all, so
//	it can't be read back (as before this work).
const std::vector<std::string> c_Samples =
{
	"plain ASCII",
	u8"Crème brûlée à la façon",
	u8"東京の夜景",
	u8"🎬 clap 🎥",
	"",
};

bool IsASCII(const std::string& i_String)
{
	return std::all_of(i_String.begin(), i_String.end(), [](char c) { return (unsigned char)(c) < 0x80; });
}

//	One string as read back: the wide string as UTF-16 code units (exact,
//	with no conversion of its own) and the narrow one as its bytes.
struct ReadString
{
	std::u16string m_Wide;
	std::string m_Narrow;
};

std::u16string ToUTF16(const itString& i_String)
{
	return std::u16string(i_String.GetString(), i_String.GetString() + i_String.GetLength());
}

//	WriteStrings writes a container chunk holding one leaf chunk per
//	string: an index, the string as a wide string, the same string as a
//	narrow (UTF-8) string, and a sentinel, which catches a string read
//	with the wrong length.
//
//	The wide string is written null-terminated (Write(const WChar*)), not
//	with chChunkParserUtil::Write(chWriter&, const itString&), which gives
//	the length with the NUL included: in a binary file the bytes are the
//	same, but the text writers store that NUL as part of the string, and
//	it reads back with it (a bug from before this work, left to step 2 of
//	the UTF-8 plan).
void WriteStrings(chWriter& io_Writer, const std::vector<std::string>& i_Strings)
{
	io_Writer.WriteChunkHeader(c_Strings, 1, true);
	for( size_t i = 0 ; i < i_Strings.size() ; i++ )
	{
		io_Writer.WriteChunkHeader(c_String, 1, false);
		io_Writer.Write(int32_t(i));
		io_Writer.Write(itString(i_Strings[i].c_str()).GetString());
		io_Writer.Write(i_Strings[i].c_str());
		io_Writer.Write(c_Sentinel);
		io_Writer.FinishChunk();
	}
	io_Writer.FinishChunk();
}

std::vector<ReadString> ReadStrings(chReader& io_Reader)
{
	std::vector<ReadString> strings;

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	EXPECT_TRUE(io_Reader.ReadChunkHeader(name, version, size));
	EXPECT_EQ(c_Strings, name);

	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		EXPECT_EQ(c_String, name);

		int32_t index = -1;
		io_Reader.Read(index);
		EXPECT_EQ(int32_t(strings.size()), index);

		itString wide;
		chChunkParserUtil::Read(io_Reader, wide);
		std::string narrow;
		io_Reader.Read(narrow);
		strings.push_back({ ToUTF16(wide), narrow });

		int32_t sentinel = 0;
		io_Reader.Read(sentinel);
		EXPECT_EQ(c_Sentinel, sentinel);

		io_Reader.FinishChunk();
	}
	io_Reader.FinishChunk();

	return strings;
}

void ExpectStrings(const std::vector<std::string>& i_Expected, const std::vector<ReadString>& i_Read)
{
	ASSERT_EQ(i_Expected.size(), i_Read.size());
	for( size_t i = 0 ; i < i_Expected.size() ; i++ )
	{
		SCOPED_TRACE(i_Expected[i]);
		EXPECT_EQ(envString::UTF8ToUTF16(i_Expected[i]), i_Read[i].m_Wide);
		EXPECT_EQ(i_Expected[i], i_Read[i].m_Narrow);
	}
}

std::filesystem::path DataPath(const char* i_Name)
{
	// __FILE__ is this file's full path in these builds
	return std::filesystem::path(__FILE__).parent_path().parent_path() / "Data" / i_Name;
}

fsLocator ToLocator(const std::filesystem::path& i_Path)
{
	fsLocator locator;
	fsFileUtil::UnicodeStringToLocator(itString(i_Path.wstring().c_str()), locator);
	return locator;
}

std::vector<uint8_t> ReadFileBytes(const std::filesystem::path& i_Path)
{
	std::ifstream in(i_Path, std::ios::binary);
	return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

bool Contains(const std::vector<uint8_t>& i_Bytes, const std::vector<uint8_t>& i_Pattern)
{
	return std::search(i_Bytes.begin(), i_Bytes.end(), i_Pattern.begin(), i_Pattern.end()) != i_Bytes.end();
}

std::vector<uint8_t> UTF16LEWithNul(const std::string& i_String)
{
	std::vector<uint8_t> bytes;
	for( char16_t unit : envString::UTF8ToUTF16(i_String) )
	{
		bytes.push_back(uint8_t(unit & 0xFF));
		bytes.push_back(uint8_t(unit >> 8));
	}
	bytes.push_back(0);
	bytes.push_back(0);
	return bytes;
}

std::vector<uint8_t> BytesWithNul(const std::string& i_String)
{
	std::vector<uint8_t> bytes(i_String.begin(), i_String.end());
	bytes.push_back(0);
	return bytes;
}

//	The old-format files: a document-style file (the gfFileBin header, as
//	documents, materials and models have, read with ReadHeader first) and
//	one with chunks only.
struct Fixture
{
	const char* m_Name;
	bool m_bHeader;
};
const Fixture c_Fixtures[] =
{
	{ "OldFormatStringsHeader.bin", true },
	{ "OldFormatStringsNoHeader.bin", false },
};

}	// namespace

TEST(chString, OldFormatFilesLoad)
{
	for( const Fixture& fixture : c_Fixtures )
	{
		SCOPED_TRACE(fixture.m_Name);
		const std::filesystem::path path = DataPath(fixture.m_Name);
		ASSERT_TRUE(std::filesystem::exists(path)) << path.string();

		gfFileBin file(ToLocator(path), fsFileStream::e_ReadOnly);
		if( fixture.m_bHeader )
		{
			gfFileBin::Header header;
			file.ReadHeader(&header);
		}
		chBinReader reader(file);
		ExpectStrings(c_Samples, ReadStrings(reader));
	}
}

//	The files really are the old format: wide strings as null-terminated
//	UTF-16LE, narrow strings as null-terminated UTF-8. Checked on the bytes,
//	independently of the readers and writers.
TEST(chString, OldFormatFilesAreUTF16)
{
	for( const Fixture& fixture : c_Fixtures )
	{
		SCOPED_TRACE(fixture.m_Name);
		const std::vector<uint8_t> bytes = ReadFileBytes(DataPath(fixture.m_Name));
		ASSERT_FALSE(bytes.empty());

		const std::vector<uint8_t> signature = { 'T', 'W', 'F', 'B' };
		EXPECT_EQ(fixture.m_bHeader, std::equal(signature.begin(), signature.end(), bytes.begin()));

		for( const std::string& sample : c_Samples )
		{
			if( sample.empty() )
				continue;
			SCOPED_TRACE(sample);
			EXPECT_TRUE(Contains(bytes, UTF16LEWithNul(sample)));
			EXPECT_TRUE(Contains(bytes, BytesWithNul(sample)));
		}
	}
}

TEST(chString, BinaryRoundTrip)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("Strings.bin");
	fsFileUtil::CreateFile(file);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		write_file.WriteHeader(0, 0, 0);
		chBinWriter writer(write_file);
		WriteStrings(writer, c_Samples);
	}

	gfFileBin read_file(file, fsFileStream::e_ReadOnly);
	gfFileBin::Header header;
	read_file.ReadHeader(&header);
	chBinReader reader(read_file);
	ExpectStrings(c_Samples, ReadStrings(reader));
}

TEST(chString, MemWriterMatchesBinWriter)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("Strings.bin");
	fsFileUtil::CreateFile(file);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		chBinWriter writer(write_file);
		WriteStrings(writer, c_Samples);
	}

	chMemWriter mem;
	WriteStrings(mem, c_Samples);
	EXPECT_EQ(ReadFileBytes(dir.GetPath() / "Strings.bin"), mem.GetBytes());
}

//	The text formats, one string per test. Until the ch readers and writers
//	move to UTF-8 (docs/plans/2026-10-28-itstring-to-std-string.md, step 2),
//	chTxtWriter and chXMLWriter store only ASCII, so the others are skipped.
class chStringText : public testing::TestWithParam<size_t>
{
	protected:
		const std::string& GetSample() const { return c_Samples[GetParam()]; }

		void SkipUntilUTF8() const
		{
			if( !IsASCII(GetSample()) )
				GTEST_SKIP() << "the text and XML chunk writers store only ASCII until step 2 of the UTF-8 plan";
		}

		std::map<std::string, chDefs::Name> m_Names = { { "Strs", c_Strings }, { "Strg", c_String } };
};

TEST_P(chStringText, TextRoundTrip)
{
	SkipUntilUTF8();
	if( IsSkipped() )
		return;
	const std::vector<std::string> samples = { GetSample() };

	ctTempDir dir;
	fsLocator file = dir.GetLocator("Strings.txt");
	fsFileUtil::CreateFile(file);
	{
		gfFileTxt write_file(file, fsFileStream::e_WriteOnly);
		chTxtWriter writer(write_file);
		writer.SetMap(&m_Names);
		WriteStrings(writer, samples);
	}

	gfFileTxt read_file(file, fsFileStream::e_ReadOnly);
	chTxtReader reader(read_file);
	reader.SetMap(&m_Names);
	ExpectStrings(samples, ReadStrings(reader));
}

TEST_P(chStringText, XMLRoundTrip)
{
	SkipUntilUTF8();
	if( IsSkipped() )
		return;
	const std::vector<std::string> samples = { GetSample() };

	// as documents are saved and loaded (docDocumentWithChunks::SaveFile_XML,
	// LoadFile_XML): the XML header, then the chunks inside a root tag, and
	// no name map (with one, chXMLWriter writes the names without tags)
	ctTempDir dir;
	fsLocator file = dir.GetLocator("Strings.xml");
	fsFileUtil::CreateFile(file);
	{
		gfFileXML write_file(file, fsFileStream::e_WriteOnly);
		write_file.WriteHeader();
		chXMLWriter writer(write_file);
		WriteStrings(writer, samples);
		write_file.WriteFooter();
	}

	gfFileXML read_file(file, fsFileStream::e_ReadOnly);
	gfFileXML::Header header;
	read_file.ReadHeader(&header);
	chXMLReader reader(read_file);

	chDefs::Name root;
	chDefs::Version version;
	chDefs::Size size;
	ASSERT_TRUE(reader.ReadChunkHeader(root, version, size));
	EXPECT_EQ(chDefs::MakeName('S', 'G', 'P', 'U'), root);
	ExpectStrings(samples, ReadStrings(reader));
}

namespace
{

std::string SampleName(const testing::TestParamInfo<size_t>& i_Info)
{
	static const char* const c_Names[] = { "ASCII", "Latin", "CJK", "Emoji" };
	return c_Names[i_Info.param];
}

}	// namespace

INSTANTIATE_TEST_SUITE_P(Samples, chStringText,
	testing::Range(size_t(0), c_Samples.size() - 1),	// not the empty string
	SampleName);
