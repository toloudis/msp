/****************************************************************************\
**  iscnRefLedger.hpp
**
**      iscnRefLedger remembers what each object brought in from a scene file
**	was when it was last built from that file (its saved bytes), and each
**	file's stamp. An object is a reference to its file while its bytes are
**	what they were then; any edit makes it differ (a copy), and undoing the
**	edit makes it equal again. iscnStampFile says whether a file changed
**	(by its size and last-modified time, without reading it).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef ISCN_REFLEDGER_HPP
#error iscnRefLedger.hpp multiply included
#endif
#define ISCN_REFLEDGER_HPP

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class chReader;
class chWriter;
class fsLocator;

//----------------------------------------------------------------------------
//	A file's size and last-modified time. Touching a file (a re-export of
//	the same bytes) changes it; the content isn't read, so other bytes of
//	the same size written with the same time wouldn't (scene files are
//	written whole by exporters, which always set a new time).
//----------------------------------------------------------------------------
struct iscnFileStamp
{
	uint64_t m_Size = 0;
	int64_t m_ModifiedTime = 0;	// std::filesystem::last_write_time's ticks (UTC); 0: unknown
	bool m_bRead = false;		// false: missing or unreadable (size and time 0)

	bool operator == (const iscnFileStamp& i_Other) const
	{
		return m_Size == i_Other.m_Size && m_ModifiedTime == i_Other.m_ModifiedTime && m_bRead == i_Other.m_bRead;
	}
	bool operator != (const iscnFileStamp& i_Other) const { return !(*this == i_Other); }
};

//----------------------------------------------------------------------------
//	The file's stamp now (its directory entry only)
//----------------------------------------------------------------------------
iscnFileStamp iscnStampFile(const fsLocator& i_File);

//============================================================================
//============================================================================
class iscnRefLedger
{
public:
	enum State { e_Reference, e_Copy };

	//------------------------------------------------------------------------
	// The object's bytes as its file built it (replaces any earlier ones)
	//------------------------------------------------------------------------
	void SetReference(const std::string& i_Name, std::vector<uint8_t> i_Bytes);
	bool HasReference(const std::string& i_Name) const;

	//------------------------------------------------------------------------
	// Reference if i_Bytes equal its snapshot; no snapshot: copy
	//------------------------------------------------------------------------
	State GetState(const std::string& i_Name, const std::vector<uint8_t>& i_Bytes) const;

	//------------------------------------------------------------------------
	// The stamp a file's references were built from; i_File is fsLocator's
	// full path. GetFileStamp: NULL if none.
	//------------------------------------------------------------------------
	void SetFileStamp(const std::string& i_File, const iscnFileStamp& i_Stamp);
	const iscnFileStamp* GetFileStamp(const std::string& i_File) const;

	void Clear();

	//------------------------------------------------------------------------
	// The SRCR chunk a scene saves
	//------------------------------------------------------------------------
	struct Saved
	{
		std::map<std::string, iscnFileStamp> m_Files;	// file -> stamp its references were built from
		std::map<std::string, State> m_Objects;			// sourced object name -> state at save
	};
	static chDefs::Name GetChunkName();		// 'SRCR'

	//------------------------------------------------------------------------
	// Write the whole chunk (version 2); Read its contents after the header
	// (version 1 stamps held a content hash: they read with no time)
	//------------------------------------------------------------------------
	static void Write(chWriter& o_Writer, const Saved& i_Saved);
	static void Read(chReader& i_Reader, chDefs::Version i_Version, chDefs::Size i_Size, Saved& o_Saved);

private:
	std::map<std::string, std::vector<uint8_t> > m_References;
	std::map<std::string, iscnFileStamp> m_Files;
};
