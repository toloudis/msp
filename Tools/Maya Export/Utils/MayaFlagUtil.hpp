/*****************************************************************************
**  MayaFlagUtil.hpp
**
**      Namespace with functions for getting attribute flags from Maya nodes.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef MAYAFLAGUTIL_HPP
#error MayaFlagUtil.hpp multiply included
#endif
#define MAYAFLAGUTIL_HPP

#include <maya/MString.h>

class MFnDagNode;

namespace MayaFlagUtil
{
	//------------------------------------------------------------------------
	// Return true if the given attribute exists on the node
	//------------------------------------------------------------------------
	bool EngineFlagExists(MFnDagNode &node, MString name);

	//------------------------------------------------------------------------
	// get boolean value from attribute with given name from node.
	// there should be no spaces in the attribute name.
	//------------------------------------------------------------------------
	const int GetEngineFlag(MFnDagNode &node, MString name);

	//------------------------------------------------------------------------
	// Individual flag tests - encapsulates the strings needed to test
	// the flags.
	//------------------------------------------------------------------------
	bool GetDoubleSidedFlag(MFnDagNode &node);
	bool GetCastsShadowsFlag(MFnDagNode &node);
	bool GetReceiveShadowsFlag(MFnDagNode &node);
	bool GetShadowHullFlag(MFnDagNode &node);
	bool GetTriangleSortFlag(MFnDagNode &node);
	bool GetClothFlag(MFnDagNode &node);
	int GetResolutionLevel(MFnDagNode &node);
	bool GetExportAsSubdivFlag(MFnDagNode &node);
	bool GetNoExportFlag(MFnDagNode &node);
	bool GetForceExportFlag(MFnDagNode &node);
	bool GetVisibleAnimFlag(MFnDagNode &node);
}

