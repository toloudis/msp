//****************************************************************************
//	actnTitleCardMgr.hpp
//
//	A manager for TitleCard
//
//	StudioGPU
//	Copyright(C) 2010 - All Rights Reserved
//****************************************************************************
#ifdef ACTN_TITLECARDMGR_HPP
#error actnTitleCardMgr.hpp multiply included
#endif
#define ACTN_TITLECARDMGR_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#ifndef FIO_TITLECARDITEM_HPP
#include "FCSupport/fio/fioTitleCardItem.hpp"
#endif
//============================================================================
//	forward references
//============================================================================
class fsLocator;

class envtScriptData;
class envtScriptObject;
class fcuiTimelineItemData;
class maTime;


//============================================================================
//============================================================================
class actnTitleCardMgr
{
public:
	///----------------------------------------------------------------------- 
	/// constructors
	///-----------------------------------------------------------------------
	actnTitleCardMgr();

	///-----------------------------------------------------------------------
	/// destructors
	///-----------------------------------------------------------------------
	~actnTitleCardMgr();

	///-----------------------------------------------------------------------
	/// Return the current instance of the mgr singleton
	///-----------------------------------------------------------------------
	static actnTitleCardMgr* Instance;

	//--------------------------------------------------------------------
	///	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	///---------------------------------------------------------------------------
	/// Perform the mode specific operations when a titlecard element needs to 
	/// execute an action.
	///---------------------------------------------------------------------------
	void ProcessTitleCard(const fsLocator& i_ElementDirectory);

	
	///-----------------------------------------------------------------------
	/// Action to perform when titlecards are clicked on the timeline
	///-----------------------------------------------------------------------
	void ProcessTitleCardTimelineItem(const fcuiTimelineItemData& i_ItemData);
	
	///-----------------------------------------------------------------------
	/// Create the TitleCard
	///-----------------------------------------------------------------------
	void CreateTitleCard();
	
	///-----------------------------------------------------------------------
	/// Add a texture driver to the titlecard
	///-----------------------------------------------------------------------
	void AddTitleCardTexture(const fsLocator& i_ElementPath);

	
	///-----------------------------------------------------------------------
	/// Write on the image
	///-----------------------------------------------------------------------
	void WriteOnTitleCard(const fsLocator& i_Filename, const itString& i_BodyText, const itString& i_TitleText, const maFloatRGBA& i_docColor);
	
	///-----------------------------------------------------------------------
	/// reload texture on a billboard
	///-----------------------------------------------------------------------
	void ReloadTexture(const itString& ObjName, const fsLocator& i_TexturePath);
	
	///-----------------------------------------------------------------------
	/// Action to perform when titlecards are moved in timeline
	///-----------------------------------------------------------------------
	void ShiftTitleCardTimelineItem(fcuiTimelineItemData& io_ItemData, const maTime& i_Duration, bool i_bManualShift);
	
	///-----------------------------------------------------------------------
	/// Action to be performed when titlecards are removed
	///-----------------------------------------------------------------------
	void RemoveTitleCardTimelineItem(const fcuiTimelineItemData& i_ItemData);
	
	///-----------------------------------------------------------------------
	/// Action to be performed when we click on the edit on the timeline item
	///-----------------------------------------------------------------------
	void EditTitleCardTimelineItem(fcuiTimelineItemData& io_ItemData);

	///-----------------------------------------------------------------------
	/// Check if any files are missing in the titlecards
	///-----------------------------------------------------------------------
	void CheckTitleCardData(fcuiTimelineItemData& io_ItemData);

	///-----------------------------------------------------------------------
	/// Return the x position for the newest billboard
	///-----------------------------------------------------------------------
	float GetMostRecentXPosition();

	///-----------------------------------------------------------------------
	/// Return the most recent name given to a new texture driver for the billboard
	///-----------------------------------------------------------------------
	itString GetMostRecentDriverName();

	///-----------------------------------------------------------------------
	/// Return the name of the billboard
	///-----------------------------------------------------------------------
	itString GetBillboardName();

	void ReadXML(fsLocator& i_XMLFile, fioTitleCardItemData& io_Data);
	void WriteXML(fsLocator& i_XMLFile, const maFloatRGBA& i_docColor);

private:
	float m_MostRecentCardPosX;
	itString m_BillboardName;
	itString m_MostRecentDriverName;
	itString m_EditDriver;
};