/*****************************************************************************
**  gfFileBinTest.cpp
**
**      gfFileBin headers, gfFileUtil endian conversion, and gfPaths
**      user-defined paths.
**
**      Ported from TestLibXLT/Core/Gf/gfTestMain.cpp (gfTextSetFile and
**      gfPakFile, which it also tested, no longer exist).
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#ifndef GF_FILEBIN_HPP
#include "Core/gf/gfFileBin.hpp"
#endif
#ifndef GF_FILEUTIL_HPP
#include "Core/gf/gfFileUtil.hpp"
#endif
#ifndef GF_FILEX_HPP
#include "Core/gf/gfFileX.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif

#include <gtest/gtest.h>

#include <cstring>

TEST(gfFileBin, HeaderRoundTrip)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("header.bin");
	fsFileUtil::CreateFile(file);

	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		write_file.WriteHeader(2, 3, 4);
		EXPECT_EQ(gfFileBin::GetHeaderSize(), write_file.GetFilePos());
	}

	gfFileBin read_file(file, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
	gfFileBin::Header header;
	std::memset(&header, 0, sizeof(header));
	read_file.ReadHeader(&header);

	EXPECT_EQ(gfFileBin::e_LittleEndian, header.m_Mode);	// the file's mode, not the header's
	EXPECT_EQ(2, header.m_Major);
	EXPECT_EQ(3, header.m_Minor);
	EXPECT_EQ(4, header.m_Rev);
}

TEST(gfFileBin, ReadHeaderRejectsOtherFiles)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("not-a-filebin.bin");
	fsFileUtil::CreateFile(file);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		write_file.Write(8, "NOTAFILE");
	}

	gfFileBin read_file(file, fsFileStream::e_ReadOnly);
	EXPECT_THROW(read_file.ReadHeader(), gfInvalidFileBinX);
}

//	A value written through gfFileUtil reads back unchanged in either
//	endian mode, and lands in the file in that mode's byte order.
//
TEST(gfFileUtil, EndianRoundTrip)
{
	const envType::UInt32 value = 0x01020304;

	struct Case
	{
		gfFileBin::EndianMode m_Mode;
		unsigned char m_Bytes[4];
	};
	const Case cases[] =
	{
		{ gfFileBin::e_LittleEndian, { 0x04, 0x03, 0x02, 0x01 } },
		{ gfFileBin::e_BigEndian,    { 0x01, 0x02, 0x03, 0x04 } },
	};

	for( const Case& c : cases )
	{
		SCOPED_TRACE(c.m_Mode == gfFileBin::e_BigEndian ? "big endian" : "little endian");

		ctTempDir dir;
		fsLocator file = dir.GetLocator("endian.bin");
		fsFileUtil::CreateFile(file);
		{
			gfFileBin write_file(file, fsFileStream::e_WriteOnly, c.m_Mode);
			write_file.WriteHeader(4, 5);
			gfFileUtil::Write(write_file, value);
		}

		{
			gfFileBin read_file(file, fsFileStream::e_ReadOnly, c.m_Mode);
			read_file.ReadHeader();
			envType::UInt32 read_value = 0;
			gfFileUtil::Read(read_file, read_value);
			EXPECT_EQ(value, read_value);
		}

		{
			// the raw bytes, without byte swapping
			fsFileStream raw(file, fsFileStream::e_ReadOnly);
			raw.SetFilePos(gfFileBin::GetHeaderSize());
			unsigned char bytes[4] = {};
			ASSERT_EQ(4, raw.Read(4, bytes));
			EXPECT_EQ(0, std::memcmp(c.m_Bytes, bytes, 4));
		}
	}
}

TEST(gfPaths, UserDefinedPath)
{
	const int my_path = gfPaths::e_FirstUserDefPath;

	fsLocator path;
	path.Push(itString("C:"));
	path.Push(itString("gfTestDir"));
	gfPaths::SetPath(my_path, path);

	EXPECT_TRUE(gfPaths::GetPath(my_path).Equals(path));
}
