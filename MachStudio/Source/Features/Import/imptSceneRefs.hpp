/*****************************************************************************
**  imptSceneRefs.hpp
**
**      imptSceneRefs tracks whether each object brought in from a scene file
**	is still a reference to it (unedited: it follows the file) or a copy
**	(edited in MachStudio: it keeps the user's values). The rulings are at
**	the top of the .cpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef IMPT_SCENEREFS_HPP
#error imptSceneRefs.hpp multiply included
#endif
#define IMPT_SCENEREFS_HPP

#ifndef IMPT_SCENEBUILD_HPP
#include "Features/Import/imptSceneBuild.hpp"
#endif
#ifndef ISCN_REFLEDGER_HPP
#include "ImportExport/iscn/iscnRefLedger.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <cstdint>
#include <map>
#include <string>
#include <vector>

//============================================================================
//============================================================================
namespace imptSceneRefs
{
	//------------------------------------------------------------------------
	// An object brought in from a scene file: a character with a source
	// identity, or a point light, projected light, camera or transform
	// group with a source file
	//------------------------------------------------------------------------
	struct Sourced
	{
		imptSceneBuild::System m_System = imptSceneBuild::e_Transform;
		nameString m_Name;
		fsLocator m_File;			// a character's m_Filename, the others' m_SourceFile
		std::string m_Identity;		// its identity in the file
	};
	std::vector<Sourced> GetSourcedObjects();
	bool FindSourced(const nameString& i_Name, Sourced& o_Object);

	//------------------------------------------------------------------------
	// The object's bytes: its data as its system saves it, without what
	// isn't an edit (editor visibility, a group's children, name UIDs)
	//------------------------------------------------------------------------
	std::vector<uint8_t> GetBytes(const Sourced& i_Object);
	iscnRefLedger::State GetState(const Sourced& i_Object);

	//------------------------------------------------------------------------
	// Snapshot the object's bytes now (it is a reference while they equal)
	//------------------------------------------------------------------------
	void Remember(const Sourced& i_Object);

	//------------------------------------------------------------------------
	// The ledger's stamp for the file = the file's stamp now
	//------------------------------------------------------------------------
	void StampFile(const fsLocator& i_File);

	//------------------------------------------------------------------------
	// The ledger's key for a file
	//------------------------------------------------------------------------
	std::string FileKey(const fsLocator& i_File);

	iscnRefLedger& Ledger();

	//------------------------------------------------------------------------
	// What the SRCR chunk saves: each sourced object's state and, for each
	// file a reference comes from, the stamp its references were built from
	//------------------------------------------------------------------------
	iscnRefLedger::Saved GetSaved();

	//------------------------------------------------------------------------
	// After a scene has loaded: references whose file changed are rebuilt
	// from it, the others remembered as they are; copies get no snapshot
	// (see the .cpp)
	//------------------------------------------------------------------------
	void Reload(const iscnRefLedger::Saved& i_Saved);

	//------------------------------------------------------------------------
	// The objects whose node their file no longer had when it was read,
	// per file key (the resource tracker lists them)
	//------------------------------------------------------------------------
	struct MissingNodes
	{
		fsLocator m_File;
		std::vector<std::string> m_Objects;
	};
	const std::map<std::string, MissingNodes>& GetMissingNodes();

	//------------------------------------------------------------------------
	// Rebuild the named objects from their files as one undoable step
	// ("Revert to <file name>"), making them references again. A file that
	// can't be read is named in a message box and its objects are left as
	// they are; an object whose node is gone is skipped with a warning.
	//------------------------------------------------------------------------
	void Revert(const std::vector<nameString>& i_Names);

	//------------------------------------------------------------------------
	// Forget every snapshot, stamp and missing node (a new or opened scene)
	//------------------------------------------------------------------------
	void Clear();

	//------------------------------------------------------------------------
	// Start watching undo and the selection (the property page's Source
	// line); stop
	//------------------------------------------------------------------------
	void Init();
	void CleanUp() throw();
}
