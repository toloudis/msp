//****************************************************************************
//	actnLightMgr.hpp
//
//	A manager for Lights
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef ACTN_LIGHTMGR_HPP
#error actnLightMgr.hpp multiply included
#endif
#define ACTN_LIGHTMGR_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//============================================================================
class actnLightMgr
{
public:
	///-----------------------------------------------------------------------
	/// constructors
	///-----------------------------------------------------------------------
	actnLightMgr();

	///-----------------------------------------------------------------------
	/// destructors
	///-----------------------------------------------------------------------
	~actnLightMgr();

	///-----------------------------------------------------------------------
	/// Return the current instance of the mgr singleton
	///-----------------------------------------------------------------------
	static actnLightMgr* Instance;

	//--------------------------------------------------------------------
	///	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	///---------------------------------------------------------------------------
	/// Perform the mode specific operations when a lighting element needs to 
	/// execute an action.
	///---------------------------------------------------------------------------
	void ProcessLights(const fsLocator& i_LightScene);

	///---------------------------------------------------------------------------
	/// Set/Get the checked item for the lights
	///---------------------------------------------------------------------------
	void SetCheckedItem(const fsLocator& i_LightScene);
	const fsLocator& GetCheckedItem();

private:
	fsLocator m_CheckedLocator;
};
