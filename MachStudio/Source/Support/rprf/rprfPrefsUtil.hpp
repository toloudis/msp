/*****************************************************************************
**	rprfPrefsObject.hpp
**
**		Shared code used by both the viewport prefs and the render layer prefs
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef RPRF_PREFSUTIL_HPP
#error rdrLayersUtil.hpp multiply included
#endif
#define RPRF_PREFSUTIL_HPP

#ifndef RPRFPREFSDATA_HPP
#include "Support/rprf/rprfPrefsData.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 


//============================================================================
//	Forward References
//============================================================================
class prtyObject;
class prtyProperty;
class prtyPropertyCallback;


//============================================================================
//============================================================================
namespace rprfPrefsUtil
{
	//------------------------------------------------------------------------
	// Init
	//------------------------------------------------------------------------
	void Initialize();

	//------------------------------------------------------------------------
	// DeInit
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	//  Both viewport and render layer prefs will use this function to register
	//	their properties
	//------------------------------------------------------------------------
	void RegisterPrefProperties(prtyObject* o_PrefsObject, rprfPrefsData& i_Data, bool i_bIsViewport = false);

	//------------------------------------------------------------------------
	// register the ssao property for the pref object
	//------------------------------------------------------------------------
	void RegisterSSAOEnable(prtyObject* o_PrefsObject, rprfPrefsData& i_Data); 

	//------------------------------------------------------------------------
	// register the ssao sampling
	//------------------------------------------------------------------------
	void RegisterSSAOSampling(prtyObject* o_PrefsObject, rprfPrefsData& i_Data);

	//------------------------------------------------------------------------
	// register the ssao gui for the pref object
	//------------------------------------------------------------------------
	void RegisterSSAOgui(prtyObject* o_PrefsObject, rprfPrefsData& i_Data);

	//------------------------------------------------------------------------
	// register the ssgi property for the pref object
	//------------------------------------------------------------------------
	void RegisterSSGIEnable(prtyObject* o_PrefsObject, rprfPrefsData& i_Data); 

	//------------------------------------------------------------------------
	// register the ssgi sampling
	//------------------------------------------------------------------------
	void RegisterSSGISampling(prtyObject* o_PrefsObject, rprfPrefsData& i_Data);

	//------------------------------------------------------------------------
	// register the ssgi gui for the pref object
	//------------------------------------------------------------------------
	void RegisterSSGIgui(prtyObject* o_PrefsObject, rprfPrefsData& i_Data);

	//------------------------------------------------------------------------
	// For each category in the render pref dialog, we need to specify which
	// ones are populated for each render type
	//------------------------------------------------------------------------
	void SetupCategories();

	//------------------------------------------------------------------------
	// When the render type is changed, we want to change which categories
	// are visible
	//------------------------------------------------------------------------
	void UpdateGUICategories(rprfPrefsData& i_Data, bool i_bIsViewport = false);

	//------------------------------------------------------------------------
	// Set the update function pointers, these function will update the 
	// appropriate render pref dialog after a render type has changed
	//------------------------------------------------------------------------
	void SetUpdateFunction(void (*i_UpdateFunction)());
	void SetUpdateFunctionVP(void (*i_UpdateFunctionVP)());

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateDialog();
	void UpdateDialogVP();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool RenderTypeEnabled( int i_RenderType );

	//------------------------------------------------------------------------
	// Return whether or not a HDR layer is valid
	//------------------------------------------------------------------------
	bool HDRLayerEnabled( int i_HDRLayer );

}