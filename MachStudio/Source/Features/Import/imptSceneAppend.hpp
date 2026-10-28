/*****************************************************************************
**  imptSceneAppend.hpp
**
**      imptSceneAppend adds a scene file's objects to the open scene, in any
**	format ImportExport reads (iscnSceneImport): characters, point and
**	projected lights, cameras, and transform groups holding the
**	hierarchy, as one undo step. The Objects dialog reaches it through
**	chtrOperations::SetFileAppender (FeaturesLayer installs it).
**	Scene trees and object properties refresh once after all objects,
**	parenting and keys have been added, rather than for each selection.
**	The Import dialog reads a file (ReadFile) and appends the objects the
**	user chose (AppendScene); a chosen object brings its ancestor groups
**	(iscnSceneChoice::WithAncestors), so its keys, in its parent's space,
**	and its rebuilds (imptSceneRefs) stay right.
**	File > Open (and drag-and-drop, -file, the recent files) opens a scene
**	file as a new, untitled scene through the document manager's foreign
**	opener (docSingleTypeMgr::SetForeignOpener, installed by Init), which
**	calls OpenFile.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef IMPT_SCENEAPPEND_HPP
#error imptSceneAppend.hpp multiply included
#endif
#define IMPT_SCENEAPPEND_HPP

#include <vector>

class fsLocator;
class nameString;
struct iscnSceneDesc;

//============================================================================
//============================================================================
namespace imptSceneAppend
{
	//------------------------------------------------------------------------
	// Install and remove the Objects dialog hook and the foreign opener
	//------------------------------------------------------------------------
	void Init();
	void CleanUp() throw();

	//------------------------------------------------------------------------
	// Is this a scene file AppendFile reads?
	//------------------------------------------------------------------------
	bool IsSceneFile(const fsLocator& i_File);

	//------------------------------------------------------------------------
	// Read the scene file; false (after telling the user) if it can't be read
	//------------------------------------------------------------------------
	bool ReadFile(const fsLocator& i_File, iscnSceneDesc& o_Scene);

	//------------------------------------------------------------------------
	// Add the chosen objects of i_Scene (read from i_File; one entry per
	// object) and their ancestors, as one undo step; false, with no undo
	// step, if nothing is chosen. o_Names, if given, gets the name each
	// object got (one per object; empty if it wasn't added).
	//------------------------------------------------------------------------
	bool AppendScene(const fsLocator& i_File, const iscnSceneDesc& i_Scene, const std::vector<bool>& i_Chosen,
					 std::vector<nameString>* o_Names = nullptr);

	//------------------------------------------------------------------------
	// Add every object of the scene file; false (after telling the user)
	// if it can't be read
	//------------------------------------------------------------------------
	bool AppendFile(const fsLocator& i_File);

	//------------------------------------------------------------------------
	// File > Open of a scene file: read it and add every object to the new,
	// empty scene the document manager has just made, then have the active
	// view look through the file's first camera, if it has one; false
	// (after telling the user) if it can't be read
	//------------------------------------------------------------------------
	bool OpenFile(const fsLocator& i_File);
}
