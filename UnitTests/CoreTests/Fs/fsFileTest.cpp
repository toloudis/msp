/*****************************************************************************
**  fsFileTest.cpp
**
**      fsFileUtil existence checks, read-only flag, file streams and
**      fsFilePosSaver.
**
**      Ported from TestLibXLT/Core/Fs/fsTestMain.cpp (which used fixed
**      files and network paths next to the old test; these make their
**      own files in a temp directory).
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#ifndef FS_FILEPOSSAVER_HPP
#include "Core/fs/fsFilePosSaver.hpp"
#endif
#ifndef FS_FILESTREAM_HPP
#include "Core/fs/fsFileStream.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif

#include <gtest/gtest.h>

#include <random>

TEST(fsFileUtil, FileAndDirectoryExist)
{
	ctTempDir dir;

	fsLocator sub_dir = dir.GetLocator("TestDir");
	EXPECT_FALSE(fsFileUtil::DirectoryExists(sub_dir));
	fsFileUtil::CreateDirectory(sub_dir);
	EXPECT_TRUE(fsFileUtil::DirectoryExists(sub_dir));

	fsLocator file = sub_dir;
	file.Push(std::string("TestFile.txt"));
	EXPECT_FALSE(fsFileUtil::FileExists(file));
	fsFileUtil::CreateFile(file);
	EXPECT_TRUE(fsFileUtil::FileExists(file));

	// a directory isn't a file, and a file isn't a directory
	EXPECT_FALSE(fsFileUtil::FileExists(sub_dir));
	EXPECT_FALSE(fsFileUtil::DirectoryExists(file));

	fsFileUtil::DeleteFile(file);
	EXPECT_FALSE(fsFileUtil::FileExists(file));
}

TEST(fsFileUtil, ReadOnlyFlag)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("TestFile-ReadOnly.txt");
	fsFileUtil::CreateFile(file);

	EXPECT_FALSE(fsFileUtil::IsReadOnly(file));
	fsFileUtil::SetReadOnly(file, true);
	EXPECT_TRUE(fsFileUtil::IsReadOnly(file));

	// clear it again so the temp directory can be deleted
	fsFileUtil::SetReadOnly(file, false);
	EXPECT_FALSE(fsFileUtil::IsReadOnly(file));
}

TEST(fsFileUtil, LocatorStringRoundTrip)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("Some File.txt");
	EXPECT_EQ("Some File.txt", file.GetLastName());

	std::string as_string;
	fsFileUtil::LocatorToUnicodeString(file, as_string);
	fsLocator back;
	fsFileUtil::UnicodeStringToLocator(as_string, back);
	EXPECT_TRUE(file.Equals(back));
}

TEST(fsFileStream, WriteThenRead)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("fsTest.bin");
	fsFileUtil::CreateFile(file);

	{
		fsFileStream write_stream(file, fsFileStream::e_WriteOnly);
		for( int16_t i = 0 ; i < 1000 ; i++ )
			write_stream.Write(sizeof(i), &i);
	}

	fsFileStream read_stream(file, fsFileStream::e_ReadOnly);
	for( int16_t i = 0 ; i < 1000 ; i++ )
	{
		int16_t val = -1;
		ASSERT_EQ(int(sizeof(val)), read_stream.Read(sizeof(val), &val));
		ASSERT_EQ(i, val);
	}

	// reading past the end returns what's left: nothing
	int16_t val;
	EXPECT_EQ(0, read_stream.Read(sizeof(val), &val));
}

//	A move back from the current position that leaves the read buffer
//	(gfFileTxt::ReadToken backs up two bytes after every token) lands where
//	it should: the stream reads ahead to fill its buffer, so the file's own
//	pointer is past the stream's position.
TEST(fsFileStream, MoveBackAcrossTheReadBuffer)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("fsTest.bin");
	fsFileUtil::CreateFile(file);

	{
		fsFileStream write_stream(file, fsFileStream::e_WriteOnly);
		for( int i = 0 ; i < 4000 ; i++ )
		{
			const uint8_t byte = uint8_t(i % 251);
			write_stream.Write(1, &byte);
		}
	}

	fsFileStream read_stream(file, fsFileStream::e_ReadOnly);
	for( int i = 0 ; i < 4000 ; i++ )
	{
		uint8_t byte = 0;
		ASSERT_EQ(1, read_stream.Read(1, &byte));
		ASSERT_EQ(uint8_t(i % 251), byte);
		if( (i % 3) == 2 )
		{
			read_stream.SetFilePos(-2, fsFileStream::e_Current);
			ASSERT_EQ(i - 1, read_stream.GetFilePos());
			ASSERT_EQ(1, read_stream.Read(1, &byte));
			ASSERT_EQ(uint8_t((i - 1) % 251), byte) << "after moving back at " << i;
			ASSERT_EQ(1, read_stream.Read(1, &byte));
		}
	}
}

TEST(fsFilePosSaver, RestoresPosition)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("fsTest.bin");
	fsFileUtil::CreateFile(file);

	{
		fsFileStream write_stream(file, fsFileStream::e_WriteOnly);
		for( int16_t i = 0 ; i < 1000 ; i++ )
			write_stream.Write(sizeof(i), &i);
	}

	fsFileStream read_stream(file, fsFileStream::e_ReadOnly);
	std::mt19937 rng(3);
	std::uniform_int_distribution<int> any_pos(0, 1999);
	for( int test_num = 0 ; test_num < 100 ; test_num++ )
	{
		int first_pos = any_pos(rng);
		int second_pos = any_pos(rng);

		read_stream.SetFilePos(first_pos);
		{
			fsFilePosSaver<fsFileStream> saver(read_stream);
			read_stream.SetFilePos(second_pos);
			EXPECT_EQ(second_pos, read_stream.GetFilePos());
		}
		EXPECT_EQ(first_pos, read_stream.GetFilePos());
	}
}
