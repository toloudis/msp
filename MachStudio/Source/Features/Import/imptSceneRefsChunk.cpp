/*****************************************************************************
**  imptSceneRefsChunk.cpp
**
**      see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Features/Import/imptSceneRefsChunk.hpp"

#ifndef IMPT_SCENEREFS_HPP
#include "Features/Import/imptSceneRefs.hpp"
#endif

#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif

#include <map>
#include <set>


//--------------------------------------------------------------------
//--------------------------------------------------------------------
imptSceneRefsChunk::imptSceneRefsChunk()
{
}

imptSceneRefsChunk::~imptSceneRefsChunk()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void imptSceneRefsChunk::Clear()
{
	m_pPending.reset();
	imptSceneRefs::Clear();
}

chDefs::Name imptSceneRefsChunk::GetChunkName() const
{
	return iscnRefLedger::GetChunkName();
}

const char* imptSceneRefsChunk::GetChunkDesc() const
{
	return "Scene References";
}

void imptSceneRefsChunk::BuildDataList(std::vector<std::string>& /*o_List*/)
{
}

void imptSceneRefsChunk::RemoveItems(const std::vector<std::string>& /*i_List*/)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void imptSceneRefsChunk::GetResourceList(fsResourceTrackerData& io_List)
{
	// each file references come from, with the objects it no longer has
	std::map<std::string, fsLocator> files;
	for (const imptSceneRefs::Sourced& object : imptSceneRefs::GetSourcedObjects())
	{
		if (imptSceneRefs::Ledger().HasReference(object.m_Name.GetString()))
			files[imptSceneRefs::FileKey(object.m_File)] = object.m_File;
	}
	const std::map<std::string, imptSceneRefs::MissingNodes>& missing = imptSceneRefs::GetMissingNodes();
	for (const auto& file : missing)
	{
		if (files.find(file.first) == files.end())
			files[file.first] = file.second.m_File;
	}
	if (files.empty())
		return;

	io_List.Add(nameString(GetChunkDesc()));
	io_List.IncrementCurrentDepth();
	for (const auto& file : files)
	{
		io_List.Add(file.second);
		auto gone = missing.find(file.first);
		if (gone == missing.end())
			continue;
		io_List.IncrementCurrentDepth();
		for (const std::string& name : gone->second.m_Objects)
			io_List.Add(nameString(name + ": not in the file"), true);
		io_List.DecrementCurrentDepth();
	}
	io_List.DecrementCurrentDepth();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void imptSceneRefsChunk::Read(chReader& i_Reader, chDefs::Version i_Version, chDefs::Size i_Size)
{
	m_pPending.reset(new iscnRefLedger::Saved);
	iscnRefLedger::Read(i_Reader, i_Version, i_Size, *m_pPending);
}

void imptSceneRefsChunk::Import(chReader& /*i_Reader*/, chDefs::Version /*i_Version*/, chDefs::Size /*i_Size*/, bool /*i_bRenameDupes*/)
{
}

void imptSceneRefsChunk::Write(chWriter& o_Writer, bool /*i_bResetDirtyFlag*/)
{
	iscnRefLedger::Write(o_Writer, imptSceneRefs::GetSaved());
}

void imptSceneRefsChunk::NotifyLoadFinished()
{
	if (!m_pPending)
		return;
	std::unique_ptr<iscnRefLedger::Saved> pending(std::move(m_pPending));
	imptSceneRefs::Reload(*pending);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool imptSceneRefsChunk::IsDirty() const
{
	return false;
}

void imptSceneRefsChunk::DataChanged()
{
}

void imptSceneRefsChunk::SetActive()
{
}

void imptSceneRefsChunk::SetInactive(bool /*i_bUpdateData*/)
{
}
