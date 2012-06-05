/********************************************************************************************\
**  chrLevel.hpp
**
**      Keeps track of expressions in viewed character.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef	CHR_LEVEL_HPP
#error	chrLevel.hpp included recursively.
#endif
#define	CHR_LEVEL_HPP

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
//	chrLevel Functions
//============================================================================================
namespace chrLevel
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
	//	Save saves a definition from a given locator
	//========================================================================
	void	Save(const fsLocator& i_Locator);

	//========================================================================
	//	Load() loads .chd character definition file.
	//========================================================================
	void	Load(const fsLocator& i_Locator);

	//========================================================================
	//	LoadModel loads geometry from the given locator
	//========================================================================
	void	LoadModel(const fsLocator& i_Locator);

	//========================================================================
	//	LoadAnimation loads animation for given model.
	//========================================================================
	void	LoadAnimation(const fsLocator& i_Locator);

	//========================================================================
	//	LoadSubAnimation loads sub-animation for given model.
	//========================================================================
	void	LoadSubAnimation(const fsLocator& i_Locator);

	//========================================================================
	//	SwitchModel changes base geometry and idle animation, leaving
	//	subanims in place.
	//========================================================================
	void	SwitchModel(const fsLocator& i_ModelLocator,
						const fsLocator& i_IdleAnimLocator);

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
	// Return number of named morph targets
	//========================================================================
	int GetNumMorphTargets();

	//========================================================================
	// Return name of morph target with given index
	//========================================================================
	std::string GetMorphTargetName(int i_Index);

	//========================================================================
	// MorphTargetWeight - usually number from 0-1 to set influence 
	//	of this morph target.
	//========================================================================
	float GetMorphTargetWeight(int i_Index);
	void SetMorphTargetWeight(int i_Index, float i_Weight);

	//========================================================================
	// Return current subdivision level being used.
	//========================================================================
	int GetCurrentSubdivLevel();

	//========================================================================
	// Set the current subdivision level being used, this should be 
	// a level less than or equal to the return value of 
	// GetMaxSubdivLevel()
	//========================================================================
	void SetCurrentSubdivLevel(int i_SubdivLevel);

	//========================================================================
	//	AddExpression loads sub-animation as expression that can then
	//	be blended with an alpha from 0-1
	//========================================================================
	void AddExpression(const fsLocator& i_Locator, const std::string &i_Name);
	
	//========================================================================
	// Return number of named expression
	//========================================================================
	int GetNumExpressions();

	//========================================================================
	// Return name of expression with given index
	//========================================================================
	std::string GetExpressionName(int i_Index);
	void SetExpressionName(int i_Index, const std::string& i_Name);

	//========================================================================
	// ExpressionWeight - usually number from 0-1 to set influence 
	//	of this morph target.
	//========================================================================
	float GetExpressionWeight(int i_Index);
	void SetExpressionWeight(int i_Index, float i_Weight);

	//========================================================================
	// Remove expression with given index.
	//========================================================================
	void DeleteExpression(int i_Index);

	//========================================================================
	// Attach real-time controlled from segment data
	//========================================================================
	void AttachController(const std::vector<mcpHTRSegmentData> &i_Segments);

	//========================================================================
	// Remove controller
	//========================================================================
	void RemoveController();

	//========================================================================
	// Update hierarchy of skeleton using segment data
	//========================================================================
	void UpdateHierarchy(const std::vector<mcpHTRSegmentData> &i_Segments);


};



