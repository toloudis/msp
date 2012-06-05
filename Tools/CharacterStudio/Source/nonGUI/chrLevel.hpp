/********************************************************************************************\
**  chrLevel.hpp
**
**      Keeps track of materials in viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef	CHR_LEVEL_HPP
#error	chrLevel.hpp included recursively.
#endif
#define	CHR_LEVEL_HPP

#ifndef CHR_CALLBACKS_HPP
#include "nonGUI/chrCallbacks.hpp"
#endif

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>
#include <vector>

//============================================================================================
//	forward references
//============================================================================================
class fsLocator;
class itString;
class maAxisBox;


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
	void	SetModelChangeCallback(chrModelChangeCallback *i_Callback);

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
	// Returns true if a model with subdivision surfaces is loaded.
	//========================================================================
	bool HasSubdivModel();

	//========================================================================
	// Returns true if an animatable model is loaded.
	//========================================================================
	bool CanAnimate();

	//========================================================================
	// Control over low resolution model display
	//========================================================================
	bool HasLowResModel();
	bool GetUseLowResModel();
	void SetUseLowResModel(bool i_bLowRes);

	//========================================================================
	// Control over joint bone display
	//		0 - model only, 1 - joints only, 2 - model and joints
	//========================================================================
	bool HasBoneDisplay();
	int GetBoneDisplay();
	void SetBoneDisplay(int i_DisplayMode);

	//========================================================================
	// Return number of named morph targets
	//========================================================================
	//int GetNumMorphTargets();

	//========================================================================
	// Return name of morph target with given index
	//========================================================================
	//std::string GetMorphTargetName(int i_Index);

	//========================================================================
	// MorphTargetWeight - usually number from 0-1 to set influence 
	//	of this morph target.
	//========================================================================
	//float GetMorphTargetWeight(int i_Index);
	//void SetMorphTargetWeight(int i_Index, float i_Weight);

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
	// Return true if the expression with the given index is used
	//	in a paired expression.
	//========================================================================
	bool IsExpressionGrouped(int i_Index);

	//========================================================================
	// Return names of expressions that are not grouped yet
	//========================================================================
	void GetUngroupedNames(std::vector<std::string> &o_Names);

	//========================================================================
	//	Pair two expressions in order to control them with one slider
	//========================================================================
	void AddExpressionPair(const std::string &i_Name,
		const std::string &i_Left, const std::string &i_Right);

	//========================================================================
	// Return number of grouped pairs of expressions
	//========================================================================
	int GetNumGroupPairs();

	//========================================================================
	// Return name of grouped pair with given index
	//========================================================================
	std::string GetGroupPairName(int i_Index);
	void SetGroupPairName(int i_Index, const std::string& i_Name);

	//========================================================================
	// GroupPairWeight - usually number from -1..1 to set influence 
	//	of the pair of expressions at once.
	//========================================================================
	float GetGroupPairWeight(int i_Index);
	void SetGroupPairWeight(int i_Index, float i_Weight);

	//========================================================================
	// Remove pair with given index.
	// If i_bDeleteExpressions is true, it will delete the underlying
	//	expressions also
	//========================================================================
	void DeleteGroupPair(int i_Index, bool i_bDeleteExpressions);

	//========================================================================
	//	Group four expressions in order to control them with one slider
	//========================================================================
	void AddExpressionFour(const std::string &i_Name,
						 const std::string &i_Left, 
						 const std::string &i_Right, 
						 const std::string &i_Down, 
						 const std::string &i_Up);

	//========================================================================
	// Return number of grouped fours of expressions
	//========================================================================
	int GetNumGroupFours();

	//========================================================================
	// Return name of grouped four with given index
	//========================================================================
	std::string GetGroupFourName(int i_Index);
	void SetGroupFourName(int i_Index, const std::string& i_Name);

	//========================================================================
	// GroupFourWeight - usually number from -1..1 to set influence 
	//	of the four of expressions at once.
	//========================================================================
	void GetGroupFourWeight(int i_Index, float &o_Weight1, float &o_Weight2);
	void SetGroupFourWeight(int i_Index, float i_Weight1, float i_Weight2);

	//========================================================================
	// Remove group four with given index.
	//========================================================================
	void DeleteGroupFour(int i_Index, bool i_bDeleteExpressions);
};



