/*****************************************************************************
**  fsFileStreamModeTest.cpp
**
**      fsFileStream: what each access type opens, the exceptions opening
**      throws, reads and writes past the buffer, seeking, the file size.
**      Written against the Win32 implementation (CreateFileW and friends)
**      before fsFileStream moved to std::filebuf, so the move keeps the
**      same behavior. (fsFileTest.cpp has the round trip and buffer
**      tests.)
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#ifndef FS_FILESTREAM_HPP
#include "Core/fs/fsFileStream.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace
{
	std::filesystem::path NativePath(const fsLocator& i_Locator)
	{
		std::string utf8;
		fsFileUtil::LocatorToUnicodeString(i_Locator, utf8);
		return std::filesystem::u8path(utf8);
	}

	void WriteText(const fsLocator& i_File, const std::string& i_Text)
	{
		std::ofstream out(NativePath(i_File), std::ios::binary);
		out << i_Text;
	}

	std::string ReadText(const fsLocator& i_File)
	{
		std::ifstream in(NativePath(i_File), std::ios::binary);
		return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	}

	void Write(fsFileStream& io_Stream, const std::string& i_Text)
	{
		io_Stream.Write(int64_t(i_Text.size()), i_Text.data());
	}

	std::string Read(fsFileStream& io_Stream, int i_Count)
	{
		std::string text(size_t(i_Count), '\0');
		text.resize(size_t(io_Stream.Read(i_Count, &text[0])));
		return text;
	}

	//	Bytes that differ at every position, so a misplaced read shows
	std::string Pattern(size_t i_Size)
	{
		std::string data(i_Size, '\0');
		for (size_t i = 0; i < i_Size; ++i)
			data[i] = char((i * 7 + i / 251) & 0xFF);
		return data;
	}
}

//----------------------------------------------------------------------------
//	Opening
//----------------------------------------------------------------------------
TEST(fsFileStreamMode, ReadOnlyFailsForAMissingFile)
{
	ctTempDir dir;
	EXPECT_THROW(fsFileStream(dir.GetLocator("missing.bin"), fsFileStream::e_ReadOnly), fsFileDoesntExistX);

	fsLocator in_missing_dir = dir.GetLocator("missing");
	in_missing_dir.Push(std::string("file.bin"));
	EXPECT_THROW(fsFileStream(in_missing_dir, fsFileStream::e_ReadOnly), fsDirectoryDoesntExistX);
}

TEST(fsFileStreamMode, OpeningAnEmptyLocatorFails)
{
	EXPECT_THROW(fsFileStream(fsLocator(), fsFileStream::e_ReadOnly), fsInvalidLocatorX);
}

TEST(fsFileStreamMode, WriteCreateNeedsTheFileToExist)
{
	// Despite its name: the Win32 code passed OPEN_ALWAYS | TRUNCATE_EXISTING,
	// which is TRUNCATE_EXISTING, so callers fsFileUtil::CreateFile first
	ctTempDir dir;
	EXPECT_THROW(fsFileStream(dir.GetLocator("missing.bin"), fsFileStream::e_WriteCreate), fsFileDoesntExistX);
	EXPECT_FALSE(fsFileUtil::FileExists(dir.GetLocator("missing.bin")));
}

TEST(fsFileStreamMode, WriteCreateEmptiesTheFile)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("file.bin");
	WriteText(file, "old contents");
	{
		fsFileStream stream(file, fsFileStream::e_WriteCreate);
		Write(stream, "new");
	}
	EXPECT_EQ("new", ReadText(file));
}

TEST(fsFileStreamMode, WriteOnlyWritesOverTheStartWithoutEmptying)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("file.bin");
	WriteText(file, "12345");
	{
		fsFileStream stream(file, fsFileStream::e_WriteOnly);
		Write(stream, "AB");
	}
	EXPECT_EQ("AB345", ReadText(file));
}

TEST(fsFileStreamMode, WriteOnlyFailsForAMissingFile)
{
	ctTempDir dir;
	EXPECT_THROW(fsFileStream(dir.GetLocator("missing.bin"), fsFileStream::e_WriteOnly), fsFileDoesntExistX);
}

TEST(fsFileStreamMode, WritingAReadOnlyFileFails)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("locked.bin");
	WriteText(file, "keep");
	fsFileUtil::SetReadOnly(file, true);
	EXPECT_THROW(fsFileStream(file, fsFileStream::e_WriteCreate), fsReadOnlyX);
	EXPECT_THROW(fsFileStream(file, fsFileStream::e_WriteOnly), fsReadOnlyX);

	// reading it is fine
	{
		fsFileStream stream(file, fsFileStream::e_ReadOnly);
		EXPECT_EQ("keep", Read(stream, 10));
	}
	fsFileUtil::SetReadOnly(file, false);
	EXPECT_EQ("keep", ReadText(file));
}

TEST(fsFileStreamMode, OpeningADirectoryFails)
{
	ctTempDir dir;
	EXPECT_THROW(fsFileStream(dir.GetLocator(), fsFileStream::e_ReadOnly), fsReadOnlyX);
}

TEST(fsFileStreamMode, NonAsciiName)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator();
	file.Push(std::string("Crème 東京 🎬.bin"));
	fsFileUtil::CreateFile(file);
	{
		fsFileStream stream(file, fsFileStream::e_WriteCreate);
		Write(stream, "data");
	}
	fsFileStream stream(file, fsFileStream::e_ReadOnly);
	EXPECT_EQ("data", Read(stream, 10));
	EXPECT_TRUE(stream.GetLocator() == file);
}

//----------------------------------------------------------------------------
//	Reading and writing
//----------------------------------------------------------------------------
TEST(fsFileStreamMode, ReadingPastTheEndGivesWhatIsThere)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("short.bin");
	WriteText(file, "abc");
	fsFileStream stream(file, fsFileStream::e_ReadOnly);
	EXPECT_EQ("abc", Read(stream, 10));
	EXPECT_EQ(3, stream.GetFilePos());
	EXPECT_EQ("", Read(stream, 10));
	EXPECT_EQ(3, stream.GetFilePos());
}

TEST(fsFileStreamMode, LargeAndSmallReadsAndWritesMix)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("big.bin");
	const std::string data = Pattern(100000);

	fsFileUtil::CreateFile(file);
	{
		// small writes go through the buffer, large ones straight to the file
		fsFileStream stream(file, fsFileStream::e_WriteCreate);
		size_t pos = 0;
		const size_t sizes[] = { 1, 7, 511, 512, 513, 4000, 3, 20000 };
		for (int i = 0; pos < data.size(); ++i)
		{
			const size_t size = std::min(sizes[i % 8], data.size() - pos);
			Write(stream, data.substr(pos, size));
			pos += size;
			EXPECT_EQ(int64_t(pos), stream.GetFilePos());
		}
	}
	EXPECT_EQ(data, ReadText(file));

	fsFileStream stream(file, fsFileStream::e_ReadOnly);
	EXPECT_EQ(int64_t(data.size()), stream.GetFileSize());
	std::string read_back;
	const int sizes[] = { 3, 600, 1, 511, 9000, 512, 2, 30000 };
	for (int i = 0; read_back.size() < data.size(); ++i)
		read_back += Read(stream, sizes[i % 8]);
	EXPECT_EQ(data, read_back);
}

//----------------------------------------------------------------------------
//	Seeking
//----------------------------------------------------------------------------
TEST(fsFileStreamMode, SeekFromEachAccessPoint)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("seek.bin");
	const std::string data = Pattern(5000);
	WriteText(file, data);

	fsFileStream stream(file, fsFileStream::e_ReadOnly);
	stream.SetFilePos(4000);
	EXPECT_EQ(4000, stream.GetFilePos());
	EXPECT_EQ(data.substr(4000, 10), Read(stream, 10));

	stream.SetFilePos(-10, fsFileStream::e_End);
	EXPECT_EQ(4990, stream.GetFilePos());
	EXPECT_EQ(data.substr(4990), Read(stream, 100));

	stream.SetFilePos(100);
	Read(stream, 5);
	stream.SetFilePos(1000, fsFileStream::e_Current);	// far outside the buffer
	EXPECT_EQ(1105, stream.GetFilePos());
	EXPECT_EQ(data.substr(1105, 10), Read(stream, 10));
}

TEST(fsFileStreamMode, SeekingPastTheEndThenWritingFillsWithZeros)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("gap.bin");
	fsFileUtil::CreateFile(file);
	{
		fsFileStream stream(file, fsFileStream::e_WriteCreate);
		Write(stream, "ab");
		stream.SetFilePos(6);
		Write(stream, "cd");
		EXPECT_EQ(8, stream.GetFilePos());
	}
	EXPECT_EQ(std::string("ab\0\0\0\0cd", 8), ReadText(file));
}

TEST(fsFileStreamMode, SeekBackAndOverwriteWhileWriting)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("patch.bin");
	fsFileUtil::CreateFile(file);
	{
		// as chunk writers do: write a placeholder size, the data, then go
		// back and write the real size
		fsFileStream stream(file, fsFileStream::e_WriteCreate);
		Write(stream, "0000");
		Write(stream, std::string(2000, 'x'));
		stream.SetFilePos(0);
		Write(stream, "2000");
		stream.SetFilePos(0, fsFileStream::e_End);
		Write(stream, "end");
	}
	EXPECT_EQ("2000" + std::string(2000, 'x') + "end", ReadText(file));
}

TEST(fsFileStreamMode, GetFileSize)
{
	ctTempDir dir;
	fsLocator file = dir.GetLocator("size.bin");
	WriteText(file, std::string(1234, 'z'));
	fsFileStream stream(file, fsFileStream::e_ReadOnly);
	EXPECT_EQ(1234, stream.GetFileSize());
	Read(stream, 100);
	EXPECT_EQ(1234, stream.GetFileSize());
}
