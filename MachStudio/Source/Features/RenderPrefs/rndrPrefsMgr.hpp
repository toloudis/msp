/*****************************************************************************
**	rndrPrefsMgr.hpp
**
**		API for preferences utilities
**
**	StudioGPU
**	Copyright(C) 2006-7 - All Rights Reserved
\****************************************************************************/
#ifdef RNDRPREFSMGR_HPP
#error rndrPrefsMgr.hpp multiply included
#endif
#define RNDRPREFSMGR_HPP

//#ifndef RPRFPREFSDATA_HPP
////#include "Support/rprf/rprfPrefsData.hpp"
//#include "Support/rprf/rprfPrefsData.hpp"
//#endif

#ifndef RNDRPREFSDATAINTEREST_HPP
#include "Features/RenderPrefs/rndrPrefsDataInterest.hpp"
#endif

#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
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
	// Data can be altered in the GUI thread, its changes are proxied
	//------------------------------------------------------------------------
	rprfPrefsData& Data(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	// ActualData should only be used in the render thread or when capturing
	// single threaded.
	//------------------------------------------------------------------------
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
	prtyObject* CreatePrefsObject(g3dPrefs::g3dRenderPrefs* i_RenderPrefs);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	//	Build a string based on the render flags, one char per flag
	//	S - Shadows
	//	M - Render Matte
	//	D - DOF
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
	//	set next render pass
	//------------------------------------------------------------------------
	void IncrementPass(rndr_Object i_ObjectID);

	//------------------------------------------------------------------------
	//	set previous render pass
	//------------------------------------------------------------------------
	void DecrementPass(rndr_Object i_ObjectID);
}
