//****************************************************************************
//	actnPostFXMgr.hpp
//
//	A manager for PostFXs
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef ACTN_POSTFXMGR_HPP
#error actnPostFXMgr.hpp multiply included
#endif
#define ACTN_POSTFXMGR_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//============================================================================
class actnPostFXMgr
{
public:
	///-----------------------------------------------------------------------
	/// constructors
	///-----------------------------------------------------------------------
	actnPostFXMgr();

	///-----------------------------------------------------------------------
	/// destructors
	///-----------------------------------------------------------------------
	~actnPostFXMgr();

	///-----------------------------------------------------------------------
	/// Return the current instance of the mgr singleton
	///-----------------------------------------------------------------------
	static actnPostFXMgr* Instance;

	//--------------------------------------------------------------------
	///	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	///---------------------------------------------------------------------------
	/// Perform the mode specific operations when a PostFX element needs to 
	/// execute an action.
	///---------------------------------------------------------------------------
	void ProcessPostFX(const fsLocator& i_PostFXScene);

	///---------------------------------------------------------------------------
	/// Set/Get the checked item for the PostFXs
	///---------------------------------------------------------------------------
	void SetCheckedItem(const fsLocator& i_PostFXScene);
	const fsLocator& GetCheckedItem();

private:
	fsLocator m_CheckedLocator;
};
