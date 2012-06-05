/*****************************************************************************
**  appCharEventHandler.hpp
**
**      appCharEventHandler is a base class which must be inherited from
**	by anything which wants to receive character events.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_CHAREVENTHANDLER_HPP
#error appCharEventHandler.hpp multiply included
#endif
#define APP_CHAREVENTHANDLER_HPP


//============================================================================
//============================================================================
class appCharEvent;


//============================================================================
//============================================================================
class appCharEventHandler
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		appCharEventHandler();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appCharEventHandler();

		//--------------------------------------------------------------------
		//	Override this function to get appCharEvents.
		//--------------------------------------------------------------------
		virtual void ReceiveCharEvent(appCharEvent& i_Event) = 0;

		//--------------------------------------------------------------------
		//	SetEnableCharEvents can be used to enable or disable char
		//	event reporting.
		//--------------------------------------------------------------------
		void SetEnableCharEvents(bool i_Enable);

		//--------------------------------------------------------------------
		//	GetEnableCharEvents will return true if the object currently 
		//	wants to receive char events.
		//--------------------------------------------------------------------
		bool GetEnableCharEvents() const;

	private:

		bool m_Enable;
};

//--------------------------------------------------------------------
//	 implementation
//--------------------------------------------------------------------
inline bool appCharEventHandler::GetEnableCharEvents() const
{
	return m_Enable;
}