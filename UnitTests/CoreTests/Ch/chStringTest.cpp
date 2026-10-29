/*****************************************************************************
**  chStringTest.cpp
**
**      Strings in chunk files: ASCII, accented Latin, CJK, characters
**      outside the Basic Multilingual Plane and the empty string, through
**      every chunk writer and reader, and files saved in the old format
**      (UTF-16 strings in binary files) still loading.
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
#include <functional>
#include <iterator>
#include <map>
#include <string>
#include <vector>

namespace
{

const chDefs::Name c_Strings = chDefs::MakeName('S', 't', 'r', 's');
const chDefs::Name c_String = chDefs::MakeName('S', 't', 'r', 'g');
const int32_t c_Sentinel = 0x5A5A5A5A;

//	The strings, as UTF-8.
const std::vector<std::string> c_Samples =
{
	"plain ASCII",
	u8"Crème brûlée à la façon",
	u8"東京の夜景",
	u8"🎬 clap 🎥",
	"",
};

//	One string as read back: the text string (an itString when it was
//	written, read with ReadItString) and the narrow one (read with Read).
struct ReadString
{
	std::string m_Text;
	std::string m_Narrow;
};

//	WriteStrings writes a container chunk holding one leaf chunk per
//	string: an index, the string as a text string, the same string as a
//	narrow string, and a sentinel, which catches a string read with the
//	wrong length. The old-format files have the same layout, written when
//	the text string was an itString (UTF-16 in binary files).
void WriteStrings(chWriter& io_Writer, const std::vector<std::string>& i_Strings)
{
	io_Writer.WriteChunkHeader(c_Strings, 1, true);
	for( size_t i = 0 ; i < i_Strings.size() ; i++ )
	{
		io_Writer.WriteChunkHeader(c_String, 1, false);
		io_Writer.Write(int32_t(i));
		io_Writer.Write(i_Strings[i]);
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

		std::string text;
		chChunkParserUtil::ReadItString(io_Reader, text);
		std::string narrow;
		io_Reader.Read(narrow);
		strings.push_back({ text, narrow });

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
		EXPECT_EQ(i_Expected[i], i_Read[i].m_Text);
		EXPECT_EQ(i_Expected[i], i_Read[i].m_Narrow);
	}
}

//	WriteParserUtilStrings writes each string in a leaf chunk through
//	chChunkParserUtil, as the chunk parsers do: as a std::string, as an
//	itString (the temporary overloads), and in a chunk of its own.
void WriteParserUtilStrings(chWriter& io_Writer, const std::vector<std::string>& i_Strings)
{
	io_Writer.WriteChunkHeader(c_Strings, 1, true);
	for( const std::string& s : i_Strings )
	{
		io_Writer.WriteChunkHeader(c_String, 1, true);
		io_Writer.WriteChunkHeader(c_String, 1, false);
		chChunkParserUtil::Write(io_Writer, s);
		chChunkParserUtil::Write(io_Writer, itString(s.c_str()));
		io_Writer.Write(c_Sentinel);
		io_Writer.FinishChunk();
		chChunkParserUtil::Write(io_Writer, s, c_String);
		io_Writer.FinishChunk();
	}
	io_Writer.FinishChunk();
}

void ReadParserUtilStrings(chReader& io_Reader, const std::vector<std::string>& i_Expected)
{
	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	ASSERT_TRUE(io_Reader.ReadChunkHeader(name, version, size));

	size_t count = 0;
	while( io_Reader.ReadChunkHeader(name, version, size) )
	{
		ASSERT_LT(count, i_Expected.size());
		const std::string& expected = i_Expected[count++];
		SCOPED_TRACE(expected);

		ASSERT_TRUE(io_Reader.ReadChunkHeader(name, version, size));
		std::string text;
		chChunkParserUtil::ReadItString(io_Reader, text);
		EXPECT_EQ(expected, text);
		itString wide;
		chChunkParserUtil::ReadItString(io_Reader, wide);
		EXPECT_EQ(expected, std::string(wide));
		int32_t sentinel = 0;
		io_Reader.Read(sentinel);
		EXPECT_EQ(c_Sentinel, sentinel);
		io_Reader.FinishChunk();

		ASSERT_TRUE(io_Reader.ReadChunkHeader(name, version, size));
		std::string in_chunk;
		chChunkParserUtil::ReadItString(io_Reader, in_chunk);
		EXPECT_EQ(expected, in_chunk);
		io_Reader.FinishChunk();

		io_Reader.FinishChunk();
	}
	io_Reader.FinishChunk();
	EXPECT_EQ(i_Expected.size(), count);
}

std::filesystem::path DataPath(const char* i_Name)
{
	// __FILE__ is this file's full path in these builds
	return std::filesystem::path(__FILE__).parent_path().parent_path() / "Data" / i_Name;
}

fsLocator ToLocator(const std::filesystem::path& i_Path)
{
	fsLocator locator;
	fsFileUtil::UnicodeStringToLocator(envString::WideCharToUTF8(i_Path.wstring()), locator);
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

bool StartsWith(const std::vector<uint8_t>& i_Bytes, const char* i_Signature)
{
	return i_Bytes.size() >= 4 && std::equal(i_Signature, i_Signature + 4, i_Bytes.begin());
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
		EXPECT_FALSE(file.HasUTF8Strings());
		chBinReader reader(file);
		ExpectStrings(c_Samples, ReadStrings(reader));
	}
}

//	The files really are the old format: text strings as null-terminated
//	UTF-16LE, narrow strings as null-terminated UTF-8. Checked on the bytes,
//	independently of the readers and writers.
TEST(chString, OldFormatFilesAreUTF16)
{
	for( const Fixture& fixture : c_Fixtures )
	{
		SCOPED_TRACE(fixture.m_Name);
		const std::vector<uint8_t> bytes = ReadFileBytes(DataPath(fixture.m_Name));
		ASSERT_FALSE(bytes.empty());

		EXPECT_EQ(fixture.m_bHeader, StartsWith(bytes, "TWFB"));

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
	EXPECT_TRUE(read_file.HasUTF8Strings());
	chBinReader reader(read_file);
	ExpectStrings(c_Samples, ReadStrings(reader));
}

//	New binary files start with PWFB and store every string as
//	null-terminated UTF-8, text strings included: no UTF-16 in them.
TEST(chString, NewBinaryFilesAreUTF8)
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

	const std::vector<uint8_t> bytes = ReadFileBytes(dir.GetPath() / "Strings.bin");
	EXPECT_TRUE(StartsWith(bytes, "PWFB"));
	for( const std::string& sample : c_Samples )
	{
		if( sample.empty() )
			continue;
		SCOPED_TRACE(sample);
		std::vector<uint8_t> twice = BytesWithNul(sample);
		const std::vector<uint8_t> once = twice;
		twice.insert(twice.end(), once.begin(), once.end());
		EXPECT_TRUE(Contains(bytes, twice));
		EXPECT_FALSE(Contains(bytes, UTF16LEWithNul(sample)));
	}
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

TEST(chString, BinaryChunkParserUtil)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("Strings.bin");
	fsFileUtil::CreateFile(file);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		write_file.WriteHeader(0, 0, 0);
		chBinWriter writer(write_file);
		WriteParserUtilStrings(writer, c_Samples);
	}

	gfFileBin read_file(file, fsFileStream::e_ReadOnly);
	read_file.ReadHeader();
	chBinReader reader(read_file);
	ReadParserUtilStrings(reader, c_Samples);
}

//	The text formats, one string per test.
class chStringText : public testing::TestWithParam<size_t>
{
	protected:
		const std::string& GetSample() const { return c_Samples[GetParam()]; }

		void TextRoundTrip(const std::function<void(chWriter&, const std::vector<std::string>&)>& i_Write,
						   const std::function<void(chReader&, const std::vector<std::string>&)>& i_Read)
		{
			const std::vector<std::string> samples = { GetSample() };

			ctTempDir dir;
			fsLocator file = dir.GetLocator("Strings.txt");
			fsFileUtil::CreateFile(file);
			{
				gfFileTxt write_file(file, fsFileStream::e_WriteOnly);
				chTxtWriter writer(write_file);
				writer.SetMap(&m_Names);
				i_Write(writer, samples);
			}

			gfFileTxt read_file(file, fsFileStream::e_ReadOnly);
			chTxtReader reader(read_file);
			reader.SetMap(&m_Names);
			i_Read(reader, samples);
		}

		//	as documents are saved and loaded (docDocumentWithChunks::SaveFile_XML,
		//	LoadFile_XML): the XML header, then the chunks inside a root tag, and
		//	no name map (with one, chXMLWriter writes the names without tags)
		void XMLRoundTrip(const std::function<void(chWriter&, const std::vector<std::string>&)>& i_Write,
						  const std::function<void(chReader&, const std::vector<std::string>&)>& i_Read)
		{
			const std::vector<std::string> samples = { GetSample() };

			ctTempDir dir;
			fsLocator file = dir.GetLocator("Strings.xml");
			fsFileUtil::CreateFile(file);
			{
				gfFileXML write_file(file, fsFileStream::e_WriteOnly);
				write_file.WriteHeader();
				chXMLWriter writer(write_file);
				i_Write(writer, samples);
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
			i_Read(reader, samples);
		}

		static void ReadAndExpect(chReader& io_Reader, const std::vector<std::string>& i_Samples)
		{
			ExpectStrings(i_Samples, ReadStrings(io_Reader));
		}

		std::map<std::string, chDefs::Name> m_Names = { { "Strs", c_Strings }, { "Strg", c_String } };
};

TEST_P(chStringText, TextRoundTrip)
{
	TextRoundTrip(WriteStrings, ReadAndExpect);
}

TEST_P(chStringText, XMLRoundTrip)
{
	XMLRoundTrip(WriteStrings, ReadAndExpect);
}

TEST_P(chStringText, TextChunkParserUtil)
{
	TextRoundTrip(WriteParserUtilStrings, ReadParserUtilStrings);
}

TEST_P(chStringText, XMLChunkParserUtil)
{
	XMLRoundTrip(WriteParserUtilStrings, ReadParserUtilStrings);
}

namespace
{

std::string SampleName(const testing::TestParamInfo<size_t>& i_Info)
{
	static const char* const c_Names[] = { "ASCII", "Latin", "CJK", "Emoji", "Empty" };
	return c_Names[i_Info.param];
}

}	// namespace

INSTANTIATE_TEST_SUITE_P(Samples, chStringText,
	testing::Range(size_t(0), c_Samples.size()),
	SampleName);
