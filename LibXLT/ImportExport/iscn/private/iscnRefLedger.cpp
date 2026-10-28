/****************************************************************************\
**  iscnRefLedger.cpp
**
**      see iscnRefLedger.hpp
**
**	Extra Large Technology
\****************************************************************************/
#ifndef ISCN_REFLEDGER_HPP
#include "ImportExport/iscn/iscnRefLedger.hpp"
#endif

#ifndef CH_READER_HPP
#include "Core/ch/chReader.hpp"
#endif
#ifndef CH_WRITER_HPP
#include "Core/ch/chWriter.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/Fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif

#include <filesystem>
#include <system_error>

namespace
{
	const chDefs::Version c_Version = 2;	// 1: stamps held a content hash
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
iscnFileStamp iscnStampFile(const fsLocator& i_File)
{
	iscnFileStamp stamp;

	std::string name;
	fsFileUtil::LocatorToANSIFilename(i_File, name);	// UTF-8
	const std::filesystem::path path = std::filesystem::u8path(name);

	std::error_code size_error, time_error;
	const uintmax_t size = std::filesystem::file_size(path, size_error);
	const std::filesystem::file_time_type time = std::filesystem::last_write_time(path, time_error);
	if (size_error || time_error)
		return stamp;		// missing or unreadable

	stamp.m_Size = uint64_t(size);
	stamp.m_ModifiedTime = int64_t(time.time_since_epoch().count());
	stamp.m_bRead = true;
	return stamp;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void iscnRefLedger::SetReference(const std::string& i_Name, std::vector<uint8_t> i_Bytes)
{
	m_References[i_Name] = std::move(i_Bytes);
}

bool iscnRefLedger::HasReference(const std::string& i_Name) const
{
	return m_References.find(i_Name) != m_References.end();
}

iscnRefLedger::State iscnRefLedger::GetState(const std::string& i_Name, const std::vector<uint8_t>& i_Bytes) const
{
	auto found = m_References.find(i_Name);
	return (found != m_References.end() && found->second == i_Bytes) ? e_Reference : e_Copy;
}

void iscnRefLedger::SetFileStamp(const std::string& i_File, const iscnFileStamp& i_Stamp)
{
	m_Files[i_File] = i_Stamp;
}

const iscnFileStamp* iscnRefLedger::GetFileStamp(const std::string& i_File) const
{
	auto found = m_Files.find(i_File);
	return found != m_Files.end() ? &found->second : NULL;
}

void iscnRefLedger::Clear()
{
	m_References.clear();
	m_Files.clear();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name iscnRefLedger::GetChunkName()
{
	return chDefs::MakeName('S', 'R', 'C', 'R');
}

void iscnRefLedger::Write(chWriter& o_Writer, const Saved& i_Saved)
{
	o_Writer.WriteChunkHeader(GetChunkName(), c_Version, false);

	o_Writer.Write(uint32_t(i_Saved.m_Files.size()));
	for (const auto& file : i_Saved.m_Files)
	{
		o_Writer.Write(file.first);
		o_Writer.Write(file.second.m_Size);
		o_Writer.Write(file.second.m_ModifiedTime);
		o_Writer.Write(file.second.m_bRead);
	}

	o_Writer.Write(uint32_t(i_Saved.m_Objects.size()));
	for (const auto& object : i_Saved.m_Objects)
	{
		o_Writer.Write(object.first);
		o_Writer.Write(int32_t(object.second));
	}

	o_Writer.FinishChunk();
}

void iscnRefLedger::Read(chReader& i_Reader, chDefs::Version i_Version, chDefs::Size /*i_Size*/, Saved& o_Saved)
{
	o_Saved = Saved();

	uint32_t files = 0;
	i_Reader.Read(files);
	for (uint32_t i = 0; i < files; ++i)
	{
		std::string name;
		iscnFileStamp stamp;
		i_Reader.Read(name);
		i_Reader.Read(stamp.m_Size);
		if (i_Version >= 2)
			i_Reader.Read(stamp.m_ModifiedTime);
		else
		{
			uint64_t hash = 0;			// version 1's content hash: no time, so changed
			i_Reader.Read(hash);
		}
		i_Reader.Read(stamp.m_bRead);
		o_Saved.m_Files[name] = stamp;
	}

	uint32_t objects = 0;
	i_Reader.Read(objects);
	for (uint32_t i = 0; i < objects; ++i)
	{
		std::string name;
		int32_t state = 0;
		i_Reader.Read(name);
		i_Reader.Read(state);
		o_Saved.m_Objects[name] = state == e_Reference ? e_Reference : e_Copy;
	}
}
