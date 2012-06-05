/*****************************************************************************
**  appFlowEventHandler.hpp
**
**      appFlowEventHandler is a base class which must be inherited from
**	by anything which wants to receive flow events.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_FLOWEVENTHANDLER_HPP
#error appFlowEventHandler.hpp multiply included
#endif
#define APP_FLOWEVENTHANDLER_HPP


//============================================================================
//============================================================================
class appStartEvent;
class appStopEvent;
class appSuspendEvent;
class appResumeEvent;
class appQuitRequestEvent;


//============================================================================
//============================================================================
class appFlowEventHandler
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		appFlowEventHandler();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appFlowEventHandler();

		//--------------------------------------------------------------------
		//	Override this function to get appStartEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveStartEvent(appStartEvent& i_Event);

		//--------------------------------------------------------------------
		//	Override this function to get appStopEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveStopEvent(appStopEvent& i_Event);

		//--------------------------------------------------------------------
		//	Override this function to get appSuspendEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveSuspendEvent(appSuspendEvent& i_Event);

		//--------------------------------------------------------------------
		//	Override this function to get appResumeEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveResumeEvent(appResumeEvent& i_Event);

		//--------------------------------------------------------------------
		//	Override this function to get appQuitReqeustedEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event);

		//--------------------------------------------------------------------
		//	SetEnableFlowEvents can be used to enable or disable flow
		//	event reporting.
		//--------------------------------------------------------------------
		void SetEnableFlowEvents(bool i_Enable);

		//--------------------------------------------------------------------
		//	GetEnableFlowEvents will return true if the object currently 
		//	wants to receive flow events.
		//--------------------------------------------------------------------
		bool GetEnableFlowEvents() const;

	private:

		bool m_Enable;
};

//--------------------------------------------------------------------
//	 implementation
//--------------------------------------------------------------------
inline bool appFlowEventHandler::GetEnableFlowEvents() const
{
	return m_Enable;
}
