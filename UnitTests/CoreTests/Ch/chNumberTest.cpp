/*****************************************************************************
**  chNumberTest.cpp
**
**      Every number type, including its limits, infinities and NaNs,
**      reads back exactly from binary, text and XML chunk files. Also
**      reads Int8/UInt8 values that older versions of the text and XML
**      writers stored as raw characters.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctNumbers.hpp"
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
#ifndef CH_XMLREADER_HPP
#include "Core/ch/chXMLReader.hpp"
#endif
#ifndef CH_XMLWRITER_HPP
#include "Core/ch/chXMLWriter.hpp"
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

#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

namespace
{

const chDefs::Name c_ChunkName = chDefs::MakeName('N', 'u', 'm', 's');

//	Written after the values, so a value that reads as too many or too
//	few tokens shows up as a wrong end marker.
const int32_t c_EndMarker = -12345;

enum Format { e_Binary, e_Text, e_XML };

const char* format_name(Format i_Format)
{
	switch( i_Format )
	{
		case e_Binary:	return "binary";
		case e_Text:	return "text";
		default:		return "XML";
	}
}

//	write_values writes one chunk holding i_Values and the end marker,
//	in i_Format.
//
template<class T>
void write_values(Format i_Format, const fsLocator& i_File, const std::vector<T>& i_Values)
{
	auto write_chunk = [&](chWriter& io_Writer)
	{
		io_Writer.WriteChunkHeader(c_ChunkName, 1, false);
		for( T value : i_Values )
			io_Writer.Write(value);
		io_Writer.Write(c_EndMarker);
		io_Writer.FinishChunk();
	};

	fsFileUtil::CreateFile(i_File);
	switch( i_Format )
	{
		case e_Binary:
		{
			gfFileBin file(i_File, fsFileStream::e_WriteOnly);
			chBinWriter writer(file);
			write_chunk(writer);
			break;
		}
		case e_Text:
		{
			gfFileTxt file(i_File, fsFileStream::e_WriteOnly);
			chTxtWriter writer(file);
			write_chunk(writer);
			break;
		}
		case e_XML:
		{
			// as docDocumentWithChunks saves
			gfFileXML file(i_File, fsFileStream::e_WriteOnly);
			file.WriteHeader();
			{
				chXMLWriter writer(file);
				write_chunk(writer);
			}
			file.WriteFooter();
			break;
		}
	}
}

//	read_values reads back the chunk write_values wrote, expecting
//	i_Count values.
//
template<class T>
std::vector<T> read_values(Format i_Format, const fsLocator& i_File, size_t i_Count)
{
	std::vector<T> values;
	auto read_chunk = [&](chReader& io_Reader)
	{
		chDefs::Name name = 0;
		chDefs::Version version = 0;
		chDefs::Size size = 0;
		ASSERT_TRUE(io_Reader.ReadChunkHeader(name, version, size));
		EXPECT_EQ(c_ChunkName, name);

		for( size_t i = 0 ; i < i_Count ; i++ )
		{
			T value = T(42);
			io_Reader.Read(value);
			values.push_back(value);
		}

		int32_t end_marker = 0;
		io_Reader.Read(end_marker);
		EXPECT_EQ(c_EndMarker, end_marker);
		io_Reader.FinishChunk();
	};

	switch( i_Format )
	{
		case e_Binary:
		{
			gfFileBin file(i_File, fsFileStream::e_ReadOnly);
			chBinReader reader(file);
			read_chunk(reader);
			break;
		}
		case e_Text:
		{
			gfFileTxt file(i_File, fsFileStream::e_ReadOnly);
			chTxtReader reader(file);
			read_chunk(reader);
			break;
		}
		case e_XML:
		{
			// as docDocumentWithChunks loads: the header line, then the
			// <SGPU> root tag that WriteHeader added, as a chunk
			gfFileXML file(i_File, fsFileStream::e_ReadOnly);
			file.ReadHeader();
			chXMLReader reader(file);
			chDefs::Name root = 0;
			chDefs::Version version = 0;
			chDefs::Size size = 0;
			EXPECT_TRUE(reader.ReadChunkHeader(root, version, size));
			EXPECT_EQ(chDefs::MakeName('S', 'G', 'P', 'U'), root);
			read_chunk(reader);
			reader.FinishChunk();
			break;
		}
	}
	return values;
}

std::string read_file(const std::filesystem::path& i_Path)
{
	std::ifstream in(i_Path, std::ios::binary);
	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void write_file(const std::filesystem::path& i_Path, const std::string& i_Data)
{
	std::ofstream out(i_Path, std::ios::binary | std::ios::trunc);
	out.write(i_Data.data(), i_Data.size());
}

}

template<class T>
class chNumber : public testing::Test
{
};
TYPED_TEST_SUITE(chNumber, ctNumberTypes);

TYPED_TEST(chNumber, RoundTrip)
{
	const std::vector<TypeParam> written = ctNumberValues<TypeParam>();

	for( Format format : { e_Binary, e_Text, e_XML } )
	{
		SCOPED_TRACE(format_name(format));

		ctTempDir dir;
		fsLocator file = dir.GetLocator("numbers");
		write_values(format, file, written);

		std::vector<TypeParam> read;
		ASSERT_NO_THROW(read = read_values<TypeParam>(format, file, written.size()));
		ASSERT_EQ(written.size(), read.size());
		for( size_t i = 0 ; i < written.size() ; i++ )
			EXPECT_TRUE(ctSameNumber(written[i], read[i])) << "value " << i;
	}
}

//	The text and XML writers used to store Int8 and UInt8 as the raw
//	byte. The readers take a lone non-numeric character as that byte,
//	so most such values still load. This writes placeholder numbers and
//	swaps the raw bytes in for them.
//
TEST(chNumber, ReadsBytesWrittenAsCharacters)
{
	struct RawByte
	{
		char m_Char;
		int8_t m_Int8;
		uint8_t m_UInt8;
	};
	const RawByte raw_bytes[] =
	{
		{ 'A',    65,   65 },
		{ '\x01', 1,    1 },	// a "true" flag
		{ '\0',   0,    0 },	// a "false" flag
		{ '-',    45,   45 },
		{ '\xC8', -56,  200 },	// not ASCII
	};

	for( Format format : { e_Text, e_XML } )
	{
		SCOPED_TRACE(format_name(format));

		ctTempDir dir;
		fsLocator file = dir.GetLocator("raw-bytes");

		// placeholders: 11110, 11111, ...
		std::vector<int32_t> placeholders;
		for( size_t i = 0 ; i < std::size(raw_bytes) ; i++ )
			placeholders.push_back(int32_t(11110 + i));
		write_values(format, file, placeholders);

		std::filesystem::path path = dir.GetPath() / "raw-bytes";
		std::string data = read_file(path);
		for( size_t i = 0 ; i < std::size(raw_bytes) ; i++ )
		{
			std::string placeholder = std::to_string(placeholders[i]);
			size_t pos = data.find(placeholder);
			ASSERT_NE(std::string::npos, pos) << placeholder;
			data.replace(pos, placeholder.size(), 1, raw_bytes[i].m_Char);
		}
		write_file(path, data);

		std::vector<int8_t> as_int8;
		ASSERT_NO_THROW(as_int8 = read_values<int8_t>(format, file, std::size(raw_bytes)));
		std::vector<uint8_t> as_uint8;
		ASSERT_NO_THROW(as_uint8 = read_values<uint8_t>(format, file, std::size(raw_bytes)));

		for( size_t i = 0 ; i < std::size(raw_bytes) ; i++ )
		{
			SCOPED_TRACE(i);
			EXPECT_EQ(raw_bytes[i].m_Int8, as_int8[i]);
			EXPECT_EQ(raw_bytes[i].m_UInt8, as_uint8[i]);
		}
	}
}

//	The text tokenizer used to split a float's exponent from its
//	mantissa ("1e-05" read as 1, then -5 as the next value).
//
TEST(chNumber, TextReadsExponents)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("exponents.txt");
	write_file(dir.GetPath() / "exponents.txt", "Nums 1e-05 2.5E+10 -3e7 -inf 4\n");

	gfFileTxt txt_file(file, fsFileStream::e_ReadOnly);
	chTxtReader reader(txt_file);

	chDefs::Name name;
	chDefs::Version version;
	chDefs::Size size;
	ASSERT_TRUE(reader.ReadChunkHeader(name, version, size));
	EXPECT_EQ(c_ChunkName, name);

	double d = 0;
	reader.Read(d);
	EXPECT_EQ(1e-05, d);
	reader.Read(d);
	EXPECT_EQ(2.5e10, d);
	float f = 0;
	reader.Read(f);
	EXPECT_EQ(-3e7f, f);
	reader.Read(f);
	EXPECT_EQ(-std::numeric_limits<float>::infinity(), f);
	int32_t i = 0;
	reader.Read(i);
	EXPECT_EQ(4, i);
}
