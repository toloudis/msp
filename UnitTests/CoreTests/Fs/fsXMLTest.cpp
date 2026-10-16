/*****************************************************************************
**  fsXMLTest.cpp
**
**      Writes elements of each type with fsXMLWriter and reads them back
**      with fsXMLReader.
**
**      Ported from TestLibXLT/Core/Fs/fsTestMain.cpp (TestXMLWrite and
**      TestXMLRead, which wrote and read unrelated files).
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#ifndef FS_XMLREADER_HPP
#include "Core/fs/fsXMLReader.hpp"
#endif
#ifndef FS_XMLWRITER_HPP
#include "Core/fs/fsXMLWriter.hpp"
#endif

#include <gtest/gtest.h>

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
		writer.WriteElement(k_int8, envType::Int8(-100));
		writer.WriteElement(k_uint8, envType::UInt8(200));
		writer.WriteElement(k_uint16, envType::UInt16(60000));
		writer.WriteElement(k_uint32, envType::UInt32(4000000000u));
		writer.WriteElement(k_uint64, envType::UInt64(10000000000ull));
		writer.WriteElement(k_float64, envType::Float64(123.456));

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

	envType::Int8 v_int8 = 0;
	convert.Convert(text["var_Int8"], v_int8);
	EXPECT_EQ(-100, v_int8);

	envType::UInt8 v_uint8 = 0;
	convert.Convert(text["var_UInt8"], v_uint8);
	EXPECT_EQ(200, v_uint8);

	envType::UInt16 v_uint16 = 0;
	convert.Convert(text["var_UInt16"], v_uint16);
	EXPECT_EQ(60000, v_uint16);

	envType::UInt32 v_uint32 = 0;
	convert.Convert(text["var_UInt32"], v_uint32);
	EXPECT_EQ(4000000000u, v_uint32);

	envType::UInt64 v_uint64 = 0;
	convert.Convert(text["var_UInt64"], v_uint64);
	EXPECT_EQ(10000000000ull, v_uint64);

	envType::Float64 v_float64 = 0;
	convert.Convert(text["var_Float64"], v_float64);
	EXPECT_NEAR(123.456, v_float64, 1e-9);
}
