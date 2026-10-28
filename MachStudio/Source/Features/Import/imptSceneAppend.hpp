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
**
**	Extra Large Technology
\****************************************************************************/
#ifdef IMPT_SCENEAPPEND_HPP
#error imptSceneAppend.hpp multiply included
#endif
#define IMPT_SCENEAPPEND_HPP

#include <vector>

class fsLocator;
struct iscnSceneDesc;

//============================================================================
//============================================================================
namespace imptSceneAppend
{
	//------------------------------------------------------------------------
	// Install and remove the Objects dialog hook
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
	// step, if nothing is chosen
	//------------------------------------------------------------------------
	bool AppendScene(const fsLocator& i_File, const iscnSceneDesc& i_Scene, const std::vector<bool>& i_Chosen);

	//------------------------------------------------------------------------
	// Add every object of the scene file; false (after telling the user)
	// if it can't be read
	//------------------------------------------------------------------------
	bool AppendFile(const fsLocator& i_File);
}
