/****************************************************************************\
**	chnlClipDriver.hpp
**
**		Data class for a clip to display in a channel control
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_CLIPDRIVER_HPP
#error chnlClipDriver.hpp multiply included
#endif
#define CHNL_CLIPDRIVER_HPP

#ifndef CHNL_CHANNELCLIP_HPP
#include "Features/Channels/wxGUI/chnlChannelClip.hpp"
#endif

//============================================================================
//============================================================================
class tmlnDriver;

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class chnlClipDriver : public chnlChannelClip
{
	public:			
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlClipDriver(	tmlnDriver& i_Driver, 
						const std::string& i_Name, 
						float i_BeginTime, 
						float i_EndTime);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlClipDriver(const chnlClipDriver& i_Clip);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~chnlClipDriver();

		//--------------------------------------------------------------------
		// Access to driver
		//--------------------------------------------------------------------
		tmlnDriver&	Driver();
			
		//--------------------------------------------------------------------
		// Update - get values from driver and set into clip. Called when
		//	a property of the driver has changed.
		//--------------------------------------------------------------------
		void Update();

	//============================================================================
	//	Virtual functions to be overriden
	//============================================================================

		//--------------------------------------------------------------------
		// Set begin and end times using duration
		//--------------------------------------------------------------------
		virtual void SetTime(float i_BeginTime, float i_Duration);

		//--------------------------------------------------------------------
		// Set blend attributes
		//--------------------------------------------------------------------
		virtual void SetBlend(BlendType i_Blend, float i_BlendTime);

		//--------------------------------------------------------------------
		// Set the restore flag
		//--------------------------------------------------------------------
		virtual void SetRestore(bool i_bRestore);

		//--------------------------------------------------------------------
		// display properties dialog for this clip
		//--------------------------------------------------------------------
		virtual void ShowProperties();

		//--------------------------------------------------------------------
		// allow driver to handle selection in its own way
		//--------------------------------------------------------------------
		virtual void Select();

		//--------------------------------------------------------------------
		// select the 3D icon for this clip
		//--------------------------------------------------------------------
		virtual void SelectIcon();

		//--------------------------------------------------------------------
		/// Return string to display when mouse hovers over control
		//--------------------------------------------------------------------
		virtual std::string GetHoverDescription();

		//--------------------------------------------------------------------
		/// Return string to display when mouse is interacting 
		//		(moving, resizing) over control
		//--------------------------------------------------------------------
		virtual std::string GetInteractionDescription(float i_InteractStart, float i_InteractDuration);

	private:
		tmlnDriver& m_Driver;
};

#endif // USE_WXWIDGETS
