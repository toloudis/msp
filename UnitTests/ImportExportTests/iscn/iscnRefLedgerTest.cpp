/*****************************************************************************
**  iscnRefLedgerTest.cpp
**
**      iscnRefLedger: an object is a reference while its bytes equal its
**      snapshot; file stamps; the SRCR chunk's round trip.
**
**	Extra Large Technology
\****************************************************************************/
#include "ietTempDir.hpp"

#ifndef ISCN_REFLEDGER_HPP
#include "ImportExport/iscn/iscnRefLedger.hpp"
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

#include <chrono>
#include <fstream>
#include <string>

namespace
{
	void WriteText(const std::filesystem::path& i_Path, const char* i_Text)
	{
		std::ofstream out(i_Path, std::ios::binary | std::ios::trunc);
		out << i_Text;
	}
}

TEST(iscnRefLedger, NoSnapshotIsCopy)
{
	iscnRefLedger ledger;
	EXPECT_FALSE(ledger.HasReference("A"));
	EXPECT_EQ(iscnRefLedger::e_Copy, ledger.GetState("A", { 1, 2, 3 }));
}

TEST(iscnRefLedger, EqualBytesAreReference)
{
	iscnRefLedger ledger;
	ledger.SetReference("A", { 1, 2, 3 });
	EXPECT_TRUE(ledger.HasReference("A"));
	EXPECT_EQ(iscnRefLedger::e_Reference, ledger.GetState("A", { 1, 2, 3 }));
	EXPECT_EQ(iscnRefLedger::e_Copy, ledger.GetState("A", { 1, 2, 4 }));
	EXPECT_EQ(iscnRefLedger::e_Reference, ledger.GetState("A", { 1, 2, 3 }));	// undone
	EXPECT_EQ(iscnRefLedger::e_Copy, ledger.GetState("B", { 1, 2, 3 }));
}

TEST(iscnRefLedger, SetReferenceReplaces)
{
	iscnRefLedger ledger;
	ledger.SetReference("A", { 1, 2, 3 });
	ledger.SetReference("A", { 4, 5 });
	EXPECT_EQ(iscnRefLedger::e_Copy, ledger.GetState("A", { 1, 2, 3 }));
	EXPECT_EQ(iscnRefLedger::e_Reference, ledger.GetState("A", { 4, 5 }));

	iscnFileStamp stamp;
	stamp.m_bRead = true;
	stamp.m_Size = 3;
	ledger.SetFileStamp("C:/a.glb", stamp);
	ASSERT_NE(nullptr, ledger.GetFileStamp("C:/a.glb"));

	ledger.Clear();
	EXPECT_FALSE(ledger.HasReference("A"));
	EXPECT_EQ(nullptr, ledger.GetFileStamp("C:/a.glb"));
}

// The stamp is the file's size and last-modified time: nothing is read
TEST(iscnRefLedger, StampChangesWithFile)
{
	ietTempDir dir;
	const std::filesystem::path path = dir.GetPath() / "scene.glb";
	const fsLocator file = dir.GetLocator("scene.glb");

	WriteText(path, "abc");
	const std::filesystem::file_time_type written = std::filesystem::last_write_time(path);
	const iscnFileStamp first = iscnStampFile(file);
	EXPECT_TRUE(first.m_bRead);
	EXPECT_EQ(3u, first.m_Size);
	EXPECT_NE(0, first.m_ModifiedTime);
	EXPECT_TRUE(first == iscnStampFile(file));		// untouched

	// touched (a re-export, even of the same bytes): changed
	std::filesystem::last_write_time(path, written + std::chrono::seconds(2));
	EXPECT_FALSE(first == iscnStampFile(file));

	// other bytes of the same size and time aren't seen (the content isn't read)
	WriteText(path, "abd");
	std::filesystem::last_write_time(path, written);
	EXPECT_TRUE(first == iscnStampFile(file));

	// another size: changed
	WriteText(path, "abcd");
	std::filesystem::last_write_time(path, written);
	EXPECT_EQ(4u, iscnStampFile(file).m_Size);
	EXPECT_FALSE(first == iscnStampFile(file));
}

TEST(iscnRefLedger, StampOfMissingFile)
{
	ietTempDir dir;
	const iscnFileStamp stamp = iscnStampFile(dir.GetLocator("missing.glb"));
	EXPECT_FALSE(stamp.m_bRead);
	EXPECT_EQ(0u, stamp.m_Size);
	EXPECT_EQ(0, stamp.m_ModifiedTime);
}

TEST(iscnRefLedger, SavedRoundTrip)
{
	iscnRefLedger::Saved saved;
	iscnFileStamp read;
	read.m_bRead = true;
	read.m_Size = 1234;
	read.m_ModifiedTime = 0x0123456789abcdefll;
	saved.m_Files["C:/scenes/Fox.glb"] = read;
	saved.m_Files["C:/scenes/gone.gltf"] = iscnFileStamp();
	saved.m_Objects["A"] = iscnRefLedger::e_Reference;
	saved.m_Objects["B"] = iscnRefLedger::e_Copy;
	saved.m_Objects["Point Light 2"] = iscnRefLedger::e_Reference;

	ietTempDir dir;
	const fsLocator file = dir.GetLocator("Refs.bin");
	fsFileUtil::CreateFile(file);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		chBinWriter writer(write_file);
		iscnRefLedger::Write(writer, saved);
	}

	iscnRefLedger::Saved back;
	{
		gfFileBin read_file(file, fsFileStream::e_ReadOnly);
		chBinReader reader(read_file);
		chDefs::Name name = 0;
		chDefs::Version version = 0;
		chDefs::Size size = 0;
		ASSERT_TRUE(reader.ReadChunkHeader(name, version, size));
		EXPECT_EQ(iscnRefLedger::GetChunkName(), name);
		EXPECT_EQ(2, version);
		iscnRefLedger::Read(reader, version, size, back);
		reader.FinishChunk();
	}

	EXPECT_TRUE(saved.m_Files == back.m_Files);
	EXPECT_TRUE(saved.m_Objects == back.m_Objects);
}

// A version 1 chunk (stamps held a content hash) reads with no modified
// time, so its files count as changed once
TEST(iscnRefLedger, ReadsVersion1)
{
	ietTempDir dir;
	const fsLocator file = dir.GetLocator("Refs1.bin");
	fsFileUtil::CreateFile(file);
	{
		gfFileBin write_file(file, fsFileStream::e_WriteOnly);
		chBinWriter writer(write_file);
		writer.WriteChunkHeader(iscnRefLedger::GetChunkName(), 1, false);
		writer.Write(uint32_t(1));
		writer.Write("C:/scenes/Fox.glb");
		writer.Write(uint64_t(1234));
		writer.Write(uint64_t(0x0123456789abcdefull));	// the hash
		writer.Write(true);
		writer.Write(uint32_t(1));
		writer.Write("A");
		writer.Write(int32_t(iscnRefLedger::e_Reference));
		writer.FinishChunk();
	}

	iscnRefLedger::Saved back;
	{
		gfFileBin read_file(file, fsFileStream::e_ReadOnly);
		chBinReader reader(read_file);
		chDefs::Name name = 0;
		chDefs::Version version = 0;
		chDefs::Size size = 0;
		ASSERT_TRUE(reader.ReadChunkHeader(name, version, size));
		iscnRefLedger::Read(reader, version, size, back);
		reader.FinishChunk();
	}

	ASSERT_EQ(1u, back.m_Files.size());
	const iscnFileStamp& stamp = back.m_Files["C:/scenes/Fox.glb"];
	EXPECT_TRUE(stamp.m_bRead);
	EXPECT_EQ(1234u, stamp.m_Size);
	EXPECT_EQ(0, stamp.m_ModifiedTime);
	ASSERT_EQ(1u, back.m_Objects.size());
	EXPECT_EQ(iscnRefLedger::e_Reference, back.m_Objects["A"]);
}
