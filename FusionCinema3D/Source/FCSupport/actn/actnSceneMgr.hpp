//****************************************************************************
//	actnSceneMgr.hpp
//
//	A manager for Scene items
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef ACTN_SCENEMGR_HPP
#error actnSceneMgr.hpp multiply included
#endif
#define ACTN_SCENEMGR_HPP

#include<map>

//============================================================================
//	forward references
//============================================================================
class fsLocator;
class fcuiTimelineItemData;
class maTime;

//============================================================================
//============================================================================
class actnSceneMgr
{
public:
	///-----------------------------------------------------------------------
	/// constructors
	///-----------------------------------------------------------------------
	actnSceneMgr();

	///-----------------------------------------------------------------------
	/// destructors
	///-----------------------------------------------------------------------
	~actnSceneMgr();

	///-----------------------------------------------------------------------
	/// Return the current instance of the mgr singleton
	///-----------------------------------------------------------------------
	static actnSceneMgr* Instance;

	//------------------------------------------------------------------------
	///	Deinitialize/Initialize
	//------------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	///-----------------------------------------------------------------------
	/// Perform the mode specific operations when a scene element needs to 
	/// execute an action.
	///-----------------------------------------------------------------------
	void ProcessScene(const fsLocator& i_SceneItem);

	///-----------------------------------------------------------------------
	/// Perfrom the appropriate operations for a timeline object belonging
	/// to the scene category
	///-----------------------------------------------------------------------
	void ProcessSceneTimelineItem(const fcuiTimelineItemData& i_ItemData);

	///---------------------------------------------------------------------------
	/// shift the timeline item belonging to the scene category
	///---------------------------------------------------------------------------
	void ShiftSceneTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration);

	///---------------------------------------------------------------------------
	///---------------------------------------------------------------------------
	void ShiftCameraKeys(const fcuiTimelineItemData& i_ItemData);

	///---------------------------------------------------------------------------
	/// shift the timeline item belonging to the scene category
	///---------------------------------------------------------------------------
	void RemoveSceneTimelineItem(const fcuiTimelineItemData& i_ItemData);

	///---------------------------------------------------------------------------
	/// Edit the timeline item belonging to the scene category
	///---------------------------------------------------------------------------
	void EditSceneTimelineItem(fcuiTimelineItemData& io_ItemData);

private:
	std::map<std::string,maTime> m_CamStartMap;

};
