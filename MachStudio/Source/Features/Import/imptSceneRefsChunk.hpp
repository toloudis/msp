/*****************************************************************************
**  imptSceneRefsChunk.hpp
**
**      The document chunk ("Scene References", SRCR) that saves whether
**	each object brought in from a scene file is a reference or a copy,
**	and each file's stamp when its references were built. When a scene
**	finishes loading, references whose file changed are rebuilt from it
**	(imptSceneRefs::Reload). It holds no objects: the objects' own chunks
**	save them in full and carry the scene's changes.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef IMPT_SCENEREFSCHUNK_HPP
#error imptSceneRefsChunk.hpp multiply included
#endif
#define IMPT_SCENEREFSCHUNK_HPP

#ifndef DOC_DOCUMENTCHUNK_HPP
#include "Tool/doc/docDocumentChunk.hpp"
#endif
#ifndef ISCN_REFLEDGER_HPP
#include "ImportExport/iscn/iscnRefLedger.hpp"
#endif

#include <memory>


//============================================================================
//============================================================================
class imptSceneRefsChunk : public docDocumentChunk
{
public:
	imptSceneRefsChunk();
	virtual ~imptSceneRefsChunk();

	virtual void Clear();
	virtual chDefs::Name GetChunkName() const;
	virtual const char* GetChunkDesc() const;

	//--------------------------------------------------------------------
	// Not offered by the .mab Import list: nothing to list or remove
	//--------------------------------------------------------------------
	virtual void BuildDataList(std::vector<std::string>& o_List);
	virtual void RemoveItems(const std::vector<std::string>& i_List);

	//--------------------------------------------------------------------
	// Each referenced file (missing ones show as missing) and the
	// objects whose node it no longer has
	//--------------------------------------------------------------------
	virtual void GetResourceList(fsResourceTrackerData& io_List);

	//--------------------------------------------------------------------
	// Read keeps the saved states until the load finishes; Import (a .mab
	// merged in) is skipped: merged objects are copies
	//--------------------------------------------------------------------
	virtual void Read(chReader& i_Reader, chDefs::Version i_Version, chDefs::Size i_Size);
	virtual void Import(chReader& i_Reader, chDefs::Version i_Version, chDefs::Size i_Size, bool i_bRenameDupes = false);
	virtual void Write(chWriter& o_Writer, bool i_bResetDirtyFlag = true);
	virtual void NotifyLoadFinished();

	//--------------------------------------------------------------------
	// Never dirty itself
	//--------------------------------------------------------------------
	virtual bool IsDirty() const;
	virtual void DataChanged();
	virtual void SetActive();
	virtual void SetInactive(bool i_bUpdateData);

private:
	std::unique_ptr<iscnRefLedger::Saved> m_pPending;
};
