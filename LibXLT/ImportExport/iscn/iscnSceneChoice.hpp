/****************************************************************************\
**  iscnSceneChoice.hpp
**
**      iscnSceneChoice helps choose which of a scene file's objects to
**	bring in (the Import dialog): the object tree, a label per object
**	saying what animation comes with it, and the rule that a chosen
**	object brings its ancestor groups (its keys are in its parent's
**	space, and a reference is rebuilt in its file parent's space).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef ISCN_SCENECHOICE_HPP
#error iscnSceneChoice.hpp multiply included
#endif
#define ISCN_SCENECHOICE_HPP

#ifndef ISCN_SCENEDESC_HPP
#include "ImportExport/iscn/iscnSceneDesc.hpp"
#endif

#include <string>
#include <vector>

//============================================================================
//============================================================================
namespace iscnSceneChoice
{
	//------------------------------------------------------------------------
	// "Group", "Mesh", "Point Light", "Spot Light", "Directional Light", "Camera"
	//------------------------------------------------------------------------
	std::string KindLabel(iscnObjectDesc::Kind i_Kind);

	//------------------------------------------------------------------------
	// "<name> (<kind>)", plus "; keyed" when it has keys, plus
	// "; clips: <name>, <name>" when it has clips
	//------------------------------------------------------------------------
	std::string ItemLabel(const iscnObjectDesc& i_Object);

	//------------------------------------------------------------------------
	// Indices of the objects whose parent is i_Parent (-1: the top level),
	// in file order
	//------------------------------------------------------------------------
	std::vector<int> Children(const iscnSceneDesc& i_Scene, int i_Parent);

	//------------------------------------------------------------------------
	// i_Chosen (one per object; missing entries false) plus every ancestor
	// of a chosen object; one entry per object
	//------------------------------------------------------------------------
	std::vector<bool> WithAncestors(const iscnSceneDesc& i_Scene, const std::vector<bool>& i_Chosen);
}
