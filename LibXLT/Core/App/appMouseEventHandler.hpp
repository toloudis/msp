/*****************************************************************************
**  appMouseEventHandler.hpp
**
**      appMouseEventHandler is a base class which must be inherited from
**	by anything which wants to receive mouse events.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_MOUSEEVENTHANDLER_HPP
#error appMouseEventHandler.hpp multiply included
#endif
#define APP_MOUSEEVENTHANDLER_HPP


//============================================================================
//============================================================================
class appMouseUpEvent;
class appMouseDownEvent;
class appMouseMoveEvent;


//============================================================================
//============================================================================
class appMouseEventHandler
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		appMouseEventHandler();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appMouseEventHandler();

		//--------------------------------------------------------------------
		//	Override this function to get appMouseUpEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveMouseUpEvent(appMouseUpEvent& i_Event) = 0;

		//--------------------------------------------------------------------
		//	Override this function to get appMouseDownEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveMouseDownEvent(appMouseDownEvent& i_Event) = 0;

		//--------------------------------------------------------------------
		//	Override this function to get appMouseDownEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveMouseMoveEvent(appMouseMoveEvent& i_Event) = 0;

		//--------------------------------------------------------------------
		//	Call this function with a true parameter if you want to receive
		//	appMouseMoveEvents, or false if you don't.  The default is true.
		//--------------------------------------------------------------------
		void SetReceiveMoveEvents(bool i_Receive);

		//--------------------------------------------------------------------
		//	This will return true if mouse move events will be given to the
		//	event handler.
		//--------------------------------------------------------------------
		bool ReceiveMoveEvents() const;		

		//--------------------------------------------------------------------
		//	SetEnableMouseEvents can be used to enable or disable mouse
		//	event reporting.  (Any events at all, as opposed to just mouse
		//	move events, as controlled by SetReceiveMoveEvents).
		//--------------------------------------------------------------------
		void SetEnableMouseEvents(bool i_Enable);

		//--------------------------------------------------------------------
		//	GetEnableMouseEvents will return true if the object currently 
		//	wants to receive mouse events.
		//--------------------------------------------------------------------
		bool GetEnableMouseEvents() const;
	
	private:

		bool m_ReceiveMoveEvents;
		bool m_Enable;
};


//--------------------------------------------------------------------
//	 implementation
//--------------------------------------------------------------------
inline bool appMouseEventHandler::GetEnableMouseEvents() const
{
	return m_Enable;
}