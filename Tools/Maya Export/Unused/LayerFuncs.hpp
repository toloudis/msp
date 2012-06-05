/*****************************************************************************
**  LayerFuncs.hpp
**
**   Namespace for IK-Joint and single-skin related functions 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef LAYERFUNCS_HPP
#error LayerFuncs.hpp multiply included
#endif
#define LAYERFUNCS_HPP

#include <Maya/MString.h>

#include <vector>

class MFnDependencyNode;
class chWriter;

namespace LayerFuncs
{
		
	//========================================================================
	//	GetDisplayLayer - return name of layer for given node
	//========================================================================
	bool GetDisplayLayer(MFnDependencyNode &fnDependNode, MString &oString);

	//========================================================================
	//	GetLayers - return list of layer names from previous call
	//	to ParseLayers
	//========================================================================
	void GetLayers(std::vector<MString> &o_LayerNames);

	//========================================================================
	//	WriteBReps - write visible meshes for the given layer
	//========================================================================
	void WriteBReps(const MString &i_LayerName, chWriter &o_Writer);

}