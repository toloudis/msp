/*****************************************************************************
**	rndrPrefsMgr.hpp
**
**		API for preferences utilities
**
**	Extra Large Technology
**	Copyright(C) 2006-7 - All Rights Reserved
\****************************************************************************/
#ifdef RNDRPREFSMGR_HPP
#error rndrPrefsMgr.hpp multiply included
#endif
#define RNDRPREFSMGR_HPP

#ifndef RNDRPREFSDATA_HPP
#include "Features/RenderPrefs/rndrPrefsData.hpp"
#endif
#ifndef RNDRPREFSDATAINTEREST_HPP
#include "Features/RenderPrefs/rndrPrefsDataInterest.hpp"
#endif

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
class prtyObject;


//============================================================================
//============================================================================
namespace rndrPrefsMgr
{
	enum rndr_Object
	{
		e_ViewportPrefs = 0,
		e_RenderFullPrefs = 1,
		e_RenderQuickPrefs = 2,
		e_PrefsNum
	};

	//
	// Viewport Preferences
	//

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp();

	//------------------------------------------------------------------------
	//	write the render preferences
	//------------------------------------------------------------------------
	void WritePrefs(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	//	read the render preferences
	//------------------------------------------------------------------------
	void ReadPrefs(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	//  AddToMenu() - add Prefs actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rndrPrefsData& Data(rndr_Object i_ObjectID);
	const g3dPrefs::g3dRenderPrefs& ActualData(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ApplyPrefs(rndr_Object i_ObjectID);

	//
	//	interest functions
	//

	//--------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//--------------------------------------------------------------------
	void RegisterInterest( rndrPrefsDataInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterInterest( rndrPrefsDataInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	Clear() - clear the interest list
	//--------------------------------------------------------------------
	void ClearInterests();

	//------------------------------------------------------------------------
	//	This gets called to let the interests know that the data has changed
	//------------------------------------------------------------------------
	void DataUpdated(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	//	Build a string based on the render flags, one char per flag
	//	S - Shadows
	//	M - Render Matte
	//	D - DOF
	//	F - Fur
	//	G - Glow
	//	A - Ambient
	//	L - Lit
	//	T - Transparent
	//	R - Reflections
	//
	//	A '-' means that flag is OFF
	//------------------------------------------------------------------------
	std::string GetPrefsString(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	// Get longer string describing the render flags, using words
	//	instead of codes.
	//------------------------------------------------------------------------
	std::string GetPrefsStringLong(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetRendererIndex(g3dSceneRendererCreate::RendererType i_Type);
}
