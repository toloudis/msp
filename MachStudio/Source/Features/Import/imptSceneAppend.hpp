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
**
**	Extra Large Technology
\****************************************************************************/
#ifdef IMPT_SCENEAPPEND_HPP
#error imptSceneAppend.hpp multiply included
#endif
#define IMPT_SCENEAPPEND_HPP

class fsLocator;

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
	// Add every object of the scene file; false (after telling the user)
	// if it can't be read
	//------------------------------------------------------------------------
	bool AppendFile(const fsLocator& i_File);
}
