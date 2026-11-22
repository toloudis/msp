/*****************************************************************************
**  fsXMLTest.cpp
**
**      Writes elements of each type with fsXMLWriter and reads them back
**      with fsXMLReader.
**
**      Ported from TestLibXLT/Core/Fs/fsTestMain.cpp (TestXMLWrite and
**      TestXMLRead, which wrote and read unrelated files). The number
**      tests also read the file through fsXMLParser.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctNumbers.hpp"
#include "ctTempDir.hpp"

#ifndef FS_XMLPARSER_HPP
#include "Core/fs/fsXMLParser.hpp"
#endif
#ifndef FS_XMLREADER_HPP
#include "Core/fs/fsXMLReader.hpp"
#endif
#ifndef FS_XMLWRITER_HPP
#include "Core/fs/fsXMLWriter.hpp"
#endif

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

namespace
{

//	ReadAll reads the file's nodes, returning the text of each element
//	and the order elements were opened in.
//
std::map<std::string, std::string> ReadAll(const fsLocator& i_File, std::vector<std::string>& o_Elements)
{
	std::map<std::string, std::string> text;

	fsXMLReader reader(i_File);
	reader.Open();

	std::string key, value;
	fsXMLData::fs_Node_Type node_type;
	while( (node_type = reader.ReadNode(key, value)) != fsXMLData::e_EOF )
	{
		if( node_type == fsXMLData::e_Element )
			o_Elements.push_back(key);
		else if( node_type == fsXMLData::e_Text )
			text[key] = value;
	}

	reader.Close();
	return text;
}

//	ParseAll is ReadAll through fsXMLParser, from the file's contents.
//
std::map<std::string, std::string> ParseAll(const std::filesystem::path& i_File)
{
	std::ifstream in(i_File, std::ios::binary);
	std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	std::map<std::string, std::string> text;

	fsXMLParser parser(contents.c_str());
	parser.Open();

	std::string key, value;
	fsXMLData::fs_Node_Type node_type;
	while( (node_type = parser.ReadNode(key, value)) != fsXMLData::e_EOF )
	{
		if( node_type == fsXMLData::e_Text )
			text[key] = value;
	}

	parser.Close();
	return text;
}

}

TEST(fsXML, WriteThenRead)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("TestXMLwrite.cfg");

	{
		fsXMLWriter writer(file);
		writer.Open();

		std::string root("Root");
		writer.WriteStartElement(root);

		std::string k_int("var_int"), k_bool("var_bool"), k_short("var_short"), k_float("var_float"),
			k_string("var_string"), k_int8("var_Int8"), k_uint8("var_UInt8"), k_uint16("var_UInt16"),
			k_uint32("var_UInt32"), k_uint64("var_UInt64"), k_float64("var_Float64");

		writer.WriteElement(k_int, int(-100));
		writer.WriteElement(k_bool, true);
		writer.WriteElement(k_short, short(100));
		writer.WriteElement(k_float, 123.5f);
		writer.WriteElement(k_string, std::string("some text"));
		writer.WriteElement(k_int8, int8_t(-100));
		writer.WriteElement(k_uint8, uint8_t(200));
		writer.WriteElement(k_uint16, uint16_t(60000));
		writer.WriteElement(k_uint32, uint32_t(4000000000u));
		writer.WriteElement(k_uint64, uint64_t(10000000000ull));
		writer.WriteElement(k_float64, double(123.456));

		writer.WriteEndElement();
		writer.Close();
	}

	std::vector<std::string> elements;
	std::map<std::string, std::string> text = ReadAll(file, elements);

	ASSERT_FALSE(elements.empty());
	EXPECT_EQ("Root", elements.front());

	fsXMLReader convert(file);

	int v_int = 0;
	convert.Convert(text["var_int"], v_int);
	EXPECT_EQ(-100, v_int);

	bool v_bool = false;
	convert.Convert(text["var_bool"], v_bool);
	EXPECT_TRUE(v_bool);

	short v_short = 0;
	convert.Convert(text["var_short"], v_short);
	EXPECT_EQ(100, v_short);

	float v_float = 0;
	convert.Convert(text["var_float"], v_float);
	EXPECT_FLOAT_EQ(123.5f, v_float);

	std::string v_string;
	convert.Convert(text["var_string"], v_string);
	EXPECT_EQ("some text", v_string);

	int8_t v_int8 = 0;
	convert.Convert(text["var_Int8"], v_int8);
	EXPECT_EQ(-100, v_int8);

	uint8_t v_uint8 = 0;
	convert.Convert(text["var_UInt8"], v_uint8);
	EXPECT_EQ(200, v_uint8);

	uint16_t v_uint16 = 0;
	convert.Convert(text["var_UInt16"], v_uint16);
	EXPECT_EQ(60000, v_uint16);

	uint32_t v_uint32 = 0;
	convert.Convert(text["var_UInt32"], v_uint32);
	EXPECT_EQ(4000000000u, v_uint32);

	uint64_t v_uint64 = 0;
	convert.Convert(text["var_UInt64"], v_uint64);
	EXPECT_EQ(10000000000ull, v_uint64);

	double v_float64 = 0;
	convert.Convert(text["var_Float64"], v_float64);
	EXPECT_NEAR(123.456, v_float64, 1e-9);
}

template<class T>
class fsXMLNumber : public testing::Test
{
};
TYPED_TEST_SUITE(fsXMLNumber, ctNumberTypes);

//	Every number type, including its limits, infinities and NaNs, reads
//	back exactly through fsXMLReader and fsXMLParser. (fsXMLWriter takes
//	Int16, Int32 and Float32 as short, int and float.)
//
TYPED_TEST(fsXMLNumber, RoundTrip)
{
	const std::vector<TypeParam> written = ctNumberValues<TypeParam>();

	ctTempDir dir;
	fsLocator file = dir.GetLocator("numbers.xml");
	{
		fsXMLWriter writer(file);
		writer.Open();
		std::string root("Root");
		writer.WriteStartElement(root);
		for( size_t i = 0 ; i < written.size() ; i++ )
		{
			std::string key = "v" + std::to_string(i);
			writer.WriteElement(key, written[i]);
		}
		writer.WriteEndElement();
		writer.Close();
	}

	std::vector<std::string> elements;
	std::map<std::string, std::string> read_text = ReadAll(file, elements);
	std::map<std::string, std::string> parsed_text = ParseAll(dir.GetPath() / "numbers.xml");

	fsXMLReader reader(file);
	fsXMLParser parser("");
	for( size_t i = 0 ; i < written.size() ; i++ )
	{
		std::string key = "v" + std::to_string(i);
		SCOPED_TRACE(key + " = " + read_text[key]);

		TypeParam read = TypeParam(42);
		reader.Convert(read_text[key], read);
		EXPECT_TRUE(ctSameNumber(written[i], read)) << "fsXMLReader";

		TypeParam parsed = TypeParam(42);
		parser.Convert(parsed_text[key], parsed);
		EXPECT_TRUE(ctSameNumber(written[i], parsed)) << "fsXMLParser";
	}
}
