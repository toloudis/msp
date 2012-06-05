/********************************************************************************************\
**  mtrLevel.hpp
**
**      Keeps track of materials in viewed object.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef	MTR_LEVEL_HPP
#error	mtrLevel.hpp included recursively.
#endif
#define	MTR_LEVEL_HPP

#ifndef MTR_CALLBACKS_HPP
#include "mtrCallbacks.hpp"
#endif


#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif

//============================================================================================
//	forward references
//============================================================================================
class fsLocator;
class itString;
class maAxisBox;
class mtrMaterialTemplate;


//============================================================================================
//	mtrLevel Functions
//============================================================================================
namespace mtrLevel
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
	//	Return number of Materials
	//============================================================================
	int	GetNumMaterials();

	//============================================================================
	//	Return name of Material with given index
	//============================================================================
	const char* GetMaterialName(int i_Index);

	//============================================================================
	//	Return Material properties structure
	//============================================================================
	const mtrMaterialTemplate& GetMaterialData(int i_Index);

	//============================================================================
	//	Set Material properties from structure
	//============================================================================
	void	SetMaterialData(int i_Index,
							const mtrMaterialTemplate& i_Data);

	//============================================================================
	//	SelectMaterial
	//============================================================================
	void	SelectMaterial(int i_Index);

	//============================================================================
	//	SetMaterialSelectCallback
	//============================================================================
	void	SetMaterialSelectCallback(mtrMaterialSelectCallback *i_Callback);

	//============================================================================
	//	SetModelChangeCallback
	//============================================================================
	void	SetModelChangeCallback(mtrModelChangeCallback *i_Callback);

	//============================================================================
	//	SetDoHighlight
	//============================================================================
	void	SetDoHighlight(bool i_bVal);

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
	//	LoadModel loads geometry from the given locator
	//========================================================================
	void	LoadModel(const fsLocator& i_Locator);
	
	//========================================================================
	//	DoMaterialPick - selects material from mouse pick
	//========================================================================
	void	DoMaterialPick( const maPoint3d &i_RayStart,
							const maPoint3d &i_RayEnd);

	//========================================================================
	//	Return directory from which to load textures
	//========================================================================
	const fsLocator&	GetTextureDir();

	//========================================================================
	// Focus camera on bounding box of object.
	//========================================================================
	void FocusCamera();

	//============================================================================
	//	Copy/Paste material info to/from clipboard
	//============================================================================
	void	CopyMaterial();
	void	PasteMaterial();
	bool	HaveClipboardData();

	//========================================================================
	//	ReloadTextures - forces reload of all textures
	//========================================================================
	void	ReloadTextures();
		
	//========================================================================
	//	ImportMaterials - read materials from file and apply
	//	to current geometry by matching names
	//========================================================================
	int	ImportMaterials(const fsLocator &i_Locator);
		
	//============================================================================
	//	Use structure from template manager to set material with given index.
	//	This applies an algorithm to find the appropriate texture names.
	//============================================================================
	void ApplyMaterialTemplate(int i_Index,
							   const mtrMaterialTemplate& i_Data);
};



