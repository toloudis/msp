/*****************************************************************************
**  gfFileBinTest.cpp
**
**      gfFileBin headers, gfFileUtil byte order, and gfPaths
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
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		gfFileBin::Header out_header = { 7, 2, 3, 4 };
		write_file.WriteHeader(out_header);
		EXPECT_EQ(gfFileBin::GetHeaderSize(), write_file.GetFilePos());
	}

	gfFileBin read_file(file, fsFileStream::e_ReadOnly);
	gfFileBin::Header header;
	std::memset(&header, 0, sizeof(header));
	read_file.ReadHeader(&header);

	EXPECT_EQ(2, header.m_Mode);	// the deprecated endian mode is always "little endian"
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

//	A value written through gfFileUtil reads back unchanged, and lands in
//	the file in little endian byte order.
//
TEST(gfFileUtil, LittleEndianRoundTrip)
{
	const uint32_t value = 0x01020304;
	const unsigned char expected_bytes[4] = { 0x04, 0x03, 0x02, 0x01 };

	ctTempDir dir;
	fsLocator file = dir.GetLocator("endian.bin");
	fsFileUtil::CreateFile(file);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		write_file.WriteHeader(4, 5);
		gfFileUtil::Write(write_file, value);
	}

	{
		gfFileBin read_file(file, fsFileStream::e_ReadOnly);
		read_file.ReadHeader();
		uint32_t read_value = 0;
		gfFileUtil::Read(read_file, read_value);
		EXPECT_EQ(value, read_value);
	}

	{
		// the raw bytes
		fsFileStream raw(file, fsFileStream::e_ReadOnly);
		raw.SetFilePos(gfFileBin::GetHeaderSize());
		unsigned char bytes[4] = {};
		ASSERT_EQ(4, raw.Read(4, bytes));
		EXPECT_EQ(0, std::memcmp(expected_bytes, bytes, 4));
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
