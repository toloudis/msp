/********************************************************************************************\
**  mcpSkeleton.hpp
**
**      Keeps track of skeleton of motion capture data.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef	MCP_SKELETON_HPP
#error	mcpSkeleton.hpp included recursively.
#endif
#define	MCP_SKELETON_HPP

#ifndef MCP_CALLBACKS_HPP
#include "mcpCallbacks.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif

#include <string>
#include <vector>

//============================================================================================
//	forward references
//============================================================================================
class fsLocator;
class itString;
class maAxisBox;
class mcpHTRSegmentData;


//============================================================================================
//	mcpSkeleton Functions
//============================================================================================
namespace mcpSkeleton
{

	//============================================================================
	//	Initialize()
	//============================================================================
	void		Initialize();

	//============================================================================
	//	DeInitialize()
	//============================================================================
	void		DeInitialize();

	//============================================================================
	//	SetModelChangeCallback
	//============================================================================
	void	SetModelChangeCallback(mcpModelChangeCallback *i_Callback);

	//============================================================================
	//	Think - Handle material animation timing
	//============================================================================
	void	Think();

	//============================================================================
	//	Clear removes all URo pieces to start new level
	//============================================================================
	void	Clear();

	//========================================================================
	//	LoadModel loads geometry from the given locator
	//========================================================================
	void	LoadModel(const fsLocator& i_Locator);

	//========================================================================
	//	LoadAnimation loads animation for given model.
	//========================================================================
	void	LoadAnimation(const fsLocator& i_Locator);

	//========================================================================
	//	Return directory from which to load textures
	//========================================================================
	const fsLocator&	GetTextureDir();

	//========================================================================
	// Focus camera on bounding box of object.
	//========================================================================
	void FocusCamera();
		
	//========================================================================
	// Returns true if model is loaded.
	//========================================================================
	bool HasModel();

	//========================================================================
	// Create a skeleton from segment data
	//========================================================================
	void CreateSkeleton(const std::vector<mcpHTRSegmentData> &i_Segments);

	//========================================================================
	// Update hierarchy of skeleton using segment data
	//========================================================================
	void UpdateHierarchy(const std::vector<mcpHTRSegmentData> &i_Segments);


};



