/*****************************************************************************
**  mnmAutoSaveMgr.hpp
**
**      auto save managment system
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_AUTOSAVEMGR_HPP
#error mnmAutoSaveMgr.hpp multiply included
#endif
#define MNM_AUTOSAVEMGR_HPP


//============================================================================
//============================================================================
namespace mnmAutoSaveMgr
{
	//------------------------------------------------------------------------
	// Initialize
	//------------------------------------------------------------------------
	void Initialize(bool i_bActive,
					int i_FrequencyInSeconds,
					int i_NumBackups);

	//------------------------------------------------------------------------
	// DeInitialize
	//------------------------------------------------------------------------
	void DeInitialize();

	//------------------------------------------------------------------------
	//	Think() - check to see if the auto save systm should save a copy
	//	of the scene.
	//------------------------------------------------------------------------
	void Think();

	//------------------------------------------------------------------------
	//	ResetAutoSaveTimer() - this should get called when a new file is 
	//	loaded to reset the auto save timer.
	//------------------------------------------------------------------------
	void ResetAutoSaveTimer();

	//------------------------------------------------------------------------
	//	BackupSavedFile() - back up the currently saved file.
	//	(see docSingleDocMgr).  This could be called right BEFORE someone
	//	saves the current scene to keep a "last good saved version".
	//------------------------------------------------------------------------
	void BackupSavedFile();
}

